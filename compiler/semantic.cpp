#include "krawl.hpp"
#include "cityhash/city.h"
#include <algorithm>




//==============================================================================
// SEMANTIC TYPES
//==============================================================================

//------------------------------------------------------------------------------
// stype_t
//------------------------------------------------------------------------------

stype_t::stype_t(unsigned int t): type(t), llvmtype(0) {}
stype_t::~stype_t() {}
stype_t *stype_t::end_type() { return this; }
int stype_t::bits() { return 0; }
std::string stype_t::to_string() { return ""; }

// this one is special, don't do end_type here
named_stype_t *stype_t::as_named()
{
	KRAWL_QASSERT(IS_STYPE_NAMED(this));
	return (named_stype_t*)this;
}

#define TCONVERT(ty, TY)				\
ty##_stype_t *stype_t::as_##ty()			\
{							\
	KRAWL_QASSERT(IS_STYPE_##TY(this));		\
	return (ty##_stype_t*)this->end_type();		\
}

TCONVERT(compound, COMPOUND)
TCONVERT(module, MODULE)
TCONVERT(void, VOID)
TCONVERT(bool, BOOL)
TCONVERT(int, INT)
TCONVERT(float, FLOAT)
TCONVERT(string, STRING)
TCONVERT(pointer, POINTER)
TCONVERT(func, FUNC)
TCONVERT(struct, STRUCT_OR_UNION)
TCONVERT(array, ARRAY)

#undef TCONVERT

//------------------------------------------------------------------------------
// compound_stype_t
//------------------------------------------------------------------------------

compound_stype_t::compound_stype_t():
	stype_t(STYPE_ABSTRACT | STYPE_COMPOUND)
{
}

std::string compound_stype_t::to_string()
{
	return "abstract compound";
}

//------------------------------------------------------------------------------
// module_stype_t
//------------------------------------------------------------------------------

module_stype_t::module_stype_t():
	stype_t(STYPE_ABSTRACT | STYPE_MODULE)
{
}

std::string module_stype_t::to_string()
{
	return "abstract module";
}

//------------------------------------------------------------------------------
// void_stype_t
//------------------------------------------------------------------------------

void_stype_t::void_stype_t():
	stype_t(STYPE_VOID)
{
}

int void_stype_t::bits()
{
	return 8;
}

//------------------------------------------------------------------------------
// bool_stype_t
//------------------------------------------------------------------------------

bool_stype_t::bool_stype_t(bool abstract):
	stype_t(STYPE_BOOL)
{
	if (abstract)
		type |= STYPE_ABSTRACT;
}

std::string bool_stype_t::to_string()
{
	if (this == builtin_stypes[BUILTIN_ABSTRACT_BOOL])
		return "abstract bool";
	return "";
}

int bool_stype_t::bits()
{
	return 8;
}

//------------------------------------------------------------------------------
// int_stype_t
//------------------------------------------------------------------------------

int_stype_t::int_stype_t(int s, bool is_signed, bool abstract):
	stype_t(STYPE_INT), size(s), is_signed(is_signed)
{
	if (abstract)
		type |= STYPE_ABSTRACT;
}

std::string int_stype_t::to_string()
{
	if (type & STYPE_ABSTRACT)
		return "abstract int";
	return "";
}

int int_stype_t::bits()
{
	return size;
}

//------------------------------------------------------------------------------
// float_stype_t
//------------------------------------------------------------------------------

float_stype_t::float_stype_t(int s, bool abstract):
	stype_t(STYPE_FLOAT), size(s)
{
	if (abstract)
		type |= STYPE_ABSTRACT;
}

std::string float_stype_t::to_string()
{
	if (type & STYPE_ABSTRACT)
		return "abstract float";
	return "";
}

int float_stype_t::bits()
{
	return size;
}

//------------------------------------------------------------------------------
// string_stype_t
//------------------------------------------------------------------------------

string_stype_t::string_stype_t():
	stype_t(STYPE_ABSTRACT | STYPE_STRING)
{
}

std::string string_stype_t::to_string()
{
	return "abstract string";
}

//------------------------------------------------------------------------------
// named_stype_t
//------------------------------------------------------------------------------

named_stype_t::named_stype_t():
	stype_t(STYPE_NAMED), prefix(""), real(0), decl(0), restored(false)
{
}

named_stype_t::named_stype_t(const char *name, stype_t *real):
	stype_t(STYPE_NAMED), prefix(""), name(name), real(real),
	decl(0), restored(false)
{
	if (real)
		assign_real(real);
}

stype_t *named_stype_t::end_type()
{
	return real->end_type();
}

std::string named_stype_t::to_string()
{
	return name;
}

int named_stype_t::bits()
{
	return real->bits();
}

void named_stype_t::assign_real(stype_t *r)
{
	type = STYPE_NAMED | (r->type & ~STYPE_BUILTIN);
	real = r;
}

//------------------------------------------------------------------------------
// pointer_stype_t
//------------------------------------------------------------------------------

pointer_stype_t::pointer_stype_t():
	stype_t(STYPE_POINTER), points_to(0)
{
}

pointer_stype_t::pointer_stype_t(stype_t *points_to):
	stype_t(STYPE_POINTER), points_to(points_to)
{
}

std::string pointer_stype_t::to_string()
{
	return std::string("*") + points_to->to_string();
}

int pointer_stype_t::bits()
{
	return sizeof(void*) == 8 ? 64 : 32;
}

bool pointer_stype_t::points_to_void()
{
	return points_to->type & STYPE_VOID;
}

//------------------------------------------------------------------------------
// func_stype_t
//------------------------------------------------------------------------------

func_stype_t::func_stype_t():
	stype_t(STYPE_FUNC), varargs(false)
{
}

std::string func_stype_t::to_string()
{
	std::string out = "func(";
	for (size_t i = 0, n = args.size(); i < n; ++i) {
		out += args[i]->to_string();
		if (i != n - 1)
			out += ", ";
	}
	if (varargs)
		out += ", ...";
	out += ")";
	if (results.empty())
		return out;

	if (results.size() == 1) {
		out += " ";
		out += results[0]->to_string();
		return out;
	}

	out += " (";
	for (size_t i = 0, n = results.size(); i < n; ++i) {
		out += results[i]->to_string();
		if (i != n - 1)
			out += ", ";
	}
	out += ")";
	return out;
}

int func_stype_t::bits()
{
	return 32;
}

//------------------------------------------------------------------------------
// struct_stype_t
//------------------------------------------------------------------------------

bool struct_field_t::operator==(const struct_field_t &r)
{
	if (padding != r.padding)
		return false;
	if (name != r.name)
		return false;
	return are_the_same(type, r.type);
}

bool struct_field_t::operator!=(const struct_field_t &r)
{
	return !operator==(r);
}

struct_stype_t::struct_stype_t():
	stype_t(STYPE_STRUCT), u(false), alignment(0), size(0), biggest(0)
{
}

std::string struct_stype_t::to_string()
{
	std::string out;
	if (u)
		out += "union { ";
	else
		out += "struct { ";
	for (size_t i = 0, n = fields.size(); i < n; ++i) {
		struct_field_t *f = &fields[i];
		out += f->name;
		out += " ";
		out += f->type->to_string();
		out += "; ";
	}
	out += "}";
	return out;
}

int struct_stype_t::bits()
{
	return size * 8;
}

bool struct_stype_t::alignment_fixed()
{
	// if there are no fields, we don't need to fix any alignment stuff
	// otherwise the size must be > 0
	return fields.empty() || size > 0;
}

//------------------------------------------------------------------------------
// array_stype_t
//------------------------------------------------------------------------------

array_stype_t::array_stype_t():
	stype_t(STYPE_ARRAY), elem(0), size(0)
{
}

array_stype_t::array_stype_t(stype_t *elem, size_t size):
	stype_t(STYPE_ARRAY), elem(elem), size(size)
{
}

std::string array_stype_t::to_string()
{
	std::string out;
	cppsprintf(&out, "[%u]%s", size, elem->to_string().c_str());
	return out;
}

int array_stype_t::bits()
{
	return size * elem->bits();
}

//------------------------------------------------------------------------------

stype_t *get_int_type(size_t bits)
{
	switch (bits) {
	case 8:
		return builtin_named_stypes[BUILTIN_INT8];
	case 16:
		return builtin_named_stypes[BUILTIN_INT16];
	case 32:
		return builtin_named_stypes[BUILTIN_INT32];
	case 64:
		return builtin_named_stypes[BUILTIN_INT64];
	}

	KRAWL_QASSERT(!"unreachable");
}

size_t stype_hash(stype_t *t)
{
	if (IS_STYPE_BUILTIN(t)) {
		for (size_t i = 0; i < BUILTIN_N; ++i) {
			if (builtin_named_stypes[i] == t)
				return i;
		}
		KRAWL_QASSERT(!"non-named built-in type in stype_hash");
	} else if (IS_STYPE_NAMED(t)) {
		return (size_t)t;
	} else if (IS_STYPE_POINTER(t)) {
		pointer_stype_t *pt = t->as_pointer();
		return 1 + stype_hash(pt->points_to);
	} else if (IS_STYPE_ARRAY(t)) {
		array_stype_t *at = t->as_array();
		return 1 + at->size + stype_hash(at->elem);
	} else if (IS_STYPE_STRUCT_OR_UNION(t)) {
		struct_stype_t *st = t->as_struct();
		size_t h = st->alignment + st->size + st->fields.size() + st->u;
		for (size_t i = 0, n = st->fields.size(); i < n; ++i) {
			h += st->fields[i].padding;
			h += CityHash64(st->fields[i].name.c_str(), st->fields[i].name.length());
			h += stype_hash(st->fields[i].type);
		}
		return h;
	} else if (IS_STYPE_FUNC(t)) {
		func_stype_t *ft = t->as_func();
		size_t h = ft->varargs + ft->args.size() + ft->results.size();
		for (size_t i = 0, n = ft->args.size(); i < n; ++i)
			h += stype_hash(ft->args[i]);
		for (size_t i = 0, n = ft->results.size(); i < n; ++i)
			h += stype_hash(ft->results[i]);
		return h;
	}

	KRAWL_QASSERT(!"unreachable");
	return 0;
}

bool are_the_same(stype_t *t1, stype_t *t2)
{
	// if both types are named, they should come from the same declaration
	// the same is true for abstract types
	if ((IS_STYPE_NAMED(t1) && IS_STYPE_NAMED(t2)) ||
	    (IS_STYPE_ABSTRACT(t1) && IS_STYPE_ABSTRACT(t2)))
		return t1 == t2;

	// if only one of the types is named, they are not the same
	// (it won't trigger 'both' because it was checked just above)
	if (IS_STYPE_NAMED(t1) || IS_STYPE_NAMED(t2))
		return false;

	// at this point type cannot be a named type, check for other cases
	if (IS_STYPE_POINTER(t1) && IS_STYPE_POINTER(t2)) {
		pointer_stype_t *pst1 = t1->as_pointer();
		pointer_stype_t *pst2 = t2->as_pointer();
		return are_the_same(pst1->points_to, pst2->points_to);
	}

	if (IS_STYPE_FUNC(t1) && IS_STYPE_FUNC(t2)) {
		func_stype_t *fst1 = t1->as_func();
		func_stype_t *fst2 = t2->as_func();
		if (fst1->args.size() != fst2->args.size())
			return false;
		if (fst1->results.size() != fst2->results.size())
			return false;

		for (size_t i = 0, n = fst1->args.size(); i < n; ++i) {
			if (!are_the_same(fst1->args[i], fst2->args[i]))
				return false;
		}
		for (size_t i = 0, n = fst1->results.size(); i < n; ++i) {
			if (!are_the_same(fst1->results[i], fst2->results[i]))
				return false;
		}

		if (fst1->varargs != fst2->varargs)
			return false;

		return true;
	}

	if (IS_STYPE_ARRAY(t1) && IS_STYPE_ARRAY(t2)) {
		array_stype_t *ast1 = t1->as_array();
		array_stype_t *ast2 = t2->as_array();

		if (ast1->size != ast2->size)
			return false;

		return are_the_same(ast1->elem, ast2->elem);
	}

	if (IS_STYPE_STRUCT_OR_UNION(t1) && IS_STYPE_STRUCT_OR_UNION(t2)) {
		struct_stype_t *sst1 = t1->as_struct();
		struct_stype_t *sst2 = t2->as_struct();

		if (sst1->size != sst2->size)
			return false;
		if (sst1->alignment != sst2->alignment)
			return false;
		if (sst1->fields.size() != sst2->fields.size())
			return false;
		if (sst1->u != sst2->u)
			return false;

		for (size_t i = 0, n = sst1->fields.size(); i < n; ++i) {
			if (sst1->fields[i] != sst2->fields[i])
				return false;
		}
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------
// NEW!
//-------------------------------------------------------------------------------

stype_t *non_abstract(stype_t *a, stype_t *b)
{
	if (IS_STYPE_ABSTRACT(a))
		return b;
	return a;
}

stype_t *biggest_stype(stype_t *a, stype_t *b)
{
	if (a->bits() < b->bits())
		return b;
	return a;
}

stype_t *numeric_binop_compat(stype_t *a, stype_t *b)
{
	// abstract operands
	if (IS_STYPE_ABSTRACT_NUMBER(a) && IS_STYPE_ABSTRACT_NUMBER(b)) {
		if (IS_STYPE_ABSTRACT_FLOAT(a) || IS_STYPE_ABSTRACT_FLOAT(b))
			return builtin_stypes[BUILTIN_ABSTRACT_FLOAT];
		return builtin_stypes[BUILTIN_ABSTRACT_INT];
	}

	// only one abstract operand
	if (IS_STYPE_ABSTRACT_NUMBER(a))
		std::swap(a, b);

	if (IS_STYPE_ABSTRACT_NUMBER(b)) {
		if (IS_STYPE_NUMBER(a))
			return a;

		return 0;
	}

	// floats
	if (IS_STYPE_FLOAT(a) && IS_STYPE_FLOAT(b))
		return biggest_stype(a, b);

	if (IS_STYPE_INT(a) && IS_STYPE_INT(b)) {
		int_stype_t *ia = a->as_int();
		int_stype_t *ib = b->as_int();
		if (ia->size == ib->size) {
			// both are signed or unsigned, just return a
			if (ia->is_signed == ib->is_signed)
				return a;

			// return the one that is signed
			if (ia->is_signed)
				return a;
			return b;
		}

		// make ia biggest
		if (ia->size < ib->size)
			std::swap(ia, ib);

		// first case: int32 OP (u)int16
		if (ia->is_signed)
			return a;

		// second case: uint32 OP uint16
		if (!ib->is_signed)
			return a;

		// third case: uint32 OP int16
		return get_int_type(ia->size);
	}

	return 0;
}

stype_t *type_binop_compat(stype_t *a, stype_t *b, unsigned int type)
{
	if ((a->type & type) && (b->type && type))
		return non_abstract(a, b);

	return 0;
}

stype_t *types_binop_compat(stype_t *a, stype_t *b, unsigned int *type)
{
	stype_t *out;
	while (*type) {
		out = type_binop_compat(a, b, *type);
		if (out)
			return out;

		type++;
	}
	return 0;
}

stype_t *vice_versa(stype_t *(*pred)(stype_t*, stype_t*), stype_t *a, stype_t *b)
{
	stype_t *out;
	out = (*pred)(a, b);
	if (out) return out;
	out = (*pred)(b, a);
	return out;
}

stype_t *pointer_op_int(stype_t *a, stype_t *b)
{
	if (IS_STYPE_POINTER(a) && IS_STYPE_INT(b))
		return a;
	return 0;
}

stype_t *pointer_int_compat(stype_t *a, stype_t *b)
{
	return vice_versa(pointer_op_int, a, b);
}

stype_t *pointer_pointer_compat(stype_t *a, stype_t *b)
{
	if (IS_STYPE_POINTER(a) && IS_STYPE_POINTER(b)) {
		if (are_the_same(a, b))
			return builtin_stypes[BUILTIN_ABSTRACT_INT];
	}
	return 0;
}

stype_t *shift_op_compat(stype_t *a, stype_t *b)
{
	if (IS_STYPE_INT(a)) {
		// abstract int is always signed and unsigned
		// checked elsewhere
		if (IS_STYPE_ABSTRACT_INT(b))
			return a;

		// ensure int is unsigned
		if (IS_STYPE_INT(b))
			return b->as_int()->is_signed ? 0 : a;
	}
	return 0;
}

stype_t *pointer_op_pointer_to_void(stype_t *a, stype_t *b)
{
	if (IS_STYPE_POINTER(a) && IS_STYPE_POINTER(b) &&
	    IS_STYPE_VOID(b->as_pointer()->points_to))
		return a;
	return 0;
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

stype_t *binop_compat(stype_t *a, stype_t *b, int tok, stype_t **optype)
{
	stype_t *out_op = 0;
	stype_t *out = 0;

#define STYPE_INT_FLOAT_POINTER_STRING_BOOL \
	(STYPE_INT | STYPE_FLOAT | STYPE_POINTER | STYPE_STRING | STYPE_BOOL)

#define STYPE_INT_FLOAT_POINTER_STRING \
	(STYPE_INT | STYPE_FLOAT | STYPE_POINTER | STYPE_STRING)

#define STYPE_INT_FLOAT_POINTER \
	(STYPE_INT | STYPE_FLOAT | STYPE_POINTER)

#define STYPE_INT_FLOAT (STYPE_INT | STYPE_FLOAT)

	switch (tok) {
	case TOK_EQ:
	case TOK_NEQ:
		if ((a->type & STYPE_INT_FLOAT_POINTER_STRING_BOOL) &&
		    (b->type & STYPE_INT_FLOAT_POINTER_STRING_BOOL))
		{
			// result of EQ or NEQ is always a bool

			// numeric
			out_op = numeric_binop_compat(a, b);
			if (out_op) {
				out = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
				break;
			}

			// ptr
			out_op = pointer_pointer_compat(a, b);
			if (out_op) {
				out = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
				break;
			}

			// ptr and *void
			out_op = vice_versa(pointer_op_pointer_to_void, a, b);
			if (out_op) {
				out = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
				break;
			}

			// string, bool
			unsigned int types[] = {
				STYPE_STRING,
				STYPE_BOOL,
				0
			};
			out_op = types_binop_compat(a, b, types);
			if (out_op) {
				out = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
				break;
			}
		}
		break;
	case TOK_PLUS:
		if ((a->type & STYPE_INT_FLOAT_POINTER_STRING) &&
		    (b->type & STYPE_INT_FLOAT_POINTER_STRING))
		{
			out_op = numeric_binop_compat(a, b);
			if (out_op) break;
			out_op = pointer_int_compat(a, b);
			if (out_op) break;
			out_op = type_binop_compat(a, b, STYPE_STRING);
		}
		break;
	case TOK_LT:
	case TOK_LE:
	case TOK_GT:
	case TOK_GE:
		if ((a->type & STYPE_INT_FLOAT_POINTER) &&
		    (b->type & STYPE_INT_FLOAT_POINTER))
		{
			// result of LT, LE, GT and GE is always a bool
			out_op = numeric_binop_compat(a, b);
			if (out_op) {
				out = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
				break;
			}
			out_op = pointer_pointer_compat(a, b);
			if (out_op) {
				out = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
				break;
			}

			// ptr and *void
			out_op = vice_versa(pointer_op_pointer_to_void, a, b);
			if (out_op) {
				out = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
				break;
			}

		}
		break;
	case TOK_MINUS:
		if ((a->type & STYPE_INT_FLOAT_POINTER) &&
		    (b->type & STYPE_INT_FLOAT_POINTER))
		{
			out_op = numeric_binop_compat(a, b);
			if (out_op) break;
			out_op = pointer_pointer_compat(a, b);
			if (out_op) break;
			out_op = pointer_op_int(a, b);
		}
		break;
	case TOK_TIMES:
	case TOK_DIVIDE:
		if ((a->type & STYPE_INT_FLOAT) && (b->type & STYPE_INT_FLOAT))
			out_op = numeric_binop_compat(a, b);
		break;
	case TOK_AND:
	case TOK_OR:
	case TOK_XOR:
	case TOK_MOD:
		if ((a->type & STYPE_INT) && (b->type & STYPE_INT))
			out_op = numeric_binop_compat(a, b);
		break;
	case TOK_SHIFTL:
	case TOK_SHIFTR:
		if ((a->type & STYPE_INT) && (b->type & STYPE_INT))
			out_op = a;
		break;
	case TOK_ANDAND:
	case TOK_OROR:
		if ((a->type & STYPE_BOOL) && (b->type & STYPE_BOOL))
			out_op = type_binop_compat(a, b, STYPE_BOOL);
		break;
	}

	if (!out)
		out = out_op;
	if (optype)
		*optype = out_op;
	return out;
}

stype_t *unop_compat(stype_t *operand, int tok, node_t *n, stype_tracker_t *tt)
{
	switch (tok) {
	case TOK_XOR:
		if (IS_STYPE_INT(operand))
			return operand;
		break;
	case TOK_PLUS:
	case TOK_MINUS:
		if (IS_STYPE_NUMBER(operand))
			return operand;
		break;
	case TOK_NOT:
		if (IS_STYPE_BOOL(operand))
			return operand;
		break;
	case TOK_AND:
		if (addressable(n))
			return new_pointer_stype(tt, operand);
		break;
	case TOK_TIMES:
		if (IS_STYPE_POINTER(operand))
			return operand->as_pointer()->points_to;
		break;
	}
	return 0;
}

bool can_typecast(stype_t *from, stype_t *to)
{
	if (IS_STYPE_NUMBER(from) && IS_STYPE_NUMBER(to))
		return true;

	if (IS_STYPE_POINTER_OR_INT(from) && IS_STYPE_POINTER_OR_INT(to))
		return true;

	return assignable(from, to);
}

bool value_fits_in_type(value_t *v, stype_t *t)
{
	t = t->end_type();

	if (IS_STYPE_ABSTRACT(t))
		return true;
	else if (IS_STYPE_INT(t)) {
		KRAWL_QASSERT(v->type == VALUE_INT);
		int_stype_t *it = t->as_int();
		int bits = mpz_sizeinbase(v->xint, 2);
		if (!it->is_signed) {
			if (mpz_sgn(v->xint) >= 0 && bits <= it->bits())
				return true;
			return false;
		} else {
			int tbits = it->bits();
			if (bits + 1 <= tbits ||
			    (bits <= tbits && mpz_sgn(v->xint) < 0 &&
			     mpz_scan1(v->xint, 0) == (mp_bitcnt_t)(tbits-1) &&
			     mpz_scan0(v->xint, tbits) == (mp_bitcnt_t)-1))
			{
				return true;
			}
			return false;
		}
	} else if (IS_STYPE_FLOAT(t)) {
		KRAWL_QASSERT(v->type == VALUE_FLOAT);
		if (mpfr_nan_p(v->xfloat) || mpfr_inf_p(v->xfloat) || mpfr_zero_p(v->xfloat))
			return true;

		mp_exp_t exp = mpfr_get_exp(v->xfloat);
		mp_exp_t max_exp;
		switch (t->bits()) {
		case 32:
			max_exp = 128;
			break;
		case 64:
			max_exp = 1024;
			break;
		default:
			KRAWL_QASSERT(0);
		}

		if (exp > max_exp)
			return false;

		return true;
	} else if (IS_STYPE_POINTER(t)) {
		if (v->type == VALUE_STRING)
			return true;

		KRAWL_QASSERT(v->type == VALUE_INT);
		int bits = mpz_sizeinbase(v->xint, 2);
		if (mpz_sgn(v->xint) >= 0 && bits <= t->bits())
			return true;
		return false;
	} else if (IS_STYPE_BOOL(t))
		return true;
	return false;
}

bool assignable(stype_t *from, stype_t *to)
{
	// both have same underlying types
	if (are_the_same(from->end_type(), to->end_type()))
		return true;

	// hack
	if (IS_STYPE_COMPOUND(from) && IS_STYPE_ARRAY_OR_STRUCT(to))
		return true;

	if (IS_STYPE_ABSTRACT_NUMBER(from) && IS_STYPE_NUMBER(to))
		return true;

	if (IS_STYPE_BOOL(from) && IS_STYPE_BOOL(to))
		return true;

	if (IS_STYPE_FLOAT(from) && IS_STYPE_FLOAT(to))
		return true;

	if (IS_STYPE_INT(from) && IS_STYPE_INT(to))
		return true;

	// string literal to *int8 type
	if (IS_STYPE_STRING(from) && IS_STYPE_POINTER(to)) {
		pointer_stype_t *p = to->as_pointer();
		if (p->points_to == builtin_named_stypes[BUILTIN_UINT8])
			return true;
		return false;
	}

	// implicit pointer type conversions
	// TODO: function pointers
	if (IS_STYPE_POINTER(from) && IS_STYPE_POINTER(to)) {
		pointer_stype_t *pfrom = from->as_pointer();
		pointer_stype_t *pto   = to->as_pointer();

		// from *void to any pointer type
		if (pfrom->points_to_void())
			return true;

		// from any pointer type to *void
		if (pto->points_to_void())
			return true;

		// from *[]T to *T
		if (IS_STYPE_ARRAY(pfrom->points_to)) {
			array_stype_t *a = pfrom->points_to->as_array();
			return are_the_same(a->elem, pto->points_to);
		}
		return false;
	}

	return false;
}

stype_visitor_t::stype_visitor_t(): terminate(false)
{
}

stype_visitor_t::~stype_visitor_t()
{
}

void stype_visitor_t::post_visit(stype_t *t)
{
}

void stype_visitor_t::traverse(stype_t *t)
{
	if (!t)
		return;

	stype_visitor_t *v = visit(t);
	if (!v)
		return;

	if (IS_STYPE_NAMED(t)) {
		named_stype_t *nst = t->as_named();
		v->traverse(nst->real);
		if (terminate) return;
	} else if (IS_STYPE_POINTER(t)) {
		pointer_stype_t *pst = t->as_pointer();
		v->traverse(pst->points_to);
		if (terminate) return;
	} else if (IS_STYPE_STRUCT_OR_UNION(t)) {
		struct_stype_t *sst = t->as_struct();
		for (size_t i = 0, n = sst->fields.size(); i < n; ++i) {
			v->traverse(sst->fields[i].type);
			if (terminate) return;
		}
	} else if (IS_STYPE_ARRAY(t)) {
		array_stype_t *ast = t->as_array();
		v->traverse(ast->elem);
		if (terminate) return;
	} else if (IS_STYPE_FUNC(t)) {
		func_stype_t *fst = t->as_func();
		for (size_t i = 0, n = fst->args.size(); i < n; ++i) {
			v->traverse(fst->args[i]);
			if (terminate) return;
		}
		for (size_t i = 0, n = fst->results.size(); i < n; ++i) {
			v->traverse(fst->results[i]);
			if (terminate) return;
		}
	}

	post_visit(t);
}

//------------------------------------------------------------------------------
// type functions
//------------------------------------------------------------------------------

struct type_loop_checker_t : stype_visitor_t {
	named_stype_t *origin;
	bool loop_detected;
	std::vector<sdecl_t*> stack;

	stype_visitor_t *visit(stype_t *t)
	{
		if (IS_STYPE_NAMED(t)) {
			named_stype_t *nst = t->as_named();

			// skip built-in types
			if (!nst->decl)
				return 0;

			if (nst->decl->incycle) {
				if (origin == nst) {
					// oops
					loop_detected = true;
					terminate = true;
				}
				return 0;
			}

			nst->decl->incycle = true;
			stack.push_back(nst->decl);
		} else if (IS_STYPE_POINTER_OR_FUNC(t)) {
			// don't go to pointer types, we don't care
			return 0;
		}
		return this;
	}

	void post_visit(stype_t *t)
	{
		if (IS_STYPE_NAMED(t)) {
			named_stype_t *nst = t->as_named();

			// skip built-in types
			if (!nst->decl)
				return;

			nst->decl->incycle = false;
			stack.pop_back();
		}
	}

	std::string report_loop_stack()
	{
		std::string out;
		const char *what = origin->decl->name.c_str();
		for (size_t i = 0, n = stack.size(); i < n; ++i) {
			KRAWL_QASSERT(stack[i] != 0);
			cppsprintf(&out, "\t'%s' depends on the size of '%s'\n",
				   what, stack[i]->name.c_str());
			what = stack[i]->name.c_str();
		}
		if (what == origin->decl->name.c_str()) {
			cppsprintf(&out, "\t'%s' depends on the size of itself",
				   what);
		} else {
			cppsprintf(&out, "\t'%s' depends on the size of '%s'",
				   what, origin->decl->name.c_str());
		}
		return out;
	}
};

void check_type_for_size_loops(diagnostic_t *diag, named_stype_t *t)
{
	t->decl->incycle = true;

	type_loop_checker_t checker;
	checker.origin = t;
	checker.loop_detected = false;
	checker.traverse(t->real);

	t->decl->incycle = false;

	if (checker.loop_detected) {
		t->decl->typeerror = true;
		for (size_t i = 0, n = checker.stack.size(); i < n; ++i) {
			KRAWL_QASSERT(checker.stack[i] != 0);
			checker.stack[i]->typeerror = true;
		}

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				t->decl->get_ident()->pos, true, 0, 0,
				"invalid recursive type:\n%s",
				checker.report_loop_stack().c_str());
		diag->report(m);
	}
}

void check_declared_type_sdecl(diagnostic_t *diag, type_sdecl_t *sd)
{
	check_type_for_size_loops(diag, sd->stype->as_named());
	if (!sd->typeerror && IS_STYPE_STRUCT_OR_UNION(sd->stype)) {
		struct_stype_t *sst = sd->stype->as_struct();
		fix_structs_alignment(sst);
	}
}

struct structs_alignment_fixer_t : stype_visitor_t {
	bool ignore_pointers;

	stype_visitor_t *visit(stype_t *t)
	{
		// don't recurse into pointer or func, they can refer to myself
		if (ignore_pointers && IS_STYPE_POINTER_OR_FUNC(t))
			return 0;

		if (IS_STYPE_STRUCT_OR_UNION(t)) {
			// fix struct types
			struct_stype_t *sst = t->as_struct();
			fix_structs_alignment(sst);
			return 0;
		}

		return this;
	}
};

static void fix_unions_alignment(struct_stype_t *sst)
{
	// fix all children that I'm depending on
	structs_alignment_fixer_t fixer;
	fixer.ignore_pointers = true;
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i)
		fixer.traverse(sst->fields[i].type);

	stype_t *max = 0;
	size_t max_size = 0;
	size_t max_align = 0;
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i) {
		struct_field_t *f = &sst->fields[i];
		size_t align = alignment_of(f->type);
		if (align > max_align)
			max_align = align;

		size_t size = f->type->bits() / 8;
		if (size > max_size) {
			max_size = size;
			max = f->type;
		}

		f->padding = 0;
	}

	sst->alignment = max_align;
	sst->size = max_size;
	sst->biggest = max;

	/*
	printf("union (alignment: %d, size: %d) {\n",
	       sst->alignment, sst->size);
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i) {
		printf("\tfield: %s, size: %d, padding: %d\n",
		       sst->fields[i].name.c_str(),
		       sst->fields[i].type->bits() / 8,
		       sst->fields[i].padding);
	}
	printf("}\n");
	*/

	// traverse all children again, now it's ok to refer to me, because my
	// alignment was fixed
	fixer.ignore_pointers = false;
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i)
		fixer.traverse(sst->fields[i].type);
}

void fix_structs_alignment(struct_stype_t *sst)
{
	if (sst->alignment_fixed())
		return;

	if (sst->u) {
		// if this is a union, we do that differently
		fix_unions_alignment(sst);
		return;
	}

	// fix all children that I'm depending on
	structs_alignment_fixer_t fixer;
	fixer.ignore_pointers = true;
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i)
		fixer.traverse(sst->fields[i].type);

	// fix my own alignment
	size_t max_nalign = 0;
	size_t offset = 0;
	for (int i = 0, n = sst->fields.size(); i < n; ++i) {
		struct_field_t *f = &sst->fields[i];
		size_t nalign = alignment_of(f->type);
		if (nalign > max_nalign)
			max_nalign = nalign;

		size_t padding = -offset & nalign - 1;
		if (i-1 >= 0)
			sst->fields[i-1].padding = padding;
		offset += padding + f->type->bits() / 8;
	}
	sst->fields.back().padding = -offset & max_nalign - 1;
	offset += sst->fields.back().padding;

	sst->alignment = max_nalign;
	sst->size = offset;

	/*
	printf("struct (alignment: %d, size: %d) {\n",
	       sst->alignment, sst->size);
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i) {
		printf("\tfield: %s, size: %d, padding: %d\n",
		       sst->fields[i].name.c_str(),
		       sst->fields[i].type->bits() / 8,
		       sst->fields[i].padding);
	}
	printf("}\n");
	*/

	// traverse all children again, now it's ok to refer to me, because my
	// alignment was fixed
	fixer.ignore_pointers = false;
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i)
		fixer.traverse(sst->fields[i].type);
}

