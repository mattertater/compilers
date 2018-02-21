#include "token.hpp"

#include <cassert>
#include <iostream>
#include <iomanip>

const char*
to_string(token_name n) {

  switch (n) {
    // Punctuators
    case tok_lBrace:    return "left-brace";
    case tok_rBrace:    return "right-brace";
    case tok_lParen:    return "left-paren";
    case tok_rParen:    return "right-paren";
    case tok_lBracket:  return "left-bracket";
    case tok_rBracket:  return "right-bracket";
    case tok_comma:     return "comma";
    case tok_semi:      return "semicolon";
    case tok_colon:     return "colon";

    // Operators
    case tok_relational_op:   return "relational-operator";
    case tok_arithmetic_op:   return "arithmetic-operator";
    case tok_bitwise_op:      return "bitwise-operator";
    case tok_logical_op:      return "logical-operator";
    case tok_conditional_op:  return "conditional-operator";
    case tok_assignment_op:   return "assignment-operator";

    // Keywords
    case kw_boolean:  return "boolean";
    case kw_def:      return "def";
    case kw_else:     return "else";
    case kw_if:       return "if";
    case kw_let:      return "let";
    case kw_var:      return "var";

    // More keywords
    case tok_identifier:          return "identifier";
    case tok_binary_integer:      return "binary-integer";
    case tok_decimal_integer:     return "decimal-integer";
    case tok_hexadecimal_integer: return "hexadecimal-integer";
    case tok_floating_point:      return "floating-point";
    case tok_character:           return "character";
    case tok_string:              return "string";
    case tok_type_specifier:      return "type-specifier";
    case tok_eof:                 return "eof";
  }
}

// Relational Operators
const char* to_string(relational_op op) {
  switch (op) {
    case op_eq:  return "eq";
    case op_ne:  return "ne";
    case op_lt:  return "lt";
    case op_gt:  return "gt";
    case op_le:  return "le";
    case op_ge:  return "ge";
  }
};

// Arithmetic Operators
const char* to_string(arithmetic_op op) {
  switch (op) {
    case op_add:  return "add";
    case op_sub:  return "sub";
    case op_mul:  return "mul";
    case op_quo:  return "quo";
    case op_rem:  return "rem";
  }
};

// Bitwise Operators
const char* to_string(bitwise_op op) {
  switch (op) {
    case op_and:  return "and";
    case op_ior:  return "ior";
    case op_xor:  return "xor";
    case op_shl:  return "shl";
    case op_shr:  return "shr";
    case op_not:  return "not";
  }
};

// Logical Operators
const char* to_string(logical_op op) {
  switch (op) {
    case logical_and:  return "and";
    case logical_or:   return "or";
    case logical_not:  return "not";
  }
};

// Special Types
const char* to_string(type_spec ts) {
  switch (ts) {
    case ts_bool:  return "char";
    case ts_int:   return "int";
    case ts_char:  return "char";
    case ts_float: return "float";
  }
};


token::token() : m_name(tok_eof) { }

token::token(token_name n, token_attr a, location loc) :  m_name(n),
                                                          m_attr(a),
                                                          m_loc(loc) { }

static bool has_attribute(token_name n) {

  switch (n) {

    case tok_relational_operator:
    case tok_arithmetic_operator:
    case tok_bitwise_operator:
    case tok_logical_operator:
    case tok_identifier:
    case tok_binary_integer:
    case tok_decimal_integer:
    case tok_hexadecimal_integer:
    case tok_boolean:
    case tok_floating_point:
    case tok_character:
    case tok_string:
    case tok_type_specifier:
      return true;

    default:
        return false;
  }
}

token::token(token_name n, location loc) : m_name(n), m_loc(loc) {
  assert(!has_attribute(n));
}

token::token(symbol sym, location loc) : m_name(tok_identifier), m_attr(sym), m_loc(loc) { }

token::token(relational_op op, location loc) : m_name(tok_relational_operator), m_attr(op), m_loc(loc) { }

token::token(arithmetic_op op, location loc) : m_name(tok_arithmetic_operator), m_attr(op), m_loc(loc) { }

token::token(bitwise_op op, location loc) : m_name(tok_bitwise_operator), m_attr(op), m_loc(loc) { }

token::token(logical_op op, location loc) : m_name(tok_logical_operator), m_attr(op), m_loc(loc) { }

token::token(long long val, location loc) : token(tok_decimal_integer, decimal, val, loc) { }



static token_name get_token_name(radix rad) {
  switch (rad) {
    case binary:       return tok_binary_integer;
    case decimal:      return tok_decimal_integer;
    case hexadecimal:  return tok_hexadecimal_integer;
  }
}



token::token(radix rad, long long val, location loc) : token(get_token_name(rad), rad, val, loc) { }



static bool check_radix(token_name n, radix rad) {

  switch (n) {

    case tok_binary_integer:       return rad == binary;
    case tok_decimal_integer:      return rad == decimal;
    case tok_hexadecimal_integer:  return rad == hexadecimal;

    default:    throw std::logic_error("invalid token name");
  }
}


token::token(token_name n, radix rad, long long val, location loc) : m_name(n),
                                                                      m_attr(integer_attr{rad, val}),
                                                                      m_loc(loc) {
  assert(check_radix(n, rad));
}

token::token(double val, location loc) : m_name(tok_floating_point), m_attr(val), m_loc(loc) { }

token::token(bool tf, location loc) : m_name(tok_boolean), m_attr(tf), m_loc(loc) { }

token::token(char c, location loc) : m_name(tok_character), m_attr(c), m_loc(loc) { }

token::token(string_attr s, location loc) : m_name(tok_string), m_attr(s), m_loc(loc) { }

token::token(type_spec ts, location loc) : m_name(tok_type_specifier), m_attr(ts), m_loc(loc) { }


static std::string escape(char c) {
  switch (c) {
    case '\'': return "\\\'";
    case '\"': return "\\\"";
    case '\\': return "\\\\";
    case '\a': return "\\a";
    case '\b': return "\\b";
    case '\f': return "\\f";
    case '\n': return "\\n";
    case '\r': return "\\r";
    case '\t': return "\\t";
    case '\v': return "\\v";
    default: return {c};
  }
}

static std::string escape(const std::string& s) {
  std::string ret;
  for (char c : s)
    ret += escape(c);
  return ret;
}

std::ostream& operator<<(std::ostream& os, token tok) {
  os << '<';

  os << to_string(tok.get_name());
  switch (tok.get_name()) {
  default:
    break;

  case tok_relational_operator:
    os << ':' << to_string(tok.get_relational_operator());
    break;

  case tok_arithmetic_operator:
    os << ':' << to_string(tok.get_arithmetic_operator());
    break;

  case tok_bitwise_operator:
    os << ':' << to_string(tok.get_bitwise_operator());
    break;

  case tok_logical_operator:
    os << ':' << to_string(tok.get_logical_operator());
    break;

  case tok_identifier:
    os << ':' << *tok.get_identifier();
    break;

  case tok_binary_integer:
  case tok_decimal_integer:
  case tok_hexadecimal_integer:
    os << ':' << std::setbase(tok.get_radix()) << tok.get_integer();
    break;

  case tok_boolean:
    os << ':' << tok.get_boolean();
    break;

  case tok_floating_point:
    os << ':' << tok.get_floating_point();
    break;

  case tok_character:
    os << ':' << escape(tok.get_character());
    break;

  case tok_string:
    os << ':' << escape(tok.get_string());
    break;

  case tok_type_specifier:
    os << ':' << to_string(tok.get_type_specifier());
    break;
  }
  os << '>';
  return os;
}
