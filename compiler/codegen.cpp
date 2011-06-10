#include "krawl.hpp"
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Type.h>
#include <llvm/PassManager.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/Timer.h>
#include <llvm/Support/Host.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetRegistry.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Intrinsics.h>
#include <algorithm>

using namespace llvm;

#define LLGC (getGlobalContext())

struct cur_loop_t {
	BasicBlock *continue_;
	BasicBlock *break_;
};

struct llvm_backend_t {
	std::string uid;
	scope_block_t *pkgscope;
	std::vector<import_sdecl_t*> *used_extern_sdecls;
	const char *out_name;
	std::vector<const char*> *libs;
	bool dump;
	bool time;

	Module *module;
	IRBuilder<> *ir;
	IRBuilder<> *ir_alloca;
	IRBuilder<> *ir_init;
	IRBuilder<> *ir_init_alloca;
	Instruction *ir_init_alloca_pt;

	func_sdecl_t *cur_func_decl;

	// for flow instructions
	cur_loop_t *cur_loop;

	// for switch statements
	SwitchInst *cur_switch;
	BasicBlock *cur_switch_end;
	bool ended_with_fallthrough;

	Constant *llvmconst(value_t *val, const Type *ty);
	const Type *llvmrettype(func_stype_t *fst);
	FunctionType *llvmfunctype(func_stype_t *fst);
	const Type *llvmtype(stype_t *st);

	void create_init_func();
	void finalize_init_func();

	void codegen_MRV(std::vector<Value*> *values,
			 std::vector<stype_t*> *vtypes, node_t *callexpr);
	void codegen_switch_stmt_case(switch_stmt_case_t *stmt);
	void codegen_switch_stmt(switch_stmt_t *stmt);
	void codegen_flow_stmt(flow_stmt_t *stmt);
	void codegen_for_stmt(for_stmt_t *stmt);
	void codegen_ifelse_stmt(ifelse_stmt_t *stmt);
	void codegen_incdec_stmt(incdec_stmt_t *stmt);
	void codegen_compound_assign_stmt(assign_stmt_t *stmt);
	void codegen_assign_stmt(assign_stmt_t *stmt);
	void codegen_var_spec(value_spec_t *spec);
	void codegen_decl_stmt(decl_stmt_t *stmt);
	void codegen_return_stmt(return_stmt_t *stmt);
	void codegen_block_stmt(block_stmt_t *stmt);
	void codegen_stmt(node_t *stmt);

	void codegen_arithmetic_conversion(Value **l, Value **r,
					   stype_t *lt, stype_t *rt,
					   int tok);
	Value *codegen_varargs_promotion(Value *v, stype_t *vtype);
	Value *codegen_implicit_conversion_up(Value *v, stype_t *to);
	Value *codegen_implicit_conversion_down(Value *v, stype_t *from,
						stype_t *to);
	Value *codegen_assignment(Value *expr, stype_t *from, stype_t *to);
	Value *codegen_binary_expr_raw(Value *l, Value *r, stype_t *et,
				       stype_t *lt, stype_t *rt, int tok);
	Value *codegen_binary_expr(binary_expr_t *e);
	Value *codegen_unary_expr(unary_expr_t *e);
	Value *codegen_type_cast_expr(type_cast_expr_t *e);
	Value *codegen_builtin_call_expr(call_expr_t *e);
	Value *codegen_call_expr(call_expr_t *e);
	Value *codegen_compound_lit(compound_lit_t *e);
	Value *codegen_union_cast(selector_expr_t *e, Value *addr);
	Value *codegen_expr_addr(node_t *e);
	Value *codegen_expr_value(node_t *e);
	Value *codegen_post_load(Value *v, stype_t *ty);
	Value *codegen_load(Value *addr, stype_t *ty);

	Value *codegen_cexpr_value(node_t *e);
	Value *codegen_cexpr_store(Value *val, Value *addr, node_t *e);

	void codegen_init_deps(node_t *e);
	void codegen_init(var_sdecl_t *d);

	void codegen_top_extern_sdecls(import_sdecl_t *isd);
	void codegen_top_func_pre(func_sdecl_t *fsd, const char *prefix);
	void codegen_top_func(func_sdecl_t *fsd);
	void codegen_top_var_pre(var_sdecl_t *vsd);
	void codegen_top_var(var_sdecl_t *vsd);
	void codegen_top_sdecl_pre(sdecl_t *d);
	void codegen_top_sdecl(sdecl_t *d);
	void codegen_top_sdecls(std::vector<const char*> *pkgdecls);

	void compile_exe(TargetMachine *machine, PassManager *pm);
	void compile_lib(TargetMachine *machine, PassManager *pm);

	// Interface
	void pass(std::vector<const char*> *pkgdecls);
};

static bool is_bb_terminated(BasicBlock *bb)
{
	return isa<TerminatorInst>(bb->back());
}

static void insert_opt_endjmp(IRBuilder<> *ir, BasicBlock *E)
{
	if (ir->GetInsertBlock()->empty() ||
	    !is_bb_terminated(ir->GetInsertBlock()))
	{
		ir->CreateBr(E);
	}
}

static bool is_bb_returns(BasicBlock *bb)
{
	return !bb->empty() && isa<ReturnInst>(bb->back());
}

static void generate_NRV_ret(IRBuilder<> *ir, func_sdecl_t *fsd)
{
	func_stype_t *fst = fsd->stype->as_func();
	func_type_t *ft = fsd->decl->ftype;

	std::vector<Value*> vals;
	vals.reserve(fst->results.size());

	for (size_t i = 0, n = ft->results.size(); i < n; ++i) {
		field_t *f = ft->results[i];
		// names cannot be empty here

		for (size_t j = 0, m = f->names.size(); j < m; ++j) {
			KRAWL_QASSERT(f->names[j]->sdecl->type == SDECL_VAR);
			var_sdecl_t *d = (var_sdecl_t*)f->names[j]->sdecl;
			// no need to perform additional assignment magic
			// types here are a 100% match anyway
			vals.push_back(ir->CreateLoad(d->addr));
		}
	}

	if (vals.size() == 1)
		ir->CreateRet(vals[0]);
	else
		ir->CreateAggregateRet(&vals[0], vals.size());
}

static void finalize_function(IRBuilder<> *ir, func_sdecl_t *fsd)
{
	BasicBlock *bb = ir->GetInsertBlock();
	// no predecessors
	if (&bb->getParent()->front() != bb && pred_begin(bb) == pred_end(bb)) {
		bb->eraseFromParent();
		return;
	}

	func_stype_t *fst = fsd->stype->as_func();
	func_type_t *ft = fsd->decl->ftype;

	if (fst->results.empty()) {
		ir->CreateRetVoid();
		return;
	}

	if (fsd->has_named_return_values() && !is_bb_returns(bb))
		generate_NRV_ret(ir, fsd);
}

static stype_t *compound_lit_stype_i(compound_lit_t *c, size_t i)
{
	if (IS_STYPE_ARRAY(c->vst.stype)) {
		array_stype_t *ast = c->vst.stype->as_array();
		return ast->elem;
	} else {
		struct_stype_t *sst = c->vst.stype->as_struct();
		return sst->fields[i].type;
	}
	KRAWL_ASSERT(false, "unreachable");
}

Constant *llvm_backend_t::llvmconst(value_t *val, const Type *ty)
{
	switch (val->type) {
	case VALUE_STRING:
	{
		ArrayType *straty = ArrayType::get(IntegerType::get(LLGC, 8),
						   strlen(val->xstr) + 1);
		Constant *strc = ConstantArray::get(LLGC, val->xstr);
		GlobalVariable *strcp;
		strcp = new GlobalVariable(*module, straty,
					   true, GlobalValue::PrivateLinkage,
					   strc, ".str");

		Constant *zero = ConstantInt::getNullValue(Type::getInt32Ty(LLGC));
		Constant *zeros[] = {zero, zero};
		return ConstantExpr::getGetElementPtr(strcp, zeros, 2);
	}
	case VALUE_INT:
	{
		std::string i = val->to_string();
		if (ty->isPointerTy()) {
			const IntegerType *ity = IntegerType::get(LLGC, sizeof(void*) * 8);
			Constant *c = ConstantInt::get(ity, i, 10);
			return ConstantExpr::getIntToPtr(c, ty);
		}
		if (ty->getPrimitiveSizeInBits() < 32)
			ty = IntegerType::get(LLGC, 32);
		return ConstantInt::get(cast<IntegerType>(ty), i, 10);
	}
	case VALUE_FLOAT:
	{
		double d = mpfr_get_d(val->xfloat, MPFR_RNDN);
		return ConstantFP::get(ty, d);
	}
	default:
		KRAWL_QASSERT(!"invalid value");
		return 0;
	}
}

