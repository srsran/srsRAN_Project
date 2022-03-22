#ifndef SRSGNB_SUPPORT_FILE_VECTOR_H
#define SRSGNB_SUPPORT_FILE_VECTOR_H

#include <fstream>
#include <vector>

namespace srsgnb {

template <typename T>
class file_vector
{
private:
  std::string file_name;

public:
  // Constructs object. It does not perform any file operation.
  file_vector(std::string filename) : file_name(std::move(filename)) {}

  // Open the file and appends in a \c std::vector<T> all the file entries.
  std::vector<T> read() const
  {
    std::ifstream binary_file(file_name, std::ios::in | std::ios::binary);
    if (!binary_file.is_open())
      return {};
    std::vector<T> read_data;

    T read_value;
    while (binary_file.read(reinterpret_cast<char*>(&read_value), sizeof(T))) {
      read_data.push_back(read_value);
    }

    return read_data;
  }
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_FILE_VECTOR_H