size_t alignment_of(stype_t *t)
{
	t = t->end_type();
	if (t->type & STYPE_ARRAY) {
		array_stype_t *ast = t->as_array();
		return alignment_of(ast->elem);
	} else if (t->type & STYPE_STRUCT) {
		struct_stype_t *sst = t->as_struct();
		return sst->alignment;
	}

	return t->bits() / 8;
}

size_t size_from_array_or_compound(node_t *v)
{
	stype_t *t = v->vst.stype;
	if (IS_STYPE_COMPOUND(t)) {
		KRAWL_QASSERT(v->type == node_t::COMPOUND_LIT);
		compound_lit_t *cl = (compound_lit_t*)v;
		return cl->elts.size();
	} else if (IS_STYPE_ARRAY(t)) {
		array_stype_t *ast = t->as_array();
		return ast->size;
	}
	return 0;
}

void free_tracked_stypes(stype_tracker_t *tt)
{
	for (size_t i = 0, n = tt->size(); i < n; ++i)
		delete (*tt)[i];
}

stype_t *new_named_stype(stype_tracker_t *tt, const char *prefix,
			 const char *name, stype_t *real)
{
	named_stype_t *nt = new named_stype_t;
	nt->prefix = prefix;
	nt->name = name;
	nt->real = real;
	tt->push_back(nt);
	return nt;
}

stype_t *new_pointer_stype(stype_tracker_t *tt, stype_t *points_to)
{
	pointer_stype_t *pt = new pointer_stype_t;
	pt->points_to = points_to;
	tt->push_back(pt);
	return pt;
}

stype_t *new_func_stype(stype_tracker_t *tt,
			stype_vector_t *args, stype_vector_t *results,
			bool varargs)
{
	func_stype_t *ft = new func_stype_t;
	ft->varargs = varargs;
	ft->args.swap(*args);
	ft->results.swap(*results);
	tt->push_back(ft);
	return ft;
}

stype_t *new_array_stype(stype_tracker_t *tt, stype_t *elem, size_t size)
{
	array_stype_t *at = new array_stype_t;
	at->elem = elem;
	at->size = size;
	tt->push_back(at);
	return at;
}

stype_t *new_struct_stype(stype_tracker_t *tt, std::vector<struct_field_t> *fields,
			  size_t alignment, size_t size, bool u)
{
	struct_stype_t *st = new struct_stype_t;
	if (u)
		st->type = STYPE_UNION;
	st->alignment = alignment;
	st->size = size;
	st->fields.swap(*fields);
	st->u = u;
	tt->push_back(st);
	return st;
}

stype_t *builtin_stypes[BUILTIN_N];
stype_t *builtin_named_stypes[BUILTIN_N];

