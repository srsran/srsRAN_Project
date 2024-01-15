/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/span.h"
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
