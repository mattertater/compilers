//
// Implementations of classes for lexer
//

#include "lexer.hpp"
#include "file.hpp"

#include <sstream>
#include <iostream>
#include <cassert>
#include <cctype>

// -------------------------------------
// Character classes
// -------------------------------------
static bool is_digit(char c) {
  return std::isdigit(c);
}

static bool is_nondigit(char c) {
  return std::isalpha(c) || c == '_';
}

static bool is_alphanumeric(char c) {
  return std::isalnum(c);
}

static bool is_binary_digit(char c) {
  return c == '0' || c == '1';
}

static bool is_hexadecimal_digit(char c) {
  return std::isxdigit(c);
}

static bool is_space(char c) {
  return c == ' ' || c == '\t';
}

static bool is_newline(char c) {
  return c == '\n';
}


// -------------------------------------
// Lexer classes
// -------------------------------------
static const char* getStartOfInput(std::string& f) {
  return f.getText().data();
}


static const char* getEndOfInput(std::string& f) {
  return f.getText().data() + f.getText().size();
}

lexer::lexer(symbol_table& syms, const file& f) : symbols(syms),
                                                  m_first(getStartOfInput(f)),
                                                  m_last(getEndOfInput(f)),
                                                  m_current_loc(f, 0, 0) {
  m_reserved.insert({

    // Keywords, same order as in token.hpp
    { m_syms.get("and"),    logical_and },
    { m_syms.get("bool"),   ts_bool },
    { m_syms.get("char"),   ts_char },
    { m_syms.get("def"),    kw_def  },
    { m_syms.get("else"),   kw_else },
    { m_syms.get("false"),  false },
    { m_syms.get("float"),  ts_float},
    { m_syms.get("if"),     kw_if   },
    { m_syms.get("int"),    ts_int  },
    { m_syms.get("let"),    kw_let  },
    { m_syms.get("not"),    logical_not },
    { m_syms.get("or"),     logical_or  },
    { m_syms.get("true"),   true  },
    { m_syms.get("var"),    kw_var  },
  });
}



bool lexer::eof() const {
  return m_first == m_last;
}



char lexer::peek() const {
  return eof() ? 0 : *m_first;
}



char lexer::peek(int n) const {
  if (n < m_last - m_first)
    return *(m_first + n);
  else
    return 0;
}



char lexer::accept() {
  assert(*m_first != '\n');
  char c = *m_first;
  ++m_first;
  ++m_current_loc.column;
  return c;
}



void lexer::accept(int n) {
  // FIXME: This is slow, but we do it for the assertions.
  while (n) {
    accept();
    --n;
  }
}




token lexer::scan() {
  while (!eof()) {

    m_token_loc = m_current_loc;

    switch (*m_first) {
      // Ignore whitespace
      case ' ':
      case '\t':  skip_space();   continue;
      case '\n':  skip_newline(); continue;
      case '#':   skip_comment(); continue;

      // Punctuators
      case '(':   return lex_punctuator(tok_left_paren);
      case ')':   return lex_punctuator(tok_right_paren);
      case '[':   return lex_punctuator(tok_left_bracket);
      case ']':   return lex_punctuator(tok_right_bracket);
      case '{':   return lex_punctuator(tok_left_brace);
      case '}':   return lex_punctuator(tok_right_brace);
      case ',':   return lex_punctuator(tok_comma);
      case ';':   return lex_punctuator(tok_semicolon);
      case ':':   return lex_punctuator(tok_colon);

      // Operators
      case '<':   if (peek(1) == '=') return lex_relational_op(2, op_le);
                  if (peek(1) == '<') return lex_bitwise_op(2, op_shl);
                  return lex_relational_op(1, op_lt);

      case '>':   if (peek(1) == '=') return lex_relational_op(2, op_ge);
                  if (peek(1) == '>') return lex_bitwise_op(2, op_shr);
                  return lex_relational_op(1, op_gt);

      case '=':   if (peek(1) == '=') return lex_relational_op(2, op_eq);
                  else return lex_assignment_op();

      case '+':   return lex_arithmetic_op(op_add);
      case '-':   return lex_arithmetic_op(op_sub);
      case '*':   return lex_arithmetic_op(op_mul);
      case '/':   return lex_arithmetic_op(op_quo);
      case '%':   return lex_arithmetic_op(op_rem);
      case '&':   return lex_bitwise_op(1, op_and);
      case '|':   return lex_bitwise_op(1, op_ior);
      case '^':   return lex_bitwise_op(1, op_xor);
      case '~':   return lex_bitwise_op(1, op_not);
      case '?':   return lex_conditional_op();
      case '\'':  return lex_character();
      case '"':   return lex_string();

      default: {
        if (is_nondigit(*m_first))
          return lex_word();
        else if (is_digit(*m_first))
          return lex_number();

        std::stringstream ss;
        ss << "invalid character '" << *m_first << '\'';
        throw std::runtime_error(ss.str());
      }
    }
  }
  return {};
}


