/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include <stdint.h>

namespace srsran {

/// \brief LDPC encoder output buffer interface.
///
/// This interface allows the LDPC encoder to provide the encoded bits to the rate matcher without using an intermediate
/// buffer.
class ldpc_encoder_buffer
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder_buffer() = default;

  /// Gets the encoded codeblock length.
  virtual unsigned get_codeblock_length() const = 0;

  /// \brief Reads the encoded codeblock data starting at bit index \c offset.
  ///
  /// The data size plus the offset shall not exceed the encoded codeblock length.
  ///
  /// \param[out] data   Read destination.
  /// \param[in]  offset Initial read position.
  virtual void write_codeblock(span<uint8_t> data, unsigned offset) const = 0;
};

} // namespace srsran