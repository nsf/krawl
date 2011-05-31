#include "crawlc.hpp"

//------------------------------------------------------------------------------
// FILE_reader_t
//------------------------------------------------------------------------------

enum {
	VARINT_U16,
	VARINT_U32,
	VARINT_U64,

	VARINT_BIT = (1 << 7)
};

FILE_reader_t::FILE_reader_t(FILE *f): file(f)
{
}

uint64_t FILE_reader_t::read_varuint()
{
	uint8_t n = read_uint8();
	if (n & VARINT_BIT) {
		switch (n & ~VARINT_BIT) {
		case VARINT_U16:
			return read_uint16();
		case VARINT_U32:
			return read_uint32();
		case VARINT_U64:
			return read_uint64();
		default:
			CRAWL_ASSERT(false, "invalid binary format in FILE_reader_t");
			break;
		}
	}
	return n;
}

std::string &FILE_reader_t::read_string()
{
	uint64_t len = read_varuint();
	if (!len) {
		strbuf = "";
		return strbuf;
	}

	strbuf.resize(len);
	size_t read = fread(&strbuf[0], 1, len, file);
	CRAWL_ASSERT(read == len,
		     "failed to read binary data in FILE_reader_t");
	return strbuf;
}

#define READ_TYPE(ty)								\
do {										\
	ty v;									\
	size_t read = fread(&v, 1, sizeof(ty), file);				\
	CRAWL_ASSERT(read == sizeof(ty),					\
		     "failed to read binary data in FILE_reader_t");		\
	return v;								\
} while (0)

uint8_t  FILE_reader_t::read_uint8()  { READ_TYPE(uint8_t);  }
uint16_t FILE_reader_t::read_uint16() { READ_TYPE(uint16_t); }
uint32_t FILE_reader_t::read_uint32() { READ_TYPE(uint32_t); }
uint64_t FILE_reader_t::read_uint64() { READ_TYPE(uint64_t); }
int8_t   FILE_reader_t::read_int8()   { READ_TYPE(int8_t);   }
int16_t  FILE_reader_t::read_int16()  { READ_TYPE(int16_t);  }
int32_t  FILE_reader_t::read_int32()  { READ_TYPE(int32_t);  }
int64_t  FILE_reader_t::read_int64()  { READ_TYPE(int64_t);  }

//------------------------------------------------------------------------------
// FILE_writer_t
//------------------------------------------------------------------------------

FILE_writer_t::FILE_writer_t(FILE *f): file(f)
{
}

void FILE_writer_t::write_varuint(uint64_t n)
{
	if (n < 128) {
		write_uint8(n);
	} else {
		if (n < 65536) {
			write_uint8(VARINT_U16 | VARINT_BIT);
			write_uint16(n);
		} else if (n < 4294967296) {
			write_uint8(VARINT_U32 | VARINT_BIT);
			write_uint32(n);
		} else {
			write_uint8(VARINT_U64 | VARINT_BIT);
			write_uint64(n);
		}
	}
}

void FILE_writer_t::write_string(const char *str)
{
	write_string(str, strlen(str));
}

void FILE_writer_t::write_string(const char *str, size_t len)
{
	write_varuint(len);
	size_t written = fwrite(str, 1, len, file);
	CRAWL_ASSERT(written == len,
		     "failed to write binary data in FILE_writer_t");
}

void FILE_writer_t::write_string(const std::string &cppstr)
{
	write_string(cppstr.c_str(), cppstr.size());
}

#define WRITE_TYPE(ty)								\
do {										\
	size_t written = fwrite(&n, 1, sizeof(ty), file);			\
	CRAWL_ASSERT(written == sizeof(ty),					\
		     "failed to write binary data in FILE_writer_t");		\
} while (0)

