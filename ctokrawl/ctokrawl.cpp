#include <clang/Lex/Token.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Frontend/TextDiagnosticBuffer.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/Stmt.h>
#include <clang/AST/Expr.h>
#include <clang/Sema/Sema.h>
#include <clang/Basic/FileSystemOptions.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/TokenKinds.h>
#include <clang/Parse/ParseAST.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Host.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <vector>
#include <tr1/unordered_set>
#include <tr1/unordered_map>

using std::tr1::unordered_set;
using std::tr1::unordered_map;
using namespace clang;

namespace {

const char *keywords[] = {"fallthrough","func","type","var","import"};

void die(const char *why)
{
	fprintf(stderr, "%s\n", why);
	exit(1);
}

void cppvsprintf(std::string *out, const char *fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);

	int len = vsnprintf(0, 0, fmt, va);
	size_t size = out->size();
	out->resize(size + len);
	vsnprintf(&(*out)[size], len+1, fmt, va2);

	va_end(va2);
}

void cppsprintf(std::string *out, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	cppvsprintf(out, fmt, va);
	va_end(va);
}

const Type *is_function_proto(const Type *t)
{
	switch (t->getTypeClass()) {
	case Type::FunctionProto:
		return cast<FunctionProtoType>(t);
	case Type::Paren:
		return is_function_proto(cast<ParenType>(t)->getInnerType().getTypePtr());
	default:
		break;
	}
	return 0;
}

struct CToKrawlContext {
	ASTContext *ctx;
	SourceManager *srcm;
	unordered_set<std::string> filenames;
	unordered_set<std::string> defined;
	unordered_map<std::string, std::string> structs;
	unordered_map<std::string, std::string> consts;
	std::string macros;
	std::string funcmock;

	CToKrawlContext()
	{
		macros.reserve(65536);
		funcmock.reserve(65536);
		funcmock.append("void ___foo___() {\n");
	}
};

struct CToKrawlASTConsumer : ASTConsumer {
	CToKrawlContext *ctx;

	CToKrawlASTConsumer(CToKrawlContext *ctx): ctx(ctx)
	{
	}

	bool was_defined(const char *what)
	{
		unordered_set<std::string>::iterator it;
		it = ctx->defined.find(what);
		return it != ctx->defined.end();
	}

	void define(const char *what)
	{
		ctx->defined.insert(what);
	}

	std::string *struct_was_defined(const char *what)
	{
		unordered_map<std::string, std::string>::iterator it;
		it = ctx->structs.find(what);
		if (it == ctx->structs.end())
			return 0;

		return &it->second;
	}

	std::string struct_body_to_krawl(RecordDecl *rd)
	{
		// union case
		std::string out;
		if (rd->getTypeForDecl()->isUnionType())
			out += "union { ";
		else
			out += "struct { ";
		RecordDecl::field_iterator it, end;
		for (it = rd->field_begin(), end = rd->field_end(); it != end; ++it) {
			llvm::StringRef keyword = it->getName();
			for (size_t i = 0, n = sizeof(keywords)/sizeof(keywords[0]); i < n; ++i) {
				if (keyword == keywords[i]) {
					out += "_";
					break;
				}
			}

			out += keyword.str();
			out += " ";
			out += clang_type_to_krawl(it->getType().getTypePtr());
			out += "; ";
		}
		out += "}";
		return out;
	}

