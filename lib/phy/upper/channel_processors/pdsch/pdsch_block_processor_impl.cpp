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

#include "pdsch_block_processor_impl.h"
#include "pdsch_processor_helpers.h"

using namespace srsran;

resource_grid_mapper::symbol_buffer&
pdsch_block_processor_impl::configure_new_transmission(span<const uint8_t>           data,
                                                       unsigned                      i_cw,
                                                       const pdsch_processor::pdu_t& pdu,
                                                       const ldpc_segmenter_buffer&  segment_buffer_,
                                                       unsigned                      start_i_cb,
                                                       unsigned                      cb_batch_len)
{
  // Init scrambling initial state.
  scrambler->init((static_cast<unsigned>(pdu.rnti) << 15U) + (i_cw << 14U) + pdu.n_id);

  // Advance scrambling sequence to the specific offset of the starting CB.
  scrambler->advance(segment_buffer_.get_cb_metadata(start_i_cb).cb_specific.cw_offset);

  // Select codeword specific parameters.
  modulation = pdu.codewords[i_cw].modulation;

  cb_size = segment_buffer_.get_segment_length();

  // Initialize the codeblock counter.
  next_i_cb = start_i_cb;
  last_i_cb = start_i_cb + cb_batch_len - 1;
  srsran_assert(last_i_cb < segment_buffer_.get_nof_codeblocks(),
                "The last codeblock index in the batch (i.e., {}) exceeds the number of codeblocks (i.e., {})",
                last_i_cb,
                segment_buffer_.get_nof_codeblocks());

  // Save the pointer to the input data.
  transport_block = data;

  // Save the pointer to the segment buffer.
  segment_buffer = &segment_buffer_;

  return *this;
}

void pdsch_block_processor_impl::new_codeblock()
{
  // Prepare codeblock data.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_SIZE> cb_data(cb_size.value());

  // Retrieve segment description.
  const codeblock_metadata cb_metadata = segment_buffer->get_cb_metadata(next_i_cb);

  // Rate Matching output length.
  unsigned rm_length = segment_buffer->get_rm_length(next_i_cb);

  // Number of symbols.
  unsigned nof_symbols = rm_length / get_bits_per_symbol(modulation);

  // Copy codeblock data, including TB and/or CB CRC if applicable, as well as filler and zero padding bits.
  segment_buffer->read_codeblock(cb_data, transport_block, next_i_cb);

  // Encode the segment into a codeblock.
  ldpc_encoder::configuration encoder_config = {
      .base_graph   = cb_metadata.tb_common.base_graph,
      .lifting_size = cb_metadata.tb_common.lifting_size,
      .Nref         = cb_metadata.tb_common.Nref,
  };
  const ldpc_encoder_buffer& rm_buffer = encoder->encode(cb_data, encoder_config);

  // Rate match the codeblock.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_RM_SIZE> temp_codeblock(rm_length);
  rate_matcher->rate_match(temp_codeblock, rm_buffer, cb_metadata);

  // Apply scrambling sequence in-place.
  scrambler->apply_xor(temp_codeblock, temp_codeblock);

  // Apply modulation.
  codeblock_symbols = span<ci8_t>(temp_codeblock_symbols).first(nof_symbols);
  modulator->modulate(codeblock_symbols, temp_codeblock, modulation);

  // Increment codeblock counter.
  ++next_i_cb;
}

span<const ci8_t> pdsch_block_processor_impl::pop_symbols(unsigned block_size)
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

unsigned pdsch_block_processor_impl::get_max_block_size() const
{
  if (!codeblock_symbols.empty()) {
    return codeblock_symbols.size();
  }

  if (next_i_cb != last_i_cb + 1) {
    unsigned rm_length       = segment_buffer->get_rm_length(next_i_cb);
    unsigned bits_per_symbol = get_bits_per_symbol(modulation);
    return rm_length / bits_per_symbol;
  }

  return 0;
}

bool pdsch_block_processor_impl::empty() const
{
  return codeblock_symbols.empty() && (next_i_cb == last_i_cb + 1);
}
