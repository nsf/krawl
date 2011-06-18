#include <getopt.h>
#include <ctype.h>
#include "krawl.hpp"
#include "cityhash/city.h"
#include <llvm/Support/Timer.h>

struct options_t {
	// specified from the command line
	const char *uid;
	const char *hash_uid;
	std::string package;
	bool print_ast;
	std::string out_name;
	std::vector<const char*> files;
	std::vector<const char*> include_dirs;
	bool dump;
	bool time;
	bool deps;

	const char *clang_path;
	const char *clang_plugin_path;

	// calculated on the fly
	std::string out_lib;
	std::string theuid;

	bool is_lib() const { return uid || hash_uid; }
};

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
	std::vector<sdecl_t*> pkgdecls;

	// diagnostic stack and ASTs
	diagnostic_t diag;
	node_t* ast;

	// import
	import_context_t ictx;

	// timers
	llvm::Timer t_parse;
	llvm::Timer t_pass1;
	llvm::Timer t_pass2;
	llvm::Timer t_pass3;
	llvm::TimerGroup t_group;

	all_t(options_t *opts): ast(0), t_group("Krawl")
	{
		init_builtin_stypes();
		fill_global_scope(&globalscope, &dtracker, &ttracker);
		pkgscope.parent = &globalscope;
		ictx.ttracker = &ttracker;
		ictx.dtracker = &dtracker;
		ictx.include_dirs = &opts->include_dirs;
		ictx.clang_path = opts->clang_path;
		ictx.clang_plugin_path = opts->clang_plugin_path;

		t_parse.init("Parse", t_group);
		t_pass1.init("Pass1", t_group);
		t_pass2.init("Pass2", t_group);
		t_pass3.init("Pass3", t_group);
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
	{"uid",          required_argument, 0, 'u'},
	{"hash-uid",     required_argument, 0, 'U'},
	{"obj-out",      required_argument, 0, 'o'},
	{"ast",          no_argument,       0, 'a'},
	{"package",      required_argument, 0, 'P'},
	{"help",         no_argument,       0, 'h'},
	{"version",      no_argument,       0, 'v'},
	{"dump",         no_argument,       0, 'D'},
	{"time",         no_argument,       0, 't'},
	{"deps",         no_argument,       0, 'm'},
	{"clang",        required_argument, 0, 'c'},
	{"clang-plugin", required_argument, 0, 'C'},
	{0,          0,                     0, 0}
};

static void print_help_and_exit(const char *app)
{
	printf(""
"usage: %s [options] <files>\n"
"\n"
"    -h, --help               print this message and exit\n"
"    -v, --version            print version and exit\n"
"    -u, --uid=<prefix>       unique prefix id for symbols\n"
"    -U, --hash-uid=<string>  unique hash-based prefix id for symbols\n"
"    -P, --package=<name>     package name\n"
"    -o, --obj-out=<output>   object file output destination\n"
"    -b, --brl-out=<output>   brawl interface file output destination\n"
"    -I <path>                search path for modules importer\n"
"    --ast                    print AST and exit (output is in the yaml format)\n"
"    --dump                   dump LLVM assembly to stdout during compilation\n"
"    --time                   enable timers\n"
"    --deps                   print source file module dependencies\n"
"\n"
"    --clang=<path>           clang executable location\n"
"    --clang-plugin=<path>    clang c-to-krawl plugin location\n"
, app);
	exit(0);
}

