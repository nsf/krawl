#include "crawlc.hpp"

static void dump_to_cout(google::protobuf::Message *msg,
			 CodedOutputStream *cout)
{
	int size = msg->ByteSize();
	cout->WriteVarint32(size);
	msg->SerializeWithCachedSizes(cout);
}

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

	pb_typeheader.Clear();
	pb_namedtype.Clear();
	pb_pointertype.Clear();
	pb_arraytype.Clear();
	pb_structtype.Clear();
	pb_functype.Clear();
}

void brawl_stypes_t::serialize_named(CodedOutputStream *cout, named_stype_t *t)
{
	pb_namedtype.set_name(t->name);
	pb_namedtype.set_real(stype_index(t->real));
	dump_to_cout(&pb_namedtype, cout);
}

void brawl_stypes_t::serialize_pointer(CodedOutputStream *cout, pointer_stype_t *t)
{
	pb_pointertype.set_points_to(stype_index(t->points_to));
	dump_to_cout(&pb_pointertype, cout);
}

void brawl_stypes_t::serialize_array(CodedOutputStream *cout, array_stype_t *t)
{
	pb_arraytype.set_size(t->size);
	pb_arraytype.set_elem(stype_index(t->elem));
	dump_to_cout(&pb_arraytype, cout);
}

void brawl_stypes_t::serialize_struct(CodedOutputStream *cout, struct_stype_t *t)
{
	pb_structtype.set_alignment(t->alignment);
	pb_structtype.set_size(t->size);
	pb_structtype.clear_field();
	for (size_t i = 0; i < t->fields.size(); ++i) {
		Brawl::StructType_Field *msgf = pb_structtype.add_field();
		struct_field_t *f = &t->fields[i];
		msgf->set_name(f->name);
		msgf->set_type(stype_index(f->type));
		msgf->set_padding(f->padding);
	}

	dump_to_cout(&pb_structtype, cout);
}

