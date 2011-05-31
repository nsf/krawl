#include <getopt.h>
#include <ctype.h>
#include "crawlc.hpp"
#include "cityhash/city.h"

struct all_t {
	// memory trackers for semantic declarations, types and other stuff
	sdecl_tracker_t dtracker;
	stype_tracker_t ttracker;
	scope_block_tracker_t stracker;

	// this structure contains all the information about files
	// including their contents
	source_group_t srcinfo;

	// global scope for predeclared identifiers
	scope_block_t globalscope;

	// package scope, a parent of all file scopes
	scope_block_t pkgscope;
	std::vector<const char*> declnames;

	// diagnostic stack and ASTs
	diagnostic_t diag;
	node_t* ast;

	// brawl
	brawl_context_t brawl;

	all_t(): ast(0)
	{
		init_builtin_stypes();
		fill_global_scope(&globalscope, &dtracker, &ttracker);
		pkgscope.parent = &globalscope;
		brawl.ttracker = &ttracker;
		brawl.dtracker = &dtracker;
	}

	~all_t()
	{
		free_builtin_stypes();
		free_tracked_stypes(&ttracker);
		free_tracked_sdecls(&dtracker);
		free_tracked_scope_blocks(&stracker);
		delete ast;
	}
};

static const struct option longopts[] = {
	{"uid",      required_argument, 0, 'U'},
	{"hash-uid", required_argument, 0, 'H'},
	{"ast",      no_argument,       0, 'a'},
	{"package",  required_argument, 0, 'P'},
	{"help",     no_argument,       0, 'h'},
	{"version",  no_argument,       0, 'v'},
	{"dump",     no_argument,       0, 'D'},
	{0,          0,                 0, 0}
};

struct options_t {
	// specified from the command line
	const char *uid;
	const char *hash_uid;
	std::string package;
	bool print_ast;
	const char *out_name;
	std::vector<const char*> libs;
	std::vector<const char*> files;
	bool dump;

	// calculated on the fly
	std::string out_lib;
	std::string theuid;

	bool is_lib() const { return uid || hash_uid; }
};

static void print_help_and_exit()
{
	printf(""
"usage: crawlc [options] <files>\n"
"\n"
"    -h, --help           print this message and exit\n"
"    -v, --version        print version and exit\n"
"    --uid=<prefix>       specify unique prefix id for symbols\n"
"    --hash-uid=<string>  specify unique hash-based prefix id for symbols\n"
"    -P, --package=<name> specify package name\n"
"    --ast                print AST and exit (output is in the yaml format)\n"
"    -o <output>          specify output destination\n"
"    -l <lib>             specify a library name to pass to a linker\n"
"    --dump               dump LLVM assembly to stdout during compilation\n"
);
	exit(0);
}

static void print_version_and_exit()
{
	printf("crawlc version 0.1\n");
	exit(0);
}

static std::string get_module_uid(const char *uid, const char *hash_uid)
{
	if (uid)
		return uid;

	if (hash_uid) {
		char buf[12] = {0};
		uint64_t hash = CityHash64(hash_uid, strlen(hash_uid));
		base62_encode64(hash, buf);
		return buf;
	}

	return "";
}


