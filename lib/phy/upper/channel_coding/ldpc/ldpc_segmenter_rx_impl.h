/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
/// \brief LDPC codeblock segmentation declaration.

#pragma once

#include "ldpc_graph_impl.h"
#include "ldpc_segmenter_helpers.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"

namespace srsran {

/// \brief Generic implementation of Rx-chain LDPC segmentation.
class ldpc_segmenter_rx_impl : public ldpc_segmenter_rx
{
public:
  /// Default constructor.
  ldpc_segmenter_rx_impl() = default;

  // See interface for the documentation.
  void segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
               span<const log_likelihood_ratio>                         codeword_llrs,
               unsigned                                                 tbs,
               const segmenter_config&                                  cfg) override;

private:
  /// Segmentation parameters.
  segment_parameters params;
};

} // namespace srsran
