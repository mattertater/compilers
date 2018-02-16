// Main file
//
// Reads input file and looks for valid tokens.
// Prints them out along with some helpful information

#include <iostream>
#include <fstream>
#include <string>
#include <fstream>

using namespace std;

int main() {
  ifstream source_file("test.mc");
  string source;

  while (!source_file.eof())
    source += source_file.get();

  lexer lex(source);

  while (!lex.eof());

}