void brawl_stypes_t::serialize_func(CodedOutputStream *cout, func_stype_t *t)
{
	pb_functype.set_varargs(t->varargs);
	pb_functype.clear_arg();
	for (size_t i = 0; i < t->args.size(); ++i)
		pb_functype.add_arg(stype_index(t->args[i]));
	pb_functype.clear_result();
	for (size_t i = 0; i < t->results.size(); ++i)
		pb_functype.add_result(stype_index(t->results[i]));

	dump_to_cout(&pb_functype, cout);
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

void brawl_stypes_t::save(CodedOutputStream *cout)
{
	cout->WriteLittleEndian32(stypes.size());
	for (size_t i = 0, n = stypes.size(); i < n; ++i) {
		stype_t *t = stypes[i];

		// write type
		pb_typeheader.set_type(t->type);
		dump_to_cout(&pb_typeheader, cout);

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

void brawl_stypes_t::deserialize_named(CodedInputStream *cin)
{
	READ_MSG(pb_namedtype);

	std::string fullname = prefix + "." + pb_namedtype.name();
	unordered_map<std::string, named_stype_t*>::iterator it;
	it = ctx->named_map.find(fullname);

	named_stype_t *t;
	if (it != ctx->named_map.end()) {
		t = it->second;
	} else {
		t = new named_stype_t;
		t->name = pb_namedtype.name();
		t->real = (stype_t*)pb_namedtype.real();
		ctx->named_map[fullname] = t;
		ctx->ttracker->push_back(t);
	}
	stypes.push_back(t);
	/*
	printf("named type :: name: %s, real: %d\n",
	       pb_namedtype.name().c_str(), pb_namedtype.real());
	*/
}

void brawl_stypes_t::deserialize_pointer(CodedInputStream *cin)
{
	READ_MSG(pb_pointertype);
	pointer_stype_t *t = new pointer_stype_t;
	t->points_to = (stype_t*)pb_pointertype.points_to();
	ctx->ttracker->push_back(t);
	stypes.push_back(t);
	/*
	printf("pointer type :: points to: %d\n", pb_pointertype.points_to());
	*/
}

void brawl_stypes_t::deserialize_array(CodedInputStream *cin)
{
	READ_MSG(pb_arraytype);
	array_stype_t *t = new array_stype_t;
	t->size = pb_arraytype.size();
	t->elem = (stype_t*)pb_arraytype.elem();
	ctx->ttracker->push_back(t);
	stypes.push_back(t);
	/*
	printf("array type :: size: %llu, elem: %d\n",
	       pb_arraytype.size(), pb_arraytype.elem());
	*/
}

void brawl_stypes_t::deserialize_struct(CodedInputStream *cin)
{
	READ_MSG(pb_structtype);
	struct_stype_t *t = new struct_stype_t;
	t->alignment = pb_structtype.alignment();
	t->size = pb_structtype.size();
	t->fields.reserve(pb_structtype.field_size());
	for (size_t i = 0, n = pb_structtype.field_size(); i < n; ++i) {
		const Brawl::StructType_Field &f = pb_structtype.field(i);
		struct_field_t field = {(stype_t*)f.type(), f.name(), f.padding()};
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

void brawl_stypes_t::deserialize_func(CodedInputStream *cin)
{
	READ_MSG(pb_functype);
	func_stype_t *t = new func_stype_t;
	t->varargs = pb_functype.varargs();
	t->args.reserve(pb_functype.arg_size());
	t->results.reserve(pb_functype.result_size());
	for (size_t i = 0, n = pb_functype.arg_size(); i < n; ++i)
		t->args.push_back((stype_t*)pb_functype.arg(i));
	for (size_t i = 0, n = pb_functype.result_size(); i < n; ++i)
		t->results.push_back((stype_t*)pb_functype.result(i));
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

void brawl_stypes_t::deserialize_types(CodedInputStream *cin, size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		READ_MSG(pb_typeheader);
		//printf("%d-------------------------------------\n", i);

		if (pb_typeheader.type() & STYPE_NAMED)
			deserialize_named(cin);
		else if (pb_typeheader.type() & STYPE_POINTER)
			deserialize_pointer(cin);
		else if (pb_typeheader.type() & STYPE_ARRAY)
			deserialize_array(cin);
		else if (pb_typeheader.type() & STYPE_STRUCT)
			deserialize_struct(cin);
		else if (pb_typeheader.type() & STYPE_FUNC)
			deserialize_func(cin);
		else
			CRAWL_QASSERT(!"bad type");

		stypes.back()->type = pb_typeheader.type();
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

void brawl_stypes_t::load(CodedInputStream *cin)
{
	CRAWL_QASSERT(ctx != 0);
	clear();
	uint32_t num;
	cin->ReadLittleEndian32(&num);
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
	pb_decl.Clear();
}

void brawl_sdecls_t::queue_for_serialization(sdecl_t *d)
{
	sdecls.push_back(d);
}

void brawl_sdecls_t::save(CodedOutputStream *cout)
{
	cout->WriteLittleEndian32(sdecls.size());
	for (size_t i = 0, n = sdecls.size(); i < n; ++i) {
		sdecl_t *d = sdecls[i];

		// skip imports
		if (d->type == SDECL_IMPORT)
			continue;

		pb_decl.set_decltype(d->type);
		pb_decl.set_name(d->name.c_str());
		pb_decl.set_type(btypes.queue_for_serialization(d->stype));
		if (d->type == SDECL_CONST) {
			const_sdecl_t *cd = (const_sdecl_t*)d;
			pb_decl.set_valuetype(cd->value.type);
			pb_decl.set_value(cd->value.to_string());
		}
		dump_to_cout(&pb_decl, cout);
		pb_decl.Clear();
	}

	btypes.save(cout);
	clear();
}

void brawl_sdecls_t::deserialize_decls(CodedInputStream *cin, size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		READ_MSG(pb_decl);
		sdecl_t *d = new_sdecl(ctx->dtracker,
				       pb_decl.name().c_str(),
				       (sdecl_type_t)pb_decl.decltype());
		d->stype = (stype_t*)pb_decl.type();
		if (pb_decl.has_value()) {
			CRAWL_QASSERT(d->type == SDECL_CONST);
			const_sdecl_t *cd = (const_sdecl_t*)d;
			cd->value = value_t(pb_decl.value().c_str(),
					    (value_type_t)pb_decl.valuetype());
		}
		sdecls.push_back(d);
	}
}

void brawl_sdecls_t::restore_pointers()
{
	for (size_t i = 0, n = sdecls.size(); i < n; ++i)
		sdecls[i]->stype = btypes.index_stype(((int64_t)sdecls[i]->stype));
}

void brawl_sdecls_t::load(CodedInputStream *cin)
{
	CRAWL_QASSERT(ctx != 0);
	clear();
	uint32_t num;
	cin->ReadLittleEndian32(&num);
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

	pb_module.Clear();
}

void brawl_module_t::save(CodedOutputStream *cout)
{
	pb_module.set_prefix(prefix);
	pb_module.set_package(package);
	dump_to_cout(&pb_module, cout);
	bdecls.save(cout);
	clear();
}

void brawl_module_t::load(CodedInputStream *cin)
{
	clear();
	READ_MSG(pb_module);
	prefix = bdecls.btypes.prefix = pb_module.prefix();
	package = pb_module.package();
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
