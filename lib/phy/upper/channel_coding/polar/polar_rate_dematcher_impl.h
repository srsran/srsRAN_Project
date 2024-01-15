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

/// \file
/// \brief Polar code rate dematcher declaration.

#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include "srsran/phy/upper/channel_coding/polar/polar_rate_dematcher.h"

namespace srsran {

/// \brief Polar code rate dematching implementation.
///
/// It reverts the rate matching procedure described in TS38.212 Section 5.4.1.
class polar_rate_dematcher_impl : public polar_rate_dematcher
{
private:
  /// Internal buffer that contains both codeblock and rate-matched codeblock.
  std::array<log_likelihood_ratio, polar_code::NMAX + polar_code::EMAX> y_e;
  /// Pointer to the rate-matched part of the buffer.
  log_likelihood_ratio* e = &y_e[polar_code::NMAX];

public:
  /// Default constructor.
  polar_rate_dematcher_impl() = default;
  /// Default destructor.
  ~polar_rate_dematcher_impl() override = default;

  // See interface for the documentation.
  void rate_dematch(span<log_likelihood_ratio>       output,
                    span<const log_likelihood_ratio> input,
                    const polar_code&                code) override;
};
} // namespace srsran