	std::string clang_type_to_krawl(const Type *t)
	{
		std::string out;

		switch (t->getTypeClass()) {
		case Type::Builtin:
		{
			const BuiltinType *bt = cast<BuiltinType>(t);
			if (bt->isInteger()) {
				if (bt->getKind() == BuiltinType::Char_S ||
				    bt->getKind() == BuiltinType::Char_U)
				{
					// special case, unspecified char is uint8 in krawl
					out.append("uint8");
					return out;
				}

				if (bt->isUnsignedInteger())
					out.append("u");
				switch (ctx->ctx->getTypeSize(bt)) {
				case 8:  out.append("int8");  break;
				case 16: out.append("int16"); break;
				case 32: out.append("int32"); break;
				case 64: out.append("int64"); break;
				default:
					 die("unsupported integer type");
				}
				return out;
			} else if (bt->isFloatingPoint()) {
				switch (ctx->ctx->getTypeSize(bt)) {
				case 32: out.append("float32"); break;
				case 64: out.append("float64"); break;
				default:
					 // fucking ugly hack for long doubles
					 cppsprintf(&out, "struct { _ [%d]byte; }",
						    ctx->ctx->getTypeSize(bt) / 8);
					 break;
				}
				return out;
			} else if (bt->getKind() == BuiltinType::Void) {
				out.append("void");
				return out;
			}
			break;
		}
		case Type::Pointer:
		{
			const PointerType *pt = cast<PointerType>(t);

			// check if it's a function pointer
			const Type *isft = is_function_proto(pt->getPointeeType().getTypePtr());
			if (isft)
				return clang_type_to_krawl(isft);

			out += "*";
			out += clang_type_to_krawl(pt->getPointeeType().getTypePtr());
			return out;
		}
		case Type::Typedef:
		{
			const TypedefType *tt = cast<TypedefType>(t);
			return tt->getDecl()->getDeclName().getAsString();
		}
		case Type::Record:
		{
			const RecordType *rt = cast<RecordType>(t);
			RecordDecl *rd = rt->getDecl();
			if (!rd->getName().empty()) {
				std::string name = rd->getNameAsString();
				if (!struct_was_defined(name.c_str()))
					ctx->structs[name] = "";
				out += "_struct__";
				out += name;
				return out;
			}

			out += struct_body_to_krawl(rd);

			return out;
		}
		case Type::TypeOfExpr:
		{
			const TypeOfExprType *tot = cast<TypeOfExprType>(t);
			return clang_type_to_krawl(tot->getUnderlyingExpr()->getType().getTypePtr());
		}
		case Type::ConstantArray:
		{
			const ConstantArrayType *cat = cast<ConstantArrayType>(t);
			cppsprintf(&out, "[%d]", cat->getSize().getZExtValue());
			out += clang_type_to_krawl(cat->getElementType().getTypePtr());
			return out;
		}
		case Type::FunctionProto:
		{
			const FunctionProtoType *fpt = cast<FunctionProtoType>(t);
			out += "func (";
			for (size_t i = 0, n = fpt->getNumArgs(); i < n; ++i) {
				out += clang_type_to_krawl(fpt->getArgType(i).getTypePtr());
				if (i != n-1)
					out += ", ";
			}
			if (fpt->isVariadic())
				out += ", ...";
			out += ") ";
			out += clang_type_to_krawl(fpt->getResultType().getTypePtr());
			return out;
		}
		case Type::Paren:
		{
			const ParenType *pt = cast<ParenType>(t);
			return clang_type_to_krawl(pt->getInnerType().getTypePtr());
		}
		case Type::Enum:
		{
			const EnumType *et = cast<EnumType>(t);
			const EnumDecl *ed = et->getDecl();
			return clang_type_to_krawl(ed->getIntegerType().getTypePtr());
		}
		default:
			break;
		}

		printf("!!!!!!!!!!!!%s\n", t->getTypeClassName());
		return "???";
	}

	void process_function_decl(FunctionDecl *fd)
	{
		std::string name = fd->getDeclName().getAsString();
		if (was_defined(name.c_str()))
			return;

		define(name.c_str());

		std::string out = "func ";
		out += name;
		out += "(";
		for (size_t i = 0, n = fd->getNumParams(); i < n; ++i) {
			ParmVarDecl *arg = fd->getParamDecl(i);
			/*
			don't care about name
			if (!arg->getName().empty()) {
				out += arg->getNameAsString();
				out += " ";
			}
			*/
			out += clang_type_to_krawl(arg->getType().getTypePtr());
			if (i != n-1)
				out += ", ";
		}
		if (fd->isVariadic())
			out += ", ...";
		out += ")";
		std::string result = clang_type_to_krawl(fd->getResultType().getTypePtr());
		if (result != "void") {
			out += " ";
			out += result;
		}
		printf("%s;\n", out.c_str());
	}

	void process_typedef_decl(TypedefDecl *td)
	{
		std::string name = td->getDeclName().getAsString();
		if (was_defined(name.c_str()))
			return;

		define(name.c_str());

		std::string out = "type ";
		out += name;
		out += " ";
		out += clang_type_to_krawl(td->getUnderlyingType().getTypePtr());
		printf("%s;\n", out.c_str());
	}

	void process_record_decl(RecordDecl *rd)
	{
		if (rd->getName().empty())
			return;
		if (!rd->isDefinition())
			return;

		std::string name = rd->getNameAsString();
		std::string *def = struct_was_defined(name.c_str());
		if (!def) {
			ctx->structs[name] = "";
			def = &ctx->structs[name];
		}

		if (!def->empty()) {
			die("should never happen");
			return;
		}

		*def = struct_body_to_krawl(rd);
	}

