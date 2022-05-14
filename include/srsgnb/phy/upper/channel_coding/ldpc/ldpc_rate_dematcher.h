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
/// \brief LDPC rate dematching.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_DEMATCHER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_DEMATCHER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/modulation_scheme.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"

namespace srsgnb {

/// LDPC rate dematcher interface. It reverts the operations of ldpc_rate_matcher.
class ldpc_rate_dematcher
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_dematcher() = default;

  /// \brief Recovers a full codeblock from its rate-matched version.
  ///
  /// This function also takes care of combining repeated codeblocks for H-ARQ schemes (both chase combining and
  /// incremental redundancy).
  ///
  /// Log-likelihood ratios corresponding to filler bits are set to \c INT8_MAX. Log-likelihood ratios that cannot
  /// be determined from the input are set to zero.
  /// \param[in,out] output          Full-length codeblock (log-likelihood ratios).
  /// \param[in]     input           Rate-matched codeblock (log-likelihood ratios).
  /// \param[in]     cfg             Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate recovering algorithm.
  virtual void
  rate_dematch(span<int8_t> output, span<const int8_t> input, bool new_data, const codeblock_metadata& cfg) = 0;
};

std::unique_ptr<ldpc_rate_dematcher> create_ldpc_rate_dematcher();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_RATE_DEMATCHER_H
