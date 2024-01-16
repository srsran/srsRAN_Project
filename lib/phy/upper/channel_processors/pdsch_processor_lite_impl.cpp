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

#include "pdsch_processor_lite_impl.h"
#include "pdsch_processor_validator_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/tx_buffer.h"
#include "srsran/phy/upper/unique_tx_buffer.h"
#include "srsran/ran/dmrs.h"

using namespace srsran;

/// \brief Computes the number of RE used for mapping PDSCH data.
///
/// The number of RE excludes the elements described by \c pdu as reserved and the RE used for DM-RS.
///
/// \param[in] pdu Describes a PDSCH transmission.
/// \return The number of resource elements.
static unsigned compute_nof_data_re(const pdsch_processor::pdu_t& pdu)
{
  // Copy reserved RE and merge DM-RS pattern.
  re_pattern_list reserved_re = pdu.reserved;
  reserved_re.merge(pdu.dmrs.get_dmrs_pattern(
      pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask));

  // Generate allocation mask.
  bounded_bitset<MAX_RB> prb_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Calculate the number of RE allocated in the grid.
  unsigned nof_grid_re = pdu.freq_alloc.get_nof_rb() * NRE * pdu.nof_symbols;

  // Calculate the number of reserved resource elements.
  unsigned nof_reserved_re = reserved_re.get_inclusion_count(pdu.start_symbol_index, pdu.nof_symbols, prb_mask);

  // Subtract the number of reserved RE from the number of allocated RE.
  srsran_assert(nof_grid_re > nof_reserved_re,
                "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
                nof_grid_re,
                nof_reserved_re);
  return nof_grid_re - nof_reserved_re;
}

void pdsch_block_processor::configure_new_transmission(unique_tx_buffer              rm_buffer_,
                                                       span<const uint8_t>           data,
                                                       unsigned                      i_cw,
                                                       const pdsch_processor::pdu_t& pdu)
{
  new_data         = pdu.codewords.front().new_data;
  unique_rm_buffer = std::move(rm_buffer_);

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = compute_nof_data_re(pdu);

  // Init scrambling initial state.
  scrambler.init((static_cast<unsigned>(pdu.rnti) << 15U) + (i_cw << 14U) + pdu.n_id);

  // Select codeword specific parameters.
  unsigned rv = pdu.codewords[i_cw].rv;
  modulation  = pdu.codewords[i_cw].modulation;

  // Prepare segmenter configuration.
  segmenter_config encoder_config;
  encoder_config.base_graph     = pdu.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = pdu.tbs_lbrm_bytes * 8;
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = nof_re_pdsch * nof_layers;

  // Clear the buffer.
  d_segments.clear();

  // Segmentation (it includes CRC attachment for the entire transport block and each individual segment).
  segmenter.segment(d_segments, data, encoder_config);

  // Initialize the codeblock counter.
  next_i_cb = 0;
}

void pdsch_block_processor::new_codeblock()
{
  srsran_assert(next_i_cb < d_segments.size(),
                "The codeblock index (i.e., {}) exceeds the number of codeblocks (i.e., {})",
                next_i_cb,
                d_segments.size());

  // Select segment description.
  const described_segment& descr_seg = d_segments[next_i_cb];

  // Rate Matching output length.
  unsigned rm_length = descr_seg.get_metadata().cb_specific.rm_length;

  // Number of symbols.
  unsigned nof_symbols = rm_length / get_bits_per_symbol(modulation);

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  bit_buffer rm_buffer =
      unique_rm_buffer.get().get_codeblock(next_i_cb, descr_seg.get_metadata().cb_specific.full_length);

  // Encode only if it is a new transmission.
  if (new_data) {
    // Encode the segment into a codeblock.
    encoder.encode(rm_buffer, descr_seg.get_data(), descr_seg.get_metadata().tb_common);
  }

  // Rate match the codeblock.
  temp_codeblock.resize(rm_length);
  rate_matcher.rate_match(temp_codeblock, rm_buffer, descr_seg.get_metadata());

  // Apply scrambling sequence in-place.
  scrambler.apply_xor(temp_codeblock, temp_codeblock);

  // Apply modulation.
  codeblock_symbols = span<ci8_t>(temp_codeblock_symbols).first(nof_symbols);
  modulator.modulate(codeblock_symbols, temp_codeblock, modulation);

  // Increment codeblock counter.
  ++next_i_cb;

  // Unlock buffer if all the codeblocks have been processed.
  if (next_i_cb == d_segments.size()) {
    unique_rm_buffer = unique_tx_buffer();
  }
}

