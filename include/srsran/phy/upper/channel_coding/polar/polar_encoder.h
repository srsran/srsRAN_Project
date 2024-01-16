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
#include <cstdint>

namespace srsran {

class polar_encoder
{
public:
  virtual ~polar_encoder() = default;

  /*!
   * Encodes the input vector into a codeword with the specified polar encoder.
   * \param[in] input The encoder input vector.
   * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
   *     It cannot be larger than the maximum code_size_log specified in q.code_size_log of
   *     the srsran_polar_encoder_t structure.
   * \param[out] output The encoder output vector.
   */
  virtual void encode(span<uint8_t> output, span<const uint8_t> input, unsigned code_size_log) = 0;
};

} // namespace srsran
