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
/// \brief LDPC rate dematching.
#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/sch/modulation_scheme.h"

namespace srsran {

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
  virtual void rate_dematch(span<log_likelihood_ratio>       output,
                            span<const log_likelihood_ratio> input,
                            bool                             new_data,
                            const codeblock_metadata&        cfg) = 0;
};

} // namespace srsran
