#include "declaration.hpp"
#include "type.hpp"

#include <iostream>

bool obj_decl::isReference() const { return getType()->isReference(); }

func_type *func_decl::getType() const {
  return static_cast<func_type *>(m_type);
}

type *func_decl::getREturnType() const { return getType()->getReturnType(); }
