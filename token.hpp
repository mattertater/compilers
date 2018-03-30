//
// Manage names, types, and properties of tokens
//

#include "symbol.hpp"

enum token {
  // Punctuators
  tok_lBrace,
  tok_rBrace,
  tok_lParen,
  tok_rParen,
  tok_lBracket,
  tok_rBracket,
  tok_comma,
  tok_semi,
  tok_colon,

  // Operators
  tok_relational_op,
  tok_arithmetic_op,
  tok_bitwise_op,
  tok_logical_op,
  tok_conditional_op,
  tok_assignment_op,

  // Keywords
  kw_and,
  kw_bool,
  kw_char,
  kw_def,
  kw_else,
  kw_false,
  kw_float,
  kw_if,
  kw_int,
  kw_let,
  kw_not,
  kw_or,
  kw_true,
  kw_var,

  // More keywords
  tok_identifier,
  tok_binary_integer,
  tok_decimal_integer,
  tok_hexadecimal_integer,
  tok_floating_point,
  tok_character,
  tok_string,
  tok_type_specifier,
  tok_eof,

};


enum relational_op {
  op_eq,
  op_ne,
  op_lt,
  op_gt,
  op_le,
  op_ge,
};

enum arithmetic_op {
  op_add,
  op_sub,
  op_mul,
  op_quo,
  op_mod,
};

enum bitwise_op {
  op_and,
  op_ior,
  op_xor,
  op_shl,
  op_shr,
  op_not,
};

enum logical_op {
  logical_and,
  logical_or,
  logical_not,
};

enum radix {
  binary = 2;
  deciamal = 10;
  hexadecimal = 16;
}

const char* toString(token_name n);
const char* toString(relational_op op);
const char* toString(arithmetic_op op);
const char* toString(bitwise_op op);
const char* toString(logical_op op);

const char* getSpelling(token_name n);



struct integer_attr {
  radix rad;
  long long value;
};

struct string_attr {
  symbol sym;
};

union token_attr {
  token_attr() = default;
  token_attr(symbol sym) : sym(sym) {}
  token_attr(relational_op op) : rel_op(op) {}
  token_attr(arithmetic_op op) : arith_op(op) {}
  token_attr(bitwise_op op) : bit_op(op) {}
  token_attr(logical_op op) : log_op(op) {}
  token_attr(integer_attr val) : int_val(val) {}
  token_attr(double val) : float_val(val) {}
  token_attr(bool tf) : tf_val(tf) {}
  token_attr(char c) : char_val(c) {}
  token_attr(string_attr s) : str_val(s) {}
  token_attr(type_spec ts) : ts(ts) {}

  symbol sym;
  relational_op rel_op;
  arithmetic_op arith_op;
  bitwise_op bit_op;
  logical_op log_op;
  integer_attr int_val;
  double float_val;
  bool tf_val;
  char char_val;
  string_attr str_val;
  type_spec ts;
};

class token {
public:
  token();
  token(token_name name);
  token(token_name name, token_attr attr);
  token(symbol sym);
  token(relational_op op);
  token(arithmetic_op op);
  token(bitwise_op op);
  token(logical_op op);
  token(long long val);
  token(radix rad, long long val);
  token(token_name name, radix rad, long long val);
  token(double val);
  token(bool tf);
  token(char c);
  token(string_attr s);
  token(type_spec ts);

  operator bool() const { return m_name != tok_eof; }

  token_name getName() const { return m_name; }
  token_attr getAttribute() const { return m_attr; }

  bool isInteger() const;
  bool isFloatingPoint() const;

  symbol getIdentifier() const;
  relational_op getRelationalOperator() const;
  arithmetic_op getArithmeticOperator() const;
  bitwise_op getBitwiseOperator() const;
  logical_op getLogicalOperator() const;
  long long getInteger() const;
  double getFloatingPoint() const;
  radix getRadix() const;
  bool getBoolean() const;
  char getChar() const;
  const std::string& getString() const;
  type_spec getTypeSpecifier() const;

private:
  token_name m_name;
  token_attr m_attr;
};

inline bool token::isInteger() const {
  return m_name >= tok_binary_integer &&
         m_name <= tok_hexadecimal_integer;
}

inline bool token::isFloatingPoint() const {
  return m_name == tok_floating_point;
}

inline symbol token::getIdentifier() const {
  assert(m_name == tok_identifier);
  return m_attr.sym;
}

inline relational_op token::getRelationalOperator() const {
  assert(m_name == tok_relational_op);
  return m_attr.rel_op;
}

inline arithmetic_op token::getRelationalOperator() const {
  assert(m_name == tok_arithmetic_op);
  return m_attr.arith_op;
}

inline bitwise_op token::getBitwiseOperator() const {
  assert(m_name == tok_bitwise_op);
  return m_attr.bit_op;
}

inline logical_op token::getLogicalOperator() const {
  assert(m_name == tok_logical_op);
  return m_attr.log_op;
}

inline long long token::getInteger() const {
  assert(isInteger());
  return m_attr.int_val.value;
}

inline radix token::getRadix() const {
  assert(isInteger());
  return m_attr.int_val.radix;
}

inlint double token::getFloatingPoint() const {
  assert(m_name == tok_floating_point);
  return m_attr.float_val;
}

inline bool token::getBoolean() const {
  assert(m_name == tok_boolean);
  return m_attr.tf_val;
}

inline char token::getChar() const {
  assert(m_name == tok_character);
  return m_name.char_val;
}

inline const std::string& token::getString() const {
  assert(m_name == tok_string);
  return *m_attr.str_val.sym;
}

inline type_spec token::getTypeSpecifier() const {
  assert(m_name == tok_type_specifier);
  return m_attr.ts;
}

std::ostream& operator<<(std::ostream& os, token tok);
