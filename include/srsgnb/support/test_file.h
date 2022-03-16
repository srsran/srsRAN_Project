#ifndef SRSGNB_TEST_FILE_H
#define SRSGNB_TEST_FILE_H

#include "srsgnb_test.h"

#ifdef __cplusplus

#include <cstdio>

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
  std::vector<T>& read() const;
};

// specialized read for std::array<uint8_t,pbch_modulator::M_bit>
template<> std::vector<uint8_t>& file_vector<uint8_t>::read() const {
  std::ifstream binary_file(file_name, std::ios::in | std::ios::binary);
  std::vector<uint8_t>* read_data = {};
  uint8_t read_value;
  while(!binary_file.eof()) {
      binary_file.read(reinterpret_cast<char*>(&read_value),sizeof(uint8_t));
      read_data->push_back(read_value);
  }
  binary_file.close();

  return *read_data;
}
  
// specialized read for std::vector<resource_grid_spy::entry_t>
template<> std::vector<resource_grid_spy::entry_t>& file_vector<resource_grid_spy::entry_t>::read() const {
  std::ifstream binary_file(file_name, std::ios::in | std::ios::binary);
  std::vector<resource_grid_spy::entry_t>* read_data = {};
  resource_grid_spy::entry_t read_entry;
  float real_val = 0;
  float imag_val = 0;
   
  while(!binary_file.eof()) {
      // read the complex float values
      binary_file.read(reinterpret_cast<char*>(&real_val),sizeof(float));
      binary_file.read(reinterpret_cast<char*>(&imag_val),sizeof(float));
      read_entry.value = real_val + imag_val*1i;
      // read the indices
      binary_file.read(reinterpret_cast<char*>(&read_entry.port),sizeof(int));
      binary_file.read(reinterpret_cast<char*>(&read_entry.l),sizeof(int));
      binary_file.read(reinterpret_cast<char*>(&read_entry.k),sizeof(int));
      // add the read entry to the return vector
      read_data->push_back(read_entry);
  }
  binary_file.close();
  
  return *read_data;
}

} // namespace srsgnb

#endif // __cplusplus

#endif // SRSGNB_TEST_FILE_H
