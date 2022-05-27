/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar code rate dematching.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_RATE_DEMATCHER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_RATE_DEMATCHER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

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

std::unique_ptr<polar_rate_dematcher> create_polar_rate_dematcher();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_RATE_DEMATCHER_H