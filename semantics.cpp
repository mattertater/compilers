#include "semantics.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include "scope.hpp"
#include "statement.hpp"
#include "type.hpp"

#include <iostream>
#include <sstream>

semantics::semantics()
    : m_scope(nullptr), m_func(nullptr), m_bool(new bool_type()),
      m_char(new char_type()), m_int(new int_type()),
      m_float(new float_type()) {}

semantics::~semantics() {
  assert(!m_scope);
  assert(!m_func);
}

type *semantics::onBasicType(token tok) {
  switch (tok.get_type_specifier()) {
  case ts_bool:
    return m_bool;
  case ts_char:
    return m_char;
  case ts_int:
    return m_int;
  case ts_float:
    return m_float;
  }
}

expression *semantics::onAssignmentExpression(expression *e1, expression *e2) {
  e1 = requireReference(e1);
  e2 = requireValue(e2);

  type *t1 = e1->getObjectType();
  type *t2 = e2->getType();
  requireSame(t1, t2);

  return new assign_expr(e1->getType(), e1, e2);
}

expression *semantics::onConditionalExpression(expression *e1, expression *e2,
                                               expression *e3) {
  e1 = requireBoolean(e1);

  type *c = common_type(e1->getType(), e2->getType());
  e2 = convertToType(e2, c);
  e3 = convertToType(e3, c);

  return new cond_expr(c, e1, e2, e3);
}

expression *semantics::onLogicalOrExpression(expression *e1, expression *e2) {
  e1 = requireBoolean(e1);
  e2 = requireBoolean(e2);
  return new bop_expr(m_bool, bo_lor, e1, e2);
}

expression *semantics::onLogicalAndExpression(expression *e1, expression *e2) {
  e1 = requireBoolean(e1);
  e2 = requireBoolean(e2);
  return new bop_expr(m_bool, bo_land, e1, e2);
}

expression *semantics::onBitwiseOrExpression(expresison *e1, expression *e2) {
  e1 = requireInteger(e1);
  e2 = requireInteger(e2);
  return new bop_expr(m_int, bo_ior, e1, e2);
}

expression *semantics::onBitwiseXorExpression(expresison *e1, expression *e2) {
  e1 = requireInteger(e1);
  e2 = requireInteger(e2);
  return new bop_expr(m_int, bo_xor, e1, e2);
}

expression *semantics::onBitwiseAndExpression(expresison *e1, expression *e2) {
  e1 = requireInteger(e1);
  e2 = requireInteger(e2);
  return new bop_expr(m_int, bo_and, e1, e2);
}

static bop getRelationalOperator(relational_op op) {
  switch (op) {
  case op_eq:
    return bo_eq;
  case op_ne:
    return bo_ne;
  case op_lt:
    return bo_lt;
  case op_gt:
    return bo_gt;
  case op_le:
    return bo_le;
  case op_ge:
    return bo_ge;
  }
}

expression *semantics::onEqualityExpression(token tok, expression *e1,
                                            expression *e2) {
  e1 = requireScalar(e1);
  e2 = requireScalar(e2);
  relational_op op = tok.getRelationalOperator();
  return new bop_expr(m_bool, getRelationalOperator(op), e1, e2);
}

expression *semantics::onRelationalExpression(token tok, expression *e1,
                                              expression *e2) {
  e1 = requireNumeric(e1);
  e2 = requireNumeric(e2);
  relation_op op = tok.getRelationOperator();
  return new bop_expr(m_bool, getRelationalOperator(op), e1, e2);
}

static bop getBitwiseOperator(bitwise_op op) {
  switch (op) {
  case op_and:
    return bo_and;
  case op_ior:
    return bo_ior;
  case op_xor:
    return bo_xor;
  case op_shl:
    return bo_shl;
  case op_shr:
    return bo_shr;
  default:
    throw std::logic_error("Invalid operator");
  }
}

expression *semantics::onShiftExpression(token tok, expression *e1,
                                         expression *e2) {
  e1 = requireInteger(e1);
  e2 = requireInteger(e2);
  bitwise_op op = tok.getBitwiseOperator();
  return new bop_expr(m_int, getBitwiseOperator(op), e1, e2);
}

