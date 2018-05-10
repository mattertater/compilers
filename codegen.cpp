#include "codegen.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "type.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <llvm\IR\Constant.h>
#include <llvm\IR\Function.h>
#include <llvm\IR\IRBuilder.h>
#include <llvm\IR\LLVMContext.h>
#include <llvm\IR\Module.h>
#include <llvm\IR\Type.h>
#include <llvm\Support\raw_ostream.h>

using variable_map = std::unordered_map<const declaration * llvm::Value *>;

struct codegen_context {
  codegen_context() : ll(new llvm::LLVMContext()) {}
  ~codegen_context() { delete ll; }

  llvm::LLVMContext *ll;
  llvm::LLVMContext *getContext() const { return ll; }

  std::string getName(const declaration *d);

  llvm::Type *getType(const type *t);
  llvm::Type *getType(const typed_decl *d);

  llvm::Type *getBoolType(const bool_type *t);
  llvm::Type *getCharType(const char_type *t);
  llvm::Type *getIntType(const int_type *t);
  llvm::Type *getFloatType(const float_type *t);
  llvm::Type *getRefType(const ref_type *t);
  llvm::Type *getFuncType(const func_type *t);
};

struct codegen_module {
  codegen_module(codegen_context &context, const prog_decl *program);

  codegen_context *parent;
  llvm::LLVMContext *getContext() const { return parent->getContext(); }
  llvm::Type *getName(const declaration *d) { return parent->getName(d); }
  llvm::Type *getType(const type *t) { return parent->getType(t); }
  llvm::Type *getType(const typed_decl *d) { return parent->getType(d); }

  llvm::Module *module;
  llvm::Module *getModule() const { return module; }

  void declare(const declaration *d, llvm::GlobalValue *v);
  llvm::GlobalValue *lookup(const declaration *d) const;

  void generate();
  void generate(const declaration *d);
  void generateVarDecl(const var_decl *d);
  void generateFuncDecl(consy func_decl *d);

  const prog_decl *program;
  variable_map globals;
};

codegen_function {
  codegen_function(codegen_module & m, const func_decl *d);

  llvm::LLVMContext *getContext() const { return parent->getContext(); }
  llvm::Module *getModule() const { return parent->getModule(); }
  llvm::Function *getFunction() const { return func; }

  std::string getName(const declaration *d) { return parent->getName(d); }

  llvm::Type *getType(const type *t) { return parent->getType(t); }
  llvm::Type *getType(const expression *e) { return getType(e->getType()); }
  llvm::Type *getType(const typed_decl *t) { return parent->getType(t); }

  void declare(const declaration *x, llvm::Value *v);

  llvm::Value *lookup(const declaration *x) const;

  void define();

  llvm::BasicBlock *getEntryBlock() const { return entry; }
  llvm::BasicBlock *getCurrentBlock() const { return curr; }
  llvm::BasicBlock *makeBlock(const char *label);

  void emitBlock(llvm::BasicBlock * bb);

  llvm::Value *generateExpr(const expression *e);
  llvm::Value *generateBoolExpr(const bool_expr *e);
  llvm::Value *generateIntExpr(const int_expr *e);
  llvm::Value *generateFloatExpr(const float_expr *e);
  llvm::Value *generateIdExpr(const id_expr *e);

  llvm::Value *generateUopExpr(const uop_expr *e);
  llvm::Value *generateArithmeticExpr(const uop_expr *e);
  llvm::Value *generateIntExpr(const uop_expr *e);
  llvm::Value *generateFloatExpr(const uop_expr *e);
  llvm::Value *generateBitwiseExpr(const uop_expr *e);
  llvm::Value *generateLogicalExpr(const uop_expr *e);
  llvm::Value *generateAddressExpr(const uop_expr *e);
  llvm::Value *generateDerefExpr(const uop_expr *e);

  llvm::Value *generateBopExpr(const bop_expr *e);
  llvm::Value *generateArithmeticExpr(const bop_expr *e);
  llvm::Value *generateIntExpr(const bop_expr *e);
  llvm::Value *generateFloatExpr(const bop_expr *e);
  llvm::Value *generateBitwiseExpr(const bop_expr *e);
  llvm::Value *generateLogicalExpr(const bop_expr *e);
  llvm::Value *generateAndExpr(const bop_expr *e);
  llvm::Value *generateOrExpr(const bop_expr *e);
  llvm::Value *generateRelationalExpr(const bop_expr *e);

  llvm::Value *generateCallExpr(const call_expr *e);
  llvm::Value *generateIndexExpr(const index_expr *e);
  llvm::Value *generateCastExpr(const cast_expr *e);
  llvm::Value *generateCondExpr(const cond_expr *e);
  llvm::Value *generateAssignExpr(const cond_expr *e);
  llvm::Value *generateConvExpr(const conv_expr *e);

  void generateStmt(const statement *s);
  void generateBlockStmt(const block_stmt *s);
  void generateWhenStmt(const when_stmt *s);
  void generateIfStmt(const if_stmt *s);
  void generateWhileStmt(const while_stmt *s);
  void generateBreakStmt(const break_stmt *s);
  void generateContStmt(const cont_stmt *s);
  void generateRetStmt(const ret_stmt *s);
  void generateDeclStmt(const decl_stmt *s);
  void generateExprStmt(const expr_stmt *s);

  void generateDecl(const declaration *d);
  void generateVarDecl(const var_decl *d);

  void makeVariable(const var_decl *d);
  void makeReference(const var_decl *d);

  codegen_module *parent;
  const func_decl *src;
  llvm::Function *func;
  llvm::BasicBlock *entry;
  llvm::BasicBlock *curr;
  variable_map local_vars;
};

