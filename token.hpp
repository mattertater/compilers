//
// Manage names, types, and properties of tokens
//

enum token_name {
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