void init_builtin_stypes()
{
	// real types
	builtin_stypes[BUILTIN_VOID]              = new void_stype_t;
	builtin_stypes[BUILTIN_BOOL]              = new bool_stype_t;
	builtin_stypes[BUILTIN_ABSTRACT_BOOL]     = new bool_stype_t(true);

	builtin_stypes[BUILTIN_INT8]              = new int_stype_t(8,  true);
	builtin_stypes[BUILTIN_INT16]             = new int_stype_t(16, true);
	builtin_stypes[BUILTIN_INT32]             = new int_stype_t(32, true);
	builtin_stypes[BUILTIN_INT64]             = new int_stype_t(64, true);
	builtin_stypes[BUILTIN_UINT8]             = new int_stype_t(8,  false);
	builtin_stypes[BUILTIN_UINT16]            = new int_stype_t(16, false);
	builtin_stypes[BUILTIN_UINT32]            = new int_stype_t(32, false);
	builtin_stypes[BUILTIN_UINT64]            = new int_stype_t(64, false);
	builtin_stypes[BUILTIN_ABSTRACT_INT]      = new int_stype_t(0,  true, true);

	builtin_stypes[BUILTIN_FLOAT32]           = new float_stype_t(32);
	builtin_stypes[BUILTIN_FLOAT64]           = new float_stype_t(64);
	builtin_stypes[BUILTIN_ABSTRACT_FLOAT]    = new float_stype_t(0, true);
	builtin_stypes[BUILTIN_ABSTRACT_STRING]   = new string_stype_t;
	builtin_stypes[BUILTIN_ABSTRACT_COMPOUND] = new compound_stype_t;
	builtin_stypes[BUILTIN_ABSTRACT_MODULE]   = new module_stype_t;

	builtin_stypes[BUILTIN_VA_LIST]           = new pointer_stype_t(builtin_stypes[BUILTIN_VOID]);

	builtin_stypes[BUILTIN_FUNC]              = new func_stype_t;

	// named types
	builtin_named_stypes[BUILTIN_VOID]    = new named_stype_t("void", builtin_stypes[BUILTIN_VOID]);
	builtin_named_stypes[BUILTIN_BOOL]    = new named_stype_t("bool", builtin_stypes[BUILTIN_BOOL]);

	builtin_named_stypes[BUILTIN_INT8]    = new named_stype_t("int8", builtin_stypes[BUILTIN_INT8]);
	builtin_named_stypes[BUILTIN_INT16]   = new named_stype_t("int16", builtin_stypes[BUILTIN_INT16]);
	builtin_named_stypes[BUILTIN_INT32]   = new named_stype_t("int32", builtin_stypes[BUILTIN_INT32]);
	builtin_named_stypes[BUILTIN_INT64]   = new named_stype_t("int64", builtin_stypes[BUILTIN_INT64]);
	builtin_named_stypes[BUILTIN_UINT8]   = new named_stype_t("uint8",  builtin_stypes[BUILTIN_UINT8]);
	builtin_named_stypes[BUILTIN_UINT16]  = new named_stype_t("uint16", builtin_stypes[BUILTIN_UINT16]);
	builtin_named_stypes[BUILTIN_UINT32]  = new named_stype_t("uint32", builtin_stypes[BUILTIN_UINT32]);
	builtin_named_stypes[BUILTIN_UINT64]  = new named_stype_t("uint64", builtin_stypes[BUILTIN_UINT64]);

	builtin_named_stypes[BUILTIN_FLOAT32] = new named_stype_t("float32", builtin_stypes[BUILTIN_FLOAT32]);
	builtin_named_stypes[BUILTIN_FLOAT64] = new named_stype_t("float64", builtin_stypes[BUILTIN_FLOAT64]);

	builtin_named_stypes[BUILTIN_VA_LIST] = new named_stype_t("va_list", builtin_stypes[BUILTIN_VA_LIST]);

	builtin_stypes[BUILTIN_VA_LIST_PTR]   = new pointer_stype_t(builtin_named_stypes[BUILTIN_VA_LIST]);

	for (size_t i = 0, n = BUILTIN_N; i < n; ++i) {
		if (builtin_stypes[i])
			builtin_stypes[i]->type |= STYPE_BUILTIN;
	}
	for (size_t i = 0, n = BUILTIN_N; i < n; ++i) {
		if (builtin_named_stypes[i])
			builtin_named_stypes[i]->type |= STYPE_BUILTIN;
	}
}

void free_builtin_stypes()
{
	for (size_t i = 0, n = BUILTIN_N; i < n; ++i)
		delete builtin_stypes[i];
	for (size_t i = 0, n = BUILTIN_N; i < n; ++i)
		delete builtin_named_stypes[i];
}




//==============================================================================
// SEMANTIC DECLARATIONS
//==============================================================================

//------------------------------------------------------------------------------
// sdecl_t
//------------------------------------------------------------------------------

sdecl_t::sdecl_t(sdecl_type_t t, const char *name):
	type(t), scope(0), name(name), stype(0),
	incycle(false), typeerror(false), inited(false), addr(0)
{
}

sdecl_t::~sdecl_t()
{
}

//------------------------------------------------------------------------------
// const_sdecl_t
//------------------------------------------------------------------------------

const_sdecl_t::const_sdecl_t(const char *name):
	sdecl_t(SDECL_CONST, name), ident(0), pos(0), init(0), ctype(0), iota(0)
{
}

const_sdecl_t::const_sdecl_t(ident_expr_t *name, source_loc_t pos,
			     node_t *init, node_t *ctype, int iota):
	sdecl_t(SDECL_CONST, name->val.c_str()), ident(name),
	pos(pos), init(init), ctype(ctype), iota(iota), value(0)
{
}

ident_expr_t *const_sdecl_t::get_ident()
{
	return ident;
}

//------------------------------------------------------------------------------
// type_sdecl_t
//------------------------------------------------------------------------------

type_sdecl_t::type_sdecl_t(const char *name):
	sdecl_t(SDECL_TYPE, name), spec(0)
{
}

type_sdecl_t::type_sdecl_t(type_spec_t *spec, stype_t *t):
	sdecl_t(SDECL_TYPE, spec->name->val.c_str()), spec(spec)
{
	stype = t;
}

ident_expr_t *type_sdecl_t::get_ident()
{
	return spec->name;
}

//------------------------------------------------------------------------------
// var_sdecl_t
//------------------------------------------------------------------------------

var_sdecl_t::var_sdecl_t(const char *name):
	sdecl_t(SDECL_VAR, name), ident(0), pos(0), init(0), vtype(0),
	index(0), namedret(-1)
{
}

var_sdecl_t::var_sdecl_t(ident_expr_t *name, source_loc_t pos, node_t *init,
			 node_t *vtype, int index):
	sdecl_t(SDECL_VAR, name->val.c_str()), ident(name),
	pos(pos), init(init), vtype(vtype), index(index), namedret(-1)
{
}

ident_expr_t *var_sdecl_t::get_ident()
{
	return ident;
}

//------------------------------------------------------------------------------
// func_sdecl_t
//------------------------------------------------------------------------------

func_sdecl_t::func_sdecl_t(const char *name):
	sdecl_t(SDECL_FUNC, name), decl(0)
{
}

func_sdecl_t::func_sdecl_t(func_decl_t *decl):
	sdecl_t(SDECL_FUNC, decl->name->val.c_str()), decl(decl)
{
}

ident_expr_t *func_sdecl_t::get_ident()
{
	return decl->name;
}

int func_sdecl_t::n_return_values()
{
	stype_vector_t *rtypes = return_types();
	return rtypes->size();
}

bool func_sdecl_t::has_named_return_values()
{
	func_type_t *ft = decl->ftype;

	for (size_t i = 0, n = ft->results.size(); i < n; ++i) {
		if (!ft->results[i]->names.empty())
			return true;
	}
	return false;
}

stype_vector_t *func_sdecl_t::return_types()
{
	KRAWL_QASSERT(stype != 0);
	func_stype_t *fst = stype->as_func();
	return &fst->results;
}

//------------------------------------------------------------------------------
// import_sdecl_t
//------------------------------------------------------------------------------

import_sdecl_t::import_sdecl_t(import_spec_t *spec):
	sdecl_t(SDECL_IMPORT, ""), spec(spec), used(false)
{
	if (spec->name)
		name = spec->name->val;
}

ident_expr_t *import_sdecl_t::get_ident()
{
	return spec->name;
}

static FILE *open_file_on(std::string *path,
			  std::vector<const char*> *include_dirs)
{
	if (path->find_first_of("./") == 0) {
		return fopen(path->c_str(), "rb");
	}

	for (size_t i = 0, n = include_dirs->size(); i < n; ++i) {
		std::string full = include_dirs->at(i);
		full += '/';
		full += *path;
		FILE *f = fopen(full.c_str(), "rb");
		if (f)
			return f;
	}

	std::string full = KRAWL_INSTALL_PREFIX "/include/krawl/" + *path;
	return fopen(full.c_str(), "rb");
}

void import_sdecl_t::load(brawl_context_t *ctx,
			  std::vector<const char*> *include_dirs,
			  const char *clang_path,
			  const char *clang_plugin_path)
{
	KRAWL_QASSERT(spec->path->val.type == VALUE_STRING);
	std::string path = spec->path->val.to_string();

	FILE *f;
	if (path.substr(path.size()-2, 2) == ".h") {
		path = update_c_module_hash(path.c_str(), clang_path,
					    clang_plugin_path);
		f = fopen(path.c_str(), "rb");
	} else {
		path += ".brl";
		f = open_file_on(&path, include_dirs);
	}

	if (!f)
		return;

	brawl_module_t bmodule(ctx);
	FILE_reader_t cin(f);
	bmodule.load(&cin);
	fclose(f);

	prefix = bmodule.prefix;
	if (name.empty())
		name = bmodule.package;
	decls.swap(bmodule.decls);
}

//------------------------------------------------------------------------------
// decl functions
//------------------------------------------------------------------------------

void free_tracked_sdecls(sdecl_tracker_t *tracker)
{
	for (size_t i = 0, n = tracker->size(); i < n; ++i)
		delete (*tracker)[i];
}

const_sdecl_t *new_const_sdecl(sdecl_tracker_t *dt, ident_expr_t *name,
			       source_loc_t pos, node_t *init, node_t *ctype,
			       int iota)
{
	const_sdecl_t *cs = new const_sdecl_t(name, pos, init, ctype, iota);
	dt->push_back(cs);
	return cs;
}

const_sdecl_t *new_const_bool_sdecl(sdecl_tracker_t *dt, bool value)
{
	const_sdecl_t *cs = new const_sdecl_t(value ? "true" : "false");
	cs->value = value_t(value ? 1 : 0);
	cs->stype = builtin_stypes[BUILTIN_ABSTRACT_BOOL];
	dt->push_back(cs);
	return cs;
}

const_sdecl_t *new_const_iota_sdecl(sdecl_tracker_t *dt)
{
	const_sdecl_t *cs = new const_sdecl_t("iota");
	cs->iota = -1;
	cs->stype = builtin_stypes[BUILTIN_ABSTRACT_INT];
	dt->push_back(cs);
	return cs;
}

const_sdecl_t *new_const_nil_sdecl(sdecl_tracker_t *dt, stype_tracker_t *tt)
{
	const_sdecl_t *cs = new const_sdecl_t("nil");
	cs->value = value_t(0);
	cs->stype = new_pointer_stype(tt, builtin_named_stypes[BUILTIN_VOID]);
	dt->push_back(cs);
	return cs;
}

type_sdecl_t *new_type_sdecl(sdecl_tracker_t *dt, type_spec_t *spec, stype_t *t)
{
	const char *name = "";
	if (spec)
		name = spec->name->val.c_str();
	type_sdecl_t *ts = new type_sdecl_t(name);
	ts->spec = spec;
	ts->stype = t;
	dt->push_back(ts);
	return ts;
}

var_sdecl_t *new_var_sdecl(sdecl_tracker_t *dt, ident_expr_t *name,
			   source_loc_t pos, node_t *init, node_t *vtype,
			   int index)
{
	var_sdecl_t *vs = new var_sdecl_t(name, pos, init, vtype, index);
	dt->push_back(vs);
	return vs;
}

var_sdecl_t *new_typed_var_sdecl(sdecl_tracker_t *dt, ident_expr_t *name,
				 stype_t *type, node_t *vtype)
{
	var_sdecl_t *vs = new var_sdecl_t(name, 0, 0, vtype, -1);
	vs->stype = type;
	dt->push_back(vs);
	return vs;
}

func_sdecl_t *new_func_sdecl(sdecl_tracker_t *dt, func_decl_t *decl)
{
	func_sdecl_t *fd = new func_sdecl_t(decl);
	dt->push_back(fd);
	return fd;
}

func_sdecl_t *new_builtin_func_sdecl(sdecl_tracker_t *dt, const char *name)
{
	func_sdecl_t *fd = new func_sdecl_t(name);
	fd->stype = builtin_stypes[BUILTIN_FUNC];
	dt->push_back(fd);
	return fd;
}

import_sdecl_t *new_import_sdecl(sdecl_tracker_t *dt, import_spec_t *spec)
{
	import_sdecl_t *id = new import_sdecl_t(spec);
	dt->push_back(id);
	return id;
}

sdecl_t *new_sdecl(sdecl_tracker_t *dt, const char *name, sdecl_type_t type)
{
	sdecl_t *d = 0;
	switch (type) {
	case SDECL_CONST:
		d = new const_sdecl_t(name);
		break;
	case SDECL_VAR:
		d = new var_sdecl_t(name);
		break;
	case SDECL_TYPE:
		d = new type_sdecl_t(name);
		break;
	case SDECL_FUNC:
		d = new func_sdecl_t(name);
		break;
	default:
		KRAWL_QASSERT(!"bad type");
		return 0;
	}
	dt->push_back(d);
	return d;
}

//------------------------------------------------------------------------------
// scope_block_t
//------------------------------------------------------------------------------

scope_block_t::scope_block_t(scope_block_t *parent):
	parent(parent)
{
}

sdecl_t *scope_block_t::lookup(const char *name)
{
	sdecl_t *sd = has(name);
	if (sd)
		return sd;

	return (parent ? parent->lookup(name) : 0);
}

sdecl_t *scope_block_t::has(const char *name)
{
	unordered_map<std::string, sdecl_t*>::iterator it;
	it = sdecls.find(name);
	if (it != sdecls.end())
		return it->second;

	return 0;
}

bool scope_block_t::add(sdecl_t *d)
{
	if (sdecls.find(d->name.c_str()) != sdecls.end())
		return false;

	sdecls[d->name.c_str()] = d;
	return true;
}

void free_tracked_scope_blocks(scope_block_tracker_t *v)
{
	for (size_t i = 0, n = v->size(); i < n; ++i)
		delete (*v)[i];
}

void fill_global_scope(scope_block_t *scope, sdecl_tracker_t *dt,
		       stype_tracker_t *tt)
{
	unordered_map<std::string, sdecl_t*> &d = scope->sdecls;
	d["void"]    = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_VOID]);
	d["bool"]    = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_BOOL]);
	d["int8"]    = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_INT8]);
	d["int16"]   = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_INT16]);
	d["int32"]   = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_INT32]);
	d["int64"]   = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_INT64]);
	d["uint8"]   = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_UINT8]);
	d["uint16"]  = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_UINT16]);
	d["uint32"]  = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_UINT32]);
	d["uint64"]  = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_UINT64]);
	d["float32"] = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_FLOAT32]);
	d["float64"] = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_FLOAT64]);
	d["va_list"] = new_type_sdecl(dt, 0, builtin_named_stypes[BUILTIN_VA_LIST]);

	// aliases
	d["int"]     = d["int32"];
	d["uint"]    = d["uint32"];
	d["byte"]    = d["uint8"];
	d["float"]   = d["float32"];
	d["double"]  = d["float64"];

	// constants
	d["true"]    = new_const_bool_sdecl(dt, true);
	d["false"]   = new_const_bool_sdecl(dt, false);
	d["iota"]    = new_const_iota_sdecl(dt);
	d["nil"]     = new_const_nil_sdecl(dt, tt);

	// built-in functions
	d["sizeof"]   = new_builtin_func_sdecl(dt, "sizeof");
	d["va_start"] = new_builtin_func_sdecl(dt, "va_start");
	d["va_copy"]  = new_builtin_func_sdecl(dt, "va_copy");
	d["va_arg"]   = new_builtin_func_sdecl(dt, "va_arg");
	d["va_end"]   = new_builtin_func_sdecl(dt, "va_end");

	unordered_map<std::string, sdecl_t*>::iterator it, end;
	for (it = d.begin(), end = d.end(); it != end; ++it)
		it->second->scope = scope;
}

scope_block_t *new_scope_block(scope_block_tracker_t *t, scope_block_t *parent)
{
	scope_block_t *sb = new scope_block_t(parent);
	t->push_back(sb);
	return sb;
}




//==============================================================================
// TYPE CHECKS
//==============================================================================

bool addressable(node_t *n)
{
	// TODO: incomplete
	switch (n->type) {
	case node_t::BINARY_EXPR:
		break;
	case node_t::UNARY_EXPR:
	{
		unary_expr_t *e = (unary_expr_t*)n;
		if (e->tok == TOK_TIMES) // pointer indirections are addressable
			return true;
		break;
	}
	case node_t::BASIC_LIT_EXPR:
		break;
	case node_t::IDENT_EXPR:
	{
		ident_expr_t *e = (ident_expr_t*)n;
		KRAWL_QASSERT(e->sdecl != 0);
		return e->sdecl->type == SDECL_VAR;
	}
	case node_t::SELECTOR_EXPR:
		return addressable(((selector_expr_t*)n)->expr);
	case node_t::INDEX_EXPR:
		// TODO: not necessary true
		// e.g. (ptr + 5)[0]
		//
		// I think there are no array literals which can be used in an
		// expression, therefore index expression is always
		// addressable?
		return addressable(((index_expr_t*)n)->expr);
	case node_t::CALL_EXPR:
		break;
	case node_t::PAREN_EXPR:
		return addressable(((paren_expr_t*)n)->expr);
	case node_t::TYPE_CAST_EXPR:
		break;
	default:
		break;
	}
	return false;
}

