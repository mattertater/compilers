#include "token.hpp"

#include <vector>

class type;
class declaration;

class expression {
private:
  kind m_kind;
  type *m_type;

public:
  enum kind {
    bool_kind,
    int_kind,
    float_kind,
    id_kind,
    uop_kind bop_kind ptr_kind,
    call_kind,
    index_kind,
    cast_kind,
    assign_kind,
    cond_kind,
    conv_kind
  };

  virtual ~expression() = default;

  kind getKind() const { return m_kind; }
  type *getType() const { return m_type; }
  type *getObjectType() const;

  bool hasType(const type *t) const;

  bool isBool() const;
  bool isInt() const;
  bool ifFloat() const;
  bool isFunction() const;
  bool isArithmetic() const;
  bool isNumeric() const;
  bool isScalar() const;

protected:
  expression(kind k) : m_kind(k), m_type(t) {}
  expression(kind k, type *t) : m_kind(k), m_type(t) {}
};

using expr_list = std::vector<expression *>;

struct bool_expr : expression {
  bool_expr(type *t, bool b) : expression(bool_kind, t), val(b) {}
  bool getValue() const { return val; }
  bool val;
};

struct int_expr : expression {
  int_expr(type *t, int n) : expression(int_kind, t), val(n) {
    bool getValue() const { return val; }
    int val;
  }
};

struct float_expr : expression {
  float_expr(type *t double n) : expression(float_kind, t), val(n) {}
  bool getValue() const { return val; }
  double val;
};

struct id_expr : expression {
  id_expr(type *t, declaration *d) : expr(id_kind, t), ref(d) {}
  declaration *getDeclaration() const { return ref; }
  declaration *ref;
}

enum uop {
  uo_pos,
  uo_neg,
  uo_cmp,
  uo_not,
  uo_addr,
  uo_deref
};

struct uop_expr : expression {
  uop_expr(uop op, expr *e1) : expression(uop_kind), m_op(op), m_arg(e1) {}
  uop getOperator() const { return m_op; }
  expression *getOperand() const { return m_arg; }

  uop m_op;
  expression *m_arg;
};

enum bop {
  bo_add,
  bo_sub,
  bo_mul,
  bo_quo,
  bo_rem,
  bo_and,
  bo_or,
  bo_xor,
  bo_shl,
  bo_shr,
  bo_land,
  bo_lor,
  bo_eq,
  bo_neq,
  bo_lt,
  bo_gt,
  bo_lte,
  bo_gte
};

struct bop_expr : expression {
  bop_expr(type *t, bop op, expression *e1, expression *e2)
      : expression(bop_kind, t), m_op(op), m_lhs(e1), m_rhs(e2) {}

  bop getOperator() const { return m_op; }

  expression *getLHS() const { return m_lhs; }
  expression *getRHS() const { return m_rhs; }

  bop m_op;
  expression *m_lhs;
  expression *m_rhs;
};

struct postfirx_expr : expression {
  postfix_expr(kind k, type *t, expression *e, const expr_list &args)
      : expression(k), m_base(e), m_args(args) {}

  const expr_list &getArguments() const { return m_args; }
  expr_list &getArguments() { return m_args; }

  expression *m_base;
  expr_list m_args;
};

struct call_expr : postfix_expr {
  call_expr(type *t, expression *e, const expr_list &args)
      : postfix_expr(call_kind, t, e, args) {}

  expression *getCallee() const { return m_base; }
};

struct index_expr : postfix_expr {
  index_expr(type *t, expression *e, const expr_list &args)
      : postfix_expr(index_kind, t, e, args) {}
}

struct cast_expr : expression {
  cast_expr(expression *e, type *t)
      : expression(cast_kind, t), m_src(e), m_dst(t) {}

  expression *m_src;
  type *m_dst;
};

struct assign_expr : expression {
  assign_expr(type *t, expression *e1, expression *e2)
      : expression(assign_kind), m_lhs(e1), m_rhs(e2) {}

  expression *getLHS() const { return m_lhs; }
  expression *getRHS() const { return m_rhs; }

  expression *m_lhs;
  expression *m_rhs;
};

struct cond_expr : expression {
  cond_expr(type *t, expression *e1, expression *e2, expression *e3)
      : expression(cond_kind, t), m_cond(e1), m_true(e2), m_false(e3) {}

  expression *getCondition() const { return m_cond; }
  expression *getTrueValue() const { return m_cond; }
  expression *getFalseValue() const { return m_cond; }

  expression *m_cond;
  expression *m_true;
  expression *m_false;
};

enum conversion {
  conv_id,
  conv_val,
  conv_bool,
  conv_char,
  conv_int,
  conv_ext,
  conv_trunc
};

struct conv_expr : expression {
  conv_expr(expression *e1, conversion c, type *t)
      : expression(conv_kind, t), m_src(e1), m_conv(c) {}

  conversion getConversion() const { return m_conv; }
  expression *getSource() const { return m_src; }

  expr *m_src;
  conversion m_conv;
};
