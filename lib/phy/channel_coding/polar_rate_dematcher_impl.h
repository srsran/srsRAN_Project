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

#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_DEMATCHER_IMPL_H_
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_DEMATCHER_IMPL_H_

#include "srsgnb/phy/channel_coding/polar_code.h"
#include "srsgnb/phy/channel_coding/polar_rate_dematcher.h"

namespace srsgnb {
class polar_rate_dematcher_impl : public polar_rate_dematcher
{
private:
  std::array<int8_t, polar_code::NMAX + polar_code::EMAX> y_e;
  int8_t*                                                 e = &y_e[polar_code::NMAX];

public:
  polar_rate_dematcher_impl()           = default;
  ~polar_rate_dematcher_impl() override = default;

  void rate_dematch(span<const int8_t> input, span<int8_t> output, const polar_code& code, unsigned int ibil) override;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_RATE_DEMATCHER_IMPL_H_
