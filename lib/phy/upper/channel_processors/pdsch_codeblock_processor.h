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
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srsvec/bit.h"
#include <memory>

namespace srsran {

/// \brief PDSCH codeblock processor.
///
/// Contains the required dependencies to process PDSCH codeblocks in parallel.
///
/// The codeblock processing consists of:
/// 1. Extracts the codeblock information bits from the transport block;
/// 2. Appends transport block CRC (if applicable);
/// 3. Appends zero padding;
/// 4. Appends codeblock CRC (if applicable);
/// 5. Appends filler bits as zeros;
/// 6. Applies LDPC encode;
/// 7. Applies LDPC rate matching;
/// 8. Applies scrambling; and
/// 9. Maps bits into modulation symbols.
class pdsch_codeblock_processor
{
public:
  /// Collects the parameters necessary for processing a PDSCH codeblock.
  struct configuration {
    /// Position of the codeblock relative to the first bit of the transport block.
    units::bits tb_offset;
    /// Codeblock number of information bits.
    units::bits cb_info_size;
    /// Codeblock size before LDPC encoder.
    units::bits cb_size;
    /// Transport block zero padding.
    units::bits zero_pad;
    /// Codeblock metadata.
    codeblock_metadata metadata;
    /// Set to true if codeblock CRC is applicable.
    bool has_cb_crc;
    /// Set to true if it belongs to a new PDSCH transmission.
    bool new_data;
    /// Scrambling pseudo-random generator initial state.
    pseudo_random_generator::state_s c_init;
  };

  /// Collects the output parameters.
  struct result {
    /// Scrambling sequence state.
    pseudo_random_generator::state_s scrambling_state;
    /// View of the codeblock symbols.
    span<const ci8_t> cb_symbols;
  };

  /// Builds a codeblock processor with the required dependencies.
  pdsch_codeblock_processor(std::unique_ptr<crc_calculator>          crc24a_,
                            std::unique_ptr<crc_calculator>          crc24b_,
                            std::unique_ptr<crc_calculator>          crc16_,
                            std::unique_ptr<ldpc_encoder>            encoder_,
                            std::unique_ptr<ldpc_rate_matcher>       rate_matcher_,
                            std::unique_ptr<pseudo_random_generator> scrambler_,
                            std::unique_ptr<modulation_mapper>       modulator_) :
    crc24a(std::move(crc24a_)),
    crc24b(std::move(crc24b_)),
    crc16(std::move(crc16_)),
    encoder(std::move(encoder_)),
    rate_matcher(std::move(rate_matcher_)),
    scrambler(std::move(scrambler_)),
    modulator(std::move(modulator_))
  {
    srsran_assert(crc24a, "Invalid CRC calculator.");
    srsran_assert(crc24b, "Invalid CRC calculator.");
    srsran_assert(crc16, "Invalid CRC calculator.");
    srsran_assert(crc24a->get_generator_poly() == crc_generator_poly::CRC24A, "Invalid CRC calculator.");
    srsran_assert(crc24b->get_generator_poly() == crc_generator_poly::CRC24B, "Invalid CRC calculator.");
    srsran_assert(crc16->get_generator_poly() == crc_generator_poly::CRC16, "Invalid CRC calculator.");
    srsran_assert(encoder, "Invalid LDPC encoder.");
    srsran_assert(rate_matcher, "Invalid LDPC rate matcher.");
    srsran_assert(scrambler, "Invalid scrambler.");
    srsran_assert(modulator, "Invalid modulator.");
  }

  /// \brief Processes a PDSCH codeblock.
  ///
  /// The PDSCH codeblock processing includes transport and code block CRC attachment if applicable, LDPC encoding,
  /// rate matching, bit packing, scrambling and modulation.
  ///
  /// \param[in]  rm_buffer  Buffer for storing temporary, full-length codeblocks, between the LDPC encoder and the LDPC
  ///                        rate matcher.
  /// \param[in]  data       Original transport block data without CRC.
  /// \param[in]  config     Required parameters for processing a codeblock.
  /// \return A struct with the results.
  result process(bit_buffer rm_buffer, span<const uint8_t> data, const configuration& config);

  /// Gets the QAM modulation scaling, as per TS38.211 Section 5.1.
  float get_scaling(modulation_scheme modulation)
  {
    static_bit_buffer<0> temp;
    return modulator->modulate(span<ci8_t>(), temp, modulation);
  }

private:
  /// Pointer to CRC24A.
  std::unique_ptr<crc_calculator> crc24a;
  /// Pointer to CRC24B.
  std::unique_ptr<crc_calculator> crc24b;
  /// Pointer to CRC16.
  std::unique_ptr<crc_calculator> crc16;
  /// Pointer to an LDPC encoder.
  std::unique_ptr<ldpc_encoder> encoder;
  /// Pointer to an LDPC rate matcher.
  std::unique_ptr<ldpc_rate_matcher> rate_matcher;
  /// Pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;
  /// Modulator.
  std::unique_ptr<modulation_mapper> modulator;

  /// \brief Maximum codeblock length.
  ///
  /// This is the maximum length of an encoded codeblock, achievable with base graph 1 (rate 1/3).
  static constexpr units::bits MAX_CB_LENGTH{3 * MAX_SEG_LENGTH.value()};
  /// \brief Temporary codeblock message.
  ///
  /// It contains codeblock information bits, codeblock CRC (if applicable) and filler bits.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_SIZE> cb_data;
  /// Buffer for storing temporary packed data between the LDPC rate matcher and the modulator.
  static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_packed_bits;
  /// Buffer for storing the codeblock symbols.
  std::array<ci8_t, pdsch_constants::CODEWORD_MAX_SYMBOLS> temp_cb_symbols;
};

} // namespace srsran