const Type *llvm_backend_t::llvmrettype(func_stype_t *fst)
{
	if (fst->results.size() == 0) {
		return Type::getVoidTy(LLGC);
	} else if (fst->results.size() == 1) {
		return llvmtype(fst->results[0]);
	} else {
		std::vector<const Type*> fields;
		fields.reserve(fst->results.size());
		for (size_t i = 0, n = fst->results.size(); i < n; ++i)
			fields.push_back(llvmtype(fst->results[i]));

		return StructType::get(LLGC, fields);
	}
}

FunctionType *llvm_backend_t::llvmfunctype(func_stype_t *fst)
{
	const Type *rettype = llvmrettype(fst);
	std::vector<const Type*> args;
	args.reserve(fst->args.size());
	for (size_t i = 0, n = fst->args.size(); i < n; ++i)
		args.push_back(llvmtype(fst->args[i]));

	return FunctionType::get(rettype, args, fst->varargs);
}

// helpers for recursive types
#define BIND_TMP_TYPE()							\
	PATypeHolder tmp = OpaqueType::get(LLGC);			\
	st->llvmtype = tmp.get()

#define REFINE_TMP_TYPE(_var)						\
	cast<OpaqueType>(tmp.get())->refineAbstractTypeTo(_var);	\
	st->llvmtype = cast<Type>(tmp.get())

const Type *llvm_backend_t::llvmtype(stype_t *st)
{
	KRAWL_QASSERT((st->type & STYPE_ABSTRACT) == 0);

	if (st->llvmtype)
		return st->llvmtype;

	if (st->type & STYPE_VOID) {
		st->llvmtype = Type::getInt8Ty(LLGC);
	} else if (st->type & STYPE_BOOL) {
		st->llvmtype = Type::getInt8Ty(LLGC);
	} else if (st->type & STYPE_INT) {
		switch (st->bits()) {
		case 8:  st->llvmtype = Type::getInt8Ty(LLGC); break;
		case 16: st->llvmtype = Type::getInt16Ty(LLGC); break;
		case 32: st->llvmtype = Type::getInt32Ty(LLGC); break;
		case 64: st->llvmtype = Type::getInt64Ty(LLGC); break;
		}
	} else if (st->type & STYPE_FLOAT) {
		switch (st->bits()) {
		case 32: st->llvmtype = Type::getFloatTy(LLGC); break;
		case 64: st->llvmtype = Type::getDoubleTy(LLGC); break;
		}
	} else if (st->type & STYPE_POINTER) {
		pointer_stype_t *pst = st->as_pointer();

		BIND_TMP_TYPE();

		const Type *points_to = llvmtype(pst->points_to);
		PointerType *ptrty = PointerType::getUnqual(points_to);

		REFINE_TMP_TYPE(ptrty);
	} else if (st->type & STYPE_FUNC) {
		func_stype_t *fst = st->as_func();

		BIND_TMP_TYPE();

		const Type *functy = PointerType::getUnqual(llvmfunctype(fst));

		REFINE_TMP_TYPE(functy);
	} else if (st->type & (STYPE_STRUCT | STYPE_UNION)) {
		struct_stype_t *sst = st->as_struct();

		BIND_TMP_TYPE();

		std::vector<const Type*> fields;
		if (sst->u) {
			fields.push_back(llvmtype(sst->biggest));
		} else {
			fields.reserve(sst->fields.size());
			for (size_t i = 0, n = sst->fields.size(); i < n; ++i)
				fields.push_back(llvmtype(sst->fields[i].type));
		}

		StructType *structty = StructType::get(LLGC, fields);

		REFINE_TMP_TYPE(structty);
	} else if (st->type & STYPE_ARRAY) {
		array_stype_t *ast = st->as_array();

		BIND_TMP_TYPE();

		const Type *elem = llvmtype(ast->elem);
		ArrayType *arrty = ArrayType::get(elem, ast->size);

		REFINE_TMP_TYPE(arrty);
	}

	KRAWL_QASSERT(st->llvmtype != 0);
	return st->llvmtype;
}

struct init_deps_codegen_t : ast_visitor_t {
	llvm_backend_t *backend;

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
				backend->codegen_init(*it);
			return this;
		}

		if (d->type != SDECL_VAR)
			return this;

		// Global variables are declared in a file scope, the parent
		// of each file scope is the package scope.
		if (d->scope->parent == 0 || d->scope->parent != backend->pkgscope)
			return this;

		backend->codegen_init((var_sdecl_t*)d);
		return this;
	}
};

void llvm_backend_t::codegen_init_deps(node_t *e)
{
	init_deps_codegen_t gen;
	gen.backend = this;
	gen.traverse(e);
}

void llvm_backend_t::codegen_MRV(std::vector<Value*> *values,
				 std::vector<stype_t*> *vtypes, node_t *expr)
{
	call_expr_t *callexpr = is_call_expr(expr);
	KRAWL_QASSERT(callexpr != 0);
	func_stype_t *fst = callexpr->expr->vst.stype->as_func();

	Value *mrv = codegen_expr_value(callexpr);
	values->reserve(fst->results.size());
	vtypes->reserve(fst->results.size());
	for (size_t i = 0, n = fst->results.size(); i < n; ++i) {
		values->push_back(ir->CreateExtractValue(mrv, i));
		vtypes->push_back(fst->results[i]);
	}
}

void llvm_backend_t::codegen_switch_stmt_case(switch_stmt_case_t *stmt)
{
	Function *curf = cast<Function>(cur_func_decl->addr);
	BasicBlock *C = 0;
	if (stmt->exprs.empty())
		C = cur_switch->getDefaultDest();
	else
		C = BasicBlock::Create(LLGC, "", curf);

	if (ended_with_fallthrough) {
		// if previous block was ended with 'fallthrough' statement,
		// jump to the current one
		ended_with_fallthrough = false;
		ir->CreateBr(C);
	}

	std::vector<Value*> values;
	values.reserve(stmt->exprs.size());
	for (size_t i = 0, n = stmt->exprs.size(); i < n; ++i)
		values.push_back(codegen_expr_value(stmt->exprs[i]));

	for (size_t i = 0, n = values.size(); i < n; ++i)
		cur_switch->addCase(cast<ConstantInt>(values[i]), C);

	ir->SetInsertPoint(C);
	for (size_t i = 0, n = stmt->stmts.size(); i < n; ++i)
		codegen_stmt(stmt->stmts[i]);

	if (!stmt->stmts.empty()) {
		node_t *last = stmt->stmts.back();
		if (last->type == node_t::FLOW_STMT &&
		    ((flow_stmt_t*)last)->tok == TOK_FALLTHROUGH)
		{
			ended_with_fallthrough = true;
			return;
		}
	}

	if (ir->GetInsertBlock()->empty() ||
	    !isa<TerminatorInst>(ir->GetInsertBlock()->back()))
	{
		ir->CreateBr(cur_switch_end);
	}
}

void llvm_backend_t::codegen_switch_stmt(switch_stmt_t *stmt)
{
	Function *curf = cast<Function>(cur_func_decl->addr);
	BasicBlock *def = BasicBlock::Create(LLGC, "", curf);

	// get switch operand value
	Value *v = codegen_expr_value(stmt->expr);

	cur_loop_t *save_cur_loop = cur_loop;
	SwitchInst *save_cur_switch = cur_switch;
	BasicBlock *save_cur_switch_end = cur_switch_end;

	cur_switch = ir->CreateSwitch(v, def, stmt->num_cases());
	cur_switch_end = BasicBlock::Create(LLGC, "", curf);

	// hack for break statement
	cur_loop_t loop = { cur_switch_end, 0 };
	cur_loop = &loop;

	codegen_block_stmt(stmt->body);

	if (def->empty()) {
		ir->SetInsertPoint(def);
		ir->CreateBr(cur_switch_end);
	}
	ir->SetInsertPoint(cur_switch_end);

	cur_loop = save_cur_loop;
	cur_switch = save_cur_switch;
	cur_switch_end = save_cur_switch_end;
	ended_with_fallthrough = false;

}

