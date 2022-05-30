/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_INTERLEAVER_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_INTERLEAVER_IMPL_H

#include "srsgnb/phy/upper/channel_coding/polar/polar_interleaver.h"

namespace srsgnb {
class polar_interleaver_impl : public polar_interleaver
{
private:
  static const unsigned                      K_MAX_IL = 164;
  static const std::array<uint8_t, K_MAX_IL> pattern;

public:
  ~polar_interleaver_impl() override = default;
  void interleave(span<uint8_t> out, span<const uint8_t> in, polar_interleaver_direction direction) override;
};
} // namespace srsgnb
#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_INTERLEAVER_IMPL_H
