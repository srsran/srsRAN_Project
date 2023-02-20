/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar code rate dematcher declaration.

#pragma once

#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_rate_dematcher.h"

namespace srsgnb {

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
} // namespace srsgnb
