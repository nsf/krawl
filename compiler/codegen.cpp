#include "crawl.hpp"
#include <string.h>

static const char *nil_value_for_type(stype_t *t)
{
	if (t->type & STYPE_BOOL)
		return "false";
	else if (t->type & STYPE_FLOAT)
		return "0.0";
	else if (t->type & (STYPE_INT | STYPE_POINTER | STYPE_FUNC))
		return "0";
	else if (t->type & (STYPE_ARRAY | STYPE_STRUCT))
		return "{0}";
	return "???";
}

static const char *token_to_sym(int tok)
{
	switch (tok) {
	case TOK_OROR: return "||";
	case TOK_ANDAND: return "&&";
	case TOK_EQ: return "==";
	case TOK_NEQ: return "!=";
	case TOK_LT: return "<";
	case TOK_LE: return "<=";
	case TOK_GT: return ">";
	case TOK_GE: return ">=";
	case TOK_PLUS: return "+";
	case TOK_MINUS: return "-";
	case TOK_OR: return "|";
	case TOK_XOR: return "^";
	case TOK_DIVIDE: return "/";
	case TOK_TIMES: return "*";
	case TOK_MOD: return "%";
	case TOK_SHIFTL: return "<<";
	case TOK_SHIFTR: return ">>";
	case TOK_AND: return "&";
	case TOK_ANDNOT: return "&^";
	case TOK_A_PLUS: return "+=";
	case TOK_A_MINUS: return "-=";
	case TOK_A_OR: return "|=";
	case TOK_A_XOR: return "^=";
	case TOK_A_DIVIDE: return "/=";
	case TOK_A_TIMES: return "*=";
	case TOK_A_MOD: return "%=";
	case TOK_A_SHIFTL: return "<<=";
	case TOK_A_SHIFTR: return ">>=";
	case TOK_A_AND: return "&=";
	case TOK_A_ANDNOT: return "&^=";
	case TOK_ASSIGN: return "=";
	case TOK_DECLARIZE: return ":=";
	case TOK_INC: return "++";
	case TOK_DEC: return "--";
	case TOK_SEMICOLON: return ";";
	case TOK_RETURN: return "return";
	case TOK_IF: return "if";
	case TOK_ELSE: return "else";
	case TOK_FOR: return "for";
	case TOK_LCURLY: return "{";
	case TOK_RCURLY: return "}";
	case TOK_IMPORT: return "import";
	case TOK_LPAREN: return "(";
	case TOK_RPAREN: return ")";
	case TOK_TYPE: return "type";
	case TOK_CONST: return "const";
	case TOK_VAR: return "var";
	case TOK_FUNC: return "func";
	case TOK_LSB: return "[";
	case TOK_RSB: return "]";
	case TOK_DOT: return ".";
	case TOK_STRUCT: return "struct";
	case TOK_COMMA: return ",";
	case TOK_NOT: return "!";
	// TODO: list is incomplete
	}
	return "???";
}

void pass3_t::append_indent(std::string *out)
{
	for (int i = 0; i < indent; ++i)
		out->append(1, '\t');
}

std::string pass3_t::gen_tmpname()
{
	std::string out;
	cppsprintf(&out, "__tmp%d", tmp_n++);
	return out;
}

const char *pass3_t::tuple_to_struct(stype_vector_t *tuple)
{
	std::string def = "{";
	for (size_t i = 0, n = tuple->size(); i < n; ++i) {
		stype_t *t = tuple->at(i);
		def += " ";
		def += ctype(t);
		cppsprintf(&def, " _%d;", i);
	}
	def += " };";

	unordered_map<std::string, std::string>::iterator it;
	it = tmp_structs.find(def);
	if (it == tmp_structs.end()) {
		std::string tmpname = std::string("struct ") + gen_tmpname();
		tmp_structs[def] = tmpname;
		cppsprintf(&tmp_struct_defs, "%s %s\n",
			   tmpname.c_str(), def.c_str());
		return tmp_structs[def].c_str();
	}

	return it->second.c_str();
}

const char *pass3_t::array_to_struct(array_stype_t *ast)
{
	std::string def;
	cppsprintf(&def, "{ %s _[%u]; };",
		   ctype(ast->elem).c_str(),
		   ast->size);

	unordered_map<std::string, std::string>::iterator it;
	it = tmp_structs.find(def);
	if (it == tmp_structs.end()) {
		std::string tmpname = std::string("struct ") + gen_tmpname();
		tmp_structs[def] = tmpname;
		cppsprintf(&tmp_struct_defs, "%s %s\n",
			   tmpname.c_str(), def.c_str());
		return tmp_structs[def].c_str();
	}

	return it->second.c_str();
}

const char *pass3_t::struct_to_struct(struct_stype_t *sst)
{
	std::string def;
	def += "{";
	for (size_t i = 0, n = sst->fields.size(); i < n; ++i) {
		struct_field_t *f = &sst->fields[i];
		def += " ";
		def += ctype(f->type);
		def += " ";
		def += f->name;
		def += ";";
	}
	def += " };";

	unordered_map<std::string, std::string>::iterator it;
	it = tmp_structs.find(def);
	if (it == tmp_structs.end()) {
		std::string tmpname = std::string("struct ") + gen_tmpname();
		tmp_structs[def] = tmpname;
		cppsprintf(&tmp_struct_defs, "%s %s\n",
			   tmpname.c_str(), def.c_str());
		return tmp_structs[def].c_str();
	}

	return it->second.c_str();
}

struct side_effect_extractor_t : ast_visitor_t {
	std::vector<std::string> *out;
	pass3_t *pass;
	bool skip_first_call_expr;