void FILE_writer_t::write_uint8(uint8_t n)   { WRITE_TYPE(uint8_t);  }
void FILE_writer_t::write_uint16(uint16_t n) { WRITE_TYPE(uint16_t); }
void FILE_writer_t::write_uint32(uint32_t n) { WRITE_TYPE(uint32_t); }
void FILE_writer_t::write_uint64(uint64_t n) { WRITE_TYPE(uint64_t); }
void FILE_writer_t::write_int8(int8_t n)     { WRITE_TYPE(int8_t);   }
void FILE_writer_t::write_int16(int16_t n)   { WRITE_TYPE(int16_t);  }
void FILE_writer_t::write_int32(int32_t n)   { WRITE_TYPE(int32_t);  }
void FILE_writer_t::write_int64(int64_t n)   { WRITE_TYPE(int64_t);  }

//------------------------------------------------------------------------------
// Brawl semantic types
//------------------------------------------------------------------------------

brawl_stypes_t::brawl_stypes_t(brawl_context_t *ctx):
	ctx(ctx), index(0)
{
}

void brawl_stypes_t::clear()
{
	// clean up
	stypes.clear();
	map.clear();
	index = 0;
}

void brawl_stypes_t::serialize_named(FILE_writer_t *cout, named_stype_t *t)
{
	cout->write_string(t->name);
	cout->write_int32(stype_index(t->real));
}

void brawl_stypes_t::serialize_pointer(FILE_writer_t *cout, pointer_stype_t *t)
{
	cout->write_int32(stype_index(t->points_to));
}

void brawl_stypes_t::serialize_array(FILE_writer_t *cout, array_stype_t *t)
{
	cout->write_varuint(t->size);
	cout->write_int32(stype_index(t->elem));
}

void brawl_stypes_t::serialize_struct(FILE_writer_t *cout, struct_stype_t *t)
{
	cout->write_varuint(t->alignment);
	cout->write_varuint(t->size);
	cout->write_varuint(t->fields.size());
	for (size_t i = 0; i < t->fields.size(); ++i) {
		struct_field_t *f = &t->fields[i];
		cout->write_string(f->name);
		cout->write_int32(stype_index(f->type));
		cout->write_varuint(f->padding);
	}
}

void brawl_stypes_t::serialize_func(FILE_writer_t *cout, func_stype_t *t)
{
	cout->write_uint8(t->varargs ? 1 : 0);

	cout->write_varuint(t->args.size());
	for (size_t i = 0; i < t->args.size(); ++i)
		cout->write_int32(stype_index(t->args[i]));

	cout->write_varuint(t->results.size());
	for (size_t i = 0; i < t->results.size(); ++i)
		cout->write_int32(stype_index(t->results[i]));
}

struct stype_serializer_t : stype_visitor_t {
	brawl_stypes_t *ctx;
	bool skip_first;

	stype_visitor_t *visit(stype_t *t)
	{
		if (skip_first) {
			skip_first = false;
			return this;
		}

		if (IS_STYPE_BUILTIN(t))
			return 0;

		if (ctx->map.find(t) != ctx->map.end())
			return 0;

		ctx->map[t] = ctx->index++;
		ctx->stypes.push_back(t);
		return this;
	}
};

int32_t brawl_stypes_t::queue_for_serialization(stype_t *t)
{
	// shortcut for built-ins
	if (IS_STYPE_BUILTIN(t))
		return builtin_stype_index(t);

	// if the type is already here, get it
	stype_map_t::iterator it = map.find(t);
	if (it != map.end())
		return it->second;

	// otherwise queue it
	int32_t curidx = index++;
	map[t] = curidx;
	stypes.push_back(t);

	// queue children
	stype_serializer_t s;
	s.ctx = this;
	s.skip_first = true;
	s.traverse(t);

	return curidx;
}

int32_t brawl_stypes_t::stype_index(stype_t *t)
{
	if (IS_STYPE_BUILTIN(t))
		return builtin_stype_index(t);

	stype_map_t::iterator it = map.find(t);
	CRAWL_QASSERT(it != map.end());
	return it->second;
}

