#include "symbol.hpp"

#include <cassert>
#include <unordered_map>

class declaration;

struct scope : std::unordered_map<symbol, decl *> {
  scope(scope *p = nullptr) : parent(p) {}

  virtual ~scope() = default;

  declaration *lookup(symbol sym) const {
    auto iter = find(sym);
    return iter == end() ? nullptr : iter->second;
  }

  void declare(symbol sym, declaration *d) {
    assert(count(sym) == 0);
    emplace(sym, d);
  }

  scope *parent;
};

struct global_scope : scope {
  using scope::scope;
};

struct parameter_scope : scope {
  using scope::scope;
};

struct block_scope : scope {
  using scope::scope;
}