	ast_visitor_t *visit(node_t *n)
	{
		switch (n->type) {
		case node_t::CALL_EXPR:
			return visit_call_expr((call_expr_t*)n);
		case node_t::COMPOUND_LIT:
			return visit_compound_lit((compound_lit_t*)n);
		default:
			break;
		}

		return this;
	}

	ast_visitor_t *visit_call_expr(call_expr_t *e)
	{
		if (skip_first_call_expr) {
			skip_first_call_expr = false;
			return this;
		}

		for (size_t i = 0, n = e->args.size(); i < n; ++i) {
			side_effect_extractor_t see;
			see.out = out;
			see.pass = pass;
			see.skip_first_call_expr = false;
			see.traverse(e->args[i]);
		}

		std::string name = pass->gen_tmpname();
		stype_t *type = e->vst.stype;

		std::string stmt;
		cppsprintf(&stmt, "%s %s = %s;",
			   pass->ctype(type).c_str(),
			   name.c_str(),
			   pass->codegen_expr(e).c_str());

		e->tmp = name;
		out->push_back(stmt);
		return 0;
	}

	ast_visitor_t *visit_compound_lit(compound_lit_t *e)
	{
		// TODO: ugly copy & paste here, remove?
		for (size_t i = 0, n = e->elts.size(); i < n; ++i) {
			side_effect_extractor_t see;
			see.out = out;
			see.pass = pass;
			see.skip_first_call_expr = skip_first_call_expr;
			see.traverse(e->elts[i]);

			skip_first_call_expr = see.skip_first_call_expr;
		}

		std::string name = pass->gen_tmpname();
		stype_t *type = e->vst.stype;

		std::string stmt;
		cppsprintf(&stmt, "%s %s = %s;",
			   pass->ctype(type).c_str(),
			   name.c_str(),
			   pass->codegen_expr(e).c_str());

		e->tmp = name;
		out->push_back(stmt);
		return 0;
	}
};

void pass3_t::extract_side_effects(std::vector<std::string> *out, node_t *expr)
{
	side_effect_extractor_t see;
	see.out = out;
	see.pass = this;
	see.skip_first_call_expr = expr->type == node_t::CALL_EXPR;
	see.traverse(expr);
}

std::string pass3_t::codegen_value(value_stype_t *vst)
{
	std::string out;
	out += "(";
	out += ctype(vst->stype);
	out += ")";
	out += vst->value.to_literal();
	return out;
}

std::string pass3_t::codegen_pointer_binary_expr(binary_expr_t *expr)
{
	stype_t *lhs = expr->lhs->vst.stype->end_type();
	stype_t *rhs = expr->rhs->vst.stype->end_type();

	std::string out;
	out += "(";
	if (lhs->type & STYPE_INT) {
		CRAWL_QASSERT(rhs->type & STYPE_POINTER);
		cppsprintf(&out, "%d*%s",
			   ((pointer_stype_t*)rhs)->points_to->bits() / 8,
			   codegen_expr(expr->lhs).c_str());
	} else
		out += codegen_expr(expr->lhs);

	out += " ";
	out += token_to_sym(expr->tok);
	out += " ";

	if (rhs->type & STYPE_INT) {
		CRAWL_QASSERT(lhs->type & STYPE_POINTER);
		cppsprintf(&out, "%d*%s",
			   ((pointer_stype_t*)lhs)->points_to->bits() / 8,
			   codegen_expr(expr->rhs).c_str());
	} else
		out += codegen_expr(expr->rhs);
	out += ")";
	return out;
}

std::string pass3_t::codegen_pointer_unary_expr(unary_expr_t *expr)
{
	std::string out;
	stype_t *t = expr->expr->vst.stype->end_type();
	if (expr->tok == TOK_TIMES) {
		// pointer dereference
		CRAWL_QASSERT(t->type & STYPE_POINTER);
		cppsprintf(&out, "*(%s*)%s",
			   ctype(((pointer_stype_t*)t)->points_to).c_str(),
			   codegen_expr(expr->expr).c_str());
	} else {
		// address of
		cppsprintf(&out, "(uint8_t*)&%s",
			   codegen_expr(expr->expr).c_str());
	}
	return out;
}

std::string pass3_t::codegen_expr_for_call(node_t *expr, func_stype_t *fst)
{
	std::string estr = codegen_expr(expr);
	if (expr->type == node_t::IDENT_EXPR) {
		ident_expr_t *e = (ident_expr_t*)expr;
		if (e->sdecl->type == SDECL_FUNC)
			return estr;
	}

	std::string out;
	cppsprintf(&out, "((%s)%s)",
		   real_func_ctype(fst).c_str(),
		   estr.c_str());
	return out;
}

