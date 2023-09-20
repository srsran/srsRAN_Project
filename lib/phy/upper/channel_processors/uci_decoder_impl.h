/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/phy/upper/channel_coding/polar/polar_deallocator.h"
#include "srsran/phy/upper/channel_coding/polar/polar_decoder.h"
#include "srsran/phy/upper/channel_coding/polar/polar_rate_dematcher.h"
#include "srsran/phy/upper/channel_coding/short/short_block_detector.h"
#include "srsran/phy/upper/channel_processors/uci_decoder.h"

namespace srsran {

/// Implementation of the UCI decoder.
class uci_decoder_impl : public uci_decoder
{
public:
  /// UCI decoder constructor.
  uci_decoder_impl(std::unique_ptr<short_block_detector> block_detector_,
                   std::unique_ptr<polar_code>           code_,
                   std::unique_ptr<polar_rate_dematcher> rate_matcher_,
                   std::unique_ptr<polar_decoder>        decoder_,
                   std::unique_ptr<polar_deallocator>    deallocator_,
                   std::unique_ptr<crc_calculator>       crc6_,
                   std::unique_ptr<crc_calculator>       crc11_) :
    block_detector(std::move(block_detector_)),
    code(std::move(code_)),
    rate_dematcher(std::move(rate_matcher_)),
    decoder(std::move(decoder_)),
    deallocator(std::move(deallocator_)),
    crc6(std::move(crc6_)),
    crc11(std::move(crc11_))
  {
    srsran_assert(block_detector, "Invalid short block detector.");
    srsran_assert(code, "Invalid polar code instance.");
    srsran_assert(rate_dematcher, "Invalid polar rate dematcher.");
    srsran_assert(decoder, "Invalid polar decoder.");
    srsran_assert(deallocator, "Invalid polar deallocator.");
    srsran_assert(crc6, "Invalid CRC6 calculator.");
    srsran_assert(crc6->get_generator_poly() == crc_generator_poly::CRC6, "Invalid CRC6 calculator.");
    srsran_assert(crc11, "Invalid CRC11 calculator.");
    srsran_assert(crc11->get_generator_poly() == crc_generator_poly::CRC11, "Invalid CRC11 calculator.");
  };

  // See interface for the documentation.
  uci_status decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) override;

private:
  /// Maximum polar rate matching buffer size.
  static constexpr unsigned MAX_RM_BUFFER_SIZE = polar_code::NMAX;
  /// Maximum polar encoder/decoder codeblock size.
  static constexpr unsigned MAX_POLAR_BUFFER_SIZE = polar_code::NMAX;
  /// Maximum polar codeblock size. Defined in TS38.212 Section 5.2.1.
  static constexpr unsigned MAX_CODEBLOCK_SIZE = 1706;

  /// Detects a short block.
  uci_status detect_short_block(span<uint8_t>                     output,
                                span<const log_likelihood_ratio>  input,
                                const uci_decoder::configuration& config);
  /// Decodes a codeword with polar code.
  uci_status decode_codeword_polar(span<uint8_t> output, span<const log_likelihood_ratio> input);

  /// Decodes a codeblock with polar code.
  uci_status decode_codeblock_polar(span<uint8_t>                    output,
                                    span<const log_likelihood_ratio> input,
                                    unsigned                         nof_filler_bits,
                                    crc_calculator&                  crc);

  /// Pointer to a short block detector.
  std::unique_ptr<short_block_detector> block_detector;
  /// Pointer to a polar code.
  std::unique_ptr<polar_code> code;
  /// Pointer to a polar rate dematcher.
  std::unique_ptr<polar_rate_dematcher> rate_dematcher;
  /// Pointer to a polar decoder.
  std::unique_ptr<polar_decoder> decoder;
  /// pointer to a polar deallocator.
  std::unique_ptr<polar_deallocator> deallocator;
  /// Pointer to a CRC calculator of order 6.
  std::unique_ptr<crc_calculator> crc6;
  /// Pointer to a CRC calculator of order 11.
  std::unique_ptr<crc_calculator> crc11;
  /// Polar rate matcher temporary output.
  std::array<log_likelihood_ratio, MAX_RM_BUFFER_SIZE> temp_rm_buffer;
  /// Polar decoder temporary output.
  std::array<uint8_t, MAX_POLAR_BUFFER_SIZE> temp_decoder_buffer;
  /// Polar deallocator temporary output.
  std::array<uint8_t, MAX_CODEBLOCK_SIZE> temp_deallocator_buffer;
};

} // namespace srsran
