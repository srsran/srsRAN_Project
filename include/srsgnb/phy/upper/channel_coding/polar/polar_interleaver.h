/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_INTERLEAVER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_INTERLEAVER_H

#include "srsgnb/adt/span.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

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

std::unique_ptr<polar_interleaver> create_polar_interleaver();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_INTERLEAVER_H