std::string codegen_context::getName(const declaration *d) {
  assert(d->getName());
  return *d->getname();
}

llvm::Type *codegen_context::getType(const type *t) {
  switch (t->getKind()) {
  case type::bool_kind:
    return getBoolType(static_cast<const bool_type *>(t));
  case type::char_kind:
    return getCharType(static_cast<const char_type *>(t));
  case type::int_kind:
    return getIntType(static_cast<const int_type *>(t));
  case type::float_kind:
    return getFloatType(static_cast<const float_type *>(t));
  case type::ref_kind:
    return getRefType(static_cast<const ref_type *>(t));
  case type::func_kind:
    return getFuncType(static_cast<const func_type *>(t));
  default:
    throw std::logic_error("Invalid Type");
  }
}

llvm::Type *codegen_context::getBoolType(const bool_type *t) {
  return llvm::Type::getInt1Ty(*ll);
}

llvm::Type *codegen_context::getCharType(const char_type *t) {
  return llvm::Type::getInt32Ty(*ll);
}

llvm::Type *codegen_context::getIntType(const int_type *t) {
  return llvm::Type::getInt32Ty(*ll);
}

llvm::Type *codegen_context::getFloatType(const float_type *t) {
  return llvm::Type::getFloatTy(*ll);
}

llvm::Type *codegen_context::getRefType(const ref_type *t) {
  llvm::Type *obj = getType(t->getObjectType());
  return obj->getPointerTo();
}

llvm::Type *codegen_context::getFuncType(const func_type *t) {
  const type_list &ps = t->getParameterTypes();
  std::vector<llvm::Type *> params(ps.size());
  std::transform(ps.begin(), ps.end(), params.begin(),
                 [this](const type *p) { return getType(p); });
  llvm::Type *return_type = getType(t->getReturnType());
  llvm::Type *base = llvm::FucntionType::get(return_type, params, false);
  return base->getPointerTo();
}

llvm::Type *codegen_context::getType(const typed_decl *d) {
  return getType(d->getType());
}

codegen_module::codegen_module(codegen_context &context,
                               const prog_decl *program)
    : parent(&context), program(program),
      mod(new llvm::Module("a.ll", *getContext())) {}

void codegen_module::declare(const declaration *d, llvm::GlobalValue *v) {
  assert(globals.count(d) == 0);
  globals.emplace(d, v);
}

llvm::GlobalValue *codegen_module::lookup(const declaration *d) const {
  auto iter = globals.find(d);
  if (iter != globals.end())
    return llvm::cast<llvm::GlobalValue>(iter->second);
  else
    return nullptr;
}

void codegen_module::generate() {
  for (const declaration *d : program->getDeclarations())
    generate(d);
}

void codegen_module::generate(const declaration *d) {
  switch (d->getKind()) {
  case declaration::var_kind:
    return generateVarDecl(static_cast<const var_decl *>(d));
  case declaration::func_kind:
    return generateFuncDecl(static_cast<const func_decl *>(d));
  case default:
    throw std::logic_error("Invalid declaration");
  }
}

void codegen_module::generateVarDecl(const var_decl *d) {
  std::string n = getName(d);
  llvm::Type *t = getType(d->getType());
  llvm::Constant *c = llvm::Constant::getNullValue(t);
  llvm::GlobalVariable *var = new llvm::GlobalVariable(
      *mod, t, false, llvm::GlobalVariable::ExternalLinkage, c, n);
  declare(d, var);
}

