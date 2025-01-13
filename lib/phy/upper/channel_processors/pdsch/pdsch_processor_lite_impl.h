/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pdsch_codeblock_processor.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

namespace srsran {

/// Internal PDSCH subprocessor.
class pdsch_block_processor : public resource_grid_mapper::symbol_buffer
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
  /// \param[in]     data       Transport block data.
  /// \param[in]     i_cw       Codeword index.
  /// \param[in]     pdu        PDSCH transmission parameters.
  void configure_new_transmission(span<const uint8_t> data, unsigned i_cw, const pdsch_processor::pdu_t& pdu);

  // See interface for documentation.
  unsigned get_max_block_size() const override;

  bool empty() const override;

  // See interface for documentation.
  span<const ci8_t> pop_symbols(unsigned block_size) override;

private:
  /// \brief Determines the maximum block size that can be processed.
  ///
  /// Set it to fit a complex float block in a cache page. Typically a cache page is 4096 bytes. It can be checked
  /// running the command <tt>$ getconf -a | grep PAGESIZE</tt>.
  static constexpr unsigned max_block_size = 4096 / sizeof(ci8_t);

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
  /// Pointer to an LDPC segmenter output buffer interface.
  const ldpc_segmenter_buffer* segment_buffer;
  /// \brief Temporary codeblock message.
  ///
  /// It contains codeblock information bits, codeblock CRC (if applicable) and filler bits.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_SIZE> cb_data;
  /// Temporary packed bits.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_RM_SIZE> temp_codeblock;
  /// Current transmission modulation.
  modulation_scheme modulation;
  /// Current codeblock index.
  unsigned next_i_cb = 0;
  /// Temporary storage of codeblock symbols.
  std::array<ci8_t, ldpc::MAX_CODEBLOCK_RM_SIZE> temp_codeblock_symbols;
  /// Current view of the codeblock modulated symbols.
  span<ci8_t> codeblock_symbols;
  /// Codeblock size. It includes information, CRC, and filler bits.
  units::bits cb_size;
  /// Local pointer to the input data.
  span<const uint8_t> transport_block;

  /// Processes a new codeblock and writes the new data in \ref temp_codeblock_symbols.
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
                            std::unique_ptr<dmrs_pdsch_processor>    dmrs_,
                            std::unique_ptr<ptrs_pdsch_generator>    ptrs_,
                            std::unique_ptr<resource_grid_mapper>    mapper_) :
    segmenter(std::move(segmenter_)),
    encoder(std::move(encoder_)),
    rate_matcher(std::move(rate_matcher_)),
    scrambler(std::move(scrambler_)),
    modulator(std::move(modulator_)),
    dmrs(std::move(dmrs_)),
    ptrs(std::move(ptrs_)),
    mapper(std::move(mapper_)),
    subprocessor(*segmenter, *encoder, *rate_matcher, *scrambler, *modulator)
  {
    srsran_assert(segmenter != nullptr, "Invalid segmenter pointer.");
    srsran_assert(scrambler != nullptr, "Invalid scrambler pointer.");
    srsran_assert(dmrs != nullptr, "Invalid dmrs pointer.");
    srsran_assert(ptrs != nullptr, "Invalid ptrs pointer.");
    srsran_assert(mapper != nullptr, "Invalid resource grid mapper pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override;

private:
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
  /// Pointer to PT-RS generator.
  std::unique_ptr<ptrs_pdsch_generator> ptrs;
  /// Pointer to resource grid mapper.
  std::unique_ptr<resource_grid_mapper> mapper;
  /// Internal block processor.
  pdsch_block_processor subprocessor;
};

} // namespace srsran