void llvm_backend_t::codegen_flow_stmt(flow_stmt_t *stmt)
{
	switch (stmt->tok) {
	case TOK_BREAK:
		ir->CreateBr(cur_loop->break_);
		break;
	case TOK_CONTINUE:
		ir->CreateBr(cur_loop->continue_);
		break;
	case TOK_FALLTHROUGH:
		break; // handled elsewhere
	default:
		KRAWL_QASSERT(!"bad token");
	}
}

void llvm_backend_t::codegen_for_stmt(for_stmt_t *stmt)
{
	Function *curf = cast<Function>(cur_func_decl->addr);

	// prepare blocks
	BasicBlock *LC = 0;
	if (stmt->cond)
		LC = BasicBlock::Create(LLGC, "", curf);
	BasicBlock *L = BasicBlock::Create(LLGC, "", curf);
	BasicBlock *P = 0;
	if (stmt->post)
		P = BasicBlock::Create(LLGC, "", curf);
	BasicBlock *E = BasicBlock::Create(LLGC, "", curf);

	// setup loop info for flow instructions generator
	cur_loop_t loop = {
		// for explanation see loop branch
		P ? P : (LC ? LC : L),
		E
	};
	cur_loop_t *save_cur_loop = cur_loop;
	cur_loop = &loop;

	// generate init statement in-place
	if (stmt->init)
		codegen_stmt(stmt->init);
	// jump to the loop body directly if there is no condition, otherwise
	// jump to the loop condition branch
	ir->CreateBr(LC ? LC : L);

	// LOOP CONDITION branch
	if (stmt->cond) {
		ir->SetInsertPoint(LC);
		ir->CreateCondBr(codegen_expr_value(stmt->cond),
				 L, E);
	}

	// LOOP branch
	ir->SetInsertPoint(L);
	codegen_block_stmt(stmt->body);

	// if there is a post increment branch, jump into it
	// otherwise:
	//     if there is a loop condition branch, jump into it
	//     otherwise:
	//         jump to the loop body
	if (ir->GetInsertBlock()->empty() ||
	    !is_bb_terminated(ir->GetInsertBlock()))
	{
		ir->CreateBr(P ? P : (LC ? LC : L));
	}

	// POST INCREMENT branch
	if (stmt->post) {
		ir->SetInsertPoint(P);
		codegen_stmt(stmt->post);
		ir->CreateBr(LC ? LC : L);
	}

	// END branch
	ir->SetInsertPoint(E);
	cur_loop = save_cur_loop;
}

void llvm_backend_t::codegen_ifelse_stmt(ifelse_stmt_t *stmt)
{
	Function *curf = cast<Function>(cur_func_decl->addr);

	// prepare blocks
	BasicBlock *T = BasicBlock::Create(LLGC, "", curf);
	BasicBlock *F = BasicBlock::Create(LLGC, "", curf);
	BasicBlock *E = F;
	if (stmt->elsestmt)
		E = BasicBlock::Create(LLGC, "", curf);

	// should be i1 anyway, no icmp or anything
	Value *cond = codegen_expr_value(stmt->cond);
	ir->CreateCondBr(cond, T, F);

	// TRUE branch
	ir->SetInsertPoint(T);
	codegen_block_stmt(stmt->body);
	insert_opt_endjmp(ir, E);

	// ELSE branch
	if (stmt->elsestmt) {
		ir->SetInsertPoint(F);
		codegen_stmt(stmt->elsestmt);
		insert_opt_endjmp(ir, E);
	}

	// END branch
	ir->SetInsertPoint(E);
}

void llvm_backend_t::codegen_incdec_stmt(incdec_stmt_t *stmt)
{
	stype_t *t = stmt->expr->vst.stype;
	Value *addr = codegen_expr_addr(stmt->expr);
	Value *l = ir->CreateLoad(addr);
	Value *r = ConstantInt::get(IntegerType::get(LLGC, t->bits()), 1);
	int tok = (stmt->tok == TOK_INC ? TOK_PLUS : TOK_MINUS);
	// playing with fire here, but...
	//
	// if 'stmt->expr' is a pointer, RHS is an integer size, but since
	// codegen_binary_expr_raw doesn't really care about that, we're ok here
	Value *result = codegen_binary_expr_raw(l, r, t, t, t, tok);
	ir->CreateStore(result, addr);
}

void llvm_backend_t::codegen_compound_assign_stmt(assign_stmt_t *stmt)
{
	Value *addr = codegen_expr_addr(stmt->lhs[0]);
	Value *l = codegen_load(addr, stmt->lhs[0]->vst.stype);
	Value *r = codegen_expr_value(stmt->rhs[0]);
	stype_t *lt = stmt->lhs[0]->vst.stype;
	stype_t *rt = stmt->rhs[0]->vst.stype;
	int tok = compound_assignment_to_binary_tok(stmt->tok);
	Value *result = codegen_binary_expr_raw(l, r, lt, lt, rt, tok);
	result = codegen_assignment(result, lt, lt);
	ir->CreateStore(result, addr);
}

void llvm_backend_t::codegen_assign_stmt(assign_stmt_t *stmt)
{
	if (stmt->tok != TOK_ASSIGN && stmt->tok != TOK_DECLARIZE) {
		// += -= *= /=, etc.
		codegen_compound_assign_stmt(stmt);
		return;
	}

	// codegen addresses before, because we need to eval side effect
	// expressions
	std::vector<Value*> addrs;
	std::vector<Value*> values;
	std::vector<stype_t*> vtypes;

	for (size_t i = 0, n = stmt->lhs.size(); i < n; ++i)
		addrs.push_back(codegen_expr_addr(stmt->lhs[i]));

	// special case, MRV
	if (stmt->lhs.size() > 1 && stmt->rhs.size() == 1)
		codegen_MRV(&values, &vtypes, stmt->rhs[0]);
	else {
		for (size_t i = 0, n = stmt->rhs.size(); i < n; ++i) {
			values.push_back(codegen_cexpr_value(stmt->rhs[i]));
			vtypes.push_back(stmt->rhs[i]->vst.stype);
		}
	}

	for (size_t i = 0, n = stmt->lhs.size(); i < n; ++i) {
		Value *addr;
		stype_t *to;

		if (stmt->tok == TOK_DECLARIZE &&
		    (stmt->lhs[i]->type == node_t::IDENT_EXPR))
		{
			// declaration, allocate store
			ident_expr_t *name = (ident_expr_t*)stmt->lhs[i];
			var_sdecl_t *vsd = (var_sdecl_t*)name->sdecl;
			const Type *ty = llvmtype(vsd->stype);
			vsd->addr = ir_alloca->CreateAlloca(ty);

			addr = vsd->addr;
			to = vsd->stype;
		} else {
			addr = addrs[i];
			to = stmt->lhs[i]->vst.stype;
		}

		if (!values[i]) {
			codegen_cexpr_store(values[i], addr, stmt->rhs[i]);
		} else {
			Value *v = codegen_assignment(values[i], vtypes[i], to);
			ir->CreateStore(v, addr);
		}
	}
}

void llvm_backend_t::codegen_var_spec(value_spec_t *spec)
{
	std::vector<Value*> inits;
	std::vector<stype_t*> inittypes;

	if (spec->names.size() > 1 && spec->values.size() == 1)
		codegen_MRV(&inits, &inittypes, spec->values[0]);
	else {
		for (size_t i = 0, n = spec->values.size(); i < n; ++i) {
			inits.push_back(codegen_cexpr_value(spec->values[i]));
			inittypes.push_back(spec->values[i]->vst.stype);
		}
	}

	for (size_t i = 0, n = spec->names.size(); i < n; ++i) {
		ident_expr_t *name = spec->names[i];
		var_sdecl_t *vsd = (var_sdecl_t*)name->sdecl;
		const Type *ty = llvmtype(vsd->stype);
		vsd->addr = ir_alloca->CreateAlloca(ty);
		if (vsd->init) {
			if (!inits[i])
				codegen_cexpr_store(inits[i], vsd->addr,
						    spec->values[i]);
			else {
				Value *v = codegen_assignment(inits[i],
							      inittypes[i],
							      vsd->stype);
				ir->CreateStore(v, vsd->addr);
			}
		}
		else
			ir->CreateStore(Constant::getNullValue(ty), vsd->addr);
	}
}

