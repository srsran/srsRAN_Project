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

#include "srsran/phy/upper/channel_coding/polar/polar_interleaver.h"

namespace srsran {
class polar_interleaver_impl : public polar_interleaver
{
private:
  static const unsigned                      K_MAX_IL = 164;
  static const std::array<uint8_t, K_MAX_IL> pattern;

public:
  ~polar_interleaver_impl() override = default;
  void interleave(span<uint8_t> out, span<const uint8_t> in, polar_interleaver_direction direction) override;
};
} // namespace srsran
