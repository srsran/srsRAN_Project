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

#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_MATCHER_IMPL_H_
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_MATCHER_IMPL_H_

#include "srsgnb/phy/channel_coding/polar_code.h"
#include "srsgnb/phy/channel_coding/polar_rate_matcher.h"

namespace srsgnb {
class polar_rate_matcher_impl : public polar_rate_matcher
{
private:
  std::array<uint8_t, polar_code::EMAX> y_e;

public:
  polar_rate_matcher_impl()           = default;
  ~polar_rate_matcher_impl() override = default;

  void rate_match(span<const uint8_t> input, span<uint8_t> output, const polar_code& code) override;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_MATCHER_IMPL_H_