std::string pass3_t::codegen_expr(node_t *expr)
{
	if (expr->vst.value.type != VALUE_NONE)
		return codegen_value(&expr->vst);

	std::string out;
	switch (expr->type) {
	case node_t::BINARY_EXPR:
	{
		binary_expr_t *e = (binary_expr_t*)expr;
		if (e->vst.stype->type & STYPE_POINTER) {
			out = codegen_pointer_binary_expr(e);
			break;
		}
		out += "(";
		out += codegen_expr(e->lhs);
		out += " ";
		out += token_to_sym(e->tok);
		out += " ";
		out += codegen_expr(e->rhs);
		out += ")";
		break;
	}
	case node_t::UNARY_EXPR:
	{
		unary_expr_t *e = (unary_expr_t*)expr;
		if (e->tok == TOK_AND || e->tok == TOK_TIMES) {
			out = codegen_pointer_unary_expr(e);
			break;
		}
		out += token_to_sym(e->tok);
		out += codegen_expr(e->expr);
		break;
	}
	case node_t::IDENT_EXPR:
	{
		ident_expr_t *e = (ident_expr_t*)expr;
		if (e->sdecl && e->sdecl->type == SDECL_VAR) {
			var_sdecl_t *vsd = (var_sdecl_t*)e->sdecl;
			if (vsd->namedret != -1) {
				cppsprintf(&out, "__multiple_return_values._%d",
					   vsd->namedret);
				break;
			}
		}
		if (!uid.empty() && e->sdecl &&
		    e->sdecl->scope->parent == pkgscope)
		{
			cppsprintf(&out, "_Crl_%s_%s", uid.c_str(), e->val.c_str());
			break;
		}
		out += e->val;
		break;
	}
	case node_t::TYPE_CAST_EXPR:
	{
		type_cast_expr_t *e = (type_cast_expr_t*)expr;
		out += "(";
		out += ctype(e->vst.stype);
		out += ")";
		out += codegen_expr(e->expr);
		break;
	}
	case node_t::PAREN_EXPR:
	{
		paren_expr_t *e = (paren_expr_t*)expr;
		out += "(";
		out += codegen_expr(e->expr);
		out += ")";
		break;
	}
	case node_t::CALL_EXPR:
	{
		call_expr_t *e = (call_expr_t*)expr;
		CRAWL_QASSERT(e->expr->vst.stype->end_type()->type & STYPE_FUNC);
		func_stype_t *fst = (func_stype_t*)e->expr->vst.stype->end_type();
		if (!e->tmp.empty())
			out = e->tmp;
		else {
			out += codegen_expr_for_call(e->expr, fst);
			out += "(";
			for (size_t i = 0, n = e->args.size(); i < n; ++i) {
				out += codegen_expr(e->args[i]);
				if (i != n - 1)
					out += ", ";
			}
			out += ")";
		}
		break;
	}
	case node_t::INDEX_EXPR:
	{
		index_expr_t *e = (index_expr_t*)expr;
		stype_t *t = e->expr->vst.stype;
		if (t->type & STYPE_ARRAY) {
			out += codegen_expr(e->expr);
			out += "._[";
			out += codegen_expr(e->index);
			out += "]";
		} else {
			CRAWL_QASSERT(t->type & STYPE_POINTER);
			pointer_stype_t *pst = (pointer_stype_t*)t->end_type();
			cppsprintf(&out, "*(%s*)(%s + %d*%s)",
				   ctype(pst->points_to).c_str(),
				   codegen_expr(e->expr).c_str(),
				   pst->points_to->bits() / 8,
				   codegen_expr(e->index).c_str());
		}
		break;
	}
	case node_t::COMPOUND_LIT:
	{
		compound_lit_t *e = (compound_lit_t*)expr;
		if (!e->tmp.empty())
			out = e->tmp;
		else {
			out += "{";
			for (size_t i = 0, n = e->elts.size(); i < n; ++i) {
				out += codegen_expr(e->elts[i]);
				if (i != n-1)
					out += ", ";
			}
			out += "}";
		}
		break;
	}
	case node_t::SELECTOR_EXPR:
	{
		selector_expr_t *e = (selector_expr_t*)expr;
		if (IS_STYPE_MODULE(e->expr->vst.stype)) {
			ident_expr_t *ident = is_ident_expr(e->expr);
			CRAWL_QASSERT(ident != 0);
			CRAWL_QASSERT(ident->sdecl->type == SDECL_IMPORT);
			import_sdecl_t *id = (import_sdecl_t*)ident->sdecl;
			if (!id->prefix.empty()) {
				out += "_Crl_";
				out += id->prefix;
				out += "_";
			}
			out += codegen_expr(e->sel);
			break;
		}
		if (IS_STYPE_POINTER(e->expr->vst.stype)) {
			pointer_stype_t *pst = (pointer_stype_t*)e->expr->vst.stype->end_type();
			cppsprintf(&out, "((%s*)%s)->",
				   ctype(pst->points_to).c_str(),
				   codegen_expr(e->expr).c_str());
		} else {
			out += codegen_expr(e->expr);
			out += ".";
		}
		out += codegen_expr(e->sel);
		break;
	}
	case node_t::BASIC_LIT_EXPR:
		break;
	default:
		CRAWL_QASSERT(!"fuck");
	}
	return out;
}

std::string pass3_t::codegen_func_def(func_sdecl_t *fsd)
{
	std::string out;
	func_stype_t *fst = (func_stype_t*)fsd->stype;
	func_type_t *ft = fsd->decl->ftype;

	std::string result = "void";
	if (fst->results.size() > 1) {
		result = tuple_to_struct(&fst->results);
	} else if (fst->results.size() == 1) {
		result = ctype(fst->results[0]);
	}

	const char *fname = fsd->name.c_str();
	if (!strcmp(fname, "main"))
		fname = "__crawl_main";

	if (uid.empty())
		cppsprintf(&out, "%s %s(", result.c_str(), fname);
	else
		cppsprintf(&out, "%s _Crl_%s_%s(",
			   result.c_str(), uid.c_str(), fname);

	size_t num = 0;
	for (size_t i = 0, n = ft->args.size(); i < n; ++i) {
		field_t *f = ft->args[i];
		if (f->names.empty()) {
			stype_t *t = fst->args[num];
			cppsprintf(&out, "%s",
				   ctype(t).c_str());
			if (num != fst->args.size() - 1)
				cppsprintf(&out, ", ");
			num++;
			continue;
		}
		for (size_t j = 0, m = f->names.size(); j < m; ++j, ++num) {
			stype_t *t = fst->args[num];
			const char *name = f->names[j]->val.c_str();

			cppsprintf(&out, "%s %s",
				   ctype(t).c_str(),
				   name);

			if (num != fst->args.size() - 1)
				cppsprintf(&out, ", ");
		}
	}
	if (ft->varargs) {
		if (!ft->args.empty())
			out += ", ";
		out += "...";
	}
	cppsprintf(&out, ")");
	return out;
}

