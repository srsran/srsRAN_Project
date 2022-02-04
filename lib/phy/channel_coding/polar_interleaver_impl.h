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

#ifndef SRSGNB_LIB_PHY_CHANNEL_CODING_POLAR_INTERLEAVER_IMPL_H_
#define SRSGNB_LIB_PHY_CHANNEL_CODING_POLAR_INTERLEAVER_IMPL_H_

#include "srsgnb/phy/channel_coding/polar_interleaver.h"

namespace srsgnb {
class polar_interleaver_impl : public polar_interleaver
{
private:
  static const unsigned                      K_MAX_IL = 164;
  static const std::array<uint8_t, K_MAX_IL> pattern;

public:
  ~polar_interleaver_impl() override = default;
  void interleave(bit_buffer in, bit_buffer out, bool dir) override;
};
} // namespace srsgnb
#endif // SRSGNB_LIB_PHY_CHANNEL_CODING_POLAR_INTERLEAVER_IMPL_H_
