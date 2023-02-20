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