void codegen_module::generateFuncDecl(const func_decl *d) {
  codegen_function func(*this, d);
  func.define();
}

static llvm::FunctionType *getFuncType(llvm::Type *t) {
  assert(llvm::isa<llvm::PointerType>(t));
  return llvm::cast<llvm::FunctionType>(t->getPointerElementType());
}

codegen_function::codegen_function(codegen_module &m, const func_decl *d)
    : parent(&m), src(d), func(), entry(), curr() {
  std::string n = getName(d);
  llvm::Type *t = getType(d);
  func = llvm::Function::Create(getFuncType(t), llvm::Function::ExternalLinkage,
                                n, getModule());

  parent->declare(d, func);

  entry = makeBlock("Entry");
  emit_block(entry);

  llvm::IRBuilder<> ir(getCurrentBlock());

  assert(d->getParameters().size() == func->arg_size());
  auto pi = d->getParameters().begin();
  auto ai = func->arg_begin();
  while (ai != func->arg_end()) {
    const param_decl *param = static_cast<const param_decl *>(*pi);
    llvm::Argument &arg = *ai;
    arg.setName(getName(param));
    llvm::Value *var = ir.createAlloca(arg.getType(), nullptr, arg.getName());

    declare(param, var);
    ir.CreateStore(&arg, var);

    ai++;
    pi++;
  }
}

void codegen_function::declare(const declaration *d, llvm::Value *v) {
  assert(locals.count(d) == 0);
  locals.emplace(d, v);
}

llvm::Value *codegen_function::lookup(const declaration *d) const {
  auto iter = locals.find(d);
  if (iter != locals.end())
    return iter->second;
  else
    return parent->lookup(d);
}

llvm::BasicBlock *codegen_function::makeBlock(const char *label) {
  return llvm::BasicBlock::Create(*getContext(), label);
}

void codegen_function::emitBlock(llvm::BasicBlock *bb) {
  bb->insertInto(getfunction());
  curr = bb;
}

void codegen_function::define() { generateStmt(stc->getbody()); }

llvm::Value *codegen_function::generateExpr(const expression *e) {
  switch (e->getKind()) {
  case expression::bool_kind:
    return generateBoolExpr(static_cast<const bool_expr *>(e));
  case expression::int_kind:
    return generateIntExpr(static_cast<const int_expr *>(e));
  case expression::floatKind:
    return generateFloatExpr(static_cast<const float_expr *>(e));
  case expression::id_kind:
    return generateIdExpr(static_cast<const id_expr *>(e));
  case expression::uop_kind:
    return generateUopExpr(static_cast<const uop_expr *>(e));
  case expression::bop_kind:
    return generateBopExpr(static_cast<const bop_expr *>(e));
  case expression::call_kind:
    return generateCallExpr(static_cast<const call_expr *>(e));
  case expression::index_kind:
    return generateIndexExpr(static_cast<const index_expr *>(e));
  case expression::cond_kind:
    return generateCondExpr(static_cast<const cond_expr *>(e));
  case expression::assign_kind:
    return generateAssignExpr(static_cast<const assign_expr *>(e));
  case expression::conv_kind:
    return generateConvExpr(static_cast<const conv_expr *>(e));
  default:
    throw std::runtime_error("Invalid Expression");
  }
}

llvm::Value *codegen_function::generateBoolExpr(const bool_expr *e) {
  return llvm::ConstantInt::get(getType(e), e->getValue(), false);
}

llvm::Value *codegen_function::generateIntExpr(const int_expr *e) {
  return llvm::ConstantInt::get(getType(e), e->getValue(), true);
}

llvm::Value *codegen_function::generateFloatExpr(const float_expr *e) {
  return llvm::ConstantFP::get(getType(e), e->getValue());
}

llvm::Value *codegen_function::generateIdExpr(const id_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateUopExpr(const uop_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateAddressExpr(const uop_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateDerefExpr(const uop_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateBopExpr(const bop_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateRelationalExpr(const bop_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateCallExpr(const call_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateIndexExpr(const index_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateAssignExpr(const assign_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateCondExpr(const cond_expr *e) {
  return nullptr;
}

llvm::Value *codegen_function::generateConvExpr(const conv_expr *c) {
  return nullptr;
}

void codegen_function::generateStmt(const statement *s) {
  switch (s->getKind()) {
  case statement::block_kind:
    return generateBlockStmt(static_cast<const block_stmt *>(s));
  case statement::when_kind:
    return generateWhenStmt(static_cast)
  }
}
