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
#include "srsran/phy/upper/channel_processors/pbch_encoder.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/support/error_handling.h"

namespace srsran {

class pbch_encoder_impl : public pbch_encoder
{
private:
  std::unique_ptr<crc_calculator>          crc24c;
  std::unique_ptr<pseudo_random_generator> scrambler;
  std::unique_ptr<polar_interleaver>       interleaver;
  std::unique_ptr<polar_allocator>         alloc;
  std::unique_ptr<polar_code>              code;
  std::unique_ptr<polar_encoder>           encoder;
  std::unique_ptr<polar_rate_matcher>      rm;

  /// \brief Implements TS 38.312 section 7.1.1 PBCH payload generation.
  /// \param[out] a Generated payload.
  /// \param[in] msg PBCH message.
  void payload_generate(span<uint8_t> a, const srsran::pbch_encoder::pbch_msg_t& msg);

  /// \brief Implements TS 38.312 section 7.1.2 Scrambling.
  /// \param[out] a_prime Scrambled payload.
  /// \param[in] msg PBCH message.
  /// \param[in] a Payload.
  void scramble(span<uint8_t> a_prime, const srsran::pbch_encoder::pbch_msg_t& msg, span<const uint8_t> a);

  /// \brief Implements TS 38.312 section 7.1.3 Transport block CRC attachment.
  /// \param[in] b Data with CRC attached.
  /// \param[in] a_prime Payload after scrambling.
  void crc_attach(span<uint8_t> b, span<const uint8_t> a_prime);

  /// \brief Implements TS 38.312 section 7.1.4 Channel coding.
  /// \param[out] d Encoded data.
  /// \param[in] c Payload after scrambling.
  void channel_coding(span<uint8_t> d, span<const uint8_t> c);

  /// \brief Implements TS 38.312 section 7.1.5 Rate matching.
  /// \param[out] f Rate matched data.
  /// \param[in] d Encoded data.
  void rate_matching(span<uint8_t> f, span<const uint8_t> d);

public:
  /// Constructs a generic PBCH encoder.
  pbch_encoder_impl(std::unique_ptr<crc_calculator>          crc24c_,
                    std::unique_ptr<pseudo_random_generator> scrambler_,
                    std::unique_ptr<polar_interleaver>       interleaver_,
                    std::unique_ptr<polar_allocator>         allocator_,
                    std::unique_ptr<polar_code>              code_,
                    std::unique_ptr<polar_encoder>           encoder_,
                    std::unique_ptr<polar_rate_matcher>      rm_) :
    crc24c(std::move(crc24c_)),
    scrambler(std::move(scrambler_)),
    interleaver(std::move(interleaver_)),
    alloc(std::move(allocator_)),
    code(std::move(code_)),
    encoder(std::move(encoder_)),
    rm(std::move(rm_))
  {
    report_fatal_error_if_not(crc24c, "Invalid CRC24C.");
    report_fatal_error_if_not(scrambler, "Invalid scrambler.");
    report_fatal_error_if_not(interleaver, "Invalid Polar interleaver.");
    report_fatal_error_if_not(alloc, "Invalid Polar allocator.");
    report_fatal_error_if_not(code, "Invalid Polar code.");
    report_fatal_error_if_not(encoder, "Invalid Polar encoder.");
    report_fatal_error_if_not(rm, "Invalid Polar RM.");
    code->set(B, E, POLAR_N_MAX_LOG, polar_code_ibil::not_present);
  }

  // See interface for documentation.
  void encode(span<uint8_t> encoded, const pbch_msg_t& pbch_msg) override;
};

} // namespace srsran
