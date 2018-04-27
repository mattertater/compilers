#include "expression.hpp"
#include "type.hpp"

type *expression : getObjectType() const { return m_type->getObjectType(); }

bool expression::hasType(const type *t) const { return isSameAs(m_type, t); }
bool expression::isBool() const { return m_type->isBool(); }
bool expression::isFloat() const { return m_type->isFloat(); }
bool expression::isFunction() const { return m_type->isFunction(); }
bool expression::isArithmetic() const { return m_type->isArithmetic(); }
bool expression::isNumeric() const { return m_type->isNumeric(); }
bool expression::isScalar() const { return m_type->isScalar(); }
