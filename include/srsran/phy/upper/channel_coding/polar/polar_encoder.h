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
/// \brief Polar encoder interface.

#pragma once

#include "srsran/adt/span.h"
#include <cstdint>

namespace srsran {

/// Polar encoder interface.
class polar_encoder
{
public:
  /// Default destructor.
  virtual ~polar_encoder() = default;

  /// \brief Encodes the input vector into a codeword with the specified polar encoder.
  /// \param[out] output       The encoder output vector.
  /// \param[in]  input        The encoder input vector.
  /// \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
  ///     It cannot be larger than the maximum \c code_size_log specified in \c q.code_size_log of
  ///     the \c srsran_polar_encoder_t structure.
  virtual void encode(span<uint8_t> output, span<const uint8_t> input, unsigned code_size_log) = 0;
};

} // namespace srsran