static bool parse_options(options_t *opts, int argc, char **argv)
{
	opts->uid = 0;
	opts->hash_uid = 0;
	opts->print_ast = false;
	opts->out_name = "crl.out";
	opts->dump = false;

	int c;
	while ((c = getopt_long(argc, argv, "vho:l:P:", longopts, 0)) != -1) {
		switch (c) {
		case 'v':
			print_version_and_exit();
			break;
		case 'h':
			print_help_and_exit();
			break;
		case 'U':
			opts->uid = optarg;
			break;
		case 'H':
			opts->hash_uid = optarg;
			break;
		case 'P':
			opts->package = optarg;
			break;
		case 'a':
			opts->print_ast = true;
			break;
		case 'o':
			opts->out_name = optarg;
			break;
		case 'l':
			opts->libs.push_back(optarg);
			break;
		case 'D':
			opts->dump = true;
			break;
		default:
			return false;
		}
	}

	// Find library interface file output
	opts->out_lib = replace_extension(opts->out_name, "brl");

	// Find real UID
	opts->theuid = get_module_uid(opts->uid, opts->hash_uid);

	// If package name wasn't provided, try to figure out it
	if (opts->package.empty()) {
		std::string stem = extract_stem(opts->out_name);
		for (size_t i = 0, n = stem.size(); i < n; ++i) {
			if (i == 0 && isdigit(stem[i])) {
				opts->package.append("_");
				opts->package.append(1, stem[i]);
				continue;
			}

			if (!isalnum(stem[i])) {
				opts->package.append("_");
				continue;
			}

			opts->package.append(1, stem[i]);
		}
	}

	for (int i = optind; i < argc; ++i)
		opts->files.push_back(argv[i]);

	return true;
}

static void generate_lib(const char *filename,
			 scope_block_t *pkgscope,
			 std::vector<const char*> *declnames,
			 const char *prefix, const char *package)
{
	brawl_module_t module(0);
	module.queue_for_serialization(pkgscope, declnames, prefix, package);

	FILE *f = fopen(filename, "wb");
	if (!f) {
		fprintf(stderr, "Failed opening file for writing: %s\n", filename);
		exit(1);
	}
	{
		FILE_writer_t cout(f);
		module.save(&cout);
	}
	fclose(f);
}

int main(int argc, char **argv)
{
	options_t opts;
	if (!parse_options(&opts, argc, argv))
		return 1;

	const char *data_name = "stdin";
	std::vector<char> data;
	if (!opts.files.empty()) {
		if (opts.files.size() > 1) {
			fprintf(stderr, "At the moment only one file "
				"as input is supported\n");
			return 1;
		}

		if (!read_file(&data, opts.files[0])) {
			fprintf(stderr, "Failed to read data from file: %s\n",
				opts.files[0]);
			return 1;
		}
		data_name = opts.files[0];
	} else if (!read_stdin(&data) || data.empty()) {
		fprintf(stderr, "Failed to read data from stdin\n");
		return 1;
	}

	data.push_back('\0');

	all_t d;

	parser_t p(&d.srcinfo, &d.diag);
	p.set_input(data_name, &data);
	d.ast = p.parse();

	if (opts.print_ast && d.ast) {
		print_ast(d.ast);
		return 0;
	}

	if (!d.diag.empty()) {
		d.diag.print_to_stderr(&d.srcinfo);
		return 1;
	}

	// PASS 1
	pass1_t p1;
	p1.stracker = &d.stracker;
	p1.dtracker = &d.dtracker;
	p1.pkgscope = &d.pkgscope;
	p1.names = &d.declnames;
	p1.diag = &d.diag;
	p1.brawl = &d.brawl;
	p1.pass(d.ast);

	// PASS 2
	pass2_t p2;
	p2.stracker = &d.stracker;
	p2.ttracker = &d.ttracker;
	p2.dtracker = &d.dtracker;
	p2.pkgscope = &d.pkgscope;
	p2.diag = &d.diag;
	p2.pass(&d.declnames);

	if (!d.diag.empty()) {
		d.diag.print_to_stderr(&d.srcinfo);
		return 1;
	}

	if (opts.is_lib())
		generate_lib(opts.out_lib.c_str(), &d.pkgscope, &d.declnames,
			     opts.theuid.c_str(), opts.package.c_str());

	// PASS 3
	pass3_t p3;
	p3.uid = opts.theuid;
	p3.pkgscope = &d.pkgscope;
	p3.used_extern_sdecls = &p2.used_extern_sdecls;
	p3.out_name = opts.out_name;
	p3.libs = &opts.libs;
	p3.dump = opts.dump;
	p3.pass(&d.declnames);

	return 0;
}
