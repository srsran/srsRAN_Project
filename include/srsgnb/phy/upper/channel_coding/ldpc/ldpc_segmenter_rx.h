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
/// \brief LDPC codeblock segmentation (receive side).
#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

namespace srsran {

/// Carries out the segmentation of a codeword into a number of codeblocks.
class ldpc_segmenter_rx
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter_rx() = default;

  /// \brief Splits a codeword into codeblocks and computes the metadata of all codeblocks.
  ///
  /// The LLRs corresponding to the received codeword are split into codeblocks to feed the decoder. Each codeblock is
  /// described by all the metadata needed for rate-dematching and decoding, as per TS38.212 Section 5.4.2.1.
  /// \param[out] described_codeblocks  Codeblock LLRs and corresponding metadata.
  /// \param[in]  codeword_llrs         Log-likelihood ratios of the received codeword.
  /// \param[in]  tbs                   Transport block size (not including CRC), as a number of bits.
  /// \param[in]  cfg                   Parameters affecting splitting and codeblock metadata.
  /// \remark The output codeblocks are just views (not copies) of the proper block of codeword log-likelihood ratios in
  /// \c codeword_llrs.
  virtual void segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
                       span<const log_likelihood_ratio>                         codeword_llrs,
                       unsigned                                                 tbs,
                       const segmenter_config&                                  cfg) = 0;
};

} // namespace srsran