void declare_decl(node_t *node, sdecl_tracker_t *dt,
		  void (*declare)(std::vector<sdecl_t*>*, void*), void *data)
{
	std::vector<sdecl_t*> v;

	switch (node->type) {
	case node_t::IMPORT_DECL:
	{
		import_decl_t *d = (import_decl_t*)node;
		for (size_t i = 0, n = d->specs.size(); i < n; ++i) {
			v.clear();
			import_spec_t *s = d->specs[i];
			import_sdecl_t *id = new_import_sdecl(dt, s);
			if (s->name)
				s->name->sdecl = id;
			v.push_back(id);
			(*declare)(&v, data);
		}
		break;
	}
	case node_t::CONST_DECL:
	{
		const_decl_t *d = (const_decl_t*)node;
		value_spec_t *last = d->specs[0];
		for (size_t i = 0, n = d->specs.size(); i < n; ++i) {
			v.clear();
			value_spec_t *s = d->specs[i];
			for (size_t j = 0, m = s->names.size(); j < m; ++j) {
				const_sdecl_t *sd;
				node_t *val = last->values[j]; // always valid
				if (!s->values.empty()) {
					val = s->values[j];
					last = s;
				}
				sd = new_const_sdecl(dt, s->names[j],
						     s->pos, val,
						     s->type, i);
				v.push_back(sd);
			}
			(*declare)(&v, data);
		}
		break;
	}
	case node_t::VAR_DECL:
	{
		var_decl_t *d = (var_decl_t*)node;
		for (size_t i = 0, n = d->specs.size(); i < n; ++i) {
			v.clear();
			value_spec_t *s = d->specs[i];
			if (s->values.size() == 1 && s->names.size() > 1) {
				call_expr_t *c = is_call_expr(s->values[0]);
				c->mexpected = s->names.size();
			}

			for (size_t j = 0, m = s->names.size(); j < m; ++j) {
				var_sdecl_t *vd;
				node_t *val = 0;
				int index = -1;
				if (!s->values.empty()) {
					if (s->values.size() == 1 &&
					    s->names.size() > 1)
					{
						index = j;
						val = s->values[0];
					} else
						val = s->values[j];
				}
				vd = new_var_sdecl(dt, s->names[j],
						   s->pos, val,
						   s->type, index);
				s->names[j]->sdecl = vd;
				v.push_back(vd);
			}
			(*declare)(&v, data);
		}
		break;
	}
	case node_t::TYPE_DECL:
	{
		type_decl_t *d = (type_decl_t*)node;
		for (size_t i = 0, n = d->specs.size(); i < n; ++i) {
			v.clear();
			type_spec_t *s = d->specs[i];
			type_sdecl_t *sd = new_type_sdecl(dt, s, 0);
			s->name->sdecl = sd;
			v.push_back(sd);
			(*declare)(&v, data);
		}
		break;
	}
	case node_t::FUNC_DECL:
	{
		v.clear();
		func_decl_t *d = (func_decl_t*)node;
		func_sdecl_t *fd = new_func_sdecl(dt, d);
		v.push_back(fd);
		(*declare)(&v, data);
		break;
	}
	case node_t::ASSIGN_STMT:
	{
		v.clear();
		assign_stmt_t *s = (assign_stmt_t*)node;
		if (s->tok != TOK_DECLARIZE)
			break;

		// if call expr case, set mexpected
		if (s->rhs.size() == 1 && s->lhs.size() > 1) {
			call_expr_t *c = is_call_expr(s->rhs[0]);
			c->mexpected = s->lhs.size();
		}

		for (size_t i = 0, n = s->lhs.size(); i < n; ++i) {
			// skip all non-ident expressions
			if (s->lhs[i]->type != node_t::IDENT_EXPR)
				continue;

			ident_expr_t *name = (ident_expr_t*)s->lhs[i];
			node_t *val;
			int index = -1;
			if (s->rhs.size() == 1 && s->lhs.size() > 1) {
				index = i;
				val = s->rhs[0];
			} else
				val = s->rhs[i];

			var_sdecl_t *vd;
			vd = new_var_sdecl(dt, name, s->pos, val, 0, index);
			name->sdecl = vd;
			v.push_back(vd);

		}
		if (v.empty())
			break;
		(*declare)(&v, data);
	}
	default:
		break;
	}
}

void redeclared_error(ident_expr_t *expr, diagnostic_t *diag)
{
	message_t *m;
	m = new_message(MESSAGE_ERROR,
			expr->pos, true, 0, 0,
			"'%s' redeclared in this block",
			expr->val.c_str());
	diag->report(m);
}

//------------------------------------------------------------------------------
// value_stype_t
//------------------------------------------------------------------------------

value_stype_t::value_stype_t():
	stype(0)
{
}

//------------------------------------------------------------------------------
// pass1_t
//------------------------------------------------------------------------------

static void gdecls_collector_declare(std::vector<sdecl_t*> *d, void *data);

struct gdecls_collector_t : ast_visitor_t {
	pass1_t *pass;
	scope_block_t *filescope;

	ast_visitor_t *visit(node_t *node)
	{
		if (node->type == node_t::PROGRAM)
			return this;

		declare_decl(node, pass->dtracker, gdecls_collector_declare, this);
		return 0;
	}

	void declare(sdecl_t *d)
	{
		if (d->type == SDECL_IMPORT) {
			import_sdecl_t *id = (import_sdecl_t*)d;
			id->load(pass->brawl,
				 pass->include_dirs,
				 pass->clang_path,
				 pass->clang_plugin_path);

			if (id->decls.empty()) {
				package_error(id);
				return;
			}

			if (filescope->has(d->name.c_str())) {
				redeclared_error(d->get_ident(), pass->diag);
				return;
			}
			filescope->add(d);
			d->scope = filescope;
		} else {
			if (pass->pkgscope->has(d->name.c_str())) {
				redeclared_error(d->get_ident(), pass->diag);
				return;
			}
			pass->pkgscope->add(d);
			pass->names->push_back(d->name.c_str());
			d->scope = filescope;
		}
	}

	void declare_many(std::vector<sdecl_t*> *decls)
	{
		for (size_t i = 0, n = decls->size(); i < n; ++i)
			declare(decls->at(i));
	}

	void package_error(import_sdecl_t *id)
	{
		source_loc_range_t range = id->spec->path->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"failed to load package");
		pass->diag->report(m);
	}
};

static void gdecls_collector_declare(std::vector<sdecl_t*> *d, void *data)
{
	gdecls_collector_t *gd = (gdecls_collector_t*)data;
	gd->declare_many(d);
}

pass1_t::pass1_t(pass1_opts_t *opts)
{
	this->opts = *opts;
}

void pass1_t::pass(node_t *ast)
{
	gdecls_collector_t visitor;
	visitor.pass = this;
	visitor.filescope = new_scope_block(stracker, pkgscope);
	visitor.traverse(ast);
}

//------------------------------------------------------------------------------
// pass2_t
//------------------------------------------------------------------------------

pass2_t::pass2_t(pass2_opts_t *opts)
{
	this->opts = *opts;
}

void pass2_t::error_args_mismatch(call_expr_t *expr, size_t num)
{
	source_loc_range_t ranges[] = {
		expr->expr->source_loc_range(),
		{ expr->pos_lp, expr->pos_rp }
	};

	const char *what = "not enough";
	if (expr->args.size() > num)
		what = "too many";

	message_t *m;
	m = new_message(MESSAGE_ERROR,
			ranges[0].beg, true, &ranges[1], 1,
			"%s arguments in function call", what);
	diag->report(m);
}

bool pass2_t::typegen_for_funcfields(stype_vector_t *sv, field_vector_t *fv)
{
	for (size_t i = 0, n = fv->size(); i < n; ++i) {
		stype_t *t = typegen(fv->at(i)->type);
		if (!t)
			return false;

		if (fv->at(i)->names.empty()) {
			sv->push_back(t);
			continue;
		}
		size_t m = fv->at(i)->names.size();
		for (size_t j = 0; j < m; ++j)
			sv->push_back(t);
	}
	return true;
}

bool pass2_t::typegen_for_structfields(std::vector<struct_field_t> *out,
				       field_vector_t *fv)
{
	for (size_t i = 0, n = fv->size(); i < n; ++i) {
		field_t *field = fv->at(i);
		stype_t *t = typegen(field->type);
		if (!t)
			return false;

		if (field->names.empty()) {
			struct_field_t f = {t, "", 0};
			out->push_back(f);
			continue;
		}

		for (size_t j = 0, m = field->names.size(); j < m; ++j) {
			ident_expr_t *name = field->names[j];
			struct_field_t f = {t, name->val, 0};
			out->push_back(f);
		}
	}
	return true;
}

size_t pass2_t::typegen_array_size(node_t *size)
{
	value_stype_t idx = typecheck_expr(size);
	if (!idx.stype || idx.value.type == VALUE_ERROR)
		return 0;

	if (idx.value.type == VALUE_NONE) {
		source_loc_range_t range = size->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"non-constant expression cannot be "
				"used as array bound");
		diag->report(m);
		return 0;
	}

	if (!IS_STYPE_NUMBER(idx.stype)) {
		source_loc_range_t range = size->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"non-integer constant of type '%s' "
				"cannot be used as array bound",
				idx.stype->to_string().c_str());
		diag->report(m);
		return 0;
	}

	if (IS_STYPE_FLOAT(idx.stype)) {
		source_loc_range_t range = size->source_loc_range();
		if (!mpfr_integer_p(idx.value.xfloat)) {
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					range.beg, false, &range, 1,
					"floating point constant "
					"truncated to integer");
			diag->report(m);
			return 0;
		}
		idx.value = idx.value.to_int();
	}

	if (mpz_sgn(idx.value.xint) > 0)
		return mpz_get_ui(idx.value.xint);
	return 0;
}

stype_t *pass2_t::typegen(node_t *expr)
{
	switch (expr->type) {
	case node_t::IDENT_EXPR:
	{
		ident_expr_t *e = (ident_expr_t*)expr;
		sdecl_t *d = cur_scope->lookup(e->val.c_str());
		if (!d) {
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					e->pos, true, 0, 0,
					"undefined: %s",
					e->val.c_str());
			diag->report(m);
			return 0;
		}

		if (d->type != SDECL_TYPE) {
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					e->pos, true, 0, 0,
					"%s is not a type",
					e->val.c_str());
			diag->report(m);
			return 0;
		}

		e->sdecl = d;
		resolve_sdecl(d);
		return d->stype;
	}
	case node_t::POINTER_TYPE:
	{
		pointer_type_t *e = (pointer_type_t*)expr;
		stype_t *points_to = typegen(e->ptype);

		if (!points_to)
			return 0;
		return new_pointer_stype(ttracker, points_to);
	}
	case node_t::FUNC_TYPE:
	{
		stype_vector_t args;
		stype_vector_t results;
		func_type_t *e = (func_type_t*)expr;
		if (!typegen_for_funcfields(&args, &e->args))
			return 0;
		if (!typegen_for_funcfields(&results, &e->results))
			return 0;

		return new_func_stype(ttracker, &args, &results, e->varargs);
	}
	case node_t::ARRAY_TYPE:
	{
		array_type_t *e = (array_type_t*)expr;
		size_t s = 0;
		if (e->size)
			s = typegen_array_size(e->size);
		if (!s && array_ctx != 0)
			s = array_ctx;
		if (!s) {
			source_loc_range_t range = e->size->source_loc_range();
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					range.beg, false, &range, 1,
					"negative or zero integer constant "
					"cannot be used as array bound");
			diag->report(m);
			return 0;
		}

		stype_t *elem = typegen(e->etype);
		if (!elem)
			return 0;

		return new_array_stype(ttracker, elem, s);
	}
	case node_t::STRUCT_TYPE:
	{
		struct_type_t *e = (struct_type_t*)expr;
		std::vector<struct_field_t> fields;
		if (!typegen_for_structfields(&fields, &e->fields))
			return 0;

		return new_struct_stype(ttracker, &fields, 0, 0,
					e->tok == TOK_UNION);
	}
	case node_t::SELECTOR_EXPR:
	{
		selector_expr_t *e = (selector_expr_t*)expr;
		ident_expr_t *ie = is_ident_expr(e->expr);
		sdecl_t *d = 0;
		if (ie)
			d = cur_scope->lookup(ie->val.c_str());
		if (!d || d->type != SDECL_IMPORT) {
			source_loc_range_t range = e->expr->source_loc_range();
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					range.beg, false, &range, 1,
					"non-module selector expression in type");
			diag->report(m);
			return 0;
		}
		import_sdecl_t *id = (import_sdecl_t*)d;
		unordered_map<std::string, sdecl_t*>::iterator it;
		it = id->decls.find(e->sel->val);
		if (it == id->decls.end()) {
			source_loc_range_t ranges[] = {
				e->expr->source_loc_range(),
				e->sel->source_loc_range()
			};
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					ranges[1].beg, false, ranges, 2,
					"undefined: %s.%s",
					ie->val.c_str(),
					e->sel->val.c_str());
			diag->report(m);
			return 0;
		}

		d = it->second;
		if (d->type != SDECL_TYPE) {
			source_loc_range_t ranges[] = {
				e->expr->source_loc_range(),
				e->sel->source_loc_range()
			};
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					ranges[1].beg, false, ranges, 2,
					"%s.%s is not a type",
					ie->val.c_str(),
					e->sel->val.c_str());
			diag->report(m);
			return 0;
		}

		return d->stype;
	}
	default:
		fprintf(stderr, "%d\n", expr->type);
		KRAWL_QASSERT(0);
		break;
	}
	return 0;
}

bool pass2_t::stmt_returns(node_t *stmt)
{
	switch (stmt->type) {
	case node_t::EXPR_STMT:
		return false;
	case node_t::ASSIGN_STMT:
		return false;
	case node_t::BLOCK_STMT:
	{
		// block statement is considered as returning, if one of its
		// statements return
		block_stmt_t *s = (block_stmt_t*)stmt;
		for (size_t i = 0, n = s->stmts.size(); i < n; ++i) {
			if (stmt_returns(s->stmts[i]))
				return true;
		}
		return false;
	}
	case node_t::RETURN_STMT:
		return true;
	case node_t::IFELSE_STMT:
	{
		// if else statement is considered as returning, if both of its
		// branches return
		ifelse_stmt_t *s = (ifelse_stmt_t*)stmt;
		if (!stmt_returns(s->body))
			return false;
		if (s->elsestmt && !stmt_returns(s->elsestmt))
			return false;
		return true;
	}
	case node_t::INCDEC_STMT:
		return false;
	case node_t::DECL_STMT:
		return false;
	case node_t::FOR_STMT:
		// for statement is always considered as non-returning, because
		// its "else" branch falls back into outer scope
		return false;
	case node_t::SWITCH_STMT:
	{
		// switch statement is considered as returning, if all of its
		// branches return
		switch_stmt_t *s = (switch_stmt_t*)stmt;
		bool has_default = false;
		for (size_t i = 0, n = s->body->stmts.size(); i < n; ++i) {
			node_t *cc = s->body->stmts[i];
			if (!stmt_returns(cc))
				return false;

			KRAWL_QASSERT(cc->type == node_t::SWITCH_STMT_CASE);
			if (((switch_stmt_case_t*)cc)->exprs.empty())
				has_default = true;
		}
		return has_default;
	}
	case node_t::SWITCH_STMT_CASE:
	{
		switch_stmt_case_t *s = (switch_stmt_case_t*)stmt;
		for (size_t i = 0, n = s->stmts.size(); i < n; ++i) {
			if (stmt_returns(s->stmts[i]))
				return true;
		}
		return false;
	}
	case node_t::FLOW_STMT:
		return false;
	default:
		KRAWL_ASSERT(false, "unexpected node type in stmt_returns"
			     ": %d\n", stmt->type);
		break;
	}
	return false;
}

value_stype_t pass2_t::typecheck_builtin_call_expr(call_expr_t *expr)
{
	ident_expr_t *e = is_ident_expr(expr->expr);
	KRAWL_QASSERT(e != 0);

	value_stype_t out;
	if (e->sdecl->name == "sizeof") {
		stype_t *ftargs = builtin_stypes[BUILTIN_VOID];
		if (!typecheck_call_expr_args(expr, &ftargs, 1, false))
			return value_stype_t();

		stype_t *ty = expr->args[0]->vst.stype;

		out.stype = builtin_stypes[BUILTIN_ABSTRACT_INT];
		out.value = value_t((unsigned int)ty->bits() / 8);
		return out;
	} else if (e->sdecl->name == "va_start" | e->sdecl->name == "va_end") {
		stype_t *ftargs = builtin_stypes[BUILTIN_VA_LIST_PTR];
		if (!typecheck_call_expr_args(expr, &ftargs, 1, false))
			return value_stype_t();

		out.stype = builtin_stypes[BUILTIN_VOID];
		return out;
	} else if (e->sdecl->name == "va_copy") {
		stype_t *ftargs[] = {
			builtin_stypes[BUILTIN_VA_LIST_PTR],
			builtin_stypes[BUILTIN_VA_LIST_PTR]
		};

		if (!typecheck_call_expr_args(expr, ftargs, 2, false))
			return value_stype_t();

		out.stype = builtin_stypes[BUILTIN_VOID];
		return out;
	} else if (e->sdecl->name == "va_arg") {
		stype_t *ftargs[] = {
			builtin_stypes[BUILTIN_VA_LIST_PTR],
			builtin_stypes[BUILTIN_VOID]
		};

		if (!typecheck_call_expr_args(expr, ftargs, 2, false))
			return value_stype_t();

		out.stype = expr->args[1]->vst.stype;
		return out;
	}
	return value_stype_t();
}

value_stype_t pass2_t::typecheck_basic_lit_expr(basic_lit_expr_t *expr)
{
	value_stype_t out;
	uint32_t u;

	switch (expr->tok) {
	case TOK_STRING:
		out.stype = builtin_stypes[BUILTIN_ABSTRACT_STRING];
		out.value = expr->val;
		break;
	case TOK_INT:
		out.stype = builtin_stypes[BUILTIN_ABSTRACT_INT];
		out.value = expr->val;
		break;
	case TOK_FLOAT:
		out.stype = builtin_stypes[BUILTIN_ABSTRACT_FLOAT];
		out.value = expr->val;
		break;
	case TOK_CHAR:
		out.stype = builtin_stypes[BUILTIN_ABSTRACT_INT];
		utf8_to_unicode(&u, expr->val.xstr);
		out.value = value_t(u);
		break;
	default:
		KRAWL_QASSERT(0);
		break;
	}

	return out;
}

