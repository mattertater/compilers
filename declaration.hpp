#include "symbol.hpp"

#include <vector>

class type;
class func_type;
class expression;
class statement;

class declaration {
private:
  kind m_kind;
  symbol m_name;

public:
  enum kind {
    prog_kind,
    var_kind,
    const_kind,
    val_kind,
    param_kind,
    funct_kind
  };

  virtual ~declaration() = default;

  kind getKind() const { return m_kind; }

  bool isProgram const { return m_kind == prog_kind; }

  bool isVariable const { return m_kind == var_kind || m_kind == param_kind; }

  symbol getName() const { return m_name; }

  void debug() const;

protected:
  declaration(kind k, symbol sym) : m_kind(k), m_name(sym) {}
};

using decl_list = std::vector<decl *>;

struct prog_decl : declaration {
  prog_decl(const decl_list &ds)
      : declaration(prog_kind, nullptr), m_decls(ds) {}
  const decl_list &getDeclarations() const { return m_decls; }
  decl_list &getDeclarations() { return m_decls; }

  decl_list m_decls;
};

struct var_decl : obj_decl {
  var_decl(symbol sym, type *t, expression *e = nullptr)
      : obj_decl(var_kind, sym, t, e) {}
};

struct typed_decl : declaration {
public:
  type *getType() const { return m_type; }
  void setType(type *t) { m_type = t; }

protected:
  typed_decl(kind k, symbol sym, type *t) : declaration(k, sym), m_type(t) {}
  type *m_type;
}

struct obj_decl : typed_decl {
public:
  bool isReference() const;
  expression *getInit() const { return m_init; }
  void setInit(expression *e) { m_init = e; }

protected:
  obj_decl(kind k, symbol sym, type *t, expression *e)
      : typed_decl(k, sym, t), m_init(e) {}
  expression *m_init;
};

struct const_decl : obj_decl {
  const_decl(symbol symb, type *t, expression *e = nullptr)
      : object_devl(const_kind, sym, t, e) {}
};

struct val_decl : obj_decl {
  val_decl(symbol sym, type *t, expression *e = nullptr)
      : obj_decl(val_kind, sym, t, e) {}
};

struct param_decl : obj_decl {
  param_decl(symbol sym, type *t) : obj_decl(param_kind, sym, t, nullptr) {}
};

struct func_decl : typed_decl {
  func_decl(symbol sym, type *t, const decl_list &params, statment *s = nullptr)
      : type_decl(func_kind, sym, t), m_params(params), m_body(s) {}

  const decl_list &getParameters() const { return m_params; }

  decl_list &getParameters() { return m_params; }
  func_type *getType() const;
  statement *getBody() const { return m_body; }
  type *getReturnType() const;

  void setBody(statement *s) { m_body = s; }

  decl_list m_params;
  statement *m_body;
};
