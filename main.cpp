//
// Main file
//
// Reads input file and looks for valid tokens.
// Prints them out along with some helpful information
//


#include <iostream>

#include "lexer.hpp"
#include "file.hpp"

int main() {
  file source_file("test.mc");

  symbol_table syms;

  // lexer lex(syms, source_file);
  // while (token tok = lex())
  //   std::cout << tok << '\n';

  parser p(syms, input);
  p.parse_declaration();

}
