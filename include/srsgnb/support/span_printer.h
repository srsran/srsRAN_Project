/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/span.h"
#include <fstream>

namespace srsran {

/// \brief Writes the given span into a file in binary format.
///
/// The file is opened in truncated mode and writen in binary format by default.
///
/// \tparam    T        Type of the data to write.
/// \param[in] filename Output file name (possibly, with path).
/// \param[in] data     Data to write.
/// \param[in] mode     Open mode for the file.
/// \return True on success, false otherwise.
/// \warning \c T must be standard layout.
template <typename T>
inline bool print(const char*             filename,
                  span<const T>           data,
                  std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::out)
{
  static_assert(std::is_standard_layout<T>::value, "T is not standard layout");
  std::ofstream file(filename, mode);

  if (!file.is_open()) {
    return false;
  }

  // Write in binary format.
  if (mode & std::ios_base::binary) {
    file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
    return true;
  }

  // Write in text format.
  for (const auto& d : data) {
    file << d << "\n";
  }

  return true;
}

} // namespace srsran
