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

#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include "srsran/phy/upper/channel_coding/polar/polar_rate_matcher.h"

namespace srsran {
class polar_rate_matcher_impl : public polar_rate_matcher
{
private:
  std::array<uint8_t, polar_code::EMAX> y_e;

public:
  polar_rate_matcher_impl()           = default;
  ~polar_rate_matcher_impl() override = default;

  void rate_match(span<uint8_t> output, span<const uint8_t> input, const polar_code& code) override;
};
} // namespace srsran
