#ifndef SRSGNB_SUPPORT_FILE_VECTOR_H
#define SRSGNB_SUPPORT_FILE_VECTOR_H

#include <fstream>

namespace srsgnb {

template <typename T>
class file_vector {
private:
    std::string file_name;
public:
  // Constructs object. It does not perform any file operation.
  file_vector(std::string filename) {
      file_name = filename;
  }
  
  // Open the file and appends in a \c std::vector<T> all the file entries.
  std::vector<T> read() const {
    std::ifstream binary_file(file_name, std::ios::in | std::ios::binary);
    std::vector<T> read_data = {};

    while(!binary_file.eof()) {
        T read_value;
        binary_file.read(reinterpret_cast<char*>(&read_value),sizeof(T));
        read_data.push_back(read_value);
    }
    // remove last repeated entry
    read_data.pop_back();
    binary_file.close();

    return read_data;
  }
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_FILE_VECTOR_H