// Whitespace skipping helper functions
void lexer::skip_space() {
  assert(is_space(*m_first));
  ignore();
  while (!eof() && is_space(*m_first))
    ignore();
}

void lexer::skip_newline() {
  assert(*m_first == '\n');
  m_current_loc.line += 1;
  m_current_loc.column = 0;
  ++m_first;
}

void lexer::skip_comment() {
  assert(*m_first == '#');
  ignore();
  while (!eof() && !is_newline(*m_first))
    ignore();
}



token lexer::lex_punctuator(token_name n) {
  accept();
  return { n, m_token_loc };
}



token lexer::lex_relational_op(int len, relational_op op) {
  accept(len);
  return { op, m_token_loc };
}



token lexer::lex_arithmetic_op(arithmetic_op op) {
  accept();
  return { op, m_token_loc };
}



token lexer::lex_bitwise_op(int len, bitwise_op op) {
  accept(len);
  return { op, m_token_loc };
}



token lexer::lex_assignment_op() {
  accept();
  return { tok_assignment_op, m_token_loc };
}



token lexer::lex_conditional_op() {
  accept();
  return { tok_conditional_op, m_token_loc };
}




token lexer::lex_word() {
  assert(is_nondigit(*m_first));
  const char* start = m_first;

  accept();
  while (!eof() && is_alphanumeric(*m_first))
    accept();

  std::string str(start, m_first);
  symbol sym = m_syms.get(str);
  auto iter = m_reserved.find(sym);
  if (iter != m_reserved.end()) {
    const token& tok = iter->second;
    return {tok.getName(), tok.get_attribute(), m_token_loc};
  }
  else
    return {sym, m_token_loc};
}




token lexer::lex_number() {
  assert(isdigit(*m_first));
  const char* start = m_first;

  if (*m_first == '0') {
    char pre = peek(1);
    switch (pre) {
      case 'b': case 'B': return lex_binary_number();
      case 'x': case 'X': return lex_hexadecimal_number();
      default: break;
    }
  }

  accept();
  while (!eof() && is_digit(*m_first))
    accept();

  if (peek() != '.') {
    std::string str(start, m_first);
    return {decimal, std::atoi(str.c_str()), m_token_loc};
  }

  accept();
  while (!eof() && is_digit(*m_first))
    accept();


  std::string str(start, m_first);
  return {std::atof(str.c_str()), m_token_loc};
}




token lexer::lex_binary_number() {
  accept(2);

  const char* start = m_first;
  while (!eof() && is_binary_digit(*m_first))
    accept();

  std::string str(start, m_first);
  return {binary, std::strtoll(str.c_str(), nullptr, 2), m_token_loc};
}




token lexer::lex_hexadecimal_number() {
  accept(2);

  const char* start = m_first;
  while (!eof() && is_hexadecimal_digit(*m_first))
    accept();

  std::string str(start, m_first);
  return {hexadecimal, std::strtoll(str.c_str(), nullptr, 16), m_token_loc};
}


static bool is_character_character(char c){
  return c != '\n' && c != '\'';
}


char lexer::scan_escape_sequence() {
  assert(*m_first == '\\');
  accept();
  if (eof())
    throw std::runtime_error("unterminated escape-sequence");
  switch (accept()) {
  case '\'':  return '\'';
  case '\"':  return '\"';
  case '\\':  return '\\';
  case 'a':   return '\a';
  case 'b':   return '\b';
  case 'f':   return '\f';
  case 'n':   return '\n';
  case 'r':   return '\r';
  case 't':   return '\t';
  case 'v':   return '\v';
  default:    throw std::runtime_error("invalid escape-sequence");
  }
}




token lexer::lex_character() {
  assert(*m_first == '\'');
  accept();

  if (eof())
    throw std::runtime_error("unterminated character literal");

  char c;
  if (*m_first == '\\')
    c = scan_escape_sequence();
  else if (is_character_character(*m_first))
    c = accept();
  else if (*m_first == '\'')
    throw std::runtime_error("invalid character literal");
  else if (*m_first == '\n')
    throw std::runtime_error("invalid multi-line character");
  else
    throw std::logic_error("unexpected character");

  if (*m_first != '\'')
    throw std::runtime_error("invalid multi-byte character");
  accept();

  return {c, m_token_loc};
}

static bool is_string_character(char c) {
  return c != '\n' && c != '"';
}




token lexer::lex_string() {
  assert(*m_first == '"');
  accept();

  if (eof())
    throw std::runtime_error("unterminated character literal");

  std::string str;
  str.reserve(32);
  while (*m_first != '"') {
    char c;
    if (*m_first == '\\')
      c = scan_escape_sequence();
    else if (is_string_character(*m_first))
      c = accept();
    else if (*m_first == '\n')
      throw std::runtime_error("invalid multi-line string");
    str += c;
  }
  accept();

  return {string_attr{m_syms.get(str)}, m_token_loc};
}