void llvm_backend_t::codegen_decl_stmt(decl_stmt_t *stmt)
{
	switch (stmt->decl->type) {
	case node_t::VAR_DECL:
	{
		var_decl_t *d = (var_decl_t*)stmt->decl;
		for (size_t i = 0, n = d->specs.size(); i < n; ++i)
			codegen_var_spec(d->specs[i]);
		break;
	}
	default:
		break;
		//KRAWL_QASSERT(!"fail");
	}
}

void llvm_backend_t::codegen_return_stmt(return_stmt_t *stmt)
{
	func_stype_t *fst = cur_func_decl->stype->as_func();
	if (stmt->returns.size() == 0) {
		if (cur_func_decl->has_named_return_values())
			generate_NRV_ret(ir, cur_func_decl);
		else
			ir->CreateRetVoid();
	} else if (stmt->returns.size() == 1) {
		Value *v = codegen_expr_value(stmt->returns[0]);
		v = codegen_assignment(v, stmt->returns[0]->vst.stype,
				       fst->results[0]);
		ir->CreateRet(v);
	} else {
		std::vector<Value*> vals;
		vals.reserve(stmt->returns.size());
		for (size_t i = 0, n = stmt->returns.size(); i < n; ++i) {
			Value *v = codegen_expr_value(stmt->returns[i]);
			v = codegen_assignment(v, stmt->returns[i]->vst.stype,
					       fst->results[i]);
			vals.push_back(v);
		}
		ir->CreateAggregateRet(&vals[0], vals.size());
	}
}

void llvm_backend_t::codegen_block_stmt(block_stmt_t *stmt)
{
	for (size_t i = 0, n = stmt->stmts.size(); i < n; ++i) {
		node_t *s = stmt->stmts[i];
		codegen_stmt(s);

		// there is no point in generating something after terminator
		// statement
		if (s->is_terminator())
			return;
	}
}

void llvm_backend_t::codegen_stmt(node_t *stmt)
{
	switch (stmt->type) {
	case node_t::EXPR_STMT:
	{
		expr_stmt_t *s = (expr_stmt_t*)stmt;
		codegen_expr_value(s->expr);
		break;
	}
	case node_t::DECL_STMT:
	{
		decl_stmt_t *s = (decl_stmt_t*)stmt;
		codegen_decl_stmt(s);
		break;
	}
	case node_t::BLOCK_STMT:
	{
		block_stmt_t *s = (block_stmt_t*)stmt;
		codegen_block_stmt(s);
		break;
	}
	case node_t::RETURN_STMT:
	{
		return_stmt_t *s = (return_stmt_t*)stmt;
		codegen_return_stmt(s);
		break;
	}
	case node_t::ASSIGN_STMT:
	{
		assign_stmt_t *s = (assign_stmt_t*)stmt;
		codegen_assign_stmt(s);
		break;
	}
	case node_t::INCDEC_STMT:
	{
		incdec_stmt_t *s = (incdec_stmt_t*)stmt;
		codegen_incdec_stmt(s);
		break;
	}
	case node_t::IFELSE_STMT:
	{
		ifelse_stmt_t *s = (ifelse_stmt_t*)stmt;
		codegen_ifelse_stmt(s);
		break;
	}
	case node_t::FOR_STMT:
	{
		for_stmt_t *s = (for_stmt_t*)stmt;
		codegen_for_stmt(s);
		break;
	}
	case node_t::SWITCH_STMT_CASE:
	{
		switch_stmt_case_t *s = (switch_stmt_case_t*)stmt;
		codegen_switch_stmt_case(s);
		break;
	}
	case node_t::SWITCH_STMT:
	{
		switch_stmt_t *s = (switch_stmt_t*)stmt;
		codegen_switch_stmt(s);
		break;
	}
	case node_t::FLOW_STMT:
	{
		flow_stmt_t *s = (flow_stmt_t*)stmt;
		codegen_flow_stmt(s);
		break;
	}
	default:
		KRAWL_QASSERT(!"bad statement type");
	}
}

Value *llvm_backend_t::codegen_assignment(Value *expr, stype_t *from, stype_t *to)
{
	if (IS_STYPE_POINTER(from) && IS_STYPE_POINTER(to)) {
		if (!are_the_same(from->end_type(), to->end_type()))
			return ir->CreatePointerCast(expr, llvmtype(to));
	}
	if (IS_STYPE_BOOL(to))
		return ir->CreateZExt(expr, Type::getInt8Ty(LLGC));

	if (IS_STYPE_NUMBER(from) && IS_STYPE_NUMBER(to)) {
		if (from->bits() < to->bits() && to->bits() > 32)
			return codegen_implicit_conversion_up(expr, to);
		return codegen_implicit_conversion_down(expr, from, to);
	}
	return expr;
}

Value *llvm_backend_t::codegen_varargs_promotion(Value *v, stype_t *vtype)
{
	if (IS_STYPE_FLOAT(vtype) && vtype->bits() < 64) {
		return ir->CreateFPExt(v, Type::getDoubleTy(LLGC));
	}
	if (IS_STYPE_INT(vtype) && vtype->bits() < 32) {
		return ir->CreateSExt(v, IntegerType::get(LLGC, 32));
	}
	return v;
}

Value *llvm_backend_t::codegen_implicit_conversion_up(Value *v, stype_t *to)
{
	if (IS_STYPE_FLOAT(to)) {
		return ir->CreateFPExt(v, llvmtype(to));
	}
	if (IS_STYPE_INT(to)) {
		int_stype_t *ist = to->as_int();
		if (ist->is_signed)
			return ir->CreateSExt(v, llvmtype(to));
		else
			return ir->CreateZExt(v, llvmtype(to));
	}
	return v;
}

Value *llvm_backend_t::codegen_implicit_conversion_down(Value *v, stype_t *from,
							stype_t *to)
{
	if (IS_STYPE_FLOAT(to) && to->bits() < from->bits())
		return ir->CreateFPTrunc(v, llvmtype(to));

	if (IS_STYPE_INT(to)) {
		if (to->bits() < 32)
			return ir->CreateTrunc(v, llvmtype(to));

		if (to->bits() < from->bits())
			return ir->CreateTrunc(v, llvmtype(to));
	}
	return v;
}

void llvm_backend_t::codegen_arithmetic_conversion(Value **l, Value **r,
						   stype_t *lt, stype_t *rt,
						   int tok)
{
	if (tok == TOK_SHIFTL || tok == TOK_SHIFTR) {
		if (lt->bits() == 64 && rt->bits() < 64)
			*r = ir->CreateZExt(*r, IntegerType::get(LLGC, 64));
		else if (rt->bits() == 64 && lt->bits() < 64)
			*r = ir->CreateTrunc(*r, IntegerType::get(LLGC, 32));
		return;
	}

	if (lt->bits() == rt->bits())
		return;

	// hello, wonderer, looking for a way to contribute? fix this crappy
	// code down below
	stype_t *big = biggest_stype(lt, rt);
	Value *v = (lt == big) ? *r : *l;
	stype_t *small = (lt == big) ? rt : lt;

	if (IS_STYPE_INT(big) && IS_STYPE_INT(small)) {
		// check special case: uint32 OP int16
		// it needs to be converted to int32
		int_stype_t *ibig   = big->as_int();
		int_stype_t *ismall = small->as_int();
		if (!ibig->is_signed && ismall->is_signed)
			big = get_int_type(ibig->size);
	}

	v = codegen_implicit_conversion_up(v, big);

	if (lt == big)
		*r = v;
	else
		*l = v;
}

