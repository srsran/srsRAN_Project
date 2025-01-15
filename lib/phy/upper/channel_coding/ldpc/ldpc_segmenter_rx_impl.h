/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