value_stype_t pass2_t::typecheck_binary_expr(binary_expr_t *expr)
{
	value_stype_t out;
	value_stype_t lhs = typecheck_expr(expr->lhs);
	value_stype_t rhs = typecheck_expr(expr->rhs);
	if (!lhs.stype || !rhs.stype)
		return out; // type error happened somewhere up on tha stack

	stype_t *optype;
	out.stype = binop_compat(lhs.stype, rhs.stype, expr->tok, &optype);
	if (out.stype) {
		// type is found, let's try to make a value
		out.value = eval_binop(&lhs, &rhs, out.stype, optype, expr);
	} else {
		// no type, means type error
		source_loc_range_t ranges[] = {
			expr->lhs->source_loc_range(),
			expr->rhs->source_loc_range()
		};

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos, true, ranges, 2,
				"invalid operands to binary expression "
				"('%s' and '%s')",
				lhs.stype->to_string().c_str(),
				rhs.stype->to_string().c_str());
		diag->report(m);
	}
	return out;
}

value_stype_t pass2_t::typecheck_unary_expr(unary_expr_t *expr)
{
	value_stype_t out;
	value_stype_t operand = typecheck_expr(expr->expr);
	if (!operand.stype)
		return out; // type error happened somewhere up on tha stack

	out.stype = unop_compat(operand.stype, expr->tok, expr->expr, ttracker);
	if (out.stype) {
		// type is found, let's try to make a value
		out.value = eval_unop(&operand, out.stype, expr);
	} else {
		// no type, means type error
		source_loc_range_t ranges[] = {
			expr->expr->source_loc_range()
		};

		message_t *m;
		switch (expr->tok) {
		case TOK_AND:
			m = new_message(MESSAGE_ERROR,
					expr->pos, true, ranges, 1,
					"cannot take the address of "
					"non-addressable expression");
			break;
		case TOK_TIMES:
			m = new_message(MESSAGE_ERROR,
					expr->pos, true, ranges, 1,
					"illegal pointer dereference of "
					"a non-pointer type ('%s')",
					operand.stype->to_string().c_str());
			break;
		default:
			m = new_message(MESSAGE_ERROR,
					expr->pos, true, ranges, 1,
					"invalid operand to unary "
					"expression ('%s')",
					operand.stype->to_string().c_str());
			break;
		}
		diag->report(m);
	}
	return out;
}

value_stype_t pass2_t::typecheck_ident_expr(ident_expr_t *expr)
{
	sdecl_t *sd = cur_scope->lookup(expr->val.c_str());
	if (!sd) {
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos, true, 0, 0,
				"undefined: %s",
				expr->val.c_str());
		diag->report(m);
		return value_stype_t();
	}

	if (sd->type == SDECL_TYPE) {
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos, true, 0, 0,
				"type '%s' is not an expression",
				expr->val.c_str());
		diag->report(m);
		return value_stype_t();
	}

	// check if loop contains only functions, it's legal to
	// have a function recursion
	if (sd->incycle && (sd->type != SDECL_FUNC || !is_function_loop(sd))) {
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos, true, 0, 0,
				"typechecking loop:\n%s",
				report_recursion_stack(sd).c_str());
		diag->report(m);
		return value_stype_t();
	}

	expr->sdecl = sd;
	resolve_sdecl(sd);

	if (cur_func_decl != 0 && sd->scope->parent != 0 &&
	    sd->scope->parent == pkgscope)
	{
		if (sd->type == SDECL_VAR) {
			cur_func_decl->deps.insert((var_sdecl_t*)sd);
		} else if (sd->type == SDECL_FUNC) {
			func_sdecl_t *fsd = (func_sdecl_t*)sd;
			unordered_set<var_sdecl_t*>::iterator it, end;
			for (it = fsd->deps.begin(), end = fsd->deps.end(); it != end; ++it)
				cur_func_decl->deps.insert(*it);
		}
	}

	value_stype_t out;
	out.stype = sd->stype;

	if (sd->type == SDECL_CONST) {
		const_sdecl_t *csd = (const_sdecl_t*)sd;
		if (csd->iota == -1) {
			// iota itself, let's see if we're in a const decl
			// context
			if (!cur_const_decl) {
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						expr->pos, true, 0, 0,
						"iota is only defined in "
						"const declarations");
				diag->report(m);
				return value_stype_t();
			}
			KRAWL_QASSERT(cur_const_decl->iota >= 0);
			out.value = value_t(cur_const_decl->iota);
		} else
			out.value = csd->value;
	}
	return out;
}

value_stype_t pass2_t::typecheck_type_cast_expr(type_cast_expr_t *expr)
{
	value_stype_t out;
	value_stype_t operand = typecheck_expr(expr->expr);
	if (!operand.stype)
		return out; // type error happened somewhere up on tha stack

	out.stype = typegen(expr->ctype);
	if (!out.stype)
		return out; // error is somewhere else

	if (!can_typecast(operand.stype, out.stype)) {
		source_loc_range_t ranges[] = {
			expr->expr->source_loc_range(),
			expr->ctype->source_loc_range()
		};

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos_dot, true, ranges, 2,
				"type cast from '%s' to '%s' is not allowed",
				operand.stype->to_string().c_str(),
				out.stype->to_string().c_str());
		diag->report(m);
		return value_stype_t();
	}

	out.value = eval_type_cast(&operand.value, operand.stype, out.stype, expr);
	return out;
}

value_stype_t pass2_t::typecheck_type_expr(type_expr_t *expr)
{
	value_stype_t out;
	out.stype = typegen(expr->etype);

	// doing it here manually, because typecheck_expr denies type
	// expressions
	expr->vst = out;
	return out;
}

bool pass2_t::typecheck_call_expr_args(call_expr_t *expr,
				       stype_t **ftargs, size_t ftargs_n,
				       bool varargs)
{
	stype_vector_t args;
	size_t args_n = 0;

	// special case, function expects many arguments and call expr contains
	// only one function call for arguments
	if (ftargs_n > 1 && expr->args.size() == 1 &&
	    is_call_expr(expr->args[0]))
	{
		call_expr_t *c = is_call_expr(expr->args[0]);
		c->mexpected = ftargs_n;
		value_stype_t vst = typecheck_call_expr(c, true);
		if (!vst.stype) {
			expr->typeerror = true;
			return false;
		}

		c->vst = vst;

		// set up type args
		KRAWL_QASSERT(vst.stype->type == STYPE_FUNC);
		stype_vector_t *r = &vst.stype->as_func()->results;
		args.assign(r->begin(), r->end());
		args_n = args.size();
	}

	if (args_n == 0)
		args_n = expr->args.size();

	// check arguments size match
	bool match;
	if (varargs)
		match = ftargs_n <= args_n;
	else
		match = ftargs_n == args_n;

	if (!match) {
		expr->typeerror = true;
		error_args_mismatch(expr, ftargs_n);
		return false;
	}

	if (args.empty()) {
		args.reserve(expr->args.size());
		for (size_t i = 0, n = expr->args.size(); i < n; ++i) {
			value_stype_t cargvt;
			if (i < ftargs_n &&
			    ftargs[i] == builtin_stypes[BUILTIN_VOID] &&
			    expr->args[i]->type == node_t::TYPE_EXPR)
			{
				type_expr_t *te = (type_expr_t*)expr->args[i];
				cargvt = typecheck_type_expr(te);
			} else
				cargvt = typecheck_expr(expr->args[i]);
			if (!cargvt.stype) {
				expr->typeerror = true;
				return false;
			}

			args.push_back(cargvt.stype);
		}
	}

	// check argument types
	for (size_t i = 0, n = ftargs_n; i < n; ++i) {
		stype_t *fargt = ftargs[i];

		// skip this special case, it is used for allowing any type
		// (e.g. type expressions)
		if (fargt == builtin_stypes[BUILTIN_VOID])
			continue;

		// check for type compatibility
		if (!assignable(args[i], fargt)) {
			size_t j = i;
			if (j >= expr->args.size())
				j = expr->args.size() - 1;

			expr->typeerror = true;
			source_loc_range_t ranges[] = {
				expr->expr->source_loc_range(),
				expr->args[j]->source_loc_range()
			};
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					ranges[0].beg, true, &ranges[1], 1,
					"cannot use type '%s' as type '%s' "
					"in function argument",
					args[i]->to_string().c_str(),
					fargt->to_string().c_str());
			diag->report(m);
			return false;
		}
	}

	return true;
}

value_stype_t pass2_t::typecheck_call_expr(call_expr_t *expr, bool mok)
{
	if (expr->typeerror)
		return value_stype_t();

	value_stype_t out;
	value_stype_t callee = typecheck_expr(expr->expr);
	if (!callee.stype) {
		expr->typeerror = true;
		return out;
	}

	// if the callee is not a function
	if ((callee.stype->type & STYPE_FUNC) == 0) {
		expr->typeerror = true;
		source_loc_range_t ranges[] = {
			expr->expr->source_loc_range(),
		};
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				ranges[0].beg, false, ranges, 1,
				"cannot call non-function type '%s'",
				callee.stype->to_string().c_str());
		diag->report(m);
		return value_stype_t();
	}

	func_stype_t *ft = callee.stype->as_func();
	if (IS_STYPE_BUILTIN(ft))
		return typecheck_builtin_call_expr(expr);

	if (!typecheck_call_expr_args(expr,
				      &ft->args[0], ft->args.size(),
				      ft->varargs))
	{
		return value_stype_t();
	}

	// if multiple return values are not allowed, issue an error
	if (!mok && ft->results.size() > 1) {
		expr->typeerror = true;
		source_loc_range_t range = expr->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"multiple return values in a "
				"single-value context");
		diag->report(m);
		return value_stype_t();
	}

	if (mok) {
		KRAWL_QASSERT(expr->mexpected != 0);
		if (expr->mexpected != ft->results.size()) {
			expr->typeerror = true;
			source_loc_range_t range = expr->source_loc_range();
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					range.beg, false, &range, 1,
					"assignment count mismatch: %d = %d",
					expr->mexpected, ft->results.size());
			diag->report(m);
			return value_stype_t();
		}
	}

	if (ft->results.empty()) {
		out.stype = builtin_stypes[BUILTIN_VOID];
		return out;
	}

	// I return the type of a function itself for multiple return values.
	// It's a convention, should be dispatched further somewhere else.
	if (ft->results.size() > 1)
		out.stype = callee.stype;
	else
		out.stype = ft->results[0];
	return out;
}

value_stype_t pass2_t::typecheck_index_expr(index_expr_t *expr)
{
	value_stype_t out;
	value_stype_t op = typecheck_expr(expr->expr);
	if (!op.stype)
		return out;

	value_stype_t idx = typecheck_expr(expr->index);
	if (!idx.stype)
		return out;

	// operand must be a pointer or array
	if (!IS_STYPE_POINTER_OR_ARRAY(op.stype)) {
		source_loc_range_t range = expr->expr->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"index operand must be a pointer "
				"or an array ('%s')",
				op.stype->to_string().c_str());
		diag->report(m);
		return out;
	}

	// index must be an integer
	if (!IS_STYPE_INT(idx.stype)) {
		source_loc_range_t range = expr->index->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"index must be an integer ('%s')",
				idx.stype->to_string().c_str());
		diag->report(m);
		return out;
	}

	stype_t *et = op.stype->end_type();
	if (et->type & STYPE_POINTER) {
		pointer_stype_t *pst = et->as_pointer();
		out.stype = pst->points_to;
	} else {
		array_stype_t *ast = et->as_array();
		out.stype = ast->elem;
	}
	return out;
}

value_stype_t pass2_t::typecheck_compound_lit(compound_lit_t *expr)
{
	value_stype_t out;
	size_t save_array_ctx = array_ctx;
	array_ctx = expr->elts.size();

	if (expr->etype) {
		out.stype = typegen(expr->etype);
		if (!out.stype) {
			array_ctx = save_array_ctx;
			return value_stype_t();
		}
	} else
		out.stype = builtin_stypes[BUILTIN_ABSTRACT_COMPOUND];

	// it's just a premature typecheck, the actual typecheck will be
	// performed later, on realization phase
	for (size_t i = 0, n = expr->elts.size(); i < n; ++i) {
		typecheck_expr(expr->elts[i]);
		if (!expr->elts[i]->vst.stype) {
			array_ctx = save_array_ctx;
			return value_stype_t();
		}
	}

	array_ctx = save_array_ctx;
	return out;
}

value_stype_t pass2_t::typecheck_selector_expr(selector_expr_t *expr)
{
	value_stype_t op = typecheck_expr(expr->expr);
	if (!op.stype)
		return value_stype_t();

	if (IS_STYPE_MODULE(op.stype)) {
		ident_expr_t *ident = is_ident_expr(expr->expr);
		KRAWL_QASSERT(ident && ident->sdecl->type == SDECL_IMPORT);

		import_sdecl_t *id = (import_sdecl_t*)ident->sdecl;
		unordered_map<std::string, sdecl_t*>::iterator it;
		it = id->decls.find(expr->sel->val);
		if (it == id->decls.end()) {
			source_loc_range_t ranges[] = {
				expr->expr->source_loc_range(),
				expr->sel->source_loc_range()
			};
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					ranges[1].beg, false, ranges, 2,
					"undefined: %s.%s",
					ident->val.c_str(),
					expr->sel->val.c_str());
			diag->report(m);
			return value_stype_t();
		}

		if (!id->used) {
			id->used = true;
			used_extern_sdecls.push_back(id);
		}
		if (!it->second->inited)
			it->second->inited = true;
		op.stype = it->second->stype;
		if (it->second->type == SDECL_CONST) {
			const_sdecl_t *csd = (const_sdecl_t*)it->second;
			op.value = csd->value;
		}
		return op;
	}

	bool is_pointer_to_struct = false;
	if (IS_STYPE_POINTER(op.stype)) {
		pointer_stype_t *pst = op.stype->as_pointer();
		if (IS_STYPE_STRUCT_OR_UNION(pst->points_to))
			is_pointer_to_struct = true;
	}

	if (!IS_STYPE_STRUCT_OR_UNION(op.stype) && !is_pointer_to_struct) {
		source_loc_range_t range = expr->expr->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"selector operand must be a struct or a union "
				"or a pointer to struct");
		diag->report(m);
		return value_stype_t();
	}

	struct_stype_t *sst;
	if (IS_STYPE_STRUCT_OR_UNION(op.stype))
		sst = op.stype->as_struct();
	else if (is_pointer_to_struct)
		sst = op.stype->as_pointer()->points_to->as_struct();

	for (size_t i = 0, n = sst->fields.size(); i < n; ++i) {
		struct_field_t *f = &sst->fields[i];
		if (f->name == expr->sel->val) {
			op.stype = f->type;
			expr->idx = (size_t)i;
			return op;
		}
	}

	source_loc_range_t ranges[] = {
		expr->expr->source_loc_range(),
		expr->sel->source_loc_range()
	};
	message_t *m;
	m = new_message(MESSAGE_ERROR,
			ranges[1].beg, false, ranges, 2,
			"struct doesn't have a field '%s'",
			expr->sel->val.c_str());
	diag->report(m);
	return value_stype_t();
}

value_stype_t pass2_t::typecheck_expr(node_t *expr)
{
	switch (expr->type) {
	case node_t::BINARY_EXPR:
	{
		binary_expr_t *e = (binary_expr_t*)expr;
		e->vst = typecheck_binary_expr(e);
		return e->vst;
	}
	case node_t::UNARY_EXPR:
	{
		unary_expr_t *e = (unary_expr_t*)expr;
		e->vst = typecheck_unary_expr(e);
		return e->vst;
	}
	case node_t::BASIC_LIT_EXPR:
	{
		basic_lit_expr_t *e = (basic_lit_expr_t*)expr;
		e->vst = typecheck_basic_lit_expr(e);
		return e->vst;
	}
	case node_t::IDENT_EXPR:
	{
		ident_expr_t *e = (ident_expr_t*)expr;
		e->vst = typecheck_ident_expr(e);
		return e->vst;
	}
	case node_t::SELECTOR_EXPR:
	{
		selector_expr_t *e = (selector_expr_t*)expr;
		e->vst = typecheck_selector_expr(e);
		return e->vst;
	}
	case node_t::INDEX_EXPR:
	{
		index_expr_t *e = (index_expr_t*)expr;
		e->vst = typecheck_index_expr(e);
		return e->vst;
	}
	case node_t::CALL_EXPR:
	{
		call_expr_t *e = (call_expr_t*)expr;
		e->vst = typecheck_call_expr(e);
		return e->vst;
	}
	case node_t::PAREN_EXPR:
	{
		paren_expr_t *e = (paren_expr_t*)expr;
		e->vst = typecheck_expr(e->expr);
		return e->vst;
	}
	case node_t::TYPE_CAST_EXPR:
	{
		type_cast_expr_t *e = (type_cast_expr_t*)expr;
		e->vst = typecheck_type_cast_expr(e);
		return e->vst;
	}
	case node_t::TYPE_EXPR:
	{
		type_expr_t *e = (type_expr_t*)expr;
		source_loc_range_t range = e->etype->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				e->pos, true, &range, 1,
				"type expression outside of a built-in function");
		diag->report(m);
		return value_stype_t();
	}
	case node_t::COMPOUND_LIT:
	{
		compound_lit_t *e = (compound_lit_t*)expr;
		e->vst = typecheck_compound_lit(e);
		return e->vst;
	}
	default:
		KRAWL_QASSERT(!"bad expression type");
	}
	return value_stype_t();
}

