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

/// \file
/// \brief LDPC encoder interface.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

class ldpc_encoder_buffer;

/// LDPC encoder interface.
class ldpc_encoder
{
public:
  /// Default destructor.
  virtual ~ldpc_encoder() = default;

  /// \brief Encodes a message.
  ///
  /// \param[in]  input   Message: original information bits, with the filler bits (if any) set to zero.
  /// \param[in]  cfg     Encoder configuration for the current codeblock.
  /// \return A reference to the LDPC encoder buffer.
  /// \note The length of the output codeblock is deduced from the size of parameter \c output.
  virtual const ldpc_encoder_buffer& encode(const bit_buffer&                             input,
                                            const codeblock_metadata::tb_common_metadata& cfg) = 0;
};

} // namespace srsran
