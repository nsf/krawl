#include "krawl.hpp"

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
			KRAWL_ASSERT(false, "invalid binary format in FILE_reader_t");
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
	KRAWL_ASSERT(read == len,
		     "failed to read binary data in FILE_reader_t");
	return strbuf;
}

#define READ_TYPE(ty)								\
do {										\
	ty v;									\
	size_t read = fread(&v, 1, sizeof(ty), file);				\
	KRAWL_ASSERT(read == sizeof(ty),					\
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
	KRAWL_ASSERT(written == len,
		     "failed to write binary data in FILE_writer_t");
}

void FILE_writer_t::write_string(const std::string &cppstr)
{
	write_string(cppstr.c_str(), cppstr.size());
}

#define WRITE_TYPE(ty)								\
do {										\
	size_t written = fwrite(&n, 1, sizeof(ty), file);			\
	KRAWL_ASSERT(written == sizeof(ty),					\
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
// Brawl serializer
//------------------------------------------------------------------------------

int32_t brawl_serializer_t::stype_index(stype_t *t)
{
	if (IS_STYPE_BUILTIN(t))
		return builtin_stype_index(t);

	stype_map_t::iterator it = stype_map.find(t);
	KRAWL_QASSERT(it != stype_map.end());
	return it->second;
}

int32_t brawl_serializer_t::builtin_stype_index(stype_t *t)
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
	KRAWL_QASSERT(!"not a built-in type in brawl_stypes_t::builtin_stype_index");
	return 0;
}

void brawl_serializer_t::serialize_named(FILE_writer_t *cout, named_stype_t *t)
{
	cout->write_string(t->name);
	cout->write_int32(stype_index(t->real));
}

void brawl_serializer_t::serialize_pointer(FILE_writer_t *cout, pointer_stype_t *t)
{
	cout->write_int32(stype_index(t->points_to));
}

void brawl_serializer_t::serialize_array(FILE_writer_t *cout, array_stype_t *t)
{
	cout->write_varuint(t->size);
	cout->write_int32(stype_index(t->elem));
}

void brawl_serializer_t::serialize_struct(FILE_writer_t *cout, struct_stype_t *t)
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

void brawl_serializer_t::serialize_func(FILE_writer_t *cout, func_stype_t *t)
{
	cout->write_uint8(t->varargs ? 1 : 0);

	cout->write_varuint(t->args.size());
	for (size_t i = 0; i < t->args.size(); ++i)
		cout->write_int32(stype_index(t->args[i]));

	cout->write_varuint(t->results.size());
	for (size_t i = 0; i < t->results.size(); ++i)
		cout->write_int32(stype_index(t->results[i]));
}

struct stype_queuer_t : stype_visitor_t {
	brawl_serializer_t *ctx;
	bool skip_first;

	stype_visitor_t *visit(stype_t *t)
	{
		if (skip_first) {
			skip_first = false;
			return this;
		}

		if (IS_STYPE_BUILTIN(t))
			return 0;

		if (ctx->stype_map.find(t) != ctx->stype_map.end())
			return 0;

		ctx->stype_map[t] = ctx->stypes.size();
		ctx->stypes.push_back(t);
		return this;
	}
};

int32_t brawl_serializer_t::queue_stype(stype_t *t)
{
	// shortcut for built-ins
	if (IS_STYPE_BUILTIN(t))
		return builtin_stype_index(t);

	// if the type is already here, get it
	stype_map_t::iterator it = stype_map.find(t);
	if (it != stype_map.end())
		return it->second;

	// otherwise queue it
	int32_t curidx = stypes.size();
	stype_map[t] = curidx;
	stypes.push_back(t);

	// queue children
	stype_queuer_t s;
	s.ctx = this;
	s.skip_first = true;
	s.traverse(t);

	return curidx;
}

void brawl_serializer_t::serialize(FILE_writer_t *cout,
				   std::vector<sdecl_t*> *pkgdecls,
				   const char *prefix, const char *package)
{
	// set active prefix
	this->cur_prefix = prefix;

	// module header
	cout->write_string(prefix);
	cout->write_string(package);

	// sdecls
	cout->write_uint32(pkgdecls->size());
	for (size_t i = 0, n = pkgdecls->size(); i < n; ++i) {
		sdecl_t *d = pkgdecls->at(i);

		// skip imports
		if (d->type == SDECL_IMPORT)
			continue;

		cout->write_uint8(d->type);
		cout->write_string(d->name);
		cout->write_int32(queue_stype(d->stype)); // queueing stypes
		if (d->type == SDECL_CONST) {
			const_sdecl_t *cd = (const_sdecl_t*)d;
			cout->write_uint8(cd->value.type);
			cout->write_string(cd->value.to_string());
		}
	}

	// stypes
	cout->write_uint32(stypes.size());
	for (size_t i = 0, n = stypes.size(); i < n; ++i) {
		stype_t *t = stypes[i];

		// hack to save prefixes, sort of like run-length encoding,
		// should provide more or less good compression
		if (IS_STYPE_NAMED(t)) {
			const char *tprefix = t->as_named()->prefix;
			if (cur_prefix != tprefix) {
				// let's introduce a new prefix first
				cout->write_uint16(0xFFFF);
				cout->write_string(tprefix);
				cur_prefix = tprefix;
			}
		}

		// write type
		cout->write_uint16(t->type);
		if (IS_STYPE_NAMED(t))
			serialize_named(cout, t->as_named()); // queueing prefixes
		else if (IS_STYPE_POINTER(t))
			serialize_pointer(cout, t->as_pointer());
		else if (IS_STYPE_STRUCT_OR_UNION(t))
			serialize_struct(cout, t->as_struct());
		else if (IS_STYPE_ARRAY(t))
			serialize_array(cout, t->as_array());
		else if (IS_STYPE_FUNC(t))
			serialize_func(cout, t->as_func());
		else
			KRAWL_QASSERT(!"unreachable");
	}
}

//------------------------------------------------------------------------------
// Brawl deserializer
//------------------------------------------------------------------------------

brawl_deserializer_t::brawl_deserializer_t(import_context_t *ctx): ctx(ctx)
{
}

stype_t *brawl_deserializer_t::index_stype(int32_t idx)
{
	if (idx < 0) {
		stype_t *t = builtin_named_stypes[-idx - 1];
		if (t)
			return t;
		return builtin_stypes[-idx - 1];
	}
	return stypes[idx];
}

const char *brawl_deserializer_t::ctx_prefix(const char *p)
{
	unordered_set<std::string>::iterator it = ctx->prefixes.find(p);
	if (it != ctx->prefixes.end())
		return it->c_str();

	return ctx->prefixes.insert(p).first->c_str();
}

void brawl_deserializer_t::deserialize_named(FILE_reader_t *cin)
{
	std::string &name = cin->read_string();
	int32_t real = cin->read_int32();

	std::string fullname = cur_prefix + "." + name;
	unordered_map<std::string, named_stype_t*>::iterator it;
	it = ctx->named_map.find(fullname);

	named_stype_t *t;
	if (it != ctx->named_map.end()) {
		t = it->second;
	} else {
		t = new named_stype_t;
		t->prefix = ctx_prefix(cur_prefix.c_str());
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

void brawl_deserializer_t::deserialize_pointer(FILE_reader_t *cin)
{
	pointer_stype_t *t = new pointer_stype_t;
	t->points_to = (stype_t*)cin->read_int32();
	ctx->ttracker->push_back(t);
	stypes.push_back(t);
	/*
	printf("pointer type :: points to: %d\n", pb_pointertype.points_to());
	*/
}

void brawl_deserializer_t::deserialize_array(FILE_reader_t *cin)
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

void brawl_deserializer_t::deserialize_struct(FILE_reader_t *cin)
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

void brawl_deserializer_t::deserialize_func(FILE_reader_t *cin)
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

void brawl_deserializer_t::deserialize_stypes(FILE_reader_t *cin, size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		uint16_t type = cin->read_uint16();
		if (type == 0xFFFF) {
			cur_prefix = cin->read_string();
			type = cin->read_uint16();
		}
		//printf("%d-------------------------------------\n", i);

		if (type & STYPE_NAMED)
			deserialize_named(cin);
		else if (type & STYPE_POINTER)
			deserialize_pointer(cin);
		else if (type & STYPE_ARRAY)
			deserialize_array(cin);
		else if (type & (STYPE_STRUCT | STYPE_UNION))
			deserialize_struct(cin);
		else if (type & STYPE_FUNC)
			deserialize_func(cin);
		else
			KRAWL_QASSERT(!"bad type");

		stypes.back()->type = type;
	}
}

void brawl_deserializer_t::restore_stype_pointers()
{
	for (size_t i = 0, n = stypes.size(); i < n; ++i) {
		stype_t *st = stypes[i];
		if (IS_STYPE_NAMED(st)) {
			named_stype_t *t = st->as_named();
			if (!t->restored) {
				t->real = index_stype((int64_t)t->real);
				t->restored = true;
			}
		} else if (IS_STYPE_POINTER(st)) {
			pointer_stype_t *t = st->as_pointer();
			t->points_to = index_stype((int64_t)t->points_to);
		} else if (IS_STYPE_ARRAY(st)) {
			array_stype_t *t = st->as_array();
			t->elem = index_stype((int64_t)t->elem);
		} else if (IS_STYPE_STRUCT_OR_UNION(st)) {
			struct_stype_t *t = st->as_struct();
			for (size_t i = 0, n = t->fields.size(); i < n; ++i) {
				struct_field_t *f = &t->fields[i];
				f->type = index_stype((int64_t)f->type);
			}
		} else if (IS_STYPE_FUNC(st)) {
			func_stype_t *t = st->as_func();
			for (size_t i = 0, n = t->args.size(); i < n; ++i)
				t->args[i] = index_stype((int64_t)t->args[i]);
			for (size_t i = 0, n = t->results.size(); i < n; ++i)
				t->results[i] = index_stype((int64_t)t->results[i]);
		}
	}
}

void brawl_deserializer_t::deserialize_sdecls(FILE_reader_t *cin, size_t n)
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

void brawl_deserializer_t::restore_sdecl_pointers()
{
	// casting from pointer to int64_t is required for amd64, because in C
	// it's ok to truncate implicitly and integer, but it cannot be done
	// while converting a pointer to an integer (weird shit)
	for (size_t i = 0, n = sdecls.size(); i < n; ++i)
		sdecls[i]->stype = index_stype(((int64_t)sdecls[i]->stype));
}

void brawl_deserializer_t::deserialize(FILE_reader_t *cin)
{
	uint32_t num;

	// module header
	prefix = cur_prefix = cin->read_string();
	package = cin->read_string();

	// sdecls
	num = cin->read_uint32();
	deserialize_sdecls(cin, num);

	// stypes
	num = cin->read_uint32();
	deserialize_stypes(cin, num);

	// restore pointers
	restore_stype_pointers();
	restore_sdecl_pointers();
}
