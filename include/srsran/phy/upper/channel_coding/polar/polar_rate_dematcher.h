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
/// \brief Polar code rate dematching.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include <cstdint>

namespace srsran {

/// Polar code rate dematching interface. It reverts the operations of polar_rate_matcher.
class polar_rate_dematcher
{
public:
  /// Default virtual destructor.
  virtual ~polar_rate_dematcher() = default;

  /// \brief Carries out the actual rate-dematching.
  /// \param[out] output    The rate-dematched codeword resulting from the rate-dematching
  ///                       operation.
  /// \param[in] input      The LLRs obtained from the channel samples that correspond to
  ///                       the codeword to be, first, rate-dematched and, second, decoded.
  /// \param[in] E          Rate-matched codeword length.
  /// \param[in] n          \f$log_2\f$ of the codeword length.
  /// \param[in] K          Message size (including CRC).
  virtual void
  rate_dematch(span<log_likelihood_ratio> output, span<const log_likelihood_ratio> input, const polar_code& code) = 0;
};

} // namespace srsran