Value *llvm_backend_t::codegen_binary_expr_raw(Value *l, Value *r, stype_t *et,
					       stype_t *lt, stype_t *rt, int tok)
{
	codegen_arithmetic_conversion(&l, &r, lt, rt, tok);
	if (IS_STYPE_INT(et)) {
		int_stype_t *ist = et->as_int();

		switch (tok) {
		case TOK_PLUS:
			return ir->CreateAdd(l, r);
		case TOK_MINUS:
			// special case for pointers, convert to pointer sized int
			// and then convert back to resulting int type
			if (IS_STYPE_POINTER(lt)) {
				// CreatePtrDiff uses i64 to calculate stuff,
				// should we change it?
				Value *dif = ir->CreatePtrDiff(l, r);
				return ir->CreateTrunc(dif, Type::getInt32Ty(LLGC));
				/*
				int bits = lt->bits();
				const Type *endty = llvmtype(ist);
				const Type *ity = IntegerType::get(LLGC, bits);
				l = ir->CreatePtrToInt(l, ity);
				r = ir->CreatePtrToInt(r, ity);
				Value *sub = ir->CreateSub(l, r);
				return ir->CreateBitCast(sub, endty);
				*/
			}
			return ir->CreateSub(l, r);
		case TOK_TIMES:
			return ir->CreateMul(l, r);
		case TOK_DIVIDE:
			if (ist->is_signed)
				return ir->CreateSDiv(l, r);
			return ir->CreateUDiv(l, r);
		case TOK_AND:
			return ir->CreateAnd(l, r);
		case TOK_OR:
			return ir->CreateOr(l, r);
		case TOK_XOR:
			return ir->CreateXor(l, r);
		case TOK_MOD:
			if (ist->is_signed)
				return ir->CreateSRem(l, r);
			return ir->CreateURem(l, r);
		case TOK_SHIFTL:
			return ir->CreateShl(l, r);
		case TOK_SHIFTR:
			if (ist->is_signed)
				return ir->CreateAShr(l, r);
			return ir->CreateLShr(l, r);
		}
	} else if (IS_STYPE_FLOAT(et)) {
		switch (tok) {
		case TOK_PLUS:
			return ir->CreateFAdd(l, r);
		case TOK_MINUS:
			return ir->CreateFSub(l, r);
		case TOK_TIMES:
			return ir->CreateFMul(l, r);
		case TOK_DIVIDE:
			return ir->CreateFDiv(l, r);
		}
	} else if (IS_STYPE_POINTER(et)) {
		// means pointer/integer addition or subtraction
		stype_t *lhs = lt;
		stype_t *rhs = rt;

		// swap elements if left one is integer, it's safe because
		// integer on the LHS is allowed only when it's an addition
		if (IS_STYPE_INT(lhs)) {
			std::swap(l, r);
			std::swap(lhs, rhs);
		}

		if (tok == TOK_MINUS) {
			const Type *ty = IntegerType::get(LLGC, rhs->bits());
			Constant *zero = ConstantInt::getNullValue(ty);
			r = ir->CreateSub(zero, r);
		}
		return ir->CreateGEP(l, r);
	}

	KRAWL_QASSERT(IS_STYPE_BOOL(et));
	if (IS_STYPE_POINTER_OR_INT(lt)) {
		if (IS_STYPE_POINTER(lt) && IS_STYPE_POINTER(rt)) {
			if (!are_the_same(lt->end_type(), rt->end_type()))
				r = ir->CreatePointerCast(r, llvmtype(lt));
		}
		bool is_signed = false;
		if (IS_STYPE_INT(lt))
			is_signed = lt->as_int()->is_signed;

		switch (tok) {
		case TOK_EQ:
			return ir->CreateICmpEQ(l, r);
		case TOK_NEQ:
			return ir->CreateICmpNE(l, r);
		case TOK_LT:
			if (is_signed)
				return ir->CreateICmpSLT(l, r);
			return ir->CreateICmpULT(l, r);
		case TOK_LE:
			if (is_signed)
				return ir->CreateICmpSLE(l, r);
			return ir->CreateICmpULE(l, r);
		case TOK_GT:
			if (is_signed)
				return ir->CreateICmpSGT(l, r);
			return ir->CreateICmpUGT(l, r);
		case TOK_GE:
			if (is_signed)
				return ir->CreateICmpSGE(l, r);
			return ir->CreateICmpUGE(l, r);
		}
	} else if (IS_STYPE_FLOAT(lt)) {
		switch (tok) {
		case TOK_EQ:
			return ir->CreateFCmpOEQ(l, r);
		case TOK_NEQ:
			return ir->CreateFCmpONE(l, r);
		case TOK_LT:
			return ir->CreateFCmpOLT(l, r);
		case TOK_LE:
			return ir->CreateFCmpOLE(l, r);
		case TOK_GT:
			return ir->CreateFCmpOGT(l, r);
		case TOK_GE:
			return ir->CreateFCmpOGE(l, r);
		}
	} else {
		KRAWL_QASSERT(IS_STYPE_BOOL(lt));
		switch (tok) {
		case TOK_EQ:
			return ir->CreateICmpEQ(l, r);
		case TOK_NEQ:
			return ir->CreateICmpNE(l, r);
		}
	}
	return 0;
}

Value *llvm_backend_t::codegen_binary_expr(binary_expr_t *expr)
{
	Value *l = 0, *r = 0;
	if (expr->tok != TOK_ANDAND && expr->tok != TOK_OROR) {
		l = codegen_expr_value(expr->lhs);
		r = codegen_expr_value(expr->rhs);
		KRAWL_QASSERT(l && r);
		return codegen_binary_expr_raw(l, r, expr->vst.stype,
					       expr->lhs->vst.stype,
					       expr->rhs->vst.stype,
					       expr->tok);
	}

	// one complex case left, '&&' and '||' operators, their semantics
	// requires me to create branching
	KRAWL_QASSERT(IS_STYPE_BOOL(expr->lhs->vst.stype));
	KRAWL_QASSERT(IS_STYPE_BOOL(expr->rhs->vst.stype));
	KRAWL_QASSERT(expr->tok == TOK_ANDAND || expr->tok == TOK_OROR);

	// eval LHS and use it as condition
	Value *cond = codegen_expr_value(expr->lhs);
	KRAWL_QASSERT(cond != 0);

	// utils
	BasicBlock *I = ir->GetInsertBlock();
	Function *f = I->getParent();
	KRAWL_QASSERT(f != 0);
	const Type *boolty = Type::getInt1Ty(LLGC);
	Constant *zero = ConstantInt::getNullValue(boolty);

	// prepare blocks
	BasicBlock *B = BasicBlock::Create(LLGC, "", f);
	BasicBlock *E = BasicBlock::Create(LLGC, "", f);

	// insertion block value, branch block value
	Value *Iv, *Bv;

	// &&: if lhs then rhs else false
	if (expr->tok == TOK_ANDAND) {
		ir->CreateCondBr(cond, B, E);
		Iv = ConstantInt::getFalse(LLGC);

		// rhs
		ir->SetInsertPoint(B);
		Bv = codegen_expr_value(expr->rhs);
		KRAWL_QASSERT(Bv != 0);
		ir->CreateBr(E);
	// ||: if lhs then true else rhs
	} else {
		ir->CreateCondBr(cond, E, B);
		Iv = ConstantInt::getTrue(LLGC);

		// rhs
		ir->SetInsertPoint(B);
		Bv = codegen_expr_value(expr->rhs);
		KRAWL_QASSERT(Bv != 0);
		ir->CreateBr(E);
	}
	// end block
	ir->SetInsertPoint(E);
	PHINode *phi = ir->CreatePHI(boolty);
	phi->reserveOperandSpace(2);
	phi->addIncoming(Iv, I);
	phi->addIncoming(Bv, B);
	return phi;
}

Value *llvm_backend_t::codegen_unary_expr(unary_expr_t *e)
{
	// unary '&' returns an address, no need to do a load
	if (e->tok == TOK_AND)
		return codegen_expr_addr(e->expr);

	Value *expr = codegen_expr_value(e->expr);
	// unary '+' is a no-op
	if (e->tok == TOK_PLUS)
		return expr;

	const Type *ty = llvmtype(e->expr->vst.stype);

	switch (e->tok) {
	case TOK_MINUS:
		KRAWL_QASSERT(IS_STYPE_NUMBER(e->expr->vst.stype));
		return ir->CreateNeg(expr);
	case TOK_XOR:
		KRAWL_QASSERT(IS_STYPE_INT(e->expr->vst.stype));
		return ir->CreateNot(expr);
	case TOK_NOT:
		KRAWL_QASSERT(IS_STYPE_BOOL(e->expr->vst.stype));
		return ir->CreateNot(expr);
	case TOK_TIMES:
		return codegen_load(expr, e->vst.stype);
	}

	KRAWL_QASSERT(!"unreachable");
	return 0;
}

Value *llvm_backend_t::codegen_type_cast_expr(type_cast_expr_t *e)
{
	stype_t *from = e->expr->vst.stype;
	stype_t *to = e->vst.stype;
	Value *expr = codegen_expr_value(e->expr);
	// case 1: float -> int OR int -> float
	if ((IS_STYPE_FLOAT(from) && IS_STYPE_INT(to)) ||
	    (IS_STYPE_INT(from) && IS_STYPE_FLOAT(to)))
	{
		if (IS_STYPE_FLOAT(from)) {
			// float -> int
			int_stype_t *ist = to->as_int();
			if (ist->is_signed)
				return ir->CreateFPToSI(expr, llvmtype(to));
			else
				return ir->CreateFPToUI(expr, llvmtype(to));
		} else {
			// int -> float
			int_stype_t *ist = from->as_int();
			if (ist->is_signed)
				return ir->CreateSIToFP(expr, llvmtype(to));
			else
				return ir->CreateUIToFP(expr, llvmtype(to));
		}
	}

	// case 2: integer conversion
	if (IS_STYPE_INT(from) && IS_STYPE_INT(to)) {
		if (from->bits() < to->bits()) {
			// smaller to larger, sext or zext
			int_stype_t *ist = from->as_int();
			if (ist->is_signed)
				return ir->CreateSExt(expr, llvmtype(to));
			else
				return ir->CreateZExt(expr, llvmtype(to));
		}

		if (from->bits() > to->bits()) {
			// larger to smaller, truncate
			return ir->CreateTrunc(expr, llvmtype(to));
		}

		// same sizes no-op
		return expr;
	}

	// case 3: floats conversion
	if (IS_STYPE_FLOAT(from) && IS_STYPE_FLOAT(to)) {
		if (from->bits() < to->bits())
			return ir->CreateFPExt(expr, llvmtype(to));
		if (from->bits() > to->bits())
			return ir->CreateFPTrunc(expr, llvmtype(to));
		// same sizes no-op
		return expr;
	}

	// case 4: both are pointers, no-op bitcast
	if (IS_STYPE_POINTER(from) && IS_STYPE_POINTER(to))
		return ir->CreatePointerCast(expr, llvmtype(to));

	if (IS_STYPE_POINTER_OR_INT(from) && IS_STYPE_POINTER_OR_INT(to)) {
		// one of them is pointer and the other is int
		if (IS_STYPE_POINTER(from))
			return ir->CreatePtrToInt(expr, llvmtype(to));
		else
			return ir->CreateIntToPtr(expr, llvmtype(to));
	}

	// all other casts are no-op
	return expr;
}

Value *llvm_backend_t::codegen_builtin_call_expr(call_expr_t *expr)
{
	ident_expr_t *e = is_ident_expr(expr->expr);
	KRAWL_QASSERT(e != 0);

	if (e->sdecl->name == "va_start") {
		Function *func;
		func = Intrinsic::getDeclaration(module, Intrinsic::vastart);
		Value *arg = codegen_expr_value(expr->args[0]);
		arg = codegen_assignment(arg, expr->args[0]->vst.stype,
					 builtin_named_stypes[BUILTIN_VA_LIST]);
		return ir->CreateCall(func, arg);
	} else if (e->sdecl->name == "va_end") {
		Function *func;
		func = Intrinsic::getDeclaration(module, Intrinsic::vaend);
		Value *arg = codegen_expr_value(expr->args[0]);
		arg = codegen_assignment(arg, expr->args[0]->vst.stype,
					 builtin_named_stypes[BUILTIN_VA_LIST]);
		return ir->CreateCall(func, arg);
	} else if (e->sdecl->name == "va_copy") {
		// that's silly, I need something generic here
		Function *func;
		func = Intrinsic::getDeclaration(module, Intrinsic::vacopy);
		std::vector<Value*> args;
		std::vector<stype_t*> argtypes;

		if (2 > expr->args.size()) {
			KRAWL_QASSERT(expr->args.size() == 1);
			codegen_MRV(&args, &argtypes, expr->args[0]);
		} else {
			args.push_back(codegen_expr_value(expr->args[0]));
			args.push_back(codegen_expr_value(expr->args[1]));
			argtypes.push_back(expr->args[0]->vst.stype);
			argtypes.push_back(expr->args[1]->vst.stype);
		}

		args[0] = codegen_assignment(args[0], argtypes[0],
					     builtin_named_stypes[BUILTIN_VA_LIST]);
		args[1] = codegen_assignment(args[1], argtypes[1],
					     builtin_named_stypes[BUILTIN_VA_LIST]);
		return ir->CreateCall(func, args.begin(), args.end());
	} else if (e->sdecl->name == "va_arg") {
		const Type *ty = llvmtype(expr->args[1]->vst.stype);
		Value *arg = codegen_expr_value(expr->args[0]);
		arg = codegen_assignment(arg, expr->args[0]->vst.stype,
					 builtin_named_stypes[BUILTIN_VA_LIST]);
		return ir->CreateVAArg(arg, ty);
	}

	KRAWL_QASSERT(!"unreachable");
	return 0;
}

Value *llvm_backend_t::codegen_call_expr(call_expr_t *e)
{
	func_stype_t *fst = e->expr->vst.stype->as_func();
	if (IS_STYPE_BUILTIN(fst))
		return codegen_builtin_call_expr(e);

	Value *func = codegen_expr_addr(e->expr);

	// hack: If it's a variable function pointer, dereference it.
	// FIXME: maybe I should have a way to determine that some other way
	if (isa<PointerType>(cast<PointerType>(func->getType())->getElementType()))
		func = ir->CreateLoad(func);

	std::vector<Value*> args;
	std::vector<stype_t*> argtypes;
	args.reserve(fst->args.size());
	argtypes.reserve(fst->args.size());

	// special case, function expects many arguments and call expr contains
	// only one function call for arguments
	if (fst->args.size() > e->args.size()) {
		KRAWL_QASSERT(e->args.size() == 1);
		codegen_MRV(&args, &argtypes, e->args[0]);
	} else {
		for (size_t i = 0, n = e->args.size(); i < n; ++i) {
			args.push_back(codegen_expr_value(e->args[i]));
			argtypes.push_back(e->args[i]->vst.stype);
		}
	}

	// apply assignment if possible
	for (size_t i = 0, n = args.size(); i < n; ++i) {
		if (i < fst->args.size()) {
			args[i] = codegen_assignment(args[i], argtypes[i],
						     fst->args[i]);
		} else {
			// if it's a varargs call, we can't figure out the
			// type, do C-like type promotion instead
			args[i] = codegen_varargs_promotion(args[i], argtypes[i]);
		}
	}

	Value *v = ir->CreateCall(func, args.begin(), args.end());
	if (fst->results.size() == 1)
		v = codegen_post_load(v, fst->results[0]);
	return v;
}

Value *llvm_backend_t::codegen_compound_lit(compound_lit_t *e)
{
	// this should be triggered when compound literal is generated as a
	// temporary rvalue (function calls, return statements, etc.)
	const Type *ty = llvmtype(e->vst.stype);
	Value *addr = ir_alloca->CreateAlloca(ty);

	std::vector<Value*> values;
	values.reserve(e->elts.size());

	for (size_t i = 0, n = e->elts.size(); i < n; ++i)
		values.push_back(codegen_expr_value(e->elts[i]));

	for (size_t i = 0, n = e->elts.size(); i < n; ++i) {
		stype_t *vt = compound_lit_stype_i(e, i);
		Value *v = codegen_assignment(values[i], e->elts[i]->vst.stype, vt);
		Value *vaddr = ir->CreateConstGEP2_32(addr, 0, i);
		ir->CreateStore(v, vaddr);
	}

	return ir->CreateLoad(addr);
}

Value *llvm_backend_t::codegen_union_cast(selector_expr_t *e, Value *addr)
{
	stype_t *t = e->expr->vst.stype;
	struct_stype_t *sst;
	if (IS_STYPE_POINTER(t))
		sst = t->as_pointer()->points_to->as_struct();
	else
		sst = t->as_struct();

	if (sst->biggest == sst->fields[e->idx].type)
		return addr;

	const Type *ty = llvmtype(sst->fields[e->idx].type);
	std::vector<const Type*> fields(1, ty);
	ty = StructType::get(LLGC, fields)->getPointerTo();
	return ir->CreateBitCast(addr, ty);
}