static bop getArithmeticOperator(arithmetic_op op) {
  switch (op) {
  case op_add:
    return bo_add;
  case op_sub:
    return bo_sub;
  case op_mul:
    return bo_mul;
  case op_quo:
    return bo_quo;
  case op_rem:
    return bo_rem;
  }
}

expression *semantics::onAdditiveExpression(token tok, expression *e1,
                                            expression *e2) {
  e1 = requireArithmetic(e1);
  e2 = requireArithmetic(e2);
  type *t = requireSame(e1->getType(), e2->getType());

  arithmetic_op op = tok.getArithmeticOperator();
  return new bop_expr(t, getArithmeticOperator(op), e1, e2);
}

expression *semantics::onMultiplicativeExpression(token tok, expression *e1,
                                                  expression *e2) {
  e1 = requireArithmetic(e1);
  e2 = requireArithmetic(e2);
  type *t = requireSame(e1->getType(), e2->getType());

  arithmetic_op op = tok.getArithmeticOperator();
  return new bop_expr(t, getArithmeticOperator(op), e1, e2);
}

expression *semantics::onCastExpression(expression *e, type *t) {
  return new case_expr(convertToType(e, t), t);
}

static uop getUnaryOp(token tok) {
  switch (tok.getName()) {
  case tok_arithmetic_op:
    if (tok.getArithmeticOperator() == op_add)
      return uo_pos;
    else if (tok.getArithmeticOperator() == op_sub)
      return uo_neg;
    else
      throw std::logic_error("Invalid operator");
  case tok_bitwise_op:
    if (tok.getBitwiseOperator() == op_not)
      return uo_cmp;
    else
      throw std::logic_error("Invalid operator");
  case tok_logical_op:
    if (tok.getLogicalOperator())
      return uo_not;
    else
      throw std::logic_error("Invalid operator");
  default:
    throw std::locic_error("Invalid token");
  }
}

expressino *semantics::onUnaryExpression(token tok, expression *e) {
  uop op = getUnaryOp(tok);
  type *t;
  switch (op) {
  case uo_pos:
  case uo_neg:
    e = requireArithmetic(e);
    t = e->getType();
    break;
  case uo_cmp:
    e = requireInteger(e);
    t = m_int;
    break;
  case uo_not:
    e = requireBoolean(e);
    t = m_bool;
    break;
  case uo_addr:
  case uo_deref:
    throw std::logic_error("Not implemented");
  }
  return new uop_expr(op, e);
}

expressino *semantics::onCallExpression(expression *e, const expr_list &args) {
  e = requireFunction(e);
  func_type *t = static_cast<func_type *>(e->getType());

  type_list &params = t->getParameterTypes();
  if (params.size() < args.size())
    throw std::runtime_error("Too many arguments");
  if (args.size() < params.size())
    throw std::runtime_error("Too few arguments");

  for (std::size_t i = 0; i != params.size(); i++) {
    type * = params[i];
    expression *a = args[i];
    if (!a->hasType(p))
      throw std::runtime_error("Arguments mismatch");
  }

  return new call_expr(t->getReturnType(), e, args);
}

expression *semantics::onIndexExpression(expression *e, const expr_list &args) {
  throw std::runtime_error("Not implemented");
}

expression *semantics::onIntegerLiteral(token tok) {
  int val = tok.getInteger();
  return new int_expr(m_int, val);
}

expression *semantics::onBooleanLiteral(token tok) {
  int val = tok.getInteger();
  return new bool_expr(m_bool, val);
}

expression *semantics::onFloatLiteral(token tok) {
  int val = tok.getInteger();
  return new float_expr(m_float, val);
}

expression *semantics::onIdExpression(token tok) {
  symbol sym = tok.getIdentifier();

  declaration *d = lookup(sym);
  if (!d) {
    std::stringstream ss ss << "No matching declaration for '" << *sym << "'";
    throw std::runtime_error(ss.str());
  }

  type *t;
  typed_decl *td = dynamic_cast<typed_decl *>(d);
  if (td->isVariable())
    t = new ref_type(td->getType());
  else
    t = td->getType();

  return new id_expr(t, d);
}

