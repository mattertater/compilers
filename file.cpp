#include "file.hpp"

#include <iterator>
#include <fstream>

file::file(const std::string& path) : m_path(path) {

  std::ifstream ifs(m_path);

  std::istreambuf_iterator<char> first{ifs};
  std::istreambuf_iterator<char> last{};
  
  m_text = std::string(first, last);

}
