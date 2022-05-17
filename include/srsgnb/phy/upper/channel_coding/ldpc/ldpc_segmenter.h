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
/// \brief LDPC codeblock segmentation.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"

namespace srsgnb {

/// Carries out the segmentation of a transport block into a number of codeblocks.
class ldpc_segmenter
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter() = default;

  /// \brief Adds the CRC to the a transport block, carries out segmentation and computes all codeblock metadata for
  /// later use (encoder and rate matching).
  ///
  /// First, the transport block CRC is attached, as per TS38.212 Section 7.2.1. Then, the transport block is split into
  /// a number of segments and, if needed, a CRC is attached to each segment. This is done according to TS38.212
  /// Section 5.2.2. The function also computes other segment metadata (e.g., coded and rate-matched length) according
  /// to TS38.212 Section 5.4.2.1.
  /// \param[out] described_segments    Segments (unpacked, one bit per entry) and corresponding metadata.
  /// \param[in]  transport_block       The transport block to segment (packed, one byte per entry).
  /// \param[in]  cfg                   Parameters affecting splitting and codeblock metadata.
  virtual void segment(static_vector<described_segment, MAX_NOF_SEGMENTS>& described_segments,
                       span<const uint8_t>                                 transport_block,
                       const segment_config&                               cfg) = 0;
};

std::unique_ptr<ldpc_segmenter> create_ldpc_segmenter();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_H
