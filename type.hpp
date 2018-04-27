#include <vector>

class type {
private:
  kind m_kind;

public:
  enum kind {
    bool_kind,
    char_kind,
    int_kind,
    float_kind,
    ptr_kind,
    ref_kind,
    func_kind
  };

  virtual ~type() = default;

  kind getKind() const { return m_kind; }
  bool isInt() const { return m_kind == int_kind; }
  bool isBool() const { return m_kind == book_kind; }
  bool isFloat() const { return m_kind == float_kind; }
  bool isChar() const { return m_kind == char_kind; }
  bool isReference() const { return m_kind == ref_kind; }
  bool isReferenceTo(const type *t);
  bool isPointer() const { return m_kind == ptr_kind; }
  bool isPointerTo(const type *t);
  bool isFunction() const { return m_kind == func_kind; }
  bool isObject() const { return !isReference(); }
  bool isArithmetic() const;
  bool isScalar() const;
  bool isNumeric() const {return (m_kind == int_kind) ||
                                 (m_kind == float_kind)};

  type *getObjectType() const;

protected:
  type(kind k) : m_kind(k) {}
};

using type_list = std::vector<type *>;

struct bool_type : type {
  bool_type() : type(bool_kind) {}
};

struct char_type : type {
  char_type() : type(char_kind) {}
};

struct int_type : type {
  int_type() : type(int_kind) {}
};

struct float_type : type {
  float_type() : type(float_kind) {}
};

struct ptr_type : type {
  ptr_type(type *t) : type(ptr_kind), m_elem(t) {}
  type *getElementType() const { return m_elem; }
  type *m_elem;
};

struct ref_type : type {
  ref_type(type *t) : type(ref_kind), m_elem(t) {}
  type *getObjectType() const { return m_elem; }
  type *m_elem;
};

struct func_type : type {
  func_type(const type_list &ps, type *ret)
      : type(func_kind), m_parms(ps), m_ret(ret) {}

  const type_list &getParameterTypes() const { return m_parms; }
  type_list &getParameterTypes() { return m_parms; }
  type *getReturnType() const { return m_ret; }

  type_list m_parms;
  type *m_ret;
};
