//
// The parser transforms a sequence of tokens to determine
// its gramatical structure with respect to the given grammar
//

#include "lexer.hpp"
#include "semantics.hpp"

#include <deque>
#include <vector>

class type;
class expression;
class statement;
class declaration;

using type_list = std::vector<type *>;
using expr_list = std::vector<expression *>;
using stmt_list = std::vector<statement *>;
using decl_list = std::vector<declaration *>;

class parser {
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

  semantics m_act;

public:
  parser(symbol_table &syms, const file &f);

  void parseType();
  void *parseBasicType();

  expression *parseExpression();
  expression *parseAssignmentExpression();
  expression *parseConditionalExpression();
  expression *parseLogicalOrExpression();
  expression *parseLogicalAndExpression();
  expression *parseBitwiseOrExpression();
  expression *parseBitwiseAndExpression();
  expression *parseEqualityExpression();
  expression *parseRelationalExpression();
  expression *parseShiftExpression();
  expression *parseAdditiveExpression();
  expression *parseMultiplicativeExpression();
  expression *parseCastExpression();
  expression *parseUnaryExpression();
  expression *parsePostfixExpression();
  expression *parsePrimaryExpression();
  expr_list parseArgumentList();

  statement *parseStatement();
  statement *parseBlockStatement();
  statement *parseIfStatement();
  statement *parseWhileStatement();
  statement *parseBreakStatement();
  statement *parseContinueStatement();
  statement *parseReturnStatement();
  statement *parseDeclarationStatement();
  statement *parseExpressionStatement();
  stmt_list parseStatementSequence();

  declaration *parseDeclaration();
  declaration *parseLocalDeclaration();
  declaration *parseObjectDefinition();
  declaration *parseVariableDefinition();
  declaration *parseConstantDefinition();
  declaration *parseValueDefinition();
  declaration *parseFunctionDefinition();
  declaration *parseParameter();
  decl_list parseParameterList();
  decl_list parseParameterClause();
  decl_list parseDeclarationSequence();

  declaration *parseProgram();
};

inline parser::parser(symbol_table &syms, const file &f)
    : m_lex(syms, f) m_tok() {
  fetch();
}