static void print_version_and_exit()
{
	printf("krawl version 0.1\n");
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
	opts->dump = false;
	opts->time = false;
	opts->deps = false;
	opts->clang_path = 0;
	opts->clang_plugin_path = 0;

	int c;
	while ((c = getopt_long(argc, argv, "vho:b:I:P:u:U:", longopts, 0)) != -1) {
		switch (c) {
		case 'v':
			print_version_and_exit();
			break;
		case 'h':
			print_help_and_exit(argv[0]);
			break;
		case 'u':
			opts->uid = optarg;
			break;
		case 'U':
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
		case 'b':
			opts->out_lib = optarg;
			break;
		case 'I':
			opts->include_dirs.push_back(optarg);
			break;
		case 'D':
			opts->dump = true;
			break;
		case 't':
			opts->time = true;
			break;
		case 'm':
			opts->deps = true;
			break;
		case 'c':
			opts->clang_path = optarg;
			break;
		case 'C':
			opts->clang_plugin_path = optarg;
			break;
		default:
			return false;
		}
	}

	for (int i = optind; i < argc; ++i)
		opts->files.push_back(argv[i]);

	// figure out different names
	if (opts->out_name.empty() && !opts->files.empty())
		opts->out_name = replace_extension(opts->files[0], "o");
	if (opts->out_lib.empty())
		opts->out_lib = replace_extension(opts->out_name.c_str(), "brl");

	// choose UID
	opts->theuid = get_module_uid(opts->uid, opts->hash_uid);

	// if package name wasn't provided, try to figure out it
	if (opts->package.empty()) {
		std::string stem = extract_stem(opts->out_name.c_str());
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

	return true;
}

static void generate_lib(const char *filename,
			 std::vector<sdecl_t*> *pkgdecls,
			 const char *prefix, const char *package)
{
	FILE *f = fopen(filename, "wb");
	if (!f) {
		fprintf(stderr, "Failed opening file for writing: %s\n", filename);
		exit(1);
	}
	{
		FILE_writer_t cout(f);
		brawl_serializer_t s;
		s.serialize(&cout, pkgdecls, prefix, package);
	}
	fclose(f);
}

static void extract_and_print_deps(std::vector<char> *data)
{
	unordered_set<std::string> modules;
	source_group_t srcinfo;
	diagnostic_t diag;

	parser_t p(&srcinfo, &diag);
	p.set_input("tmp", data);
	node_t *ast = p.parse();
	if (!ast)
		DIE("failed to parse source file");

	KRAWL_QASSERT(ast->type == node_t::PROGRAM);
	program_t *prog = (program_t*)ast;
	for (size_t i = 0, n = prog->decls.size(); i < n; ++i) {
		node_t *d = prog->decls[i];
		if (d->type == node_t::IMPORT_DECL) {
			import_decl_t *id = (import_decl_t*)d;
			for (size_t i = 0, n = id->specs.size(); i < n; ++i) {
				import_spec_t *spec = id->specs[i];
				std::string p = spec->path->val.to_string();
				if (modules.find(p) == modules.end()) {
					printf("%s\n", p.c_str());
					modules.insert(p);
				}
			}
		}
	}
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

	if (opts.deps) {
		extract_and_print_deps(&data);
		return 0;
	}

	all_t d(&opts);

#define START_TIMER(timer) if (opts.time) d.timer.startTimer()
#define STOP_TIMER(timer) if (opts.time) d.timer.stopTimer()

	//=========================================================== PARSING
	START_TIMER(t_parse);
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
	STOP_TIMER(t_parse);

	//=========================================================== PASS 1
	START_TIMER(t_pass1);
	pass1_opts_t p1opts = {
		&d.stracker,
		&d.dtracker,
		&d.pkgscope,
		&d.pkgdecls,
		&d.diag,
		&d.ictx,
	};
	pass1_t p1(&p1opts);
	p1.pass(d.ast);
	STOP_TIMER(t_pass1);

	//=========================================================== PASS 2
	START_TIMER(t_pass2);
	pass2_opts_t p2opts = {
		opts.theuid.c_str(),
		&d.stracker,
		&d.ttracker,
		&d.dtracker,
		&d.pkgscope,
		&d.diag,
	};
	pass2_t p2(&p2opts);
	p2.pass(&d.pkgdecls);

	if (!d.diag.empty()) {
		d.diag.print_to_stderr(&d.srcinfo);
		return 1;
	}

	if (opts.is_lib())
		generate_lib(opts.out_lib.c_str(), &d.pkgdecls,
			     opts.theuid.c_str(), opts.package.c_str());
	STOP_TIMER(t_pass2);

	//=========================================================== PASS 3
	START_TIMER(t_pass3);
	pass3_opts_t p3opts = {
		opts.theuid.c_str(),
		&d.pkgscope,
		&p2.used_extern_sdecls,
		opts.out_name.c_str(),
		opts.dump,
		opts.time,
	};
	pass3_t p3(&p3opts);
	p3.pass(&d.pkgdecls);
	STOP_TIMER(t_pass3);

	return 0;
}
