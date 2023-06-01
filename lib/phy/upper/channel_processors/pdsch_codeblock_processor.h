/*
 *
 * Copyright 2021-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "srsran/srsvec/bit.h"
#include <memory>

namespace srsran {

/// \brief PDSCH codeblock processor.
///
/// Contains the required dependencies to processes PDSCH codeblocks concurrently.
class pdsch_codeblock_processor
{
public:
  /// Builds a codeblock processor with the required dependencies.
  pdsch_codeblock_processor(std::unique_ptr<ldpc_encoder> encoder_, std::unique_ptr<ldpc_rate_matcher> rate_matcher_) :
    encoder(std::move(encoder_)), rate_matcher(std::move(rate_matcher_))
  {
    srsran_assert(encoder, "Invalid LDPC encoder.");
    srsran_assert(rate_matcher, "Invalid LDPC rate matcher.");
  }

  /// \brief Processes a PDSCH codeblock.
  /// \param[out] output    Resultant codeblock output.
  /// \param[in]  descr_seg Codeblock parameters.
  void process(span<uint8_t> output, const described_segment& descr_seg);

private:
  /// Pointer to an LDPC encoder.
  std::unique_ptr<ldpc_encoder> encoder;
  /// Pointer to an LDPC rate matcher.
  std::unique_ptr<ldpc_rate_matcher> rate_matcher;

  /// \brief Maximum codeblock length.
  ///
  /// This is the maximum length of an encoded codeblock, achievable with base graph 1 (rate 1/3).
  static constexpr units::bits MAX_CB_LENGTH{3 * MAX_SEG_LENGTH.value()};
  /// Buffer for storing temporary unpacked data between the LDPC segmenter and the LDPC encoder.
  std::array<uint8_t, MAX_SEG_LENGTH.value()> temp_unpacked_cb = {};
  /// Buffer for storing temporary, full-length codeblocks, between the LDPC encoder and the LDPC rate matcher.
  std::array<uint8_t, MAX_CB_LENGTH.value()> buffer_cb = {};
};

} // namespace srsran