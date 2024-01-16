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

#include "srsran/support/file_vector.h"

namespace srsran {

/// \brief Binary file input interface for data with multiple dimensions.
///
/// Allows reading values of type \c T with \c NDIMS dimensions from a binary file.
///
/// \tparam NDIMS      Number of dimensions of the data.
/// \tparam T          Data type.
/// \tparam Index_type Data type used for representing dimension indexes.
template <unsigned NDIMS, typename T, typename Index_type = unsigned>
class file_tensor
{
private:
  const char*                 file_name;
  std::array<unsigned, NDIMS> dimension_sizes;
  openmode                    op;

public:
  /// \brief Constructor.
  ///
  /// Sets the name and the mode (default is \e read-only) of the binary file, as well as the data dimensions. No
  /// operations are performed on the file.
  ///
  /// \param[in] filename  Input file name.
  /// \param[in] dim_sizes Array describing the data dimensions.
  /// \param[in] op_       File opening mode.
  file_tensor(const char* filename, const std::array<unsigned, NDIMS> dim_sizes, openmode op_ = openmode::read_only) :
    file_name(filename), dimension_sizes(dim_sizes), op(op_)
  {
  }

  /// Returns the file name.
  const char* get_file_name() const { return file_name; }

  /// \brief Reads the file.
  ///
  /// The contents of the file are dumped to a \c dynamic_tensor using the dimensions set during construction. The file
  /// is closed when the operation is completed. \warning An exception arises if the file cannot be opened or if the
  /// write-only open mode was specified at creation.
  dynamic_tensor<NDIMS, T, Index_type> read() const
  {
    report_fatal_error_if_not(op != openmode::write_only, "The file_vector was created with write-only permissions.");
    std::ifstream binary_file(file_name, std::ios::in | std::ios::binary);

    report_fatal_error_if_not(binary_file.is_open(), "Error opening file '{}'. {}.", file_name, strerror(errno));

    dynamic_tensor<NDIMS, T, Index_type> read_data(dimension_sizes);
    span<T>                              flat_tensor_view(read_data.get_data());

    T read_value;
    while (binary_file.read(reinterpret_cast<char*>(&read_value), sizeof(T))) {
      flat_tensor_view.front() = read_value;
      flat_tensor_view         = flat_tensor_view.last(flat_tensor_view.size() - 1);
    }

    report_fatal_error_if_not(flat_tensor_view.size() == 0, "Not all tensor elements have been filled.");

    return read_data;
  }
};

} // namespace srsran
