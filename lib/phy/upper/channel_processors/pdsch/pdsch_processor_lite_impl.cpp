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

#include "pdsch_processor_lite_impl.h"
#include "pdsch_processor_helpers.h"
#include "pdsch_processor_validator_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"

using namespace srsran;

/// \brief Looks at the output of the validator and, if unsuccessful, fills msg with the error message.
///
/// This is used to call the validator inside the process methods only if asserts are active.
[[maybe_unused]] static bool handle_validation(std::string& msg, const error_type<std::string>& err)
{
  bool is_success = err.has_value();
  if (!is_success) {
    msg = err.error();
  }
  return is_success;
}

void pdsch_block_processor::configure_new_transmission(span<const uint8_t>           data,
                                                       unsigned                      i_cw,
                                                       const pdsch_processor::pdu_t& pdu)
{
  // The number of layers is equal to the number of ports.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = pdsch_compute_nof_data_re(pdu);

  // Init scrambling initial state.
  scrambler.init((static_cast<unsigned>(pdu.rnti) << 15U) + (i_cw << 14U) + pdu.n_id);

  // Select codeword specific parameters.
  unsigned rv = pdu.codewords[i_cw].rv;
  modulation  = pdu.codewords[i_cw].modulation;

  // Calculate rate match buffer size.
  units::bits Nref = ldpc::compute_N_ref(
      pdu.tbs_lbrm, ldpc::compute_nof_codeblocks(units::bytes(data.size()).to_bits(), pdu.ldpc_base_graph));

  // Prepare segmenter configuration.
  segmenter_config encoder_config;
  encoder_config.base_graph     = pdu.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = Nref.value();
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = nof_re_pdsch * nof_layers;

  // Initialize the segmenter.
  segment_buffer = &segmenter.new_transmission(data, encoder_config);

  cb_size = segment_buffer->get_segment_length();

  // Initialize the codeblock counter.
  next_i_cb = 0;

  // Save the pointer to the input data.
  transport_block = data;
}

void pdsch_block_processor::new_codeblock()
{
  srsran_assert(next_i_cb < segment_buffer->get_nof_codeblocks(),
                "The codeblock index (i.e., {}) exceeds the number of codeblocks (i.e., {})",
                next_i_cb,
                segment_buffer->get_nof_codeblocks());

  // Prepare codeblock data.
  cb_data.resize(cb_size.value());

  // Retrieve segment description.
  const codeblock_metadata cb_metadata = segment_buffer->get_cb_metadata(next_i_cb);

  // Rate Matching output length.
  unsigned rm_length = segment_buffer->get_rm_length(next_i_cb);

  // Number of symbols.
  unsigned nof_symbols = rm_length / get_bits_per_symbol(modulation);

  // Copy codeblock data, including TB and/or CB CRC if applicable, as well as filler and zero padding bits.
  segment_buffer->read_codeblock(cb_data, transport_block, next_i_cb);

  // Encode the segment into a codeblock.
  const ldpc_encoder_buffer& rm_buffer = encoder.encode(cb_data, cb_metadata.tb_common);

  // Rate match the codeblock.
  temp_codeblock.resize(rm_length);
  rate_matcher.rate_match(temp_codeblock, rm_buffer, cb_metadata);

  // Apply scrambling sequence in-place.
  scrambler.apply_xor(temp_codeblock, temp_codeblock);

  // Apply modulation.
  codeblock_symbols = span<ci8_t>(temp_codeblock_symbols).first(nof_symbols);
  modulator.modulate(codeblock_symbols, temp_codeblock, modulation);

  // Increment codeblock counter.
  ++next_i_cb;
}

span<const ci8_t> pdsch_block_processor::pop_symbols(unsigned block_size)
{
  // Process a new codeblock if the buffer with codeblock symbols is empty.
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

  if (next_i_cb != segment_buffer->get_nof_codeblocks()) {
    unsigned rm_length       = segment_buffer->get_rm_length(next_i_cb);
    unsigned bits_per_symbol = get_bits_per_symbol(modulation);
    return rm_length / bits_per_symbol;
  }

  return 0;
}

bool pdsch_block_processor::empty() const
{
  return codeblock_symbols.empty() && (next_i_cb == segment_buffer->get_nof_codeblocks());
}

void pdsch_processor_lite_impl::process(resource_grid_writer&                                           grid,
                                        pdsch_processor_notifier&                                       notifier,
                                        static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
                                        const pdu_t&                                                    pdu)
{
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, pdsch_processor_validator_impl().is_valid(pdu)), "{}", msg);

  // Configure new transmission.
  subprocessor.configure_new_transmission(data.front().get_buffer(), 0, pdu);

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
  mapper->map(grid, subprocessor, allocation, reserved, precoding2);

  if (pdu.ptrs) {
    // Prepare PT-RS configuration and generate.
    pdsch_process_ptrs(grid, *ptrs, pdu);
  }

  // Process DM-RS.
  pdsch_process_dmrs(grid, *dmrs, pdu);

  // Notify the end of the processing.
  notifier.on_finish_processing();
}