void pass2_t::pass(std::vector<const char*> *pkgdecls)
{
	cur_const_decl = 0;
	cur_func_decl = 0;
	cur_scope = 0;
	cur_switch_stmt = 0;
	recursion_stack.clear();
	array_ctx = 0;
	used_extern_sdecls.clear();

	// walk types first and check for type loops
	for (size_t i = 0, n = pkgdecls->size(); i < n; ++i) {
		sdecl_t *sd = pkgscope->sdecls[pkgdecls->at(i)];
		if (sd->type != SDECL_TYPE)
			continue;

		resolve_sdecl(sd);

		if (!sd->stype || sd->typeerror)
			continue;

		check_type_for_size_loops(diag, sd->stype->as_named());

		if (!sd->typeerror && IS_STYPE_STRUCT_OR_UNION(sd->stype)) {
			struct_stype_t *sst = sd->stype->as_struct();
			fix_structs_alignment(sst);
		}
	}

	for (size_t i = 0, n = pkgdecls->size(); i < n; ++i) {
		sdecl_t *sd = pkgscope->sdecls[pkgdecls->at(i)];
		if (sd->type == SDECL_TYPE)
			continue;
		resolve_sdecl(sd);
	}
}

stype_t *pass2_t::typecheck_var_init(node_t *init, int index)
{
	if (index != -1) {
		// special case, init expr must be a call expr
		call_expr_t *c = is_call_expr(init);
		KRAWL_QASSERT(c != 0);

		value_stype_t vst;
		if (!c->vst.stype) {
			vst = typecheck_call_expr(c, true);
			if (!vst.stype)
				return 0;
			// do the typecheck_expr's job here
			c->vst = vst;
		} else
			vst = c->vst;

		// our type
		return vst.stype->as_func()->results[index];
	}

	value_stype_t vst = typecheck_expr(init);
	return vst.stype;
}

static void pass2_declare(std::vector<sdecl_t*> *d, void *data)
{
	pass2_t *p = (pass2_t*)data;
	p->declare_many(d);
}

void pass2_t::declare_many(std::vector<sdecl_t*> *decls)
{
	KRAWL_QASSERT(!decls->empty());
	KRAWL_QASSERT(cur_scope != 0);

	// var a, b = 10, 10;
	// both 'a' and 'b' has scope which doesn't have them
	// so, we generate 'a' and 'b', assign 'cur_scope' then
	// we do typecheck and then we add them to scope
	//
	// type A *A;
	// for type we generate 'A', assign 'cur_scope', add to scope and then
	// typecheck

	// type is the same for all decls
	sdecl_type_t type = decls->at(0)->type;

	for (size_t i = 0, n = decls->size(); i < n; ++i) {
		sdecl_t *d = decls->at(i);
		if (cur_scope->has(d->name.c_str())) {
			redeclared_error(d->get_ident(), diag);
			// erase decl in vector, I'll skip it later
			decls->at(i) = 0;
			continue;
		}

		d->scope = cur_scope;
	}

	// shortcut for type
	if (type == SDECL_TYPE) {
		// type can only be alone anyway
		sdecl_t *d = decls->at(0);
		if (d) {
			cur_scope->add(d);
			resolve_sdecl(d);
			return;
		}
	}

	// resolve all the decls
	for (size_t i = 0, n = decls->size(); i < n; ++i) {
		sdecl_t *d = decls->at(i);
		if (d)
			resolve_sdecl(d);
	}

	// add them to scope
	for (size_t i = 0, n = decls->size(); i < n; ++i) {
		sdecl_t *d = decls->at(i);
		if (d) {
			// shouldn't fail at this point
			bool ok = cur_scope->add(d);
			KRAWL_QASSERT(ok == true);
		}
	}
}

void pass2_t::declare_function_vars(func_type_t *ft, func_stype_t *fst)
{
	size_t num = 0;
	for (size_t i = 0, n = ft->args.size(); i < n; ++i) {
		field_t *f = ft->args[i];
		for (size_t j = 0, m = f->names.size(); j < m; ++j, ++num) {
			ident_expr_t *name = f->names[j];
			stype_t *type = fst->args[num];
			node_t *vtype = f->type;
			sdecl_t *sd = new_typed_var_sdecl(dtracker, name,
							  type, vtype);
			name->sdecl = sd;
			sd->scope = cur_scope;
			if (!cur_scope->add(sd))
				redeclared_error(name, diag);
		}
	}
	num = 0;
	for (size_t i = 0, n = ft->results.size(); i < n; ++i) {
		field_t *f = ft->results[i];
		for (size_t j = 0, m = f->names.size(); j < m; ++j, ++num) {
			ident_expr_t *name = f->names[j];
			stype_t *type = fst->results[num];
			node_t *vtype = f->type;
			var_sdecl_t *sd = new_typed_var_sdecl(dtracker, name,
							      type, vtype);
			name->sdecl = sd;
			sd->namedret = num;
			sd->scope = cur_scope;
			if (!cur_scope->add(sd))
				redeclared_error(name, diag);
		}
	}
}

void pass2_t::typecheck_block_stmt(block_stmt_t *stmt)
{
	for (size_t i = 0, n = stmt->stmts.size(); i < n; ++i)
		typecheck_stmt(stmt->stmts.at(i));
}

void pass2_t::typecheck_decl_stmt(decl_stmt_t *stmt)
{
	const char *what = 0;
	switch (stmt->decl->type) {
	case node_t::IMPORT_DECL:
		what = "import declaration";
		break;
	case node_t::FUNC_DECL:
		what = "function declaration";
		break;
	default:
		declare_decl(stmt->decl, dtracker, pass2_declare, this);
		if (stmt->decl->type == node_t::TYPE_DECL) {
			type_decl_t *td = (type_decl_t*)stmt->decl;
			for (size_t i = 0, n = td->specs.size(); i < n; ++i) {
				type_sdecl_t *tsd;
				tsd = (type_sdecl_t*)td->specs[i]->name->sdecl;
				check_declared_type_sdecl(diag, tsd);
			}
		}
		return;
	}

	KRAWL_QASSERT(what != 0);

	source_loc_range_t range = stmt->decl->source_loc_range();
	message_t *m;
	m = new_message(MESSAGE_ERROR,
			range.beg, true, 0, 0,
			"%s is only allowed in package block", what);
	diag->report(m);
}

void pass2_t::typecheck_return_stmt(return_stmt_t *stmt)
{
	stype_vector_t returns;
	size_t returns_n = 0;

	// special case, function expects many return values and return
	// statement contains only one function call
	if (cur_func_decl->n_return_values() > 1 && stmt->returns.size() == 1 &&
	    is_call_expr(stmt->returns[0]))
	{
		call_expr_t *c = is_call_expr(stmt->returns[0]);
		c->mexpected = cur_func_decl->n_return_values();
		value_stype_t vst = typecheck_call_expr(c, true);
		if (!vst.stype)
			return;

		c->vst = vst;

		KRAWL_QASSERT(vst.stype->type == STYPE_FUNC);
		stype_vector_t *r = &vst.stype->as_func()->results;
		returns.assign(r->begin(), r->end());
		returns_n = returns.size();
	}

	// For a return statement a number of return values should match the
	// number in function type. If function has named return types, then
	// zero value is also valid.

	if (returns_n == 0)
		returns_n = stmt->returns.size();

	bool matches = false;
	if (cur_func_decl->n_return_values() == (int)returns_n)
		matches = true;
	if (cur_func_decl->has_named_return_values() && (int)stmt->returns.empty())
		matches = true;

	if (!matches) {
		std::vector<source_loc_range_t> ranges;
		ranges.reserve(stmt->returns.size());
		for (size_t i = 0, n = stmt->returns.size(); i < n; ++i)
			ranges.push_back(stmt->returns[i]->source_loc_range());

		const char *what = "not enough";
		if (cur_func_decl->n_return_values() < stmt->returns.size())
			what = "too many";

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				stmt->pos, true, &ranges[0], ranges.size(),
				"%s arguments to return", what);
		diag->report(m);
		return;
	}

	// now, the typecheck
	if (returns.empty()) {
		returns.reserve(stmt->returns.size());
		for (size_t i = 0, n = stmt->returns.size(); i < n; ++i) {
			value_stype_t rval = typecheck_expr(stmt->returns[i]);
			if (!rval.stype)
				return;
			returns.push_back(rval.stype);
		}
	}

	stype_vector_t *rtypes = cur_func_decl->return_types();
	for (size_t i = 0, n = returns.size(); i < n; ++i) {
		if (!assignable(returns[i], rtypes->at(i))) {
			size_t j = i;
			if (j >= stmt->returns.size())
				j = stmt->returns.size() - 1;

			source_loc_range_t range = stmt->returns[j]->source_loc_range();
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					stmt->pos, true, &range, 1,
					"cannot use type '%s' as type '%s' "
					"in return statement",
					returns[i]->to_string().c_str(),
					rtypes->at(i)->to_string().c_str());
			diag->report(m);
			return;
		}

	}

	for (size_t i = 0, n = stmt->returns.size(); i < n; ++i)
		realize_expr_type(stmt->returns[i], rtypes->at(i));
}

void pass2_t::typecheck_compound_assign_stmt(assign_stmt_t *stmt)
{
	if (stmt->rhs.size() != 1 || stmt->lhs.size() != 1) {
		std::vector<source_loc_range_t> ranges;
		for (size_t i = 0, n = stmt->rhs.size(); i < n; ++i)
			ranges.push_back(stmt->rhs[i]->source_loc_range());
		for (size_t i = 0, n = stmt->lhs.size(); i < n; ++i)
			ranges.push_back(stmt->lhs[i]->source_loc_range());

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				stmt->pos, true, &ranges[0], ranges.size(),
				"Only 1 expression is allowed on both "
				"sides of the compound assignment statement");
		diag->report(m);
		return;
	}

	value_stype_t lhs = typecheck_expr(stmt->lhs[0]);
	value_stype_t rhs = typecheck_expr(stmt->rhs[0]);
	if (!lhs.stype || !rhs.stype)
		return;

	if (!addressable(stmt->lhs[0])) {
		source_loc_range_t range = stmt->lhs[0]->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				stmt->pos, true, &range, 1,
				"non-addressable expression on the "
				"left-handed side of assignment");
		diag->report(m);
		return;
	}

	int op = compound_assignment_to_binary_tok(stmt->tok);
	if (!binop_compat(lhs.stype, rhs.stype, op, 0)) {
		// no type, means type error
		source_loc_range_t ranges[] = {
			stmt->lhs[0]->source_loc_range(),
			stmt->rhs[0]->source_loc_range()
		};

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				stmt->pos, true, ranges, 2,
				"invalid operands to compound assignment "
				"statement ('%s' and '%s')",
				lhs.stype->to_string().c_str(),
				rhs.stype->to_string().c_str());
		diag->report(m);
		return;
	}

	realize_expr_type(stmt->lhs[0], 0);

	stype_t *ctx = lhs.stype;
	if (op == TOK_SHIFTL || op == TOK_SHIFTR)
		ctx = builtin_named_stypes[BUILTIN_UINT32];

	realize_expr_type(stmt->rhs[0], ctx);
}

void pass2_t::typecheck_assign_stmt(assign_stmt_t *stmt)
{
	if (stmt->tok != TOK_DECLARIZE && stmt->tok != TOK_ASSIGN) {
		// *= += -= /=, etc.
		typecheck_compound_assign_stmt(stmt);
		return;
	}

	// set mexpected for call expr case
	if (stmt->rhs.size() == 1 && stmt->lhs.size() > 1) {
		call_expr_t *c = is_call_expr(stmt->rhs[0]);
		c->mexpected = stmt->lhs.size();
	}
	// 1. typecheck all non-declaration expressions on the LHS
	//    (they must be addressable)
	// 2. typecheck all init expressions on the RHS for non-decls
	//    (they must be assignable to LHS counterpart)
	// 3. realize RHS expression here as well
	for (size_t i = 0, n = stmt->lhs.size(); i < n; ++i) {
		node_t *e = stmt->lhs[i];

		// skip declarations in ':=' case
		if (e->type == node_t::IDENT_EXPR && stmt->tok == TOK_DECLARIZE)
			continue;

		// 1
		value_stype_t vst = typecheck_expr(e);
		if (!vst.stype)
			continue;

		if (!addressable(e)) {
			source_loc_range_t range = e->source_loc_range();
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					stmt->pos, true, &range, 1,
					"non-addressable expression on the "
					"left-handed side of assignment");
			diag->report(m);
			continue;
		}

		// we need to do realize for some cases, like:
		// array[5] = 10; // constant '5' needs to be realized
		realize_expr_type(e, 0);

		// 2
		node_t *val;
		int index = -1;
		if (stmt->rhs.size() == 1 && stmt->lhs.size() > 1) {
			index = i;
			val = stmt->rhs[0];
		} else {
			val = stmt->rhs[i];
		}

		stype_t *type = typecheck_var_init(val, index);
		if (!type)
			continue;

		if (!assignable(type, vst.stype)) {
			source_loc_range_t ranges[] = {
				e->source_loc_range(),
				val->source_loc_range()
			};

			message_t *m;
			m = new_message(MESSAGE_ERROR,
					stmt->pos, true, ranges, 2,
					"'%s' is not assignable to '%s'",
					type->to_string().c_str(),
					vst.stype->to_string().c_str());
			diag->report(m);
			continue;
		}

		// 3
		if (index != -1) {
			if (index == 0)
				realize_expr_type(val, vst.stype);
		} else
			realize_expr_type(val, vst.stype);
	}

	declare_decl(stmt, dtracker, pass2_declare, this);
}

void pass2_t::typecheck_incdec_stmt(incdec_stmt_t *stmt)
{
	const char *what = "increment";
	if (stmt->tok == TOK_DEC)
		what = "decrement";

	// incdec expression must be addressable and must have an int type
	value_stype_t vst = typecheck_expr(stmt->expr);
	if (!vst.stype)
		return;

	if (!addressable(stmt->expr)) {
		source_loc_range_t range = stmt->expr->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"non-addressable expression as %s "
				"statement operand", what);
		diag->report(m);
		return;
	}

	if (!IS_STYPE_POINTER_OR_INT(vst.stype)) {
		source_loc_range_t range = stmt->expr->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"%s statement operand must be a "
				"pointer or an integer ('%s')",
				what, vst.stype->to_string().c_str());
		diag->report(m);
		return;
	}

	realize_expr_type(stmt->expr, 0);
}

void pass2_t::typecheck_ifelse_stmt(ifelse_stmt_t *stmt)
{
	// 1. typecheck 'cond', must have a bool type, needs realization (ctx == 0)
	value_stype_t vst = typecheck_expr(stmt->cond);
	if (vst.stype) {
		if (!IS_STYPE_BOOL(vst.stype)) {
			source_loc_range_t range = stmt->cond->source_loc_range();
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					range.beg, false, &range, 1,
					"condition in an if statement must "
					"have a boolean type ('%s')",
					vst.stype->to_string().c_str());
			diag->report(m);
			return;
		} else
			realize_expr_type(stmt->cond, 0);
	}

	// 2. typecheck body stmt (needs block setup)
	scope_block_t *save_cur_scope = cur_scope;
	cur_scope = new_scope_block(stracker, cur_scope);

	typecheck_block_stmt(stmt->body);

	cur_scope = save_cur_scope;

	// 3. if exists, typecheck else stmt
	if (stmt->elsestmt)
		typecheck_stmt(stmt->elsestmt);
}

void pass2_t::typecheck_for_stmt(for_stmt_t *stmt)
{
	// 0. introduce a new scope block
	scope_block_t *save_cur_scope = cur_scope;
	cur_scope = new_scope_block(stracker, cur_scope);

	// 1. typecheck 'init' statement if any
	if (stmt->init)
		typecheck_stmt(stmt->init);

	// 2. typecheck 'cond', must have a bool type, needs realization (ctx == 0)
	if (stmt->cond) {
		value_stype_t vst = typecheck_expr(stmt->cond);
		if (vst.stype) {
			if (!IS_STYPE_BOOL(vst.stype)) {
				source_loc_range_t range;
				range = stmt->cond->source_loc_range();
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						range.beg, false, &range, 1,
						"condition in a for statement "
						"must have a boolean type ('%s')",
						vst.stype->to_string().c_str());
				diag->report(m);
			} else
				realize_expr_type(stmt->cond, 0);
		}
	}

	// 3. typecheck 'post' statement if any
	if (stmt->post)
		typecheck_stmt(stmt->post);

	// 4. typecheck body
	typecheck_block_stmt(stmt->body);

	cur_scope = save_cur_scope;
}

void pass2_t::typecheck_switch_stmt_case(switch_stmt_case_t *stmt)
{
	KRAWL_QASSERT(cur_switch_stmt != 0);
	node_t *se = cur_switch_stmt->expr;
	for (size_t i = 0, n = stmt->exprs.size(); i < n; ++i) {
		node_t *e = stmt->exprs[i];
		value_stype_t vst = typecheck_expr(e);
		if (!vst.stype || !se->vst.stype)
			continue;

		if (vst.value.type == VALUE_NONE)
			cur_switch_stmt->constant = false;

		if (!binop_compat(vst.stype, se->vst.stype, TOK_EQ, 0)) {
			source_loc_range_t range = e->source_loc_range();
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					range.beg, false, &range, 1,
					"incompatible type in a switch "
					"case clause ('%s' and '%s')",
					se->vst.stype->to_string().c_str(),
					vst.stype->to_string().c_str());
			diag->report(m);
		} else
			realize_expr_type(e, se->vst.stype);
	}

	for (size_t i = 0, n = stmt->stmts.size(); i < n; ++i)
		typecheck_stmt(stmt->stmts.at(i));
}

void pass2_t::typecheck_switch_stmt(switch_stmt_t *stmt)
{
	// 0. introduce a new scope block
	scope_block_t *save_cur_scope = cur_scope;
	cur_scope = new_scope_block(stracker, cur_scope);

	// 1. typecheck 'expr'
	if (stmt->expr) {
		value_stype_t vst = typecheck_expr(stmt->expr);
		if (vst.stype) {
			if (!binop_compat(vst.stype, vst.stype, TOK_EQ, 0)) {
				source_loc_range_t range;
				range = stmt->expr->source_loc_range();
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						range.beg, false, &range, 1,
						"switch statement expression must "
						"be comparable");
				diag->report(m);

				// to silence further errors
				stmt->expr->vst.stype = 0;
			} else
				realize_expr_type(stmt->expr, 0);
		}
	}

	// 3. introduce current switch statement
	switch_stmt_t *save_switch_stmt = cur_switch_stmt;
	cur_switch_stmt = stmt;

	// 4. typecheck body
	typecheck_block_stmt(stmt->body);

	cur_switch_stmt = save_switch_stmt;
	cur_scope = save_cur_scope;
}

