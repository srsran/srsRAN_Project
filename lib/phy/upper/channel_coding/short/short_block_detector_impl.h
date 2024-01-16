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
/// \brief Short-block detector implementation.

#pragma once

#include "srsran/phy/upper/channel_coding/short/short_block_detector.h"

namespace srsran {

/// Maximum length of a codeword.
static constexpr unsigned MAX_BLOCK_LENGTH = 32;
/// Maximum length of a message.
static constexpr unsigned MAX_MSG_LENGTH = 11;
/// Half of the maximum codebook size.
static constexpr unsigned MAX_NOF_CODEWORDS_2 = 1U << (MAX_MSG_LENGTH - 1);

/// \brief Generic implementation of the short-block detector.
///
/// Implements a brute-force, maximum-likelihood detector for short-block codes (up to 11 information bits).
class short_block_detector_impl : public short_block_detector
{
public:
  // See interface for the documentation.
  bool detect(span<uint8_t> output, span<const log_likelihood_ratio> input, modulation_scheme mod) override;

private:
  /// \brief Brute force ML detection for messages of length 3-11 bits.
  /// \param[out] output The detected message.
  /// \param[in]  input  The received codeword (represented as a sequence of 32 LLRs).
  /// \returns The value of the GLRT detection metric.
  /// \remark The size of \c output must match the length of the expected message.
  static double detect_3_11(span<uint8_t> output, span<const log_likelihood_ratio> input);

  /// \brief Look-up table of possible transmitted sequences of 32 bits.
  ///
  /// Row \f$r\f$ corresponds to the codeword obtained by encoding the binary expansion of \f$2r\f$ as described in
  /// TS 38.211 Section 5.3.3.3.
  /// \note The codeword corresponding to the binary expansion of \f$ 2r + 1 \f$ can be obtained from the codeword with
  /// index \f$ r \f$ by inverting all the signs.
  static const std::array<std::array<int8_t, MAX_BLOCK_LENGTH>, MAX_NOF_CODEWORDS_2> DETECT_TABLE;
};

} // namespace srsran
