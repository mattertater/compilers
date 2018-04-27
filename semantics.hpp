#include "token.hpp"

class type;
class expression;
class statement;
class declaration;
class func_decl;

using type_list = std::vector<type *>;
using expr_list = std::vector<expression *>;
using stmt_list = std::vector<statement *>;
using decl_list = std::vector<declaration *>;

class scope;

class semantics {
private:
  scope *m_scope;

  func_decl *m_func;

  type *m_bool;
  type *m_char;
  type *m_int;
  type *m_float;

public:
  semantics();
  ~semantics();

  type *onBasicType(token tok);

  expression *onAssignmentExpression(expression *e1, expression *e2);
  expression *onConditionalExpression(expression)
};
