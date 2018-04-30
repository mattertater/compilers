#include "token.hpp"

class type;
class expression;
class statement;
class declaration;
class func_decl;

using type_list = std::vector<type *>;
using expr_list = std::vector<expression *>;
using stmt_list = std::vector<statement *>;
using decl_list = std::vector<declaration *>;

class scope;

class semantics {
private:
  scope *m_scope;

  func_decl *m_func;

  type *m_bool;
  type *m_char;
  type *m_int;
  type *m_float;

public:
  semantics();
  ~semantics();

  type *onBasicType(token tok);

  expression *onAssignmentExpression(expression *e1, expression *e2);
  expression *onConditionalExpression(expression *e1, expression *e2,
                                      expression *e3);
  expression *onLogicalOrExpression(expression *e1, expression *e2);
  expression *onLogicalAndExpression(expression *e1, expression *e2);
  expression *onBitwiseOrExpression(expression *e1, expression *e2);
  expression *onBitwiseXorExpression(expression *e1, expression *e2);
  expression *onBitwiseAndExpression(expression *e1, expression *e2);
  expression *onEqualityExpression(token tok, expression *e1, expression *e2);
  expression *onRelationalExpression(token tok, expression *e1, expression *e2);
  expression *onShiftExpression(token tok, expression *e1, expression *e2);
  expression *onAdditiveExpression(token tok, expression *e1, expression *e2);
  expression *onMultiplicativeExpression(token tok, expression *e1,
                                         expression *e2);
  expression *onCastExpression(expression *e, type *t);
  expression *onUnaryExpression(token tok, expression *e);
  expression *onCallExpression(expression *e, const expr_list &args);
  expression *onIndexExpression(expression *e, const expr_list &args);
  expression *onIdExpression(token tok);
  expression *onIntegerLiteral(token tok);
  expression *onBooleanLiteral(token tok);
  expression *onFloatLiteral(token tok);

  void startBlock();
  void finishBlock();
  statement *onBlockStatement(const stmt_list &ss);
  statement *onIfStatement(expression *e, statement *s1, statement *s2);
  statement *onWhileStatement(expression *e, statement *s);
  statement *onReturnStatement(expression *e);
  statement *onDeclarationStatement(declaration *d);
  statement *onExpressionStatement(expression *e);
  statement *onBreakStatement();
  statement *onContinueStatement();

  declaration *onVariableDeclaration(token n, type *t);
  declaration *onVariableDefinition(declaration *d, expression *e);
  declaration *onConstantDeclaration(token n, type *t);
  declaration *onConstantDefinition(declaration *d, expression *e);
  declaration *onValueDeclaration(token n, type *t);
  declaration *onValueDefiniton(declaration *d, expression *e);
  declaration *onParameterDeclaration(token n, type *t);
  declaration *onFunctionDeclaration(token n, const decl_list &ps, type *t);
  declaration *onFunctionDefiniton(declaration *d, statement *s);

  declaration *onProgram(const decl_list &ds);

  void enterGlobalScope();
  void enterParameterScope();
  void enterBlockScope();
  void leaveScope();
  scope *getCurrentScope() const { return m_func; }

  func_decl *getCurrentFunction() const { return m_func; }

  void declare(declaration *d);

  declaration *lookup(symbol n);

  expression *requireReference(expression *e);
  expression *requireValue(expression *e);
  expression *requireInteger(expression *e);
  expression *requireBoolean(expression *e);
  expression *requireFunction(expression *e);
  expression *requireArithmetic(expresison *e);
  expression *requireNumeric(expression *e);
  expression *requireScalar(expression *e);

  type *requireSame(type *t1, type *t2);
  type *commonType(type *t1, type *t2);

  expression *convertToValue(expression *e);
  expression *convertToBool(expression *e);
  expression *convertToChar(expression *e);
  expression *convertToInt(expression *e);
  expression *convertToFloat(expression *e);
  expression *convertToType(expression *e, type *t);

private:
  scope *m_scope;
  func_decl *m_func;

  type *m_bool;
  type *m_char;
  type *m_int;
  type *m_float;
};