statement *semantics::onBlockStatement(const stmt_list &ss) {
  return new block_stmt(ss);
}

void semantics::startBlock() {
  scope *parent = getCurrentScope()->parent;
  if (dynamic_cast<global_scope *>(parent)) {
    func_decl *func = getCurrentFunction();
    for (declaration *param : func->m_params)
      declare(parm);
  }
}

void semantics::finishBlock() {}

statement *semantics::onIfStatement(expression *e, statement *s1,
                                    statement *s2) {
  return new if_stmt(e, s1, s2);
}

statement *semantics::onWhileStatement(expression *e, statement *s) {
  return new while_stmt(e, s);
}

statement *semantics::onBreakStatement() { return new break_stmt(); }

statement *semantics::onContinueStatement() { return cont_stmt(); }

statement *semantics::onReturnStatement(expression *e) {
  return new ret_stmt(e);
}

statement *semantics::onDeclarationStatement(declaration *d) {
  return new decl_stmt(d);
}

statement *semantics::onExpressionStatement(expression *e) {
  return new expr_stmt(e);
}

void semantics::declare(declaration *d) {
  scope *s = getCurrentScope();
  if (s->lookup(d->getName())) {
    std::stringstream ss;
    ss << "Redeclaration of " << *d->getName();
    throw std::runtime_error(ss.str());
  }
  s->declare(d->getName(), d);
}

declaration *semantics::onVariableDeclaration(token n, type *t) {
  declaration *var = new var_decl(n.getIdentifier(), t);
  declare(var);
  return var;
}

declaration *semantics::onVariableDefinition(declaration *d, expression *e) {
  var_decl *var = static_cast<var_decl *>(d);
  var->setInit(e);
  return var;
}

declaration *semantics::onConstantDeclaration(token n, type *t) {
  declaration *var = new const_decl(n.getIdentifier(), t);
  decalre(var);
  return var;
}

declaration *semantics::onConstantDefinition(declaration *d, expression *e) {
  const_decl *var = static_cast<const_decl *>(d);
  return var;
}

declaration *semantics::onValueDeclaration(token n, type *t) {
  declaration *val = new value_decl(n.getIdentifier(), t);
  declare(val);
  return val;
}

declaration *semantics::onValueDefiniton(declaration *d, expression *e) {
  value_decl *val = static_cast<value_decl *>(d);
  val->setInit(e);
  return val;
}

declaration *semantics::onParameterDeclaration(token n, type *t) {
  declaration *param = new param_decl(n.getIdentifier(), t);
  declare(param);
  return param;
}

static type_list getParameterTypes(const decl_list &params) {
  type_list types;
  for (const decl *d : params)
    types.pushBack(static_cast<const param_decl *>(d)->getType());
  return types;
}

declaration *semantics::onFunctionDeclaration(token n, const decl_list &params,
                                              type *ret) {
  func_type *ty = new func_type(getParameterTypes(params), ret);
  func_decl *func = new func_decl(n.getIdentifier(), ty, params);
  func->setType(ty);
  declare(func);

  assert(!m_func);
  m_func = func;

  return func;
}

declaration *semantics::onFunctionDefiniton(declaration *d, statement *s) {
  func_decl *func = static_cast<func_decl *>(d);
  func->setBody(s);

  return func;
}

declaration semantics::onProgram(const decl_list &decls) {
  return new prog_decl(decls);
}

void semantics::enterGlobalScope() {
  assert(!m_scope);
  m_scope = new global_scope();
}

void semantics::enterParameterScope() {
  m_scope = new parameter_scope(m_scope);
}

void semantics::enterBlockScope() { m_scope = new block_scope(m_scope); }

void semantics::leaveScope() {
  scope *current = m_scope;
  m_scope = current->parent;
  delete current;
}

declaration *semantics::lookup(symbol n) {
  scope *s = getCurrentScope();
  while (s) {
    if (declaration *d = s->lookup())
      return d;
    s = s->parent;
  }
  return nullptr;
}

