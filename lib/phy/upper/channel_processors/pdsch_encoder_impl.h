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
/// \brief PDSCH encoder declaration.

#pragma once

#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

namespace srsran {

/// Generic implementation of the PDSCH encoder.
class pdsch_encoder_impl : public pdsch_encoder
{
public:
  /// \brief Initializes the internal LDPC blocks.
  /// \param[in] seg Unique pointer to an LDPC segmenter.
  /// \param[in] enc Unique pointer to an LDPC encoder.
  /// \param[in] rm  Unique pointer to an LDPC rate matcher.
  pdsch_encoder_impl(std::unique_ptr<ldpc_segmenter_tx> seg,
                     std::unique_ptr<ldpc_encoder>      enc,
                     std::unique_ptr<ldpc_rate_matcher> rm) :
    segmenter(std::move(seg)), encoder(std::move(enc)), rate_matcher(std::move(rm))
  {
    srsran_assert(segmenter, "Invalid LDPC segmenter.");
    srsran_assert(encoder, "Invalid LDPC encoder.");
    srsran_assert(rate_matcher, "Invalid LDPC rate matcher.");
  }

  // See interface for the documentation.
  void encode(span<uint8_t>        codeword,
              tx_buffer&           rm_buffer,
              span<const uint8_t>  transport_block,
              const configuration& config) override;

private:
  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Pointer to an LDPC encoder.
  std::unique_ptr<ldpc_encoder> encoder;
  /// Pointer to an LDPC rate matcher.
  std::unique_ptr<ldpc_rate_matcher> rate_matcher;

  /// Buffer for storing data segments obtained after transport block segmentation.
  static_vector<described_segment, MAX_NOF_SEGMENTS> d_segments;
  /// \brief Maximum codeblock length.
  ///
  /// This is the maximum length of an encoded codeblock, achievable with base graph 1 (rate 1/3).
  static constexpr units::bits MAX_CB_LENGTH{3 * MAX_SEG_LENGTH.value()};
  /// Buffer for storing temporary encoded and packed codeblock.
  static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()> codeblock_packed;
};

} // namespace srsran
