#include <string>

class file {
  std::string m_path;
  std::string m_text;
  
public:
  file(const std::string& path);

  const std::string& getPath() const;
  const std::string& getText() const;

};

inline const std::string& file::getPath() const {
  return m_path;
}

inline const std::string& file::getText() const {
  return m_text;
}