	void process_enum_decl(EnumDecl *ed)
	{
		ctx->macros.append("enum {\n");
		EnumDecl::enumerator_iterator it, end;
		for (it = ed->enumerator_begin(), end = ed->enumerator_end(); it != end; ++it) {
			llvm::SmallVector<char, 128> tmp;
			it->getInitVal().toString(tmp);
			std::string name = it->getNameAsString();

			ctx->macros.append("\t");
			ctx->macros.append(name);
			ctx->macros.append(" = ");
			ctx->macros.append(&tmp[0], tmp.size());
			ctx->macros.append(",\n");

			if (ctx->consts.find(name) != ctx->consts.end())
				continue;
			ctx->consts[name] = std::string(tmp.begin(), tmp.end());
		}
		ctx->macros.append("};\n");
	}

	void HandleTopLevelDecl(DeclGroupRef d)
	{
		DeclGroupRef::iterator it, end;
		for (it = d.begin(), end = d.end(); it != end; ++it) {
			Decl *d = *it;
			switch (d->getKind()) {
			case Decl::Function:
				process_function_decl(cast<FunctionDecl>(*it));
				break;
			case Decl::Typedef:
				process_typedef_decl(cast<TypedefDecl>(*it));
				break;
			case Decl::Record:
				process_record_decl(cast<RecordDecl>(*it));
				break;
			case Decl::Enum:
				process_enum_decl(cast<EnumDecl>(*it));
				break;
			default:
				break;
			}

		}
	}

	void HandleTranslationUnit(ASTContext &unused)
	{
		unordered_map<std::string, std::string>::iterator it, end;
		it = ctx->structs.begin();
		end = ctx->structs.end();

		for (; it != end; ++it) {
			std::string def = "struct {}";
			if (!it->second.empty())
				def = it->second;

			printf("type _struct__%s %s;\n", it->first.c_str(), def.c_str());
		}
	}
};

struct ConstantExprExtractor : ASTConsumer {
	ASTContext *ctx;
	SourceManager *srcm;
	unordered_map<std::string, std::string> *consts;

	llvm::StringRef extract_expr_name(Expr *e)
	{
		SourceLocation loc = e->getExprLoc();
		SourceLocation instance_loc = srcm->getInstantiationLoc(loc);
		const char *beg = srcm->getCharacterData(instance_loc);
		const char *end = strchr(beg, ';');
		return llvm::StringRef(beg, end-beg);
	}

	void process_function_decl(FunctionDecl *fd)
	{
		Stmt *s = fd->getBody();
		Stmt::child_iterator it, end;
		for (it = s->child_begin(), end = s->child_end(); it != end; ++it) {
			Expr *e = dyn_cast<Expr>(*it);
			if (!e)
				continue;

			Expr::EvalResult er;
			bool ok = e->Evaluate(er, *ctx);
			if (!ok)
				continue;

			llvm::SmallVector<char, 128> tmp;
			APValue &v = er.Val;
			switch (v.getKind()) {
			case APValue::Int:
				v.getInt().toString(tmp);
				break;
			case APValue::Float:
				v.getFloat().toString(tmp);
				break;
			default:
				break;
			}

			if (!tmp.empty()) {
				std::string name = extract_expr_name(e).str();
				if (consts->find(name) != consts->end())
					continue;

				(*consts)[name] = std::string(tmp.begin(), tmp.end());
			}
		}
	}

	void HandleTopLevelDecl(DeclGroupRef d)
	{
		DeclGroupRef::iterator it, end;
		for (it = d.begin(), end = d.end(); it != end; ++it) {
			Decl *d = *it;
			if (d->isFunctionOrFunctionTemplate())
				process_function_decl(cast<FunctionDecl>(d));
		}
	}
};

struct PPIncludeCollector : PPCallbacks {
	CToKrawlContext *ctx;

	PPIncludeCollector(CToKrawlContext *ctx): ctx(ctx)
	{
	}

	void FileChanged(SourceLocation loc, FileChangeReason reason, SrcMgr::CharacteristicKind filetype)
	{
		if (reason == EnterFile && loc.isFileID() &&
		    !ctx->srcm->isFromMainFile(loc))
		{
			PresumedLoc pl = ctx->srcm->getPresumedLoc(loc);
			if (pl.getFilename()[0] != '<')
				ctx->filenames.insert(pl.getFilename());
		}
	}

