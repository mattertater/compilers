#include "type.hpp"

#include <iostream>

bool type::isReferenceTo(const type *t) {
  if (const ref_type *rt = dynamic_cast<const ref_type *>(this))
    return isSameAs(rt->getObjectType(), t);
  return false;
}

bool type::isPointerTo(const type *t) {
  if (const ptr_type *rt = dynamic_cast<const ptr_type *>(this))
    return isSameAs(rt->getElementType(), t);
  return false;
}

bool type::isNumeric() const {
  switch (m_kind) {
  case bool_kind:
  case char_kind:
  case int_kind:
  case float_kind:
    return true;
  default:
    return false;
  }
}

type *type::getObjectType() const {
  if (const ref_type *rt = dynamic_cast<const ref_type *>(this))
    return rt->getObjectType();
  return const_cast<type *>(this);
}

static bool isSameAsPtr(const ptr_type *t1, const ptr_type *t2) {
  return isSameAs(t1->getElementType(), t2->getElementType());
}

static bool isSameAsRef(const ref_type *t1, const ref_type *t2) {
  return isSameAs(t1->getObjectType(), t2->getObjectType());
}

static bool isSameAsFunc(const func_type *t1, const func_type *t2) {
  auto cmp = [](const type *a, const type *b) { return isSameAs(a, b); };
  const type_list &p1 = t1->getParameterTypes();
  const type_list &p2 = t2->getParameterTypes();
  return std::equal(p1.begin(), p1.end(), p2.begin(), p2.end(), cmp);
}

bool isSameAs(const type *t1, const type *t2) {
  if (t1 == t2)
    return true;

  if (t1->getKind() != t2->getKind())
    return false;

  switch (t1->getKind()) {
  case type::bool_kind:
  case type::char_kind:
  case type::int_kind:
  case type::float_kind:
    return true;
  case type::ptr_kind:
    return isSameAsPtr(static_cast<const ptr_type *>(t1),
                       static_cast<const ptr_type *>(t2));
  case type::ref_kind:
    return isSameAsRef(static_cast<const ref_type *>(t1),
                       static_cast<const ref_type *>(t2));
  case type::ptr_func:
    return isSameAsFunc(static_cast<const func_type *>(t1),
                        static_cast<const func_type *>(t2));
  }
}
