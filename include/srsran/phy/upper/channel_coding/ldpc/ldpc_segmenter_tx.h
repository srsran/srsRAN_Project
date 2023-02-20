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
/// \brief LDPC codeblock segmentation (transmit side).
#pragma once

#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/codeblock_metadata.h"

namespace srsran {

/// Carries out the segmentation of a transport block into a number of codeblocks.
class ldpc_segmenter_tx
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter_tx() = default;

  /// \brief Adds the CRC to the a transport block, carries out segmentation and computes all codeblock metadata for
  /// later use (encoder and rate matching).
  ///
  /// First, the transport block CRC is attached, as per TS38.212 Section 7.2.1. Then, the transport block is split into
  /// a number of segments and, if needed, a CRC is attached to each segment_tx. This is done according to TS38.212
  /// Section 5.2.2. The function also computes other segment_tx metadata (e.g., coded and rate-matched length)
  /// according to TS38.212 Section 5.4.2.1.
  ///
  /// \param[out] described_segments    Segments (unpacked, one bit per entry) and corresponding metadata.
  /// \param[in]  transport_block       The transport block to segment_tx (packed, one byte per entry).
  /// \param[in]  cfg                   Parameters affecting splitting and codeblock metadata.
  virtual void segment(static_vector<described_segment, MAX_NOF_SEGMENTS>& described_segments,
                       span<const uint8_t>                                 transport_block,
                       const segmenter_config&                             cfg) = 0;
};

} // namespace srsran