void pass2_t::typecheck_stmt(node_t *stmt)
{
	switch (stmt->type) {
	case node_t::EXPR_STMT:
	{
		expr_stmt_t *s = (expr_stmt_t*)stmt;
		value_stype_t vst = typecheck_expr(s->expr);
		if (vst.stype)
			realize_expr_type(s->expr, 0);
		break;
	}
	case node_t::DECL_STMT:
	{
		decl_stmt_t *s = (decl_stmt_t*)stmt;
		typecheck_decl_stmt(s);
		break;
	}
	case node_t::BLOCK_STMT:
	{
		// here we expect generic "stmt" and if we got block stmt,
		// it's our responsibility to introduce new scope block
		scope_block_t *save_cur_scope = cur_scope;
		cur_scope = new_scope_block(stracker, cur_scope);

		block_stmt_t *s = (block_stmt_t*)stmt;
		typecheck_block_stmt(s);

		cur_scope = save_cur_scope;
		break;
	}
	case node_t::RETURN_STMT:
	{
		return_stmt_t *s = (return_stmt_t*)stmt;
		typecheck_return_stmt(s);
		break;
	}
	case node_t::ASSIGN_STMT:
	{
		assign_stmt_t *s = (assign_stmt_t*)stmt;
		typecheck_assign_stmt(s);
		break;
	}
	case node_t::INCDEC_STMT:
	{
		incdec_stmt_t *s = (incdec_stmt_t*)stmt;
		typecheck_incdec_stmt(s);
		break;
	}
	case node_t::IFELSE_STMT:
	{
		ifelse_stmt_t *s = (ifelse_stmt_t*)stmt;
		typecheck_ifelse_stmt(s);
		break;
	}
	case node_t::FOR_STMT:
	{
		for_stmt_t *s = (for_stmt_t*)stmt;
		typecheck_for_stmt(s);
		break;
	}
	case node_t::SWITCH_STMT_CASE:
	{
		scope_block_t *save_cur_scope = cur_scope;
		cur_scope = new_scope_block(stracker, cur_scope);

		switch_stmt_case_t *s = (switch_stmt_case_t*)stmt;
		typecheck_switch_stmt_case(s);

		cur_scope = save_cur_scope;
		break;
	}
	case node_t::SWITCH_STMT:
	{
		switch_stmt_t *s = (switch_stmt_t*)stmt;
		typecheck_switch_stmt(s);
		break;
	}
	// TODO: these should be only in controlable structures
	case node_t::FLOW_STMT:
		break;
	default:
		KRAWL_QASSERT(!"bad statement type");
	}
}

void pass2_t::resolve_sdecl(sdecl_t *d)
{
	if (d->typeerror || d->stype)
		return;

	recursion_stack.push_back(d);
	scope_block_t *save_cur_scope = cur_scope;
	cur_scope = d->scope;

	switch (d->type) {
	case SDECL_CONST:
	{
		const_sdecl_t *save_cur = cur_const_decl;

		const_sdecl_t *csd = (const_sdecl_t*)d;
		source_loc_range_t ranges[] = {
			csd->ident->source_loc_range(),
			csd->init->source_loc_range()
		};

		// begin typecheck
		cur_const_decl = csd;
		csd->incycle = true;

		value_stype_t vst = typecheck_expr(csd->init);
		if (!vst.stype) {
			// type error, it should be reported somewhere
			// in the typecheck_expr
			csd->typeerror = true;
		} else if (vst.value.type == VALUE_NONE) {
			// const expression must have a constant value
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					csd->pos, true, ranges, 2,
					"const initializer must be constant");
			diag->report(m);
		}

		if (csd->ctype) {
			csd->stype = typegen(csd->ctype);
			if (!csd->stype) {
				csd->typeerror = true;
			} else if (assignable(vst.stype, csd->stype)) {
				csd->value = eval_assignment(&vst.value, vst.stype,
							     csd->stype, csd->ident,
							     csd->pos, csd->init);
			} else {
				csd->typeerror = true;
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						csd->pos, true, ranges, 2,
						"'%s' is not assignable to '%s'",
						vst.stype->to_string().c_str(),
						csd->stype->to_string().c_str());
				diag->report(m);
				csd->stype = 0;
			}
		} else {
			csd->stype = vst.stype;
			csd->value = vst.value;
		}

		// end typecheck
		csd->incycle = false;
		cur_const_decl = save_cur;
		break;
	}
	case SDECL_VAR:
	{
		size_t save_array_ctx = array_ctx;

		// begin typecheck
		var_sdecl_t *vsd = (var_sdecl_t*)d;
		vsd->incycle = true;

		// typecheck init expression if any
		if (vsd->init) {
			vsd->stype = typecheck_var_init(vsd->init, vsd->index);
			if (!vsd->stype)
				vsd->typeerror = true;
			else if (IS_STYPE_COMPOUND_OR_ARRAY(vsd->stype))
				// if there are no type error, we also need to
				// setup array_ctx here
				array_ctx = size_from_array_or_compound(vsd->init);
		}

		if (vsd->vtype) {
			// we have an own type here, let's gen it and see if
			// 'init' expression is compatible
			stype_t *owntype = typegen(vsd->vtype);
			if (!owntype) {
				vsd->typeerror = true;
			} else if (vsd->stype) {
				// vsd->stype was set by var init here
				// check for assignability
				if (assignable(vsd->stype, owntype)) {
					// all ok
					vsd->stype = owntype;
				} else {
					source_loc_range_t ranges[] = {
						vsd->ident->source_loc_range(),
						vsd->init->source_loc_range()
					};
					vsd->typeerror = true;
					message_t *m;
					m = new_message(MESSAGE_ERROR,
							vsd->pos, true, ranges, 2,
							"'%s' is not assignable to '%s'",
							vsd->stype->to_string().c_str(),
							owntype->to_string().c_str());
					diag->report(m);
					vsd->stype = 0;
				}
			} else
				vsd->stype = owntype;
		}

		if (vsd->init && !vsd->typeerror) {
			stype_t *ctx = vsd->vtype ? vsd->stype : 0;
			if (vsd->index != -1) {
				// realize call expr only once for mutliple
				// return values case
				if (vsd->index == 0)
					realize_expr_type(vsd->init, ctx);

				stype_t *t = vsd->init->vst.stype;
				KRAWL_QASSERT(t != 0 && t->type == STYPE_FUNC);
				if (IS_STYPE_ABSTRACT(vsd->stype))
					vsd->stype = t->as_func()->results[vsd->index];
			} else {
				realize_expr_type(vsd->init, ctx);
				if (IS_STYPE_ABSTRACT(vsd->stype))
					vsd->stype = vsd->init->vst.stype;
			}
		}

		// end typecheck
		vsd->incycle = false;
		array_ctx = save_array_ctx;
		break;
	}
	case SDECL_TYPE:
	{
		type_sdecl_t *tsd = (type_sdecl_t*)d;

		// Create named type before 'real' type is actually known. I
		// need that because it is possible to have recursive types.
		tsd->stype = new_named_stype(ttracker, uid,
					     tsd->spec->name->val.c_str(),
					     0);
		named_stype_t *nst = tsd->stype->as_named();
		nst->decl = tsd;

		stype_t *rt = typegen(tsd->spec->type);
		if (rt) {
			nst->assign_real(rt);
		} else {
			// type error, it should be reported somewhere else
			tsd->typeerror = true;
			tsd->stype = 0;
		}

		break;
	}
	case SDECL_FUNC:
	{
		func_sdecl_t *fsd = (func_sdecl_t*)d;
		fsd->incycle = true;

		func_stype_t *ft = typegen(fsd->decl->ftype)->as_func();
		KRAWL_QASSERT(IS_STYPE_FUNC(ft));
		if (ft) {
			fsd->stype = ft;
		} else {
			fsd->typeerror = true;
		}

		if (fsd->decl->body && !fsd->typeerror) {
			scope_block_t *save_cur_scope = cur_scope;
			cur_scope = new_scope_block(stracker, cur_scope);

			func_sdecl_t *save_cur_func_decl = cur_func_decl;
			cur_func_decl = fsd;

			declare_function_vars(fsd->decl->ftype, ft);
			typecheck_block_stmt(fsd->decl->body);

			if (!ft->results.empty() &&
			    !fsd->has_named_return_values() &&
			    !stmt_returns(fsd->decl->body))
			{
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						fsd->decl->body->pos_rcb, true, 0, 0,
						"function ends without a return statement");
				diag->report(m);
				fsd->typeerror = true;
			}

			cur_func_decl = save_cur_func_decl;
			cur_scope = save_cur_scope;
		}

		fsd->incycle = false;
		break;
	}
	case SDECL_IMPORT:
	{
		import_sdecl_t *isd = (import_sdecl_t*)d;
		isd->stype = builtin_stypes[BUILTIN_ABSTRACT_MODULE];
		break;
	}
	default:
		break;
	}

	cur_scope = save_cur_scope;
	recursion_stack.pop_back();
}

bool pass2_t::is_function_loop(sdecl_t *f)
{
	for (int i = recursion_stack.size()-1; i >= 0; --i) {
		if (recursion_stack[i] == f)
			return true;
		if (recursion_stack[i]->type != SDECL_FUNC)
			return false;
	}
	KRAWL_QASSERT(!"unreachable");
	return false;
}

size_t pass2_t::find_loop_start(sdecl_t *d)
{
	for (int i = recursion_stack.size()-1; i >= 0; --i) {
		if (recursion_stack[i] == d)
			return (size_t)i;
	}
	KRAWL_QASSERT(!"unreachable");
	return 0;
}

std::string pass2_t::report_recursion_stack(sdecl_t *d)
{
	std::string out;
	for (size_t i = find_loop_start(d), n = recursion_stack.size(); i < n; ++i)
		cppsprintf(&out, "\t'%s' refers to\n",
			   recursion_stack[i]->name.c_str());
	out.resize(out.size()-1); // remove latest '\n'
	cppsprintf(&out, " '%s'", d->name.c_str());
	return out;
}

void pass2_t::realize_unary_expr_type(unary_expr_t *expr, stype_t *ctx)
{
	value_stype_t *vst = &expr->vst;
	realize_expr_type(expr->expr, ctx);

	// we only care about new type if an old one was abstract
	if (IS_STYPE_ABSTRACT(vst->stype))
		vst->stype = expr->expr->vst.stype;
}

void pass2_t::realize_binary_expr_type(binary_expr_t *e, stype_t *ctx)
{
	bool is_shift = e->tok == TOK_SHIFTL || e->tok == TOK_SHIFTR;
	bool is_cmp = e->tok == TOK_NEQ ||
		      e->tok == TOK_EQ ||
		      e->tok == TOK_LT ||
		      e->tok == TOK_GT ||
		      e->tok == TOK_LE ||
		      e->tok == TOK_GE;

	// at this point it means we're in a non-value expression and we need
	// to recurse, but binary expression affects the way we choose context
	value_stype_t *vst = &e->vst;
	if (IS_STYPE_ABSTRACT(vst->stype)) {
		// only shift and cmp binary expr can be abstract, choose ctx
		// or default
		ctx = ctx ? ctx : default_abstract_context(vst->stype);
		vst->stype = ctx;
	} else
		ctx = non_abstract_or_null(e);

	if (IS_STYPE_POINTER(vst->stype) && e->tok == TOK_PLUS)
		ctx = 0;

	if (is_cmp)
		ctx = non_abstract_or_null(e);

	realize_expr_type(e->lhs, ctx);
	if (is_shift)
		ctx = builtin_named_stypes[BUILTIN_UINT32];
	realize_expr_type(e->rhs, ctx);

	if (is_shift &&
	    !binop_compat(e->lhs->vst.stype, e->rhs->vst.stype, e->tok, 0))
	{
		// no type, means type error
		source_loc_range_t ranges[] = {
			e->lhs->source_loc_range(),
			e->rhs->source_loc_range()
		};

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				e->pos, true, ranges, 2,
				"invalid operands to binary expression "
				"('%s' and '%s')",
				e->lhs->vst.stype->to_string().c_str(),
				e->rhs->vst.stype->to_string().c_str());
		diag->report(m);
	}
}

void pass2_t::realize_type_cast_expr_type(type_cast_expr_t *expr, stype_t *ctx)
{
	// type cast always overrides context and cannot be abstract by itself
	ctx = expr->vst.stype;
	realize_expr_type(expr->expr, ctx);
}

void pass2_t::realize_call_expr_type(call_expr_t *expr, stype_t *ctx)
{
	// call expression always overrides context and cannot be abstract
	func_stype_t *ft = expr->expr->vst.stype->as_func();

	// It is possible that expr->args is smaller than ft->args, but it's ok,
	// because the only case where it's possible is call expr with multiple
	// return values as arguments. Since call expression overrides context
	// anyway, I don't care about that.

	realize_expr_type(expr->expr, ctx);
	for (size_t i = 0, n = expr->args.size(); i < n; ++i) {
		stype_t *ftt = 0;
		if (i < ft->args.size())
			ftt = ft->args[i];
		realize_expr_type(expr->args[i], ftt);
	}
}

void pass2_t::realize_index_expr_type(index_expr_t *expr, stype_t *ctx)
{
	realize_expr_type(expr->expr, ctx);
	realize_expr_type(expr->index, 0);
}

void pass2_t::realize_compound_lit_type(compound_lit_t *expr, stype_t *ctx)
{
	if (!ctx)
		ctx = expr->vst.stype;
	else
		expr->vst.stype = ctx;

	array_stype_t *ast = 0;
	struct_stype_t *sst = 0;
	size_t n_expected = 0;
	if (IS_STYPE_ARRAY(ctx)) {
		ast = ctx->as_array();
		n_expected = ast->size;
	} else {
		KRAWL_QASSERT(IS_STYPE_STRUCT(ctx));
		sst = ctx->as_struct();
		n_expected = sst->fields.size();
	}

	if (expr->elts.size() != n_expected) {
		source_loc_range_t range = expr->source_loc_range();

		const char *what = "not enough";
		if (expr->elts.size() > n_expected)
			what = "too many";

		message_t *m;
		m = new_message(MESSAGE_ERROR,
				range.beg, false, &range, 1,
				"%s values in compound literal "
				"(got: %d, expected: %d)",
				what, expr->elts.size(), n_expected);
		diag->report(m);
		return;
	}

	for (size_t i = 0, n = expr->elts.size(); i < n; ++i) {
		node_t *e = expr->elts[i];
		stype_t *t = 0;
		if (IS_STYPE_ARRAY(ctx))
			t = ast->elem;
		else
			t = sst->fields[i].type;

		if (!assignable(e->vst.stype, t)) {
			source_loc_range_t range = e->source_loc_range();

			message_t *m;
			m = new_message(MESSAGE_ERROR,
					range.beg, false, &range, 1,
					"'%s' is not assignable to '%s'",
					e->vst.stype->to_string().c_str(),
					t->to_string().c_str());
			diag->report(m);
		} else
			realize_expr_type(e, t);
	}
}

void pass2_t::realize_selector_expr_type(selector_expr_t *expr, stype_t *ctx)
{
	realize_expr_type(expr->expr, ctx);
}

void pass2_t::realize_expr_type(node_t *expr, stype_t *ctx)
{
	KRAWL_QASSERT(expr->is_expr());
	if (expr->vst.value.type != VALUE_NONE) {
		if (!IS_STYPE_ABSTRACT(expr->vst.stype))
			return;

		value_stype_t *vst = &expr->vst;
		// if no context is specified, pick default type, for each
		// abstract type we have a specific default type
		if (!ctx)
			ctx = default_abstract_context(vst->stype);

		KRAWL_QASSERT(assignable(vst->stype, ctx));
		stype_t *from = vst->stype;
		stype_t *to = ctx;
		vst->stype = ctx;

		if (vst->value.type != VALUE_ERROR) {
			source_loc_range_t range = expr->source_loc_range();

			if (IS_STYPE_INT(from) && IS_STYPE_FLOAT(to)) {
				vst->value = vst->value.to_float();
			} else if (IS_STYPE_FLOAT(from) && IS_STYPE_INT(to)) {
				if (!mpfr_integer_p(vst->value.xfloat)) {
					message_t *m;
					m = new_message(MESSAGE_ERROR,
							range.beg, false, &range, 1,
							"floating point constant "
							"truncated to integer");
					diag->report(m);
					return;
				} else
					vst->value = vst->value.to_int();
			}

			if (!value_fits_in_type(&vst->value, to)) {
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						range.beg, false, &range, 1,
						"constant %s overflows '%s'",
						vst->value.to_string().c_str(),
						ctx->to_string().c_str());
				diag->report(m);
			}
		}
		return;
	}

	switch (expr->type) {
	case node_t::BINARY_EXPR:
	{
		realize_binary_expr_type((binary_expr_t*)expr, ctx);
		break;
	}
	case node_t::UNARY_EXPR:
	{
		realize_unary_expr_type((unary_expr_t*)expr, ctx);
		break;
	}
	case node_t::SELECTOR_EXPR:
	{
		realize_selector_expr_type((selector_expr_t*)expr, ctx);
		break;
	}
	case node_t::INDEX_EXPR:
	{
		realize_index_expr_type((index_expr_t*)expr, ctx);
		break;
	}
	case node_t::CALL_EXPR:
	{
		realize_call_expr_type((call_expr_t*)expr, ctx);
		break;
	}
	case node_t::PAREN_EXPR:
	{
		paren_expr_t *e = (paren_expr_t*)expr;
		realize_expr_type(e->expr, ctx);
		e->vst = e->expr->vst;
		break;
	}
	case node_t::TYPE_CAST_EXPR:
	{
		realize_type_cast_expr_type((type_cast_expr_t*)expr, ctx);
		break;
	}
	case node_t::COMPOUND_LIT:
	{
		realize_compound_lit_type((compound_lit_t*)expr, ctx);
		break;
	}
	case node_t::BASIC_LIT_EXPR:
		break; // ignore this one
	case node_t::IDENT_EXPR:
		break; // ignore this one
	case node_t::TYPE_EXPR:
		break; // ignore this one
	default:
		KRAWL_QASSERT(!"bad expression type");
	}
}