int32_t brawl_stypes_t::builtin_stype_index(stype_t *t)
{
	if (IS_STYPE_BUILTIN(t)) {
		for (int i = 0; i < BUILTIN_N; ++i) {
			if (builtin_named_stypes[i] == t)
				return (-1 - i);
		}
		// in case if this is an abstract type (consts)
		for (int i = 0; i < BUILTIN_N; ++i) {
			if (builtin_stypes[i] == t)
				return (-1 - i);
		}
	}
	CRAWL_QASSERT(!"not a built-in type in brawl_stypes_t::builtin_stype_index");
	return 0;
}

stype_t *brawl_stypes_t::index_stype(int32_t idx)
{
	if (idx < 0) {
		stype_t *t = builtin_named_stypes[-idx - 1];
		if (t)
			return t;
		return builtin_stypes[-idx - 1];
	}
	return stypes[idx];
}

void brawl_stypes_t::save(FILE_writer_t *cout)
{
	cout->write_uint32(stypes.size());
	for (size_t i = 0, n = stypes.size(); i < n; ++i) {
		stype_t *t = stypes[i];

		// write type
		cout->write_uint16(t->type);
		if (IS_STYPE_NAMED(t))
			serialize_named(cout, (named_stype_t*)t);
		else if (IS_STYPE_POINTER(t))
			serialize_pointer(cout, (pointer_stype_t*)t);
		else if (IS_STYPE_STRUCT(t))
			serialize_struct(cout, (struct_stype_t*)t);
		else if (IS_STYPE_ARRAY(t))
			serialize_array(cout, (array_stype_t*)t);
		else if (IS_STYPE_FUNC(t))
			serialize_func(cout, (func_stype_t*)t);
		else
			CRAWL_QASSERT(!"unreachable");
	}

	clear();
}

