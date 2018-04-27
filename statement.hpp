#include <vector>

class expression;
class declaration;

class statement {
private:
  kind m_kind;

public:
  enum kind {
    block_kind,
    when_kind,
    if_kind,
    while_kind,
    break_kind,
    cont_kind,
    ret_kind,
    decl_kind,
    expr_kind
  };

  virtual ~statement() = default;
  kind getKind() const { return m_kind; }

protected:
  statement(kind k) : m_kind(k) {}
};

using statement_list = std::vector<statement *>;

struct block_stmt : statement {
  block_stmt(const stmt_list &ss) : statement(block_kind), m_stmts(ss) {}

  const stmt_list &getStatements() const { return m_stmts; }
  stmt_list &getStatements() { return m_stmts; }

  stmt_list m_stmts;
};

struct when_stmt : statement {
  when_stmt(expression *c, statement *s1)
      : statement(when_kind), m_cond(c), m_body(s1) {}

  expression *getCondition() const { return m_cond; }
  statement *getBody() const { return m_body; }

  expression *m_cond;
  statement *m_body;
};

struct if_stmt : statement {
  if_stmt(expression *c, statement *s1, statement *s2)
      : statement(if_kind), m_cond(c), m_true(s1), m_false(s2) {}

  expression *getCondition() const { return m_cond; }
  statement *getTrueBranch() const { return m_true; }
  statement *getFalseBranch() const { return m_false; }

  expression *m_cond;
  statement *m_true;
  statement *m_false;
};

struct while_stmt : statement {
  while_stmt(expression *c, statement *s1)
      : statement(while_kind), m_cond(c), m_body(s1) {}

  expression *getCondition() const { return m_cond; }
  statement *getBody() const { return m_body; }

  expression *m_cond;
  statememt *m_body;
};

struct break_stmt : statement {
  break_stmt() : statement(break_kind) {}
};

struct cont_stmt : statement {
  cont_stmt() : statement(break_kind) {}
};

struct ret_stmt : statement {
  ret_stmt(expression *e) : statement(ret_kind), m_val(e) {}
  expression *getValue() const { return m_val; }
  expression *m_val;
};

struct decl_stmt : statement {
  decl_stmt(declaration *d) : statement(decl_kind), m_decl(d) {}
  declaration *getDeclaration() const { return m_decl; }
  declaration *m_decl;
};

struct expr_stmt : statement {
  expr_stmt(expression *e) : statement(expr_kind), m_expr(e) {}
  expression *getExpression() const { return m_expr; }
  expression *m_expr;
};
