//
// The lexer transforms input characters into distinct tokens
// All whitespace is ignored and is not meaningful to the output
//

#include "token.hpp"

#include <unordered_map>

class file;

class lexer {

public:
  lexer(symbol_table& syms, const file& f);

  token operator()() { return scan(); }

  token scan();

  // If at the end of the file or not
  bool eof() const;

  // Either peek 1 character with no params, or peek n chars
  char peek() const;
  char peek(int n) const;


private:
  // Either accept 1 character with no params, or accept n chars
  char accept();
  char accept(int n);


  char ignore();

  // Make it easy to skip past certain types of whitespace
  void skip_space();
  void skip_newline();
  void skip_comment();

  // Fucntions that return a token based on what is accepted
  token lex_punctuator(token n);
  token lex_relational_op(int len, relational_op op);
  token lex_arithmetic_op(arithmetic_op op);
  token lex_bitwise_op(int len, bitwise_op op);
  token lex_conditional_op();
  token lex_assignment_op();
  token lex_word();
  token lex_number();
  token lex_binary_number();
  token lex_hexadecimal_number();
  token lex_character();
  token lex_string();

  // gives back the unescaped character rather than a token
  char scan_escape_sequence();

  // Keeps track of the current position
  const char* m_first;
  const char* m_last;

  std::unordered_map<symbol, token> m_res_words;
};
