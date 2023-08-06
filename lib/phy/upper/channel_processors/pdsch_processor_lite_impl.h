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

#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srsvec/bit.h"

namespace srsran {

/// Internal PDSCH subprocessor.
class pdsch_block_processor
{
public:
  pdsch_block_processor(ldpc_segmenter_tx&       segmenter_,
                        ldpc_encoder&            encoder_,
                        ldpc_rate_matcher&       rate_matcher_,
                        pseudo_random_generator& scrambler_,
                        modulation_mapper&       modulator_) :
    segmenter(segmenter_), encoder(encoder_), rate_matcher(rate_matcher_), scrambler(scrambler_), modulator(modulator_)
  {
  }

  /// \brief Configures a new transmission.
  /// \param[in] data Transport block data.
  /// \param[in] i_cw Codeword index.
  /// \param[in] pdu  PDSCH transmission parameters.
  void configure_new_transmission(span<const uint8_t> data, unsigned i_cw, const pdsch_processor::pdu_t& pdu);

  /// Gets a number of symbols.
  void get_symbols(span<cf_t> symbols);

private:
  /// LDPC segmenter.
  ldpc_segmenter_tx& segmenter;
  /// LDPC encoder.
  ldpc_encoder& encoder;
  /// LDPC rate matcher.
  ldpc_rate_matcher& rate_matcher;
  /// Scrambling pseudo-random generator.
  pseudo_random_generator& scrambler;
  /// Modulation mapper.
  modulation_mapper& modulator;
  /// Buffer for storing data segments obtained after transport block segmentation.
  static_vector<described_segment, MAX_NOF_SEGMENTS> d_segments = {};
  /// Temporary codeblock unpacked bits.
  std::array<uint8_t, pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_cb_unpacked_bits = {};
  /// Temporary packed bits.
  static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_packed_bits = {};
  /// Current transmission modulation.
  modulation_scheme modulation;
  /// Current codeblock index.
  unsigned next_i_cb = 0;
  /// Codeblock unpacked bits buffer.
  span<uint8_t> encoded_bit_buffer;

  /// Processes a new codeblock and writes the new data in \ref encoded_bit_buffer.
  void new_codeblock();
};

/// Implements a PDSCH processor with low memory footprint codeblock processing.
class pdsch_processor_lite_impl : public pdsch_processor
{
public:
  /// Creates a concurrent PDSCH processor with all the dependencies.
  pdsch_processor_lite_impl(std::unique_ptr<ldpc_segmenter_tx>       segmenter_,
                            std::unique_ptr<ldpc_encoder>            encoder_,
                            std::unique_ptr<ldpc_rate_matcher>       rate_matcher_,
                            std::unique_ptr<pseudo_random_generator> scrambler_,
                            std::unique_ptr<modulation_mapper>       modulator_,
                            std::unique_ptr<dmrs_pdsch_processor>    dmrs_) :
    segmenter(std::move(segmenter_)),
    encoder(std::move(encoder_)),
    rate_matcher(std::move(rate_matcher_)),
    scrambler(std::move(scrambler_)),
    modulator(std::move(modulator_)),
    dmrs(std::move(dmrs_)),
    subprocessor(*segmenter, *encoder, *rate_matcher, *scrambler, *modulator)
  {
    srsran_assert(segmenter != nullptr, "Invalid segmenter pointer.");
    srsran_assert(scrambler != nullptr, "Invalid scrambler pointer.");
    srsran_assert(dmrs != nullptr, "Invalid dmrs pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_mapper&                                        mapper,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override;

private:
  /// \brief Asserts PDU.
  ///
  /// It triggers an assertion if the PDU is not valid for this processor.
  void assert_pdu(const pdu_t& pdu) const;

  /// \brief Processes DM-RS.
  /// \param[out] mapper Resource grid mapper interface.
  /// \param[in]  pdu Necessary parameters to process the DM-RS.
  void process_dmrs(resource_grid_mapper& mapper, const pdu_t& pdu);

  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Pointer to an LDPC encoder.
  std::unique_ptr<ldpc_encoder> encoder;
  /// Pointer to an LDPC rate matcher.
  std::unique_ptr<ldpc_rate_matcher> rate_matcher;
  /// Pointer to scrambling pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;
  /// Pointer to modulation mapper.
  std::unique_ptr<modulation_mapper> modulator;
  /// Pointer to DM-RS processor.
  std::unique_ptr<dmrs_pdsch_processor> dmrs;
  /// Internal block processor.
  pdsch_block_processor subprocessor;
};

} // namespace srsran