Value *llvm_backend_t::codegen_expr_addr(node_t *expr)
{
	switch (expr->type) {
	case node_t::IDENT_EXPR:
	{
		ident_expr_t *e = (ident_expr_t*)expr;
		return e->sdecl->addr;
	}
	case node_t::PAREN_EXPR:
	{
		paren_expr_t *e = (paren_expr_t*)expr;
		return codegen_expr_addr(e->expr);
	}
	case node_t::UNARY_EXPR:
	{
		// *A, we just need to get a value of A, because it's a pointer
		unary_expr_t *e = (unary_expr_t*)expr;
		return codegen_expr_value(e->expr);
	}
	case node_t::SELECTOR_EXPR:
	{
		selector_expr_t *e = (selector_expr_t*)expr;
		stype_t *t = e->expr->vst.stype;
		if (IS_STYPE_MODULE(t)) {
			ident_expr_t *ident = is_ident_expr(e->expr);
			KRAWL_QASSERT(ident != 0);
			KRAWL_QASSERT(ident->sdecl->type == SDECL_IMPORT);
			import_sdecl_t *id = (import_sdecl_t*)ident->sdecl;
			return id->decls[e->sel->val]->addr;
		}

		Value *addr;
		if (IS_STYPE_POINTER(t))
			addr = codegen_expr_value(e->expr);
		else
			addr = codegen_expr_addr(e->expr);

		if (IS_STYPE_UNION(t) ||
		    (IS_STYPE_POINTER(t) &&
		     IS_STYPE_UNION(t->as_pointer()->points_to)))
		{
			addr = codegen_union_cast(e, addr);
			return ir->CreateConstInBoundsGEP2_32(addr, 0, 0);
		}

		return ir->CreateConstInBoundsGEP2_32(addr, 0, e->idx);
	}
	case node_t::INDEX_EXPR:
	{
		index_expr_t *e = (index_expr_t*)expr;
		if (IS_STYPE_ARRAY(e->expr->vst.stype)) {
			Value *addr = codegen_expr_addr(e->expr);
			Value *idx[] = {
				ConstantInt::getNullValue(Type::getInt32Ty(LLGC)),
				codegen_expr_value(e->index)
			};
			return ir->CreateGEP(addr, &idx[0], &idx[2]);
		} else {
			Value *addr = codegen_expr_value(e->expr);
			Value *idx = codegen_expr_value(e->index);
			return ir->CreateGEP(addr, idx);
		}
	}
	default:
		break;
	}

	KRAWL_QASSERT(!"unreachable");
	return 0;
}

Value *llvm_backend_t::codegen_expr_value(node_t *expr)
{
	if (expr->vst.value.type != VALUE_NONE) {
		if (IS_STYPE_BOOL(expr->vst.stype)) {
			if (expr->vst.value.to_bool())
				return ConstantInt::getTrue(LLGC);
			else
				return ConstantInt::getFalse(LLGC);
		}

		const Type *ty = llvmtype(expr->vst.stype);
		Constant *c = llvmconst(&expr->vst.value, ty);
		return c;
	}

	switch (expr->type) {
	case node_t::BINARY_EXPR:
	{
		binary_expr_t *e = (binary_expr_t*)expr;
		return codegen_binary_expr(e);
	}
	case node_t::UNARY_EXPR:
	{
		unary_expr_t *e = (unary_expr_t*)expr;
		return codegen_unary_expr(e);
	}
	case node_t::TYPE_CAST_EXPR:
	{
		type_cast_expr_t *e = (type_cast_expr_t*)expr;
		return codegen_type_cast_expr(e);
	}
	case node_t::PAREN_EXPR:
	{
		paren_expr_t *e = (paren_expr_t*)expr;
		return codegen_expr_value(e->expr);
	}
	case node_t::CALL_EXPR:
	{
		call_expr_t *e = (call_expr_t*)expr;
		return codegen_call_expr(e);
	}
	case node_t::COMPOUND_LIT:
	{
		compound_lit_t *e = (compound_lit_t*)expr;
		return codegen_compound_lit(e);
	}
	default:
		break;
	}

	Value *addr = codegen_expr_addr(expr);
	return codegen_load(addr, expr->vst.stype);
}

Value *llvm_backend_t::codegen_post_load(Value *v, stype_t *ty)
{
	if (IS_STYPE_BOOL(ty))
		return ir->CreateTrunc(v, Type::getInt1Ty(LLGC));
	if (IS_STYPE_INT(ty)) {
		int_stype_t *ist = ty->as_int();
		if (ist->size < 32)
			return ir->CreateZExt(v, Type::getInt32Ty(LLGC));
		return v;
	}
	return v;
}

Value *llvm_backend_t::codegen_load(Value *addr, stype_t *ty)
{
	// we can't dereference functions
	if (IS_STYPE_FUNC(ty)) {
		if (isa<PointerType>(cast<PointerType>(addr->getType())->getElementType()))
			return ir->CreateLoad(addr);
		return addr;
	}

	Value *v = ir->CreateLoad(addr);
	return codegen_post_load(v, ty);
}

Value *llvm_backend_t::codegen_cexpr_value(node_t *e)
{
	if (e->type == node_t::COMPOUND_LIT) {
		compound_lit_t *c = (compound_lit_t*)e;
		c->vals.clear();
		c->vals.reserve(c->elts.size());
		for (size_t i = 0, n = c->elts.size(); i < n; ++i)
			c->vals.push_back(codegen_cexpr_value(c->elts[i]));
		return 0;
	}
	return codegen_expr_value(e);
}

Value *llvm_backend_t::codegen_cexpr_store(Value *val, Value *addr, node_t *e)
{
	KRAWL_QASSERT(e->type == node_t::COMPOUND_LIT);

	compound_lit_t *c = (compound_lit_t*)e;
	for (size_t i = 0, n = c->vals.size(); i < n; ++i) {
		Value *vaddr = ir->CreateConstGEP2_32(addr, 0, i);
		Value *v = c->vals[i];
		if (!v)
			codegen_cexpr_store(v, vaddr, c->elts[i]);
		else {
			stype_t *vt = compound_lit_stype_i(c, i);
			v = codegen_assignment(v, c->elts[i]->vst.stype, vt);
			ir->CreateStore(v, vaddr);
		}
	}
	return 0;
}

void llvm_backend_t::codegen_init(var_sdecl_t *d)
{
	if (d->inited)
		return;

	IRBuilder<> *save_ir = ir;
	IRBuilder<> *save_ir_alloca = ir_alloca;
	ir = ir_init;
	ir_alloca = ir_init_alloca;

	codegen_init_deps(d->init);
	Value *expr = codegen_expr_value(d->init);
	expr = codegen_assignment(expr, d->init->vst.stype, d->stype);
	ir->CreateStore(expr, d->addr);

	ir = save_ir;
	ir_alloca = save_ir_alloca;
}

void llvm_backend_t::codegen_top_var_pre(var_sdecl_t *vsd)
{
	const Type *ty = llvmtype(vsd->stype);
	if (!vsd->init) {
		vsd->addr = new GlobalVariable(*module, ty, false,
					       GlobalValue::ExternalLinkage,
					       Constant::getNullValue(ty),
					       vsd->name);
		vsd->inited = true;
	} else if (vsd->init->vst.value.type != VALUE_NONE) {
		// we have a value, we can generate the code!
		Constant *init = llvmconst(&vsd->init->vst.value, ty);
		vsd->addr = new GlobalVariable(*module, ty, false,
					       GlobalValue::ExternalLinkage,
					       init, vsd->name);
		vsd->inited = true;
	} else {
		// otherwise just place an init-less declaration
		// will be initialized later
		vsd->addr = new GlobalVariable(*module, ty, false,
					       GlobalValue::ExternalLinkage,
					       Constant::getNullValue(ty),
					       vsd->name);
	}
}

void llvm_backend_t::codegen_top_var(var_sdecl_t *vsd)
{
	codegen_init(vsd);
}

void llvm_backend_t::codegen_top_extern_sdecls(import_sdecl_t *isd)
{
	unordered_map<std::string, sdecl_t*>::iterator it, end;
	for (it = isd->decls.begin(), end = isd->decls.end(); it != end; ++it) {
		sdecl_t *d = it->second;
		if (d->type != SDECL_FUNC)
			continue;
		if (!d->inited) // means "was used"
			continue;

		func_sdecl_t *fsd = (func_sdecl_t*)d;
		codegen_top_func_pre(fsd, isd->prefix.c_str());
	}
}

void llvm_backend_t::codegen_top_func_pre(func_sdecl_t *fsd, const char *prefix)
{
	std::string name;
	if (strlen(prefix) == 0)
		name = fsd->name.c_str();
	else
		cppsprintf(&name, "_Krl_%s_%s", prefix, fsd->name.c_str());

	const Type *functy = llvmfunctype(fsd->stype->as_func());
	Function *F = Function::Create(cast<FunctionType>(functy),
				       Function::ExternalLinkage, name, module);
	fsd->addr = F;
}