#define READ_MSG(msg)							\
	uint32_t size;							\
	bool result = cin->ReadVarint32(&size);				\
	if (!result) DIE("failed to read Varint32");			\
	CodedInputStream::Limit l = cin->PushLimit(size);		\
	result = msg.ParseFromCodedStream(cin);				\
	if (!result) DIE("failed to parse " #msg);			\
	cin->PopLimit(l)

void brawl_stypes_t::deserialize_named(FILE_reader_t *cin)
{
	std::string &name = cin->read_string();
	int32_t real = cin->read_int32();

	std::string fullname = prefix + "." + name;
	unordered_map<std::string, named_stype_t*>::iterator it;
	it = ctx->named_map.find(fullname);

	named_stype_t *t;
	if (it != ctx->named_map.end()) {
		t = it->second;
	} else {
		t = new named_stype_t;
		t->name = name;
		t->real = (stype_t*)real;
		ctx->named_map[fullname] = t;
		ctx->ttracker->push_back(t);
	}
	stypes.push_back(t);
	/*
	printf("named type :: name: %s, real: %d\n",
	       pb_namedtype.name().c_str(), pb_namedtype.real());
	*/
}

void brawl_stypes_t::deserialize_pointer(FILE_reader_t *cin)
{
	pointer_stype_t *t = new pointer_stype_t;
	t->points_to = (stype_t*)cin->read_int32();
	ctx->ttracker->push_back(t);
	stypes.push_back(t);
	/*
	printf("pointer type :: points to: %d\n", pb_pointertype.points_to());
	*/
}

void brawl_stypes_t::deserialize_array(FILE_reader_t *cin)
{
	array_stype_t *t = new array_stype_t;
	t->size = cin->read_varuint();
	t->elem = (stype_t*)cin->read_int32();
	ctx->ttracker->push_back(t);
	stypes.push_back(t);
	/*
	printf("array type :: size: %llu, elem: %d\n",
	       pb_arraytype.size(), pb_arraytype.elem());
	*/
}

void brawl_stypes_t::deserialize_struct(FILE_reader_t *cin)
{
	struct_stype_t *t = new struct_stype_t;
	t->alignment = cin->read_varuint();
	t->size = cin->read_varuint();

	size_t fields_n = cin->read_varuint();
	t->fields.reserve(fields_n);
	for (size_t i = 0, n = fields_n; i < n; ++i) {
		std::string &name = cin->read_string();
		int32_t type = cin->read_int32();
		uint64_t padding = cin->read_varuint();

		struct_field_t field = {(stype_t*)type, name, padding};
		t->fields.push_back(field);
	}
	ctx->ttracker->push_back(t);
	stypes.push_back(t);
	/*
	printf("struct type :: alignment: %u, size: %u, fields:\n",
	       pb_structtype.alignment(), pb_structtype.size());
	for (size_t i = 0, n = pb_structtype.field_size(); i < n; ++i) {
		const StructType_Field &f = pb_structtype.field(i);
		printf("\tfield :: name: %s, type: %d, padding: %u\n",
		       f.name().c_str(), f.type(), f.padding());
	}
	*/
}

void brawl_stypes_t::deserialize_func(FILE_reader_t *cin)
{
	func_stype_t *t = new func_stype_t;
	t->varargs = cin->read_uint8() == 1 ? true : false;

	size_t args_n = cin->read_varuint();
	t->args.reserve(args_n);
	for (size_t i = 0, n = args_n; i < n; ++i)
		t->args.push_back((stype_t*)cin->read_int32());

	size_t results_n = cin->read_varuint();
	t->results.reserve(results_n);
	for (size_t i = 0, n = results_n; i < n; ++i)
		t->results.push_back((stype_t*)cin->read_int32());
	ctx->ttracker->push_back(t);
	stypes.push_back(t);
	/*
	printf("func type :: varargs: %d, args:\n", pb_functype.varargs());
	for (size_t i = 0, n = pb_functype.arg_size(); i < n; ++i)
		printf("\targ :: type: %d\n", pb_functype.arg(i));
	printf("results:\n");
	for (size_t i = 0, n = pb_functype.result_size(); i < n; ++i)
		printf("\tresult :: type: %d\n", pb_functype.result(i));
	*/
}

void brawl_stypes_t::deserialize_types(FILE_reader_t *cin, size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		uint16_t type = cin->read_uint16();
		//printf("%d-------------------------------------\n", i);

		if (type & STYPE_NAMED)
			deserialize_named(cin);
		else if (type & STYPE_POINTER)
			deserialize_pointer(cin);
		else if (type & STYPE_ARRAY)
			deserialize_array(cin);
		else if (type & STYPE_STRUCT)
			deserialize_struct(cin);
		else if (type & STYPE_FUNC)
			deserialize_func(cin);
		else
			CRAWL_QASSERT(!"bad type");

		stypes.back()->type = type;
	}
}

void brawl_stypes_t::restore_pointers()
{
	// restore pointers
	for (size_t i = 0, n = stypes.size(); i < n; ++i) {
		stype_t *st = stypes[i];
		if (IS_STYPE_NAMED(st)) {
			named_stype_t *t = (named_stype_t*)st;
			if (!t->restored) {
				t->real = (index_stype((int64_t)t->real));
				t->restored = true;
			}
		} else if (IS_STYPE_POINTER(st)) {
			pointer_stype_t *t = (pointer_stype_t*)st;
			t->points_to = index_stype((int64_t)t->points_to);
		} else if (IS_STYPE_ARRAY(st)) {
			array_stype_t *t = (array_stype_t*)st;
			t->elem = index_stype((int64_t)t->elem);
		} else if (IS_STYPE_STRUCT(st)) {
			struct_stype_t *t = (struct_stype_t*)st;
			for (size_t i = 0, n = t->fields.size(); i < n; ++i) {
				struct_field_t *f = &t->fields[i];
				f->type = index_stype((int64_t)f->type);
			}
		} else if (IS_STYPE_FUNC(st)) {
			func_stype_t *t = (func_stype_t*)st;
			for (size_t i = 0, n = t->args.size(); i < n; ++i)
				t->args[i] = index_stype((int64_t)t->args[i]);
			for (size_t i = 0, n = t->results.size(); i < n; ++i)
				t->results[i] = index_stype((int64_t)t->results[i]);
		}
	}
}