std::string pass3_t::codegen_extern_func_def(func_sdecl_t *fsd, const char *uid)
{
	std::string out;
	func_stype_t *fst = (func_stype_t*)fsd->stype;

	std::string result = "void";
	if (fst->results.size() > 1) {
		result = tuple_to_struct(&fst->results);
	} else if (fst->results.size() == 1) {
		result = ctype(fst->results[0]);
	}

	const char *fname = fsd->name.c_str();
	if (!strcmp(fname, "main"))
		fname = "__crawl_main";

	if (strlen(uid) == 0)
		cppsprintf(&out, "%s %s(", result.c_str(), fname);
	else
		cppsprintf(&out, "%s _Crl_%s_%s(", result.c_str(), uid, fname);

	for (size_t i = 0, n = fst->args.size(); i < n; ++i) {
		stype_t *t = fst->args[i];
		cppsprintf(&out, "%s",
			   ctype(t).c_str());
		if (i != fst->args.size() - 1)
			cppsprintf(&out, ", ");
	}
	if (fst->varargs) {
		if (!fst->args.empty())
			out += ", ";
		out += "...";
	}
	cppsprintf(&out, ")");
	return out;
}

std::string pass3_t::codegen_extern_sdecls()
{
	std::string out;
	for (size_t i = 0, n = used_extern_sdecls->size(); i < n; ++i) {
		import_sdecl_t *id = used_extern_sdecls->at(i);
		unordered_map<std::string, sdecl_t*>::iterator it, end;
		for (it = id->decls.begin(), end = id->decls.end(); it != end; ++it) {
			sdecl_t *d = it->second;
			if (d->type != SDECL_FUNC)
				continue;
			if (!d->inited)
				continue;

			func_sdecl_t *fsd = (func_sdecl_t*)d;

			cppsprintf(&out, "%s;\n",
				   codegen_extern_func_def(fsd, id->prefix.c_str()).c_str());
		}
	}
	out.append(1, '\n');
	return out;
}

std::string pass3_t::codegen_top_sdecl(sdecl_t *d)
{
	std::string out;
	scope_block_t *save_cur_scope = cur_scope;
	cur_scope = d->scope;

	switch (d->type) {
	case SDECL_VAR:
	{
		var_sdecl_t *vsd = (var_sdecl_t*)d;
		if (!vsd->init) {
			cppsprintf(&out, "%s %s = %s;\n",
				   ctype(vsd->stype).c_str(),
				   vsd->name.c_str(),
				   nil_value_for_type(vsd->stype));
			vsd->inited = true;
		} else if (vsd->init->vst.value.type != VALUE_NONE) {
			// we have a value, we can generate the code!
			std::string init = codegen_value(&vsd->init->vst);
			cppsprintf(&out, "%s %s = %s;\n",
				   ctype(vsd->stype).c_str(),
				   vsd->name.c_str(),
				   init.c_str());
			vsd->inited = true;
		} else {
			// otherwise just place an init-less declaration
			// and add the sdecl to 'inits'
			cppsprintf(&out, "%s %s;\n",
				   ctype(vsd->stype).c_str(),
				   vsd->name.c_str());
			inits.push_back(vsd);
		}
		break;
	}
	case SDECL_FUNC:
	{
		func_sdecl_t *fsd = (func_sdecl_t*)d;
		cppsprintf(&out, "%s;\n",
			   codegen_func_def(fsd).c_str());
		if (fsd->decl->body)
			funcs.push_back(fsd);
		break;
	}
	default:
		break;
	}

	cur_scope = save_cur_scope;
	return out;
}

std::string pass3_t::codegen_top_sdecls(std::vector<const char*> *pkgdecls)
{
	std::string out;
	for (size_t i = 0, n = pkgdecls->size(); i < n; ++i) {
		sdecl_t *sd = pkgscope->sdecls[pkgdecls->at(i)];
		out += codegen_top_sdecl(sd);
	}
	out.append(1, '\n');
	return out;
}

std::string pass3_t::pass(std::vector<const char*> *pkgdecls)
{
	tmp_n = 0;
	cur_scope = 0;
	cur_func_decl = 0;
	cur_for_stmt = 0;
	inits.clear();
	funcs.clear();
	tmp_structs.clear();
	tmp_struct_defs.clear();
	indent = 0;

	// codegen stuff
	std::string c_externdecls = codegen_extern_sdecls();
	std::string c_topsdecls = codegen_top_sdecls(pkgdecls);
	std::string c_funcs = codegen_funcs();
	std::string c_inits = codegen_inits();

	// compose out
	std::string out;
	out += "#include <stdint.h>\n";
	out += "#include <stdbool.h>\n";
	out += "\n";

	// temporary stucts
	out += tmp_struct_defs;

	// predeclarations
	out += c_externdecls;
	out += c_topsdecls;

	// function definitions
	out += c_funcs;

	// global inits
	if (!inits.empty()) {
		if (uid.empty())
			cppsprintf(&out, "static void __init_crawl_globals()\n{\n");
		else
			cppsprintf(&out, "void _Crl_%s___init_globals()\n{\n",
				   uid.c_str());
		cppsprintf(&out, "%s", c_inits.c_str());
		cppsprintf(&out, "}\n\n");
	}

	// main
	if (uid.empty()) {
		cppsprintf(&out, "int main(int argc, char **argv)\n"
			   "{\n");
		if (!inits.empty())
			cppsprintf(&out, "\t__init_crawl_globals();\n");
		cppsprintf(&out, "\treturn __crawl_main(argc, (uint8_t*)argv);\n"
			   "}\n");
	}

	return out;
}