void llvm_backend_t::codegen_top_func(func_sdecl_t *fsd)
{
	func_stype_t *fst = fsd->stype->as_func();
	func_type_t *ft = fsd->decl->ftype;

	if (!fsd->decl->body)
		return;

	func_sdecl_t *save_cur_func_decl = cur_func_decl;
	cur_func_decl = fsd;

	Function *F = cast<Function>(fsd->addr);
	BasicBlock *entry = BasicBlock::Create(LLGC, "", F);

	// setup IR builders, one for instructions and other for allocas
	IRBuilder<> ib(LLGC);
	IRBuilder<> ib_alloca(LLGC);
	ib.SetInsertPoint(entry);

	// dummy instruction for alloca inserter
	Instruction *alloca_pt;
	Constant *zero = ConstantInt::getNullValue(Type::getInt32Ty(LLGC));
	alloca_pt = CastInst::Create(Instruction::BitCast, zero,
				     Type::getInt32Ty(LLGC));
	ib.Insert(alloca_pt);
	ib_alloca.SetInsertPoint(alloca_pt);

	IRBuilder<> *save_ir = ir;
	IRBuilder<> *save_ir_alloca = ir_alloca;
	ir = &ib;
	ir_alloca = &ib_alloca;

	// hack for main function, call __init_krawl_globals
	if (fsd->name == "main")
		ir->CreateCall(ir_init->GetInsertBlock()->getParent());

	// allocate stack space for arguments
	Function::arg_iterator it = F->arg_begin();
	for (size_t i = 0, n = ft->args.size(); i < n; ++i) {
		field_t *f = ft->args[i];

		if (f->names.empty()) {
			// no name, means unused arg (actually all of them)
			it++;
			continue;
		}

		for (size_t j = 0, m = f->names.size(); j < m; ++j, ++it) {
			KRAWL_QASSERT(f->names[j]->sdecl->type == SDECL_VAR);
			var_sdecl_t *d = (var_sdecl_t*)f->names[j]->sdecl;

			d->addr = ir_alloca->CreateAlloca(llvmtype(d->stype));
			ir->CreateStore(it, d->addr);
		}
	}

	// add to the stack and zero initialize named return values if any
	if (fsd->has_named_return_values()) {
		for (size_t i = 0, n = ft->results.size(); i < n; ++i) {
			field_t *f = ft->results[i];
			// names cannot be empty here

			for (size_t j = 0, m = f->names.size(); j < m; ++j) {
				KRAWL_QASSERT(f->names[j]->sdecl->type == SDECL_VAR);
				var_sdecl_t *d = (var_sdecl_t*)f->names[j]->sdecl;

				const Type *ty = llvmtype(d->stype);
				d->addr = ir_alloca->CreateAlloca(ty);
				Value *nil = Constant::getNullValue(ty);
				ir->CreateStore(nil, d->addr);
			}
		}
	}

	// generate instructions for body
	codegen_block_stmt(fsd->decl->body);

	finalize_function(ir, fsd);

	ir = save_ir;
	ir_alloca = save_ir_alloca;
	alloca_pt->eraseFromParent();
	cur_func_decl = save_cur_func_decl;
}

void llvm_backend_t::codegen_top_sdecl_pre(sdecl_t *d)
{
	switch (d->type) {
	case SDECL_VAR:
		codegen_top_var_pre((var_sdecl_t*)d);
		break;
	case SDECL_FUNC:
		codegen_top_func_pre((func_sdecl_t*)d, uid.c_str());
		break;
	default:
		break;
	}
}

void llvm_backend_t::codegen_top_sdecl(sdecl_t *d)
{
	switch (d->type) {
	case SDECL_VAR:
		codegen_top_var((var_sdecl_t*)d);
		break;
	case SDECL_FUNC:
		codegen_top_func((func_sdecl_t*)d);
		break;
	default:
		break;
	}
}

void llvm_backend_t::codegen_top_sdecls(std::vector<const char*> *pkgdecls)
{
	for (size_t i = 0, n = used_extern_sdecls->size(); i < n; ++i)
		codegen_top_extern_sdecls(used_extern_sdecls->at(i));

	for (size_t i = 0, n = pkgdecls->size(); i < n; ++i) {
		sdecl_t *sd = pkgscope->sdecls[pkgdecls->at(i)];
		codegen_top_sdecl_pre(sd);
	}
	for (size_t i = 0, n = pkgdecls->size(); i < n; ++i) {
		sdecl_t *sd = pkgscope->sdecls[pkgdecls->at(i)];
		codegen_top_sdecl(sd);
	}
}

void llvm_backend_t::create_init_func()
{
	FunctionType *ft = FunctionType::get(Type::getVoidTy(LLGC), false);
	Function *f = Function::Create(ft, Function::PrivateLinkage,
				       "__init_krawl_globals", module);

	BasicBlock *entry = BasicBlock::Create(LLGC, "", f);
	ir_init = new IRBuilder<>(LLGC);
	ir_init_alloca = new IRBuilder<>(LLGC);

	ir_init->SetInsertPoint(entry);

	Constant *zero = ConstantInt::getNullValue(Type::getInt32Ty(LLGC));
	ir_init_alloca_pt = CastInst::Create(Instruction::BitCast, zero,
					     Type::getInt32Ty(LLGC));
	ir_init->Insert(ir_init_alloca_pt);
	ir_init_alloca->SetInsertPoint(ir_init_alloca_pt);
}

void llvm_backend_t::finalize_init_func()
{
	ir_init->CreateRetVoid();
	ir_init_alloca_pt->eraseFromParent();
}

void llvm_backend_t::compile_exe(TargetMachine *machine, PassManager *pm)
{

	int fd;
	SmallVector<char, 128> tmpfilename;
	sys::fs::unique_file("obj-%%%%%%", fd, tmpfilename);
	tmpfilename.push_back(0);
	{
		raw_fd_ostream tmpfile(fd, true);
		formatted_raw_ostream out(tmpfile);
		if (machine->addPassesToEmitFile(*pm, out,
						 TargetMachine::CGFT_ObjectFile,
						 CodeGenOpt::Default))
		{
			DIE("failed to add file emission passes");
		}
		pm->run(*module);
	}

	std::string libs_str;
	for (size_t i = 0, n = libs->size(); i < n; ++i) {
		cppsprintf(&libs_str, "-l%s ", libs->at(i));
	}

	std::string cmd;
	cppsprintf(&cmd, "clang -o %s %s -L. %s",
		   out_name, &tmpfilename[0], libs_str.c_str());
	system(cmd.c_str());
	unlink(&tmpfilename[0]);
}

void llvm_backend_t::compile_lib(TargetMachine *machine, PassManager *pm)
{
	FILE *f = fopen(out_name, "wb");
	if (!f)
		DIE("failed to open file for writing: %s", out_name);

	{
		raw_fd_ostream outstream(fileno(f), true);
		formatted_raw_ostream out(outstream);
		if (machine->addPassesToEmitFile(*pm, out,
						 TargetMachine::CGFT_ObjectFile,
						 CodeGenOpt::Default))
		{
			DIE("failed to add file emission passes");
		}
		pm->run(*module);
	}
	fclose(f);
}

void llvm_backend_t::pass(std::vector<const char*> *pkgdecls)
{
	module = new Module("Main", LLGC);
	ir = 0;
	ir_alloca = 0;
	create_init_func();

	cur_func_decl = 0;
	cur_loop = 0;
	cur_switch = 0;
	ended_with_fallthrough = false;

	codegen_top_sdecls(pkgdecls);
	finalize_init_func();

	if (dump)
		module->dump();
	verifyModule(*module);

	// CODEGEN
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();

	std::string error;
	std::string triple = sys::getHostTriple();
	const Target *target = TargetRegistry::lookupTarget(triple, error);
	if (!target)
		DIE("%s\n", error.c_str());

	TargetMachine *machine = target->createTargetMachine(triple, "");
	PassManager *pm = new PassManager();
	pm->add(new TargetData(*machine->getTargetData()));

	if (uid.empty()) {
		compile_exe(machine, pm);
	} else {
		compile_lib(machine, pm);
	}

}

void pass3_t::pass(std::vector<const char*> *pkgdecls)
{
	llvm_backend_t be;
	be.uid = uid;
	be.pkgscope = pkgscope;
	be.used_extern_sdecls = used_extern_sdecls;
	be.out_name = out_name;
	be.libs = libs;
	be.dump = dump;
	be.time = time;
	be.pass(pkgdecls);
}
