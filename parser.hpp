//
// The parser transforms a sequence of tokens to determine
// its gramatical structure with respect to the given grammar
//

#include "lexer.hpp"

class parser {
public:
  parser(symbol_table& syms, const file& f);

  void parseType();

  void parseExpression();
  void parseAssignmentExpression();
  void parseConditionalExpression();
  void parseLogicalOrExpression();
  void parseLogicalAndExpression();
  void parseBitwiseOrExpression();
  void parseBitwiseAndExpression();
  void parseEqualityExpression();
  void parseRelationalExpression();
  void parseShiftExpression();
  void parseAdditiveExpression();
  void parseMultiplicativeExpression();
  void parseCastExpression();
  void parseUnaryExpression();
  void parsePostfixExpression();
  void parsePrimaryExpression();

  void parseStatement();

  void parseDeclaration();
  void parseLocalDeclaration();

  void parseObjectDefinition();
  void parseVariableDefinition();
  void parseConstantDefinition();
  void parseValueDefinition();
  void parseFunctionDefinition();

  void parseDeclarationSequence();

  void parseProgram();

private:
  token_name lookahead();
  token_name lookahead(int n);

  token match(token_name n);
  token matchIf(token_name n);
  token matchIfEquality();
  token matchIfRelational();
  token matchIfShift();
  token matchIfAdditive();
  token matchIfMultiplicative();

  token accept();
  token peek();
  void fetch();

  lexer m_lex;

  std::deque<token> m_tok;
};

inline parser::parser(symbol_table& syms, const file& f) : m_lex(syms, f) m_tok() {
  fetch();
}