struct init_deps_codegen_t : ast_visitor_t {
	pass3_t *pass;
	std::string *out;

	ast_visitor_t *visit(node_t *n)
	{
		if (n->type != node_t::IDENT_EXPR)
			return this;

		ident_expr_t *e = (ident_expr_t*)n;
		sdecl_t *d = e->sdecl;
		if (!d)
			return 0;

		if (d->type == SDECL_FUNC) {
			func_sdecl_t *fsd = (func_sdecl_t*)d;
			unordered_set<var_sdecl_t*>::iterator it, end;
			for (it = fsd->deps.begin(), end = fsd->deps.end(); it != end; ++it)
				pass->codegen_init(out, *it);
			return this;
		}

		if (d->type != SDECL_VAR)
			return this;

		// Global variables are declared in a file scope, the parent
		// of each file scope is the package scope.
		if (d->scope->parent == 0 || d->scope->parent != pass->pkgscope)
			return this;

		pass->codegen_init(out, (var_sdecl_t*)d);
		return this;
	}
};

void pass3_t::codegen_init_deps(std::string *out, node_t *e)
{
	init_deps_codegen_t gen;
	gen.pass = this;
	gen.out = out;
	gen.traverse(e);
}

void pass3_t::codegen_init(std::string *out, var_sdecl_t *d)
{
	if (d->inited)
		return;

	codegen_init_deps(out, d->init);

	std::vector<std::string> side_effects;
	extract_side_effects(&side_effects, d->init);

	for (size_t i = 0, n = side_effects.size(); i < n; ++i)
		cppsprintf(out, "\t%s\n", side_effects[i].c_str());

	std::string init = codegen_expr(d->init);
	cppsprintf(out, "\t%s = %s;\n", d->name.c_str(), init.c_str());
	d->inited = true;
}

std::string pass3_t::codegen_inits()
{
	std::string out;
	for (size_t i = 0, n = inits.size(); i < n; ++i)
		codegen_init(&out, inits[i]);
	return out;
}

std::string pass3_t::codegen_tmp_expr(std::string *out, node_t *expr)
{
	std::vector<std::string> side_effects;
	extract_side_effects(&side_effects, expr);
	codegen_side_effects(out, &side_effects);

	std::string name = gen_tmpname();
	stype_t *type = expr->vst.stype;

	append_indent(out);
	cppsprintf(out, "%s %s = %s;\n",
		   ctype(type).c_str(),
		   name.c_str(),
		   codegen_expr(expr).c_str());
	return name;
}

void pass3_t::codegen_side_effects(std::string *out, std::vector<std::string> *se)
{
	for (size_t i = 0, n = se->size(); i < n; ++i) {
		append_indent(out);
		out->append(se->at(i));
		out->append(1, '\n');
	}
}

void pass3_t::codegen_func(std::string *out, func_sdecl_t *f)
{
	func_sdecl_t *save_func_decl = cur_func_decl;
	cur_func_decl = f;

	func_stype_t *fst = (func_stype_t*)f->stype;

	cppsprintf(out, "%s\n",
		   codegen_func_def(f).c_str());

	indentpp(out);
	if (fst->results.size() > 1) {
		const char *tuple = tuple_to_struct(&fst->results);
		append_indent(out);
		cppsprintf(out, "%s __multiple_return_values", tuple);
		if (f->decl->ftype->named_results()) {
			// named results are default initialized to zero
			out->append(" = {");
			for (size_t i = 0, n = fst->results.size(); i < n; ++i) {
				out->append(nil_value_for_type(fst->results[i]));
				if (i != n-1)
					out->append(", ");
			}
			out->append("}");
		}
		out->append(";\n");
	}

	codegen_block_stmt(out, f->decl->body);
	indentmm(out);

	cur_func_decl = save_func_decl;
}

void pass3_t::codegen_block_stmt(std::string *out, block_stmt_t *stmt)
{
	for (size_t i = 0, n = stmt->stmts.size(); i < n; ++i)
		codegen_stmt(out, stmt->stmts[i]);
}

void pass3_t::codegen_full_block_stmt(std::string *out, block_stmt_t *stmt)
{
	indentpp(out);
	codegen_block_stmt(out, stmt);
	indentmm(out);
}

void pass3_t::codegen_compound_assign_stmt(std::string *out, assign_stmt_t *stmt)
{
	std::vector<std::string> side_effects;
	extract_side_effects(&side_effects, stmt->lhs[0]);
	extract_side_effects(&side_effects, stmt->rhs[0]);
	codegen_side_effects(out, &side_effects);

	append_indent(out);
	out->append(codegen_expr(stmt->lhs[0]));
	out->append(" ");
	out->append(token_to_sym(stmt->tok));
	out->append(" ");
	out->append(codegen_expr(stmt->rhs[0]));
	out->append(";\n");
}

