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
