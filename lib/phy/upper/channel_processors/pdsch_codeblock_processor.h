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

#include "srsran/phy/support/re_buffer.h"
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
/// Contains the required dependencies to processes PDSCH codeblocks concurrently.
class pdsch_codeblock_processor
{
public:
  /// Builds a codeblock processor with the required dependencies.
  pdsch_codeblock_processor(std::unique_ptr<ldpc_encoder>            encoder_,
                            std::unique_ptr<ldpc_rate_matcher>       rate_matcher_,
                            std::unique_ptr<pseudo_random_generator> scrambler_,
                            std::unique_ptr<modulation_mapper>       modulator_) :
    encoder(std::move(encoder_)),
    rate_matcher(std::move(rate_matcher_)),
    scrambler(std::move(scrambler_)),
    modulator(std::move(modulator_))
  {
    srsran_assert(encoder, "Invalid LDPC encoder.");
    srsran_assert(rate_matcher, "Invalid LDPC rate matcher.");
    srsran_assert(scrambler, "Invalid scrambler.");
    srsran_assert(modulator, "Invalid modulator.");
  }

  /// \brief Processes a PDSCH codeblock.
  ///
  /// The PDSCH codeblock processing includes LDPC encoding, rate matching, bit packing, scrambling, modulation and
  /// layer mapping.
  ///
  /// \param[out] buffer     Resource element buffer destination.
  /// \param[in]  descr_seg  Description of the codeblock to be processed.
  /// \param[in]  c_init     Scrambling initial state for the codeblocks to process.
  /// \param[in]  nof_layers Number of layers the codeblocks are mapped onto.
  /// \return The final pseudo-random generator scrambling state.
  pseudo_random_generator::state_s process(re_buffer_writer&                buffer,
                                           const described_segment&         descr_seg,
                                           pseudo_random_generator::state_s c_init,
                                           unsigned                         nof_layers);

private:
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
  /// Buffer for storing temporary unpacked data between the LDPC segmenter and the LDPC encoder.
  std::array<uint8_t, MAX_SEG_LENGTH.value()> temp_unpacked_cb = {};
  /// Buffer for storing temporary, full-length codeblocks, between the LDPC encoder and the LDPC rate matcher.
  std::array<uint8_t, MAX_CB_LENGTH.value()> buffer_cb = {};
  /// Buffer for storing temporary unpacked data between the LDPC rate matcher and the modulator.
  std::array<uint8_t, pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_unpacked_bits = {};
  /// Buffer for storing temporary packed data between the LDPC rate matcher and the modulator.
  static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_packed_bits = {};
  /// Temporary modulated symbols.
  std::array<cf_t, pdsch_constants::CODEWORD_MAX_SYMBOLS> temp_modulated;
};

} // namespace srsran