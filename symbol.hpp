#include <string>
#include <unordered_set>

using symbol = const std::string*;


class symbol_table {
  std::unordered_set<std::string> m_syms;

public:
  symbol get(const char* str);
  symbol get(const std::string& str);

};

inline symbol symbol_table::get(const char* str) {
  return &*m_syms.insert(str).first;
}

inline symbol symbol_table::get(const std::string& str) {
  return &*m_syms.insert(str).first;
}
