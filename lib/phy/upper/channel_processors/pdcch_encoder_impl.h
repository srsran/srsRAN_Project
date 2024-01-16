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

#pragma once

#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/phy/upper/channel_coding/polar/polar_allocator.h"
#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include "srsran/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsran/phy/upper/channel_coding/polar/polar_interleaver.h"
#include "srsran/phy/upper/channel_coding/polar/polar_rate_matcher.h"
#include "srsran/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsran/ran/pdcch/pdcch_constants.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// \brief PDCCH encoder, as per TS38.212 Section 7.3.
///
/// For the PHY Downlink Control CHannel (PDCCH) it carries out CRC attachment,
/// Polar encoding and rate matching.
class pdcch_encoder_impl : public pdcch_encoder
{
private:
  /// Defines CRC length in bits.
  static constexpr unsigned CRC_LEN = 24;

  /// Defines RNTI length in bits.
  static constexpr unsigned RNTI_LEN = 16;

  /// Defines the maximum payload size in bits, including 24 CRC bits.
  static constexpr unsigned MAX_K = pdcch_constants::MAX_DCI_PAYLOAD_SIZE + CRC_LEN;

  std::unique_ptr<crc_calculator>     crc24c;
  std::unique_ptr<polar_interleaver>  interleaver;
  std::unique_ptr<polar_allocator>    alloc;
  std::unique_ptr<polar_code>         code;
  std::unique_ptr<polar_encoder>      encoder;
  std::unique_ptr<polar_rate_matcher> rm;

  /// Temporary payload with CRC appended.
  std::array<uint8_t, MAX_K + CRC_LEN> temp_c;
  /// Temporary encoded data prior to rate matching.
  std::array<uint8_t, polar_code::NMAX> temp_d;

  /// \brief CRC attachment
  ///
  /// \param[out] c     Payload with CRC attached
  /// \param[in]  a     Payload
  /// \param[in]  rnti  RNTI used for CRC bits scrambling
  ///
  /// \remark CRC attachment follows TS 38.312 section 7.3.2
  void crc_attach(span<uint8_t>& c, span<const uint8_t> a, unsigned rnti);

  /// \brief Payload encoding
  ///
  /// \param[out] d Data after encoding
  /// \param[in]  c Payload after CRC attachment
  ///
  /// \remark Channel codding follows TS 38.312 section 7.3.3
  void channel_coding(span<uint8_t> d, span<const uint8_t> c);

  /// \brief Rate matching of the encoded data
  ///
  /// \param[out] f Data after rate matching
  /// \param[in]  d Data after encoding
  ///
  /// \remark Rate matching follows TS 38.312 section 7.3.4
  void rate_matching(span<uint8_t> f, span<const uint8_t> d);

public:
  pdcch_encoder_impl(std::unique_ptr<crc_calculator>     crc24c_,
                     std::unique_ptr<polar_interleaver>  interleaver_,
                     std::unique_ptr<polar_allocator>    alloc_,
                     std::unique_ptr<polar_code>         code_,
                     std::unique_ptr<polar_encoder>      encoder_,
                     std::unique_ptr<polar_rate_matcher> rm_) :
    crc24c(std::move(crc24c_)),
    interleaver(std::move(interleaver_)),
    alloc(std::move(alloc_)),
    code(std::move(code_)),
    encoder(std::move(encoder_)),
    rm(std::move(rm_))
  {
    report_fatal_error_if_not(crc24c, "Invalid CRC calculator.");
    report_fatal_error_if_not(crc24c->get_generator_poly() == crc_generator_poly::CRC24C, "Invalid CRC calculator.");
    report_fatal_error_if_not(interleaver, "Invalid polar interleaver.");
    report_fatal_error_if_not(alloc, "Invalid polar allocator.");
    report_fatal_error_if_not(code, "Invalid polar code.");
    report_fatal_error_if_not(encoder, "Invalid polar encoder.");
    report_fatal_error_if_not(rm, "Invalid polar rate matcher.");
  }

  // See interface for the documentation.
  void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) override;
};

} // namespace srsran