void pass3_t::codegen_assign_stmt(std::string *out, assign_stmt_t *stmt)
{
	if (stmt->tok != TOK_ASSIGN && stmt->tok != TOK_DECLARIZE) {
		// += -= *= /=, etc.
		codegen_compound_assign_stmt(out, stmt);
		return;
	}

	std::vector<std::string> side_effects;
	for (size_t i = 0, n = stmt->lhs.size(); i < n; ++i)
		extract_side_effects(&side_effects, stmt->lhs[i]);

	// generate side effects for the RHS, only if it's a single assignment
	// otherwise side effects will be generated in temporaries
	if (stmt->lhs.size() == 1) {
		for (size_t i = 0, n = stmt->rhs.size(); i < n; ++i)
			extract_side_effects(&side_effects, stmt->rhs[i]);
	}
	codegen_side_effects(out, &side_effects);

	if (stmt->lhs.size() == 1) {
		// single assignment
		append_indent(out);
		if (stmt->tok == TOK_DECLARIZE &&
		    (stmt->lhs[0]->type == node_t::IDENT_EXPR))
		{
			ident_expr_t *name = (ident_expr_t*)stmt->lhs[0];
			cppsprintf(out, "%s %s = %s;\n",
				   ctype(name->sdecl->stype).c_str(),
				   name->val.c_str(),
				   codegen_expr(stmt->rhs[0]).c_str());
		} else {
			cppsprintf(out, "%s = %s;\n",
				   codegen_expr(stmt->lhs[0]).c_str(),
				   codegen_expr(stmt->rhs[0]).c_str());
		}
		return;
	}

	// multiple assignment, use temporaries
	// at this point LHS > 1

	std::vector<std::string> tmps;
	if (stmt->rhs.size() == 1) {
		// special call expr on the RHS case
		call_expr_t *c = is_call_expr(stmt->rhs[0]);
		CRAWL_QASSERT(c != 0);
		CRAWL_QASSERT(c->expr->vst.stype->type & STYPE_FUNC);
		func_stype_t *fst = (func_stype_t*)c->expr->vst.stype;

		std::vector<std::string> side_effects;
		extract_side_effects(&side_effects, c);
		codegen_side_effects(out, &side_effects);

		std::string tuple_tmp = gen_tmpname();
		append_indent(out);
		cppsprintf(out, "%s %s = %s;\n",
			   tuple_to_struct(&fst->results),
			   tuple_tmp.c_str(),
			   codegen_expr(c).c_str());

		for (size_t i = 0, n = fst->results.size(); i < n; ++i) {
			std::string tmp;
			cppsprintf(&tmp, "%s._%d", tuple_tmp.c_str(), i);
			tmps.push_back(tmp);
		}
	} else {
		for (size_t i = 0, n = stmt->rhs.size(); i < n; ++i)
			tmps.push_back(codegen_tmp_expr(out, stmt->rhs[i]));
	}

	for (size_t i = 0, n = stmt->lhs.size(); i < n; ++i) {
		append_indent(out);
		if (stmt->tok == TOK_DECLARIZE &&
		    (stmt->lhs[i]->type == node_t::IDENT_EXPR))
		{
			ident_expr_t *name = (ident_expr_t*)stmt->lhs[i];
			cppsprintf(out, "%s %s = %s;\n",
				   ctype(name->sdecl->stype).c_str(),
				   name->val.c_str(),
				   tmps[i].c_str());
		} else {
			cppsprintf(out, "%s = %s;\n",
				   codegen_expr(stmt->lhs[i]).c_str(),
				   tmps[i].c_str());
		}
	}
}

void pass3_t::codegen_expr_stmt(std::string *out, expr_stmt_t *stmt)
{
	std::vector<std::string> side_effects;
	extract_side_effects(&side_effects, stmt->expr);
	codegen_side_effects(out, &side_effects);

	append_indent(out);
	cppsprintf(out, "%s;\n", codegen_expr(stmt->expr).c_str());
}

void pass3_t::codegen_var_spec(std::string *out, value_spec_t *spec)
{
	std::vector<std::string> tmps;

	// hack for a case, where only one variable, don't generate temporaries
	if (!spec->values.empty()) {
		if (spec->names.size() == 1) {
			std::vector<std::string> side_effects;
			extract_side_effects(&side_effects, spec->values[0]);
			codegen_side_effects(out, &side_effects);
			tmps.push_back(codegen_expr(spec->values[0]));
		} else {
			// TODO: handle special case (multiple results from a call expr)
			for (size_t i = 0, n = spec->values.size(); i < n; ++i)
				tmps.push_back(codegen_tmp_expr(out, spec->values[i]));
		}
	}

	for (size_t i = 0, n = spec->names.size(); i < n; ++i) {
		ident_expr_t *name = spec->names[i];
		var_sdecl_t *vsd = (var_sdecl_t*)name->sdecl;
		append_indent(out);
		cppsprintf(out, "%s %s",
			   ctype(vsd->stype).c_str(),
			   vsd->name.c_str());

		std::string init;
		if (vsd->init)
			init = tmps[i];
		else
			init = nil_value_for_type(vsd->stype);

		cppsprintf(out, " = %s;\n", init.c_str());
	}
}

void pass3_t::codegen_decl_stmt(std::string *out, decl_stmt_t *stmt)
{
	switch (stmt->decl->type) {
	case node_t::VAR_DECL:
	{
		var_decl_t *d = (var_decl_t*)stmt->decl;
		for (size_t i = 0, n = d->specs.size(); i < n; ++i)
			codegen_var_spec(out, d->specs[i]);
		break;
	}
	default:
		CRAWL_QASSERT(!"fail");
	}
}

