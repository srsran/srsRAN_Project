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

enum class polar_interleaver_direction { tx, rx };

class polar_interleaver
{
public:
  virtual ~polar_interleaver() = default;

  /**
   * @brief Implements Polar code interleaver as described in TS 38.212 V15.9.0 Section 5.3.1.1
   *
   * @attention The input and output data cannot be the same.
   *
   * @param[in] in bit Input data
   * @param[out] out bit Output data
   * @param[in] dir Set to TX or RX for encoder or decoder
   */
  virtual void interleave(span<uint8_t> out, span<const uint8_t> in, polar_interleaver_direction direction) = 0;
};

} // namespace srsran
