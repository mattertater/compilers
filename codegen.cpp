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
}