void pass3_t::codegen_return_stmt(std::string *out, return_stmt_t *stmt)
{
	if (stmt->returns.size() == 0) {
		CRAWL_QASSERT(cur_func_decl != 0);
		append_indent(out);
		if (cur_func_decl->decl->ftype->named_results())
			out->append("return __multiple_return_values;\n");
		else
			out->append("return;\n");
	} else if (stmt->returns.size() == 1) {
		// TODO: special case, call expr
		std::vector<std::string> side_effects;
		extract_side_effects(&side_effects, stmt->returns[0]);
		codegen_side_effects(out, &side_effects);
		append_indent(out);
		cppsprintf(out, "return %s;\n",
			   codegen_expr(stmt->returns[0]).c_str());
	} else {
		// TODO: it is possible to avoid temporaries here, if multiple
		// results are not named
		std::vector<std::string> tmps;
		for (size_t i = 0, n = stmt->returns.size(); i < n; ++i)
			tmps.push_back(codegen_tmp_expr(out, stmt->returns[i]));

		for (size_t i = 0, n = stmt->returns.size(); i < n; ++i) {
			append_indent(out);
			cppsprintf(out, "__multiple_return_values._%d = %s;\n",
				   i, tmps[i].c_str());
		}
		append_indent(out);
		out->append("return __multiple_return_values;\n");
	}
}

void pass3_t::codegen_ifelse_stmt(std::string *out, ifelse_stmt_t *stmt)
{
	std::vector<std::string> cond_se; // side effects
	extract_side_effects(&cond_se, stmt->cond);

	if (!cond_se.empty()) {
		indentpp(out);
		codegen_side_effects(out, &cond_se);
	}

	// condition
	append_indent(out);
	cppsprintf(out, "if (%s)\n", codegen_expr(stmt->cond).c_str());

	// body
	codegen_full_block_stmt(out, stmt->body);

	if (stmt->elsestmt) {
		append_indent(out);
		out->append("else\n");
		codegen_stmt(out, stmt->elsestmt);
	}

	if (!cond_se.empty())
		indentmm(out);
}

void pass3_t::codegen_incdec_stmt(std::string *out, incdec_stmt_t *stmt)
{
	const char *action = "+";
	if (stmt->tok == TOK_DEC)
		action = "-";

	stype_t *t = stmt->expr->vst.stype->end_type();

	std::vector<std::string> side_effects;
	extract_side_effects(&side_effects, stmt->expr);
	codegen_side_effects(out, &side_effects);

	append_indent(out);
	if (t->type & STYPE_INT) {
		out->append(codegen_expr(stmt->expr));
		out->append(token_to_sym(stmt->tok));
		out->append(";\n");
		return;
	}

	// must be a pointer
	CRAWL_QASSERT(t->type & STYPE_POINTER);
	std::string expr = codegen_expr(stmt->expr);
	cppsprintf(out, "%s = %s+%d;\n",
		   expr.c_str(), expr.c_str(),
		   ((pointer_stype_t*)t)->points_to->bits() / 8);
}

void pass3_t::codegen_for_stmt(std::string *out, for_stmt_t *stmt)
{
	if (stmt->init) {
		indentpp(out);
		codegen_stmt(out, stmt->init);
	}

	append_indent(out);
	if (!stmt->cond) {
		out->append("while (1)\n");
		indentpp(out);
	} else {
		std::vector<std::string> cond_se;
		extract_side_effects(&cond_se, stmt->cond);

		std::string cond = codegen_expr(stmt->cond);

		if (cond_se.empty()) {
			cppsprintf(out, "while (%s)\n", cond.c_str());
			indentpp(out);
		} else {
			out->append("while (1)\n");
			indentpp(out);

			codegen_side_effects(out, &cond_se);
			append_indent(out);
			cppsprintf(out, "if (!%s) break;\n", cond.c_str());
		}
	}

	stmt->postgoto = gen_tmpname() + "_continue";

	for_stmt_t *save_for_stmt = cur_for_stmt;
	cur_for_stmt = stmt;
	codegen_block_stmt(out, stmt->body);
	cur_for_stmt = save_for_stmt;

	if (stmt->post) {
		out->append(stmt->postgoto);

		// I use ':;' here, because C only allows labeled statements.
		// But sometimes I need to have labeled declarations (post
		// increment with side effects). So, this is a simple hack to
		// allow that.
		out->append(":;\n");
		codegen_stmt(out, stmt->post);
	}

	indentmm(out);

	if (stmt->init)
		indentmm(out);
}

void pass3_t::codegen_switch_stmt_case(std::string *out, switch_stmt_case_t *stmt)
{
	if (stmt->exprs.empty()) {
		append_indent(out);
		out->append("default:\n");
	}
	for (size_t i = 0, n = stmt->exprs.size(); i < n; ++i) {
		node_t *e = stmt->exprs[i];
		append_indent(out);
		cppsprintf(out, "case %s:\n", codegen_expr(e).c_str());
	}

	indentpp(out);
	for (size_t i = 0, n = stmt->stmts.size(); i < n; ++i) {
		codegen_stmt(out, stmt->stmts[i]);
	}
	if (stmt->stmts.empty() ||
	    stmt->stmts.back()->type != node_t::FLOW_STMT ||
	    ((flow_stmt_t*)stmt->stmts.back())->tok != TOK_FALLTHROUGH)
	{
		append_indent(out);
		out->append("break;\n");
	}
	indentmm(out);
}

void pass3_t::codegen_switch_stmt(std::string *out, switch_stmt_t *stmt)
{
	// TODO: implement unrestricted switch statements?
	CRAWL_QASSERT(stmt->constant == true);

	std::vector<std::string> expr_se;
	extract_side_effects(&expr_se, stmt->expr);

	if (!expr_se.empty()) {
		indentpp(out);
		codegen_side_effects(out, &expr_se);
	}

	// expr
	append_indent(out);
	cppsprintf(out, "switch (%s)\n", codegen_expr(stmt->expr).c_str());

	// body
	codegen_full_block_stmt(out, stmt->body);

	if (!expr_se.empty())
		indentmm(out);
}

