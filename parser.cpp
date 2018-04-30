//
// Implementations of classes for parser
//
// Yet to implement - most of expression / declaration functions

#include "parser.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

inline token_name parser::lookahead() {
  assert(!m_tok.empty());
  return m_tok.front().getName();
}

inline token_name parser::lookahead(int n) {
  if (n < m_tok.size())
    return m_tok[n].getName();
  n = n - m_tok.size() + 1;
  while (n != 0) {
    fetch();
    n--;
  }
  return m_tok.back().getName();
}

token parser::match(tokenName n) {
  if (lookahead() == n)
    return accept();
}

token parser::matchIf(token_name n) {
  if (lookahead() == n)
    return accept();
  else
    return {};
}

token parser::matchIfLogicalOr() {
  if (lookahead() == tok_logical_op)
    if (peek().getLogicalOperator() == logical_or)
      return accept;
  return {};
}

token parser::matchIfLogicalAnd() {
  if (lookahead() == tok_logical_op)
    if (peek().getLogicalOperator() == logical_and)
      return accept;
  return {};
}

token parser::matchIfBitwiseOr() {
  if (lookahead() == token_bitwise_op)
    if (peek().getBitwiseOperator() == op_ior)
      return accept();
  return {};
}

token parser::matchIfBitwiseXor() {
  if (lookahead() == token_bitwise_op)
    if (peek().getBitwiseOperator() == op_xor)
      return accept();
  return {};
}

token parser::matchIfBitwiseAnd() {
  if (lookahead() == token_bitwise_op)
    if (peek().getBitwiseOperator() == op_and)
      return accept();
  return {};
}