void brawl_stypes_t::load(FILE_reader_t *cin)
{
	CRAWL_QASSERT(ctx != 0);
	clear();

	uint32_t num = cin->read_uint32();
	deserialize_types(cin, num);
	restore_pointers();
}

//------------------------------------------------------------------------------
// Brawl semantic decls
//------------------------------------------------------------------------------

brawl_sdecls_t::brawl_sdecls_t(brawl_context_t *ctx):
	ctx(ctx), btypes(ctx)
{
}

void brawl_sdecls_t::clear()
{
	btypes.clear();
	sdecls.clear();
}

void brawl_sdecls_t::queue_for_serialization(sdecl_t *d)
{
	sdecls.push_back(d);
}

void brawl_sdecls_t::save(FILE_writer_t *cout)
{
	cout->write_uint32(sdecls.size());
	for (size_t i = 0, n = sdecls.size(); i < n; ++i) {
		sdecl_t *d = sdecls[i];

		// skip imports
		if (d->type == SDECL_IMPORT)
			continue;

		cout->write_uint8(d->type);
		cout->write_string(d->name);
		cout->write_int32(btypes.queue_for_serialization(d->stype));
		if (d->type == SDECL_CONST) {
			const_sdecl_t *cd = (const_sdecl_t*)d;
			cout->write_uint8(cd->value.type);
			cout->write_string(cd->value.to_string());
		}
	}

	btypes.save(cout);
	clear();
}

void brawl_sdecls_t::deserialize_decls(FILE_reader_t *cin, size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		uint8_t dtype = cin->read_uint8();
		std::string &name = cin->read_string();
		int32_t type = cin->read_int32();

		sdecl_t *d = new_sdecl(ctx->dtracker, name.c_str(),
				       (sdecl_type_t)dtype);
		d->stype = (stype_t*)type;
		if (dtype == SDECL_CONST) {
			uint8_t vtype = cin->read_uint8();
			std::string &value = cin->read_string();

			const_sdecl_t *cd = (const_sdecl_t*)d;
			cd->value = value_t(value.c_str(), (value_type_t)vtype);
		}
		sdecls.push_back(d);
	}
}

void brawl_sdecls_t::restore_pointers()
{
	for (size_t i = 0, n = sdecls.size(); i < n; ++i)
		sdecls[i]->stype = btypes.index_stype(((int64_t)sdecls[i]->stype));
}

void brawl_sdecls_t::load(FILE_reader_t *cin)
{
	CRAWL_QASSERT(ctx != 0);
	clear();
	uint32_t num = cin->read_uint32();
	deserialize_decls(cin, num);
	btypes.load(cin);
	restore_pointers();
}

//------------------------------------------------------------------------------
// Brawl module
//------------------------------------------------------------------------------

brawl_module_t::brawl_module_t(brawl_context_t *ctx):
	bdecls(ctx)
{
}

void brawl_module_t::clear()
{
	bdecls.clear();
	prefix.clear();
	package.clear();
	decls.clear();
}

void brawl_module_t::save(FILE_writer_t *cout)
{
	cout->write_string(prefix);
	cout->write_string(package);
	bdecls.save(cout);
	clear();
}

void brawl_module_t::load(FILE_reader_t *cin)
{
	clear();
	prefix = bdecls.btypes.prefix = cin->read_string();
	package = cin->read_string();
	bdecls.load(cin);
	for (size_t i = 0, n = bdecls.sdecls.size(); i < n; ++i) {
		sdecl_t *d = bdecls.sdecls[i];
		decls[d->name] = d;
	}
	bdecls.clear();
}

void brawl_module_t::queue_for_serialization(scope_block_t *pkgscope,
					     std::vector<const char*> *declnames,
					     const char *prefix, const char *package)
{
	this->prefix = prefix;
	this->package = package;
	for (size_t i = 0, n = declnames->size(); i < n; ++i) {
		sdecl_t *sd = pkgscope->sdecls[declnames->at(i)];
		bdecls.queue_for_serialization(sd);
	}
}