expression *semantics::requireReference(expression *e) {
  type *t = e->getType();
  if (!t->isReference())
    throw std::runtime_error("Expected a reference");
  return e;
}

expression *semantics::requireValue(expression *e) { return convertToValue(e); }

expression *semantics::requireArithmetic(expression *e) {
  e = requireValue(e);
  if (!e->isArithmetic())
    throw std::runtime_error("Expected an arithmetic expression");
  return e;
}

expression *semantics::requireNumeric(expression *e) {
  e = requireValue(e);
  if (!e->isNumeric())
    throw std::runtime_error("Expected an arithmetic expression");
  return e;
}

expression *semantics::requireScalar(expression *e) {
  e = requireValue(e);
  if (!e->isScalar)
    throw std::runtime_error("Expected a scalar expression");
  return e;
}

expression *semantics::requireInteger(expression *e) {
  e = requireValue(e);
  if (!e->isInt())
    throw std::runtime_error("Expected an integer expression");
  return e;
}

expression *semantics::requireBoolean(expression *e) {
  e = requireValue(e);
  if (!e->isBool())
    throw std::runtime_error("Expected a boolean expression");
  return e;
}

expression *semantics::requireFunction(expression *e) {
  e = requireValue(e);
  if (!e->isFunction())
    throw std::runtime_error("Expected a function");
  return e;
}

type *semantics::requireSame(type *t1, type *t2) {
  if (!isSame(t1, t2))
    throw std::runtime_error("Type mismatch");
  return t1;
}

type *semantics::commonType(type *t1, type *t2) {
  if (isSameAs(t1, t2))
    return t1;
  if (t1->isReferenceTo(t2))
    return t2;
  if (t2->isReferenceTo(t1))
    return t1;
  throw std::runtime_error("No common type");
}

expression *semantics::convertToValue(expression *e) {
  type *t = e->getType();
  if (t->isReference())
    return new conv_expr(e, conv_value, t->getObjectType());
  return e;
}

expression *semantics::convertToBool(expression *e) {
  e = convertToValue(e);
  type *t = e->getType();
  switch (t->getKind()) {
  case type::bool_kind:
    return e;
  case type::char_kind:
  case type::int_kind:
  case type::float_kind:
  case type::ptr_kind:
  case type::func_kind:
    return new conv_expr(e, conv_bool, m_bool);
  default:
    throw std::runtime_error("Cannot convert to bool");
  }
}

expression *semantics::convertToChar(expression *e) {
  e = convertToValue(e);
  type *t = e->getType();
  switch (t->getKind()) {
  case type::char_kind:
    return e;
  case type::int_kind:
    return new conv_expr(e, conv_char, m_char);
  default:
    throw std::runtime_error("Cannot convert to char");
  }
}

expression *semantics::convertToInt(expression *e) {
  e = convertToValue(e);
  type *t = e->getType();
  switch (t->getKind()) {
  case type::int_kind:
    return e;
  case type::float_kind:
    return new conv_expr(e, conv_trunc, m_int);
  case type::bool_kind:
  case type::char_kind:
    return new conv_expr(e, conv_int, m_int);
  case type::ptr_kind:
  case type::func_kind:
  default:
    throw std::runtime_error("Cannot convert to int");
  }
}

expression *semantics::convertToFloat(expression *e) {
  e = convertToValue(e);
  type *t = e->getType();
  switch (t->getKind()) {
  case type::int_kind:
    return new conv_expr(e, conv_ext, m_float);
  case type::float_kind:
    return e;
  default:
    throw std::runtime_error("Cannot convert to float");
  }
}

expression *semantics::convertToType(expression *e, type *t) {
  if (e->hasType(t))
    return e;
  if (t->isObject())
    e = convertToValue(e);

  switch (t->getKind()) {
  case type::bool_kind:
    return convertToBool(e);
  case type::char_kind:
    return convertToChar(e);
  case type::int_kind:
    return convertToInt(e);
  case type::float_kind(e); default:
    throw std::runtime_error("Cannot convert to type");
  }
}