	bool is_builtin(const MacroInfo *mi)
	{
		SourceLocation loc = mi->getDefinitionLoc();
		PresumedLoc ploc = ctx->srcm->getPresumedLoc(loc);
		if (strcmp("<built-in>", ploc.getFilename()) == 0)
			return true;
		return false;
	}

	void append_macro(const MacroInfo *mi)
	{
		if (mi->tokens_empty())
			return;
		if (is_builtin(mi))
			return;

		MacroInfo::tokens_iterator last = mi->tokens_end() - 1;
		SourceLocation beg = mi->getDefinitionLoc();
		SourceLocation end = last->getLocation();

		const char *s = ctx->srcm->getCharacterData(beg);
		const char *e = ctx->srcm->getCharacterData(end) + last->getLength();

		ctx->macros.append("#define ");
		ctx->macros.append(s, e-s);
		ctx->macros.append("\n");
	}

	void MacroDefined(const Token &name, const MacroInfo *mi)
	{
		append_macro(mi);

		// we support only zero arg macros, which are constant
		// definitions, most likely
		if (mi->getNumArgs() != 0)
			return;

		if (mi->tokens_empty())
			return;

		// for some reason mi->isBuiltinMacro() doesn't work
		if (is_builtin(mi))
			return;

		llvm::StringRef nm = name.getIdentifierInfo()->getName();
		ctx->funcmock.append("\t");
		ctx->funcmock.append(nm.data(), nm.size());
		ctx->funcmock.append(";\n");
	}

	void EndOfMainFile()
	{
		ctx->funcmock.append("}\n");
		ctx->macros.append(ctx->funcmock);

		// Parse macros mock
		CompilerInstance ci;
		ci.getLangOpts().C99 = 1;
		ci.getLangOpts().CPlusPlus = 0;

		ci.getTargetOpts().Triple = llvm::sys::getHostTriple();

		ci.createDiagnostics(0, 0);
		ci.setTarget(TargetInfo::CreateTargetInfo(ci.getDiagnostics(),
							  ci.getTargetOpts()));
		ci.getDiagnostics().setSuppressAllDiagnostics();
		ci.getDiagnostics().setErrorsAsFatal(false);

		ci.createFileManager();
		ci.createSourceManager(ci.getFileManager());
		ci.createPreprocessor();
		ci.createASTContext();

		llvm::MemoryBuffer *mb;
		mb = llvm::MemoryBuffer::getMemBuffer(ctx->macros, "macrosmock.c");
		ci.getSourceManager().createMainFileIDForMemBuffer(mb);

		ConstantExprExtractor consumer;
		consumer.ctx = &ci.getASTContext();
		consumer.srcm = &ci.getSourceManager();
		consumer.consts = &ctx->consts;

		ParseAST(ci.getPreprocessor(), &consumer, ci.getASTContext());
		// DONE

		{
			unordered_map<std::string, std::string>::iterator it, end;
			it = ctx->consts.begin();
			end = ctx->consts.end();
			for (; it != end; ++it) {
				printf("const %s = %s;\n",
				       it->first.c_str(), it->second.c_str());
			}
		}

		printf("-*-\n");
		unordered_set<std::string>::iterator it, end;
		it = ctx->filenames.begin();
		end = ctx->filenames.end();
		for (; it != end; ++it)
			printf("%s\n", it->c_str());
	}
};

struct CToKrawl : PluginASTAction {
	CToKrawlContext context;

	bool BeginSourceFileAction(CompilerInstance &CI, llvm::StringRef filename)
	{
		PPIncludeCollector *ppi = new PPIncludeCollector(&context);
		CI.getPreprocessor().addPPCallbacks(ppi);
		return true;
	}

	ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef)
	{
		context.ctx = &CI.getASTContext();
		context.srcm = &CI.getSourceManager();
		return new CToKrawlASTConsumer(&context);
	}

	bool ParseArgs(const CompilerInstance &CI,
		       const std::vector<std::string> &args)
	{
		return true;
	}

	void PrintHelp(llvm::raw_ostream &out)
	{
		out << "Help for CToKrawl goes here\n";
	}
};

} // anonymous namespace

static FrontendPluginRegistry::Add<CToKrawl>
X("c-to-krawl", "convert C declarations to Krawl");