stype_t *pass2_t::non_abstract_or_null(binary_expr_t *expr)
{
	if (!IS_STYPE_ABSTRACT(expr->lhs->vst.stype))
		return expr->lhs->vst.stype;
	if (!IS_STYPE_ABSTRACT(expr->rhs->vst.stype))
		return expr->rhs->vst.stype;
	return 0;
}

stype_t *pass2_t::default_abstract_context(stype_t *abstract)
{
	KRAWL_QASSERT(IS_STYPE_ABSTRACT(abstract));
	if (IS_STYPE_INT(abstract))
		return builtin_named_stypes[BUILTIN_INT32];
	else if (IS_STYPE_FLOAT(abstract))
		return builtin_named_stypes[BUILTIN_FLOAT32];
	else if (IS_STYPE_BOOL(abstract))
		return builtin_named_stypes[BUILTIN_BOOL];
	return new_pointer_stype(ttracker, builtin_named_stypes[BUILTIN_UINT8]);
}

//==============================================================================
// CONST EVALUATION
//==============================================================================

value_t pass2_t::eval_binop(value_stype_t *vs1, value_stype_t *vs2,
			    stype_t *result, stype_t *optype, binary_expr_t *expr)
{
	// we need both values in order to evaluate the expression
	// if NONE + ERROR is the case, priority goes to NONE, because
	// it reports more errors
	if (vs1->value.type == VALUE_NONE || vs2->value.type == VALUE_NONE)
		return value_t::none;
	if (vs1->value.type == VALUE_ERROR || vs2->value.type == VALUE_ERROR)
		return value_t::error;

	// string can type cannot be named, no need to check for "fits in type"
	// issues
	if (IS_STYPE_STRING(optype))
		return eval_string_binop(&vs1->value, &vs2->value, expr);

	value_t out;
	value_t v1 = vs1->value;
	value_t v2 = vs2->value;

	if (IS_STYPE_FLOAT(optype)) {
		if (v1.type != VALUE_FLOAT) {
			KRAWL_QASSERT(v1.type == VALUE_INT);
			v1 = v1.to_float();
		}
		if (v2.type != VALUE_FLOAT) {
			KRAWL_QASSERT(v2.type == VALUE_INT);
			v2 = v2.to_float();
		}
		out = eval_float_binop(&v1, &v2, expr);
	} else if (IS_STYPE_POINTER_OR_INT(optype)) {
		if (v1.type != VALUE_INT) {
			KRAWL_QASSERT(v1.type == VALUE_FLOAT);
			if (!mpfr_integer_p(v1.xfloat)) {
				source_loc_range_t ranges[] = {
					expr->lhs->source_loc_range()
				};
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						expr->pos, true, ranges, 1,
						"floating point constant "
						"truncated to integer");
				diag->report(m);
				return value_t::error;
			}
			v1 = v1.to_int();
		}
		if (v2.type != VALUE_INT) {
			KRAWL_QASSERT(v2.type == VALUE_FLOAT);
			if (!mpfr_integer_p(v2.xfloat)) {
				source_loc_range_t ranges[] = {
					expr->rhs->source_loc_range()
				};
				message_t *m;
				m = new_message(MESSAGE_ERROR,
						expr->pos, true, ranges, 1,
						"floating point constant "
						"truncated to integer");
				diag->report(m);
				return value_t::error;
			}
			v2 = v2.to_int();
		}
		// integer or pointer arithmetic here
		out = eval_int_binop(&v1, &v2, expr);
	} else if (IS_STYPE_BOOL(optype)) {
		// boolean operation, but value type is int (1 or 0)
		out = eval_bool_binop(&v1, &v2, expr);
	} else {
		KRAWL_QASSERT(!"unexpected case");
	}

	if (out.type == VALUE_ERROR)
		return out;

	KRAWL_QASSERT(out.type != VALUE_NONE);

	// check whether type is large enough to hold the value
	if (!value_fits_in_type(&out, result)) {
		source_loc_range_t ranges[] = {
			expr->lhs->source_loc_range(),
			expr->rhs->source_loc_range()
		};
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos, true, ranges, 2,
				"constant %s overflows '%s'",
				out.to_string().c_str(),
				result->to_string().c_str());
		diag->report(m);
		return value_t::error;
	}

	return out;
}

value_t pass2_t::eval_unop(value_stype_t *v, stype_t *result, unary_expr_t *expr)
{
	if (v->value.type == VALUE_NONE)
		return value_t::none;
	if (v->value.type == VALUE_ERROR)
		return value_t::error;

	value_t out;
	stype_t *e = v->stype->end_type();

	if (IS_STYPE_INT(e)) {
		out = eval_int_unop(&v->value, e->as_int(), expr);
	} else if (IS_STYPE_FLOAT(e))
		out = eval_float_unop(&v->value, expr);
	else
		out = eval_bool_unop(&v->value, expr);

	// check whether type is large enough to hold the value
	if (!value_fits_in_type(&out, result)) {
		source_loc_range_t range = expr->expr->source_loc_range();
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos, true, &range, 1,
				"constant %s overflows '%s'",
				out.to_string().c_str(),
				result->to_string().c_str());
		diag->report(m);
		return value_t::error;
	}

	return out;
}

value_t pass2_t::eval_string_binop(value_t *v1, value_t *v2, binary_expr_t *expr)
{
	std::string s;
	KRAWL_QASSERT(v1->type == VALUE_STRING);
	KRAWL_QASSERT(v2->type == VALUE_STRING);
	switch (expr->tok) {
	case TOK_PLUS:
		s = v1->xstr;
		s += v2->xstr;
		return value_t(s.c_str(), TOK_STRING);
	case TOK_EQ:
		if (strcmp(v1->xstr, v2->xstr) == 0)
			return value_t(1);
		return value_t(0);
	case TOK_NEQ:
		if (strcmp(v1->xstr, v2->xstr) != 0)
			return value_t(1);
		return value_t(0);
	default:
		KRAWL_QASSERT(!"invalid op for eval_string_binop");
		break;
	}
	return value_t::error;
}

value_t pass2_t::eval_int_binop(value_t *v1, value_t *v2, binary_expr_t *expr)
{
	KRAWL_QASSERT(v1->type == VALUE_INT);
	KRAWL_QASSERT(v2->type == VALUE_INT);

	const char *msg;
	mp_bitcnt_t nbits;
	value_t out(0); // init to INT

	switch (expr->tok) {
	case TOK_PLUS:
		mpz_add(out.xint, v1->xint, v2->xint);
		return out;
	case TOK_MINUS:
		mpz_sub(out.xint, v1->xint, v2->xint);
		return out;
	case TOK_AND:
		mpz_and(out.xint, v1->xint, v2->xint);
		return out;
	case TOK_OR:
		mpz_ior(out.xint, v1->xint, v2->xint);
		return out;
	case TOK_XOR:
		mpz_xor(out.xint, v1->xint, v2->xint);
		return out;
	case TOK_TIMES:
		mpz_mul(out.xint, v1->xint, v2->xint);
		return out;
	case TOK_DIVIDE:
		if (mpz_sgn(v2->xint) != 0) {
			mpz_tdiv_q(out.xint, v1->xint, v2->xint);
			return out;
		} else {
			msg = "division by zero";
			break;
		}
	case TOK_MOD:
		if (mpz_sgn(v2->xint) != 0) {
			mpz_tdiv_r(out.xint, v1->xint, v2->xint);
			return out;
		} else {
			msg = "division by zero";
			break;
		}
	case TOK_SHIFTL:
		nbits = mpz_get_ui(v2->xint);
		// check also for very big nbits number, because it can cause
		// gmplib crash due to memory bloat
		if (mpz_cmp_ui(v2->xint, nbits) != 0 || nbits > 0x100000) {
			msg = "shift count overflow";
			break;
		}
		mpz_mul_2exp(out.xint, v1->xint, nbits);
		return out;
	case TOK_SHIFTR:
		nbits = mpz_get_ui(v2->xint);
		if (mpz_cmp_ui(v2->xint, nbits) != 0) {
			msg = "shift count overflow";
			break;
		}
		if (mpz_cmp_ui(v1->xint, 0) >= 0)
			mpz_tdiv_q_2exp(out.xint, v1->xint, nbits);
		else
			mpz_fdiv_q_2exp(out.xint, v1->xint, nbits);
		return out;
	// comparison
	case TOK_EQ:
		if (mpz_cmp(v1->xint, v2->xint) == 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_NEQ:
		if (mpz_cmp(v1->xint, v2->xint) != 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_GT:
		if (mpz_cmp(v1->xint, v2->xint) > 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_GE:
		if (mpz_cmp(v1->xint, v2->xint) >= 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_LT:
		if (mpz_cmp(v1->xint, v2->xint) < 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_LE:
		if (mpz_cmp(v1->xint, v2->xint) <= 0)
			mpz_set_ui(out.xint, 1);
		return out;
	default:
		KRAWL_QASSERT(!"invalid op for eval_int_binop");
		break;
	}

	// if we got here, we have an error
	KRAWL_QASSERT(msg != 0);
	source_loc_range_t ranges[] = {
		expr->lhs->source_loc_range(),
		expr->rhs->source_loc_range()
	};

	message_t *m;
	m = new_message(MESSAGE_ERROR, expr->pos, true, ranges, 2, msg);
	diag->report(m);
	return value_t::error;
}

value_t pass2_t::eval_float_binop(value_t *v1, value_t *v2, binary_expr_t *expr)
{
	KRAWL_QASSERT(v1->type == VALUE_FLOAT);
	KRAWL_QASSERT(v2->type == VALUE_FLOAT);

	const char *msg;
	value_t out;

	switch (expr->tok) {
	case TOK_PLUS:
		out.type = VALUE_FLOAT;
		mpfr_init(out.xfloat);
		mpfr_add(out.xfloat, v1->xfloat, v2->xfloat, MPFR_RNDN);
		return out;
	case TOK_MINUS:
		out.type = VALUE_FLOAT;
		mpfr_init(out.xfloat);
		mpfr_sub(out.xfloat, v1->xfloat, v2->xfloat, MPFR_RNDN);
		return out;
	case TOK_TIMES:
		out.type = VALUE_FLOAT;
		mpfr_init(out.xfloat);
		mpfr_mul(out.xfloat, v1->xfloat, v2->xfloat, MPFR_RNDN);
		return out;
	case TOK_DIVIDE:
		if (mpfr_zero_p(v2->xfloat)) {
			msg = "division by zero";
			break;
		}
		out.type = VALUE_FLOAT;
		mpfr_init(out.xfloat);
		mpfr_div(out.xfloat, v1->xfloat, v2->xfloat, MPFR_RNDN);
		return out;
	// comparison
	// TODO: requires constaining to type bits?
	case TOK_EQ:
		return value_t(mpfr_cmp(v1->xfloat, v2->xfloat) == 0 ? 1 : 0);
	case TOK_NEQ:
		return value_t(mpfr_cmp(v1->xfloat, v2->xfloat) != 0 ? 1 : 0);
	case TOK_LT:
		return value_t(mpfr_cmp(v1->xfloat, v2->xfloat) < 0 ? 1 : 0);
	case TOK_GT:
		return value_t(mpfr_cmp(v1->xfloat, v2->xfloat) > 0 ? 1 : 0);
	case TOK_LE:
		return value_t(mpfr_cmp(v1->xfloat, v2->xfloat) <= 0 ? 1 : 0);
	case TOK_GE:
		return value_t(mpfr_cmp(v1->xfloat, v2->xfloat) >= 0 ? 1 : 0);
	default:
		KRAWL_QASSERT(!"invalid op for eval_float_binop");
		break;
	}

	// if we got here, we have an error
	KRAWL_QASSERT(msg != 0);
	source_loc_range_t ranges[] = {
		expr->lhs->source_loc_range(),
		expr->rhs->source_loc_range()
	};

	message_t *m;
	m = new_message(MESSAGE_ERROR, expr->pos, true, ranges, 2, msg);
	diag->report(m);
	return value_t::error;
}

value_t pass2_t::eval_bool_binop(value_t *v1, value_t *v2, binary_expr_t *expr)
{
	KRAWL_QASSERT(v1->type == VALUE_INT);
	KRAWL_QASSERT(v2->type == VALUE_INT);

	value_t out(0); // init to INT

	switch (expr->tok) {
	case TOK_EQ:
		if (mpz_cmp(v1->xint, v2->xint) == 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_NEQ:
		if (mpz_cmp(v1->xint, v2->xint) != 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_ANDAND:
		if (mpz_cmp_ui(v1->xint, 1) == 0 && mpz_cmp_ui(v2->xint, 1) == 0)
			mpz_set_ui(out.xint, 1);
		return out;
	case TOK_OROR:
		if (mpz_cmp_ui(v1->xint, 1) == 0 || mpz_cmp_ui(v2->xint, 1) == 0)
			mpz_set_ui(out.xint, 1);
		return out;
	default:
		KRAWL_QASSERT(!"invalid op for eval_bool_binop");
		break;
	}
	return value_t::error;
}

value_t pass2_t::eval_int_unop(value_t *v, int_stype_t *t, unary_expr_t *expr)
{
	KRAWL_QASSERT(v->type == VALUE_INT);
	value_t out(0);

	switch (expr->tok) {
	case TOK_PLUS:
		mpz_set(out.xint, v->xint);
		return out;
	case TOK_MINUS:
		mpz_neg(out.xint, v->xint);
		return out;
	case TOK_XOR:
		if (t->is_signed)
			mpz_com(out.xint, v->xint);
		else {
			int bits = t->bits();
			mpz_t typemax;

			// 1 << bits - 1
			mpz_init_set_ui(typemax, 1);
			mpz_mul_2exp(typemax, typemax, bits);
			mpz_sub_ui(typemax, typemax, 1);

			// ^x == type.max - x
			mpz_sub(out.xint, typemax, v->xint);

			// free 'typemax'
			mpz_clear(typemax);
		}
		return out;
	default:
		KRAWL_QASSERT(!"invalid op for eval_int_unop");
		break;
	}
	return value_t::error;
}

value_t pass2_t::eval_float_unop(value_t *v, unary_expr_t *expr)
{
	KRAWL_QASSERT(v->type == VALUE_FLOAT);
	value_t out;
	out.type = VALUE_FLOAT;
	mpfr_init(out.xfloat);

	switch (expr->tok) {
	case TOK_PLUS:
		mpfr_set(out.xfloat, v->xfloat, MPFR_RNDN);
		return out;
	case TOK_MINUS:
		mpfr_neg(out.xfloat, v->xfloat, MPFR_RNDN);
		return out;
	default:
		KRAWL_QASSERT(!"invalid op for eval_float_unop");
		break;
	}
	return value_t::error;
}

value_t pass2_t::eval_bool_unop(value_t *v, unary_expr_t *expr)
{
	KRAWL_QASSERT(v->type == VALUE_INT);
	value_t out(0);

	switch (expr->tok) {
	case TOK_NOT:
		mpz_set_ui(out.xint, mpz_cmp_si(v->xint, 0) == 0 ? 1 : 0);
		return out;
	default:
		KRAWL_QASSERT(!"invalid op for eval_bool_unop");
		break;
	}
	return value_t::error;
}

value_t pass2_t::eval_type_cast(value_t *v, stype_t *from,
				stype_t *to, type_cast_expr_t *expr)
{
	if (v->type == VALUE_NONE)
		return value_t::none;
	if (v->type == VALUE_ERROR)
		return value_t::error;

	// special case, type cast: "123".(*byte)
	if (IS_STYPE_STRING(from) && IS_STYPE_POINTER(to))
		return value_t::none;

	source_loc_range_t ranges[] = {
		expr->expr->source_loc_range(),
		expr->ctype->source_loc_range()
	};

	value_t val = *v;

	if (IS_STYPE_INT(from) && IS_STYPE_FLOAT(to)) {
		val = v->to_float();
	} else if (IS_STYPE_FLOAT(from) && IS_STYPE_INT(to)) {
		if (!mpfr_integer_p(v->xfloat)) {
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					expr->pos_dot, true, ranges, 2,
					"floating point constant "
					"truncated to integer");
			diag->report(m);
			return value_t::error;
		}
		val = v->to_int();
	}

	if (!value_fits_in_type(&val, to)) {
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				expr->pos_dot, true, ranges, 2,
				"constant %s overflows '%s'",
				val.to_string().c_str(),
				to->to_string().c_str());
		diag->report(m);
		return value_t::error;
	}
	return val;
}

value_t pass2_t::eval_assignment(value_t *v, stype_t *from, stype_t *to,
				 ident_expr_t *name, source_loc_t pos,
				 node_t *init)
{
	if (v->type == VALUE_NONE)
		return value_t::none;
	if (v->type == VALUE_ERROR)
		return value_t::error;

	source_loc_range_t ranges[] = {
		name->source_loc_range(),
		init->source_loc_range()
	};

	value_t val = *v;

	if (IS_STYPE_INT(from) && IS_STYPE_FLOAT(to)) {
		val = v->to_float();
	} else if (IS_STYPE_FLOAT(from) && IS_STYPE_INT(to)) {
		if (!mpfr_integer_p(v->xfloat)) {
			message_t *m;
			m = new_message(MESSAGE_ERROR,
					pos, true, ranges, 2,
					"floating point constant "
					"truncated to integer");
			diag->report(m);
			return value_t::error;
		}
		val = v->to_int();
	}

	if (!value_fits_in_type(&val, to)) {
		message_t *m;
		m = new_message(MESSAGE_ERROR,
				pos, true, ranges, 2,
				"constant %s overflows '%s'",
				val.to_string().c_str(),
				to->to_string().c_str());
		diag->report(m);
		return value_t::error;
	}
	return val;
}