void pass3_t::codegen_flow_stmt(std::string *out, flow_stmt_t *stmt)
{
	switch (stmt->tok) {
	case TOK_BREAK:
		append_indent(out);
		out->append("break;\n");
		break;
	case TOK_CONTINUE:
		append_indent(out);
		if (cur_for_stmt && cur_for_stmt->post) {
			out->append("goto ");
			out->append(cur_for_stmt->postgoto);
			out->append(";\n");
		} else
			out->append("continue;\n");
		break;
	case TOK_FALLTHROUGH:
		// do nothing, the statement simply omits generating break at
		// the end of a case clause
		break;
	default:
		CRAWL_QASSERT(!"bad token");
	}
}

void pass3_t::codegen_stmt(std::string *out, node_t *stmt)
{
	switch (stmt->type) {
	case node_t::EXPR_STMT:
	{
		expr_stmt_t *s = (expr_stmt_t*)stmt;
		codegen_expr_stmt(out, s);
		break;
	}
	case node_t::DECL_STMT:
	{
		decl_stmt_t *s = (decl_stmt_t*)stmt;
		codegen_decl_stmt(out, s);
		break;
	}
	case node_t::BLOCK_STMT:
	{
		block_stmt_t *s = (block_stmt_t*)stmt;
		codegen_full_block_stmt(out, s);
		break;
	}
	case node_t::RETURN_STMT:
	{
		return_stmt_t *s = (return_stmt_t*)stmt;
		codegen_return_stmt(out, s);
		break;
	}
	case node_t::ASSIGN_STMT:
	{
		assign_stmt_t *s = (assign_stmt_t*)stmt;
		codegen_assign_stmt(out, s);
		break;
	}
	case node_t::INCDEC_STMT:
	{
		incdec_stmt_t *s = (incdec_stmt_t*)stmt;
		codegen_incdec_stmt(out, s);
		break;
	}
	case node_t::IFELSE_STMT:
	{
		ifelse_stmt_t *s = (ifelse_stmt_t*)stmt;
		codegen_ifelse_stmt(out, s);
		break;
	}
	case node_t::FOR_STMT:
	{
		for_stmt_t *s = (for_stmt_t*)stmt;
		codegen_for_stmt(out, s);
		break;
	}
	case node_t::SWITCH_STMT_CASE:
	{
		switch_stmt_case_t *s = (switch_stmt_case_t*)stmt;
		codegen_switch_stmt_case(out, s);
		break;
	}
	case node_t::SWITCH_STMT:
	{
		switch_stmt_t *s = (switch_stmt_t*)stmt;
		codegen_switch_stmt(out, s);
		break;
	}
	case node_t::FLOW_STMT:
	{
		flow_stmt_t *s = (flow_stmt_t*)stmt;
		codegen_flow_stmt(out, s);
		break;
	}
	default:
		CRAWL_QASSERT(!"bad statement type");
	}
}

std::string pass3_t::codegen_funcs()
{
	std::string out;
	for (size_t i = 0, n = funcs.size(); i < n; ++i) {
		codegen_func(&out, funcs[i]);
		out.append(1, '\n');
	}
	return out;
}
void pass3_t::indentpp(std::string *out)
{
	append_indent(out);
	out->append("{\n");
	indent++;
}
void pass3_t::indentmm(std::string *out)
{
	indent--;
	append_indent(out);
	out->append("}\n");
}

std::string pass3_t::real_func_ctype(func_stype_t *fst)
{
	std::string out;
	if (fst->results.empty())
		out += "void";
	else if (fst->results.size() == 1)
		out += ctype(fst->results[0]);
	else
		out += tuple_to_struct(&fst->results);

	out += " (*)(";
	for (size_t i = 0, n = fst->args.size(); i < n; ++i) {
		out += ctype(fst->args[i]);
		if (i != n-1)
			out += ", ";
	}
	if (fst->varargs) {
		if (!fst->args.empty())
			out += ", ";
		out += "...";
	}
	out += ")";
	return out;
}

std::string pass3_t::ctype(stype_t *st)
{
	// good place to make sure that there are no abstract types in pass3
	CRAWL_QASSERT((st->type & STYPE_ABSTRACT) == 0);

	if (st->type & STYPE_VOID) {
		return "void";
	} else if (st->type & STYPE_BOOL) {
		return "bool";
	} else if (st->type & STYPE_INT) {
		int_stype_t *ist = (int_stype_t*)st->end_type();
		if (ist->unsignd) {
			switch (ist->size) {
			case 8:  return "uint8_t";
			case 16: return "uint16_t";
			case 32: return "uint32_t";
			case 64: return "uint64_t";
			}
		} else {
			switch (ist->size) {
			case 8:  return "int8_t";
			case 16: return "int16_t";
			case 32: return "int32_t";
			case 64: return "int64_t";
			}
		}
	} else if (st->type & STYPE_FLOAT) {
		float_stype_t *fst = (float_stype_t*)st->end_type();
		switch (fst->size) {
		case 32: return "float";
		case 64: return "double";
		}
	} else if (st->type & (STYPE_POINTER | STYPE_FUNC)) {
		return "uint8_t*";
	} else if (st->type & STYPE_ARRAY) {
		array_stype_t *ast = (array_stype_t*)st->end_type();
		return array_to_struct(ast);
	} else if (st->type & STYPE_STRUCT) {
		struct_stype_t *sst = (struct_stype_t*)st->end_type();
		return struct_to_struct(sst);
	}
	return "???";
}