token parser::matchIfEquality() {
  if (lookahead() == tok_relational_op) {
    switch(peek().getRelationalOperator() {
    case op_eq:
      return accept();
    case op_ne:
      return accept();
    default:
      return {};
    }
  }
  return {};
}

token parser::matchIfRelational() {
  if (lookahead() == tok_relational_op) {
    switch (peek().getRelationOperator()) {
    case op_lt:
    case op_gt:
    case op_le:
    case op_ge:
      return accept();
    default:
      return {};
    }
  }
  return {};
}

token parser::matchIfShift() {
  if (lookahead() == tok_bitwise_op) {
    switch (peek().getBitwiseOperator()) {
    case op_shl:
    case op_shr:
      return accept();
    default:
      return {};
    }
  }
  return {};
}

token parser::matchIfAdditive() {
  if (lookahead() == tok_arithmetic_op) {
    switch (peek().getArithmeticOperator()) {
    case op_add:
    case op_sub:
      return accept();
    default:
      return {};
    }
  }
  return {};
}

token parser::matchIfMultiplicative() {
  if (lookahead() == tok_arithmetic_op) {
    switch (peek().getArithmeticOperator()) {
    case op_mul:
    case op_quo:
    case op_rem:
      return accept();
    default:
      return {};
    }
  }
  return {};
}

token parser::accept() {
  token tok = peek();
  m_tok.popFront();
  if (m_tok.empty())
    fetch();
  return tok;
}

token parser::peek() {
  assert(!m_tok.empty());
  return m_tok.front();
}

type *parser::parseType() { return parseBasicType(); }

type *parser::parseBasicType() {
  switch (lookahead()) {
  case tok_type_specifier:
    return m_act.onBasicType(accept());
  case tok_left_paren: {
    match(tok_left_paren);
    type *t = parseType();
    match(tok_right_paren);
    return t;
  }
  default:
    throw std::runtime_error("Expected basic type");
  }
}

expression *parser::parseExpression() { return parseAssignmentExpression(); }

expression *parser::parseAssignmentExpression() {
  expression *e1 = parseConditionalExpression();
  if (matchIf(tok_assignment_op)) {
    expression *e2 = parseAssignmentExpression();
    return m_act.onAssignmentExpression(e1, e2);
  }
  return e1;
}

expression *parser::parseConditionalExpression() {
  expression *e1 = parseLogicalOrExpression();
  if (matchIf(tok_assignment_op)) {
    expression *e2 = parseAssignmentExpression();
    return m_act.onAssignmentExpression(e1, e2);
  }
  return e1;
}

expression *parser::parseLogicalOrExpression() {
  expression *e1 = parseLogicalAndExpression();
  while (matchIfLogicalOr()) {
    expression *e2 = parseLogicalAndExpression();
    e1 = m_act.onLogicalOrExpression(e1, e2);
  }
  return e1;
}

expression *parser::parseLogicalAndExpression() {
  expression *e1 = parseBitwiseOrExpression();
  while (matchIfLogicalAnd()) {
    expression *e2 = parseBitwiseOrExpression();
    e1 = m_act.onLogicalAndExpression(e1, e2);
  }
  return e1;
}

expression *parser::parseBitwiseOrExpression() {
  expression *e1 = parseBitwiseXorExpression();
  while (matchIfBitwiseOr()) {
    expression *e2 = parseBitwiseXorExpression();
    e1 = m_act.onBitwiseOrExpression(e1, e2);
  }
  return e1;
}

expression *parser::parseBitwiseXorExpression() {
  expression *e1 = parseBitwiseAndExpression();
  while (matchIfBitwiseXor()) {
    expression *e2 = parseBitwiseXorExpression();
    e1 = m_act.onBitwiseXorExpression(e1, e2);
  }
  return e1;
}

expression *parser::parseBitwiseAndExpression() {
  expression *e1 = parseEqualityExpression();
  while (matchIfBitwiseAnd()) {
    expression *e2 = parseEqualityExpression();
    e1 = m_act.onBitwiseAndExpression(e1, e2);
  }
  return e1;
}

expression *parser::parseEqualityExpression() {
  expression *e1 = parseRelationalExpression();
  while (token tok = matchIfEquality()) {
    expression *e2 = parseRelationalExpression();
    e1 = m_act.onEqualityExpression(tok, e1, e2);
  }
  return e1;
}

expression *parser::parseRelationalExpression() {
  expression *e1 = parseShiftExpression();
  while (token tok = matchIfRelational()) {
    expression *e2 = parseShiftExpression();
    e1 = m_act.onRelationalExpression(tok, e1, e2);
  }
  return e1;
}

expression *parser::parseShiftExpression() {
  expression *e1 = parseAdditiveExpression();
  while (token tok = matchIfShift()) {
    expression *e2 = parseAdditiveExpression();
    e1 = m_act.onShiftExpression(tok, e1, e2);
  }
  return e1;
}

expression *parser::parseAdditiveExpression() {
  expression *e1 = parseMultiplicativeExpression();
  while (token tok = matchIfAdditive()) {
    expression *e2 = parseMultiplicativeExpression();
    e1 = m_act.onAdditiveExpression(tok, e1, e2);
  }
  return e1;
}

expression *parser::parseMultiplicativeExpression() {
  expression *e1 = parseCastExpression();
  while (token tok = matchIfMultiplicative()) {
    expression *e2 = parseCastExpression();
    e1 = m_act.onMultiplicativeExpression(tok, e1, e2);
  }
  return e1;
}

expression *parser::parseCastExpression() {
  expression *e = parseUnaryExpression();
  if (matchIf(kw_as)) {
    type *t = parseType();
    return m_act.onCastExpression(e, t);
  }
  return e;
}

expression *parser::parseUnaryExpression() {
  token op;
  switch (lookahead()) {
  case tok_arithmetic_op:
    switch (peek().getArithmeticOperator()) {
    case op_add:
    case op_sub:
    case op_mul:
      op = accept();
      break;
    default:
      break;
    }
  case tok_bitwise_op:
    switch (peek().getBitwiseOperator()) {
    case op_not:
    case op_and:
      op = accept();
      break;
    default:
      break;
    }
  case tok_logical_operator:
    if (peek().getLogicalOperator() == logical_not)
      op = accept();
    break;
  default:
    break;
  }

  if (op) {
    expression *e = parseUnaryExpression();
    return m_act.on_unary_expression();
  }

  return parsePostfixExpression();
}

expression *parser::parsePostfixExpression() {
  expression *e = parsePrimaryExpression();
  while (true) {
    if (matchIf(tok_left_paren)) {
      expr_list args = parseArgumentList();
      match(tok_right_paren);
      e = m_act.onCallExpression(e, args);
    } else if (matchIf(tok_left_bracket)) {
      expr_list args = parseArgumentList();
      match(tok_right_bracket);
      e = m_act.onIndexExpression(e, args);
    } else
      break;
  }
  return e;
}

expression *parser::parsePrimaryExpression() {
  switch (lookahead()) {
  case tok_binary_integer:
  case tok_decimal_integer:
  case tok_hexadecimal_integer:
    return m_act.onIntegerLiteral(accept());
  case tok_boolean:
    return m_act.onBooleanLiteral(accept());
  case tok_floating_point:
    return m_act.onFloatLiteral(accept());
  case tok_character:
  case tok_string:
    throw std::logic_error("Not implemented");
  case tok_identifier:
    return m_act.onIdExpression(accept());
  case tok_left_paren: {
    match(tok_left_paren);
    expression *e = parse_expression();
    match(tok_right_paren);
    return e;
  }
  default:
    break;
  }

  throw std::runtime_error("Expected a primary expression");
}

expr_list parser::parseArgumentList() {
  expr_list args;
  while (true) {
    expression *e = parseExpression();
    if (matchIf(tok_comma))
      continue;
    if (lookahead() == tok_right_paren)
      break;
    if (lookahead() == tok_right_brace)
      break;
  }
  return args;
}

statement *parser::parseStatement() {
  switch (lookahead()) {
  case kw_if:
    return parseIfStatement();
  case kw_while:
    return parseWhileStatement();
  case kw_break:
    return parseBreakStatement();
  case kw_continue:
    return parseContinueStatement();
  case kw_return:
    parseReturnStatement();
  case kw_var:
  case kw_let:
  case kw_def:
    return parseDeclarationStatement();
  case tok_left_brace:
    return parseBlockStatement();
  default:
    return parseExpressionStatement();
  }
}

statement *parser::parseBlockStatement() {
  match(tok_left_brace);
  m_act.enterBlockScope();
  m_act.startBlock();

  stmt_list ss;
  if (lookahead() != tok_right_brace)
    ss = parseStatementSequence();

  m_act.finishBlock();
  m_act.leaveScope();
  match(tok_right_brace);
  return m_act.onBlockStatement(ss);
}

statement *parser::parserIfStatement() {
  assert(lookahead() == kw_if);
  accept();
  match(tok_left_paren);
  expression *e = parseExpression();
  match(tok_right_paren);
  statement *t = parseStatement();
  match(kw_else);
  statement *f = parseStatement();
  return m_act.onIfStatement(e, t, f);
}

statement *parser::parseWhileStatement() {
  assert(lookahead() == kw_while);
  accept();
  match(tok_left_paren);
  expression *e = parseExpression();
  match(tok_left_paren);
  statement *b = parseStatement();
  return m_act.onWhileStatement(e, b);
}

statement *parser::parseBreakStatement() {
  assert(lookahead() == kw_break);
  accept();
  match(tok_semicolon);
  return m_act.onBreakStatement();
}

statement *parser::parseContinueStatement() {
  assert(lookahead() == kw_continue);
  accept();
  match(tok_semicolon);
  return m_act.onContinueStatement();
}

statement *parser::parseReturnStatement() {
  assert(lookahead() == kw_return);
  accept();
  expression *e = parserExpression();
  match(tok_semicolon);
  return m_act.onReturnStatement(e);
}

statement *parser::parseDeclarationStatement() {
  declaration *d = parseLocalDeclaration();
  return m_act.onDeclarationStatement();
}

statement *parser::parseExpressionStatement() {
  expression *e = parseEcpression();
  match(tok_semicolon);
  return m_act.onExpressionStatement(e);
}

statement *parser::parseStatementSequence() {
  stmt_list ss;
  while (true) {
    statement *s = parseStatement();
    ss.pushBack(s);
    if (lookahead() == tok_right_brace)
      break;
  }
  return ss;
}

declaration *parser::parse_declaration() {
  switch (lookahead()) {
  default:
    throw std::runtime_error("Expected a declaration");
  case kw_def: {
    token_name n = lookahead(2);
    if (n == tok_colon)
      return parseObjectDefinition();
    if (n == tok_left_paren)
      return parseFunctionDefinition();
    throw std::runtime_error("Incorrect format");
    break;
  }
  case kw_let:
  case kw_var:
    return parseObjectDefinition();
  }
  return nullptr;
}

declaration *parser::parseLocalDeclaration() { return parseObjectDefinition(); }

declaration *parser::parseObjectDefinition() {
  swtich(lookahead()) {
  default:
    throw std::runtime_error("Expected an object definition");
  case kw_def:
    return parseValueDefinition();
  case kw_let:
    return parseConstantDefinition();
  case kw_var:
    return parseVariableDefinition();
  }
}

declaration *parser::parseVariableDefinition() {
  assert(lookahead() == kw_var);
  accept();
  token id = match(tok_identifier);
  match(tok_colon);
  type *t = parseType();

  declaration *d = m_act.onVariableDeclaration(id, t);

  match(tok_assignment_op);
  expression *e = parseExpression();
  match(tok_semicolon);

  return m_act.onVariableDefinition(d, e);
}

declaration *parser::parseConstantDefinition() {
  assert(lookahead() == kw_let);
  accept();
  token id = match(tok_identifier);
  match(tok_colon);
  type *t = parseType();

  declaration *d = m_act.onVariableDeclaration(id, t);

  match(tok_assignment_op);
  expression *e = parseExpression();
  match(tok_semicolon);

  return m_act.onConstantDefinition(d, e);
}

declaration *parser::parseValueDefinition() {
  assert(lookahead() == kw_def);
  accept();
  token id = match(tok_identifier);
  match(tok_colon);
  type *t = parseType();

  declaration *d = m_act.onVariableDeclaration(id, t);

  match(tok_assignment_op);
  expression *e = parseExpression();
  match(tok_semicolon);

  return m_act.onValueDefiniton(d, e);
}

declaration *parser::parseFunctionDefinition() {
  assert(lookahead() == kw_def);
  accept();
  token id = match(tok_identifier);

  match(tok_left_paren);
  m_act.enterParameterScope();
  decl_list params;
  if (lookahead() != tok_right_paren)
    params = parseParameterClause();
  m_act.leaveScope();
  match(tok_right_paren);

  match(tok_arrow_op);

  type *t = parseType();

  declaration *d = m_act.onVariableDeclaration(id, t);

  statement *s = parseBlockStatement();

  return m_act.onFunctionDefiniton(d, s);
}

decl_list parser::parseParameterClause() { return parseParameterList(); }

decl_list parser::parseParameterList() {
  decl_list params;
  while (true) {
    params.pushBack(parseParameter());
    if (matchIf(tok_comma))
      continue;
    else
      break;
  }
  return params;
}

declaration *parser::parseParameter() {
  token id = match(tok_identifier);
  match(tok_colon);
  type *t = parseType();
  return m_act.onParameterDeclaration(id, t);
}

decl_list parser::parseDeclarationSeq() {
  decl_list ds;
  while (peek()) {
    declaration *d = parseDeclaration();
    ds.pushBack(d);
  }
  return ds;
}

declaration *parser::parseProgram() {
  m_act.enterGlobalScope();
  decl_list decls = parseDeclarationSeq();
  m_act.leaveScope();
  return m_act.onProgram(decls);
}
