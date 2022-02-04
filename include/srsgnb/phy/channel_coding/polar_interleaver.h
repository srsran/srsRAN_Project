/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_INTERLEAVER_H_
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_INTERLEAVER_H_

#include "srsgnb/adt/bit_buffer.h"
#include <memory>

namespace srsgnb {

class polar_interleaver
{
public:
  virtual ~polar_interleaver() = default;

  /**
   * @brief Implements Polar code interleaver as described in TS 38.212 V15.9.0 Section 5.3.1.1
   *
   * @attention The input and output data cannot be the same.
   *
   * @param in bit Input data
   * @param out bit Output data
   * @param K Number of elements
   * @param dir Set to true for encoder and false for decoder
   */
  virtual void interleave(bit_buffer in, bit_buffer out, bool dir) = 0;
};

std::unique_ptr<polar_interleaver> create_polar_interleaver();

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_INTERLEAVER_H_
