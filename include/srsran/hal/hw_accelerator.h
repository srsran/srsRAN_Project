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

/// \file
/// \brief Definition of an abstract interface to a hardware accelerator.
#pragma once

#include "srsran/adt/span.h"

namespace srsran {
namespace hal {

/// \brief This interface provides the basic functions to enqueue and dequeue an operation to hardware accelerator.
/// \tparam T Type of the input data, and of the packed codeword in the case of encoders or soft-data in case of
///           decoders, to the hardware accelerator. \tparam U Type of the output data to the hardware accelerator.
template <typename T, typename U>
/// Generic hardware accelerator interface definition.
class hw_accelerator
{
public:
  /// Default destructor.
  virtual ~hw_accelerator() = default;

  /// \brief Enqueues an accelerated operation and provides the input data.
  /// \param[in] data      The input data to the hardware accelerator.
  /// \param[in] aux_data  Optional soft combining data typically used by hardware-accelerated decoding operations.
  /// \param[in] cb_index  Optional index of the CB within the TB.
  /// \return True if the operation was successfully enqueued, false otherwise.
  virtual bool enqueue_operation(span<const T> data, span<const T> aux_data = {}, unsigned cb_index = 0) = 0;

  /// \brief Dequeues an accelerated operation and retrieves both the output data, while using a soft-buffer.
  /// \param[out] data          The output data from the hardware accelerator.
  /// \param[out] aux_data      Soft combining data for hardware-accelerated decoding operations, packed codeword for
  ///                           hardware-accelerated encoding operations.
  /// \param[in]  segment_index Optional index of the segment within the TB.
  /// \return True if the operation was successfully dequeued, false otherwise.
  virtual bool dequeue_operation(span<U> data, span<T> aux_data = {}, unsigned segment_index = 0) = 0;
};

} // namespace hal
} // namespace srsran