span<const ci8_t> pdsch_block_processor::pop_symbols(unsigned block_size)
{
  // Process a new code block if the buffer with code block symbols is empty.
  if (codeblock_symbols.empty()) {
    new_codeblock();
  }

  srsran_assert(block_size <= codeblock_symbols.size(),
                "The block size (i.e., {}) exceeds the number of available symbols (i.e., {}).",
                block_size,
                codeblock_symbols.size());

  // Select view of the current block.
  span<ci8_t> symbols = codeblock_symbols.first(block_size);

  // Advance read pointer.
  codeblock_symbols = codeblock_symbols.last(codeblock_symbols.size() - block_size);

  return symbols;
}

unsigned pdsch_block_processor::get_max_block_size() const
{
  if (!codeblock_symbols.empty()) {
    return codeblock_symbols.size();
  }

  if (next_i_cb != d_segments.size()) {
    unsigned rm_length       = d_segments[next_i_cb].get_metadata().cb_specific.rm_length;
    unsigned bits_per_symbol = get_bits_per_symbol(modulation);
    return rm_length / bits_per_symbol;
  }

  return 0;
}

bool pdsch_block_processor::empty() const
{
  return codeblock_symbols.empty() && (next_i_cb == d_segments.size());
}

void pdsch_processor_lite_impl::process(resource_grid_mapper&                                        mapper,
                                        unique_tx_buffer                                             rm_buffer,
                                        pdsch_processor_notifier&                                    notifier,
                                        static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                                        const pdu_t&                                                 pdu)
{
  pdsch_processor_validator_impl::assert_pdu(pdu);

  // Configure new transmission.
  subprocessor.configure_new_transmission(std::move(rm_buffer), data[0], 0, pdu);

  // Get the PRB allocation mask.
  const bounded_bitset<MAX_RB> prb_allocation_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // First symbol used in this transmission.
  unsigned start_symbol_index = pdu.start_symbol_index;

  // Calculate the end symbol index (excluded) and assert it does not exceed the slot boundary.
  unsigned end_symbol_index = pdu.start_symbol_index + pdu.nof_symbols;

  srsran_assert(end_symbol_index <= MAX_NSYMB_PER_SLOT,
                "The time allocation of the transmission [{}, {}) exceeds the slot boundary.",
                start_symbol_index,
                end_symbol_index);

  // PDSCH OFDM symbol mask.
  symbol_slot_mask symbols;
  symbols.fill(start_symbol_index, end_symbol_index);

  // Allocation pattern for the mapper.
  re_pattern_list allocation;
  re_pattern      pdsch_pattern;

  // Reserved REs, including DM-RS and CSI-RS.
  re_pattern_list reserved(pdu.reserved);

  // Get DM-RS RE pattern.
  re_pattern dmrs_pattern = pdu.dmrs.get_dmrs_pattern(
      pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask);

  // Merge DM-RS RE pattern into the reserved RE patterns.
  reserved.merge(dmrs_pattern);

  // Set PDSCH allocation pattern.
  pdsch_pattern.prb_mask = prb_allocation_mask;
  pdsch_pattern.re_mask  = ~re_prb_mask();
  pdsch_pattern.symbols  = symbols;
  allocation.merge(pdsch_pattern);

  // Apply scaling over the precoding.
  static_bit_buffer<0>    tmp;
  float                   scaling    = modulator->modulate(span<ci8_t>(), tmp, pdu.codewords.front().modulation);
  precoding_configuration precoding2 = pdu.precoding;
  if (std::isnormal(pdu.ratio_pdsch_data_to_sss_dB)) {
    scaling *= convert_dB_to_amplitude(-pdu.ratio_pdsch_data_to_sss_dB);
  }
  precoding2 *= scaling;

  // Map PDSCH.
  mapper.map(subprocessor, allocation, reserved, precoding2);

  // Process DM-RS.
  process_dmrs(mapper, pdu);

  // Notify the end of the processing.
  notifier.on_finish_processing();
}

void pdsch_processor_lite_impl::process_dmrs(resource_grid_mapper& mapper, const pdu_t& pdu)
{
  bounded_bitset<MAX_RB> rb_mask_bitset = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned dmrs_reference_point_k_rb = 0;
  if (pdu.ref_point == pdu_t::PRB0) {
    dmrs_reference_point_k_rb = pdu.bwp_start_rb;
  }

  // Prepare DM-RS configuration.
  dmrs_pdsch_processor::config_t dmrs_config;
  dmrs_config.slot                 = pdu.slot;
  dmrs_config.reference_point_k_rb = dmrs_reference_point_k_rb;
  dmrs_config.type                 = pdu.dmrs;
  dmrs_config.scrambling_id        = pdu.scrambling_id;
  dmrs_config.n_scid               = pdu.n_scid;
  dmrs_config.amplitude            = convert_dB_to_amplitude(-pdu.ratio_pdsch_dmrs_to_sss_dB);
  dmrs_config.symbols_mask         = pdu.dmrs_symbol_mask;
  dmrs_config.rb_mask              = rb_mask_bitset;
  dmrs_config.precoding            = pdu.precoding;

  // Put DM-RS.
  dmrs->map(mapper, dmrs_config);
}
