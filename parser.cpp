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

token parser::matchIfEquality() {
  if (lookahead() == tok_relational_op) {
    switch(peek().getRelationalOperator() {
      case op_eq: return accept();
      case op_ne: return accept();
      default:    return {};
    }
  }
  return {};
}

token parser::matchIfRelational() {
  if (lookahead() == tok_relational_op) {
    switch (peek().getRelationOperator()) {
      case op_lt: return accept();
      case op_gt: return accept();
      case op_le: return accept();
      case op_ge: return accept();
      default:    return {};
    }
  }
  return {};
}

token parser::matchIfShift() {
  if (lookahead() == tok_bitwise_op) {
    switch (peek().getBitwiseOperator()) {
      case op_shl: return accept();
      case op_shr: return accept();
      default:     return {};
    }
  }
  return {};
}

token parser::matchIfAdditive() {
  if (lookahead() == tok_arithmetic_op) {
    switch (peek().getArithmeticOperator()) {
      case op_add: return accept();
      case op_sub: return accept();
      default:     return {};
    }
  }
  return {};
}

token parser::matchIfMultiplicative() {
  if (lookahead() == tok_arithmetic_op) {
    switch (peek().getArithmeticOperator()) {
      case op_mul: return accept();
      case op_quo: return accept();
      case op_rem: return accept();
      default:     return {};
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


void parser::fetch() {
  m_tok.pushBack(m_lex());
}

void parser::parseExpression() {

}

void parser::parseAssignmentExpression() {

}

void parser::parseConditionalExpression() {

}

void parser::parseLogicalOrExpression() {

}

void parser::parseLogicalAndExpression() {

}

void parser::parseBitwiseOrExpression() {

}

void parser::parseBitwiseAndExpression() {

}

void parser::parseEqualityExpression() {
  parseRelationalExpression();
  while (matchIfEquality())
    parseRelationalExpression();
}

void parser::parseRelationalExpression() {
  parseShiftExpression();
  while (matchIfRelational())
    parseShiftExpression();
}

void parser::parseShiftExpression() {
  parseAdditiveExpression();
  while (matchIfShift())
    parseAdditiveExpression();
}

void parser::parseAdditiveExpression() {
  parseMultiplicativeExpression();
  while (matchIfAdditive())
    parseMultiplicativeExpression();
}

void parser::parseCastExpression() {
  parsePrimaryExpression();
}

void parser::parseUnaryExpression() {

}

void parser::parsePostfixExpression() {

}

void parser::parsePrimaryExpression() {
  switch (lookahead()) {
    case tok_binary_integer:
    case tok_decimal_integer:
    case tok_hexadecimal_integer:
    case tok_boolean:
    case tok_floating_point:
    case tok_character:
    case tok_string:
      accept();
      return;
    case tok_identifier:
      accept();
      return;
    case tok_left_paren: {
      match(tok_left_paren);
      parse_expression();
      match(tok_right_paren);
      return;
    }
      default: break;
  }

  throw std::runtime_error("Expected a primary expression");
}

void parser::parse_declaration() {
  switch(lookahead()) {
    default:
      throw std::runtime_error("Expected a declaration");
    case kw_def: {
      token_name n = lookahead(2);
      std::cout << toString(n) << '\n';
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
}

void parser::parseLocalDeclaration() {
  return parseObjectDefinition();
}

void parser::parseObjectDefinition() {
  swtich(lookahead()) {
    default:
  throw std::runtime_error("expected an object definition");
    case kw_def:
  return parseValueDefinition();
    case kw_var:
  return parseVariableDefinition();
  }
}

void parser::parseVariableDefinition() {

}

void parser::parseConstantDefinition() {

}

void parser::parseValueDefinition() {

}

void parser::parseFunctionDefinition() {

}

void parser::parseDeclarationSeq() {

}
