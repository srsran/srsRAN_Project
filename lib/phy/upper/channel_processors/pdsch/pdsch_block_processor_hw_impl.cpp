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

#include "pdsch_block_processor_hw_impl.h"
#include "pdsch_processor_helpers.h"

using namespace srsran;

resource_grid_mapper::symbol_buffer&
pdsch_block_processor_hw_impl::configure_new_transmission(span<const uint8_t>           data,
                                                          unsigned                      i_cw,
                                                          const pdsch_processor::pdu_t& pdu,
                                                          const ldpc_segmenter_buffer&  segment_buffer_,
                                                          unsigned                      start_i_cb_,
                                                          unsigned                      cb_batch_len)
{
  // Init scrambling initial state.
  scrambler->init((static_cast<unsigned>(pdu.rnti) << 15U) + (i_cw << 14U) + pdu.n_id);

  // Advance scrambling sequence to the specific offset of the starting CB.
  scrambler->advance(segment_buffer_.get_cb_metadata(start_i_cb_).cb_specific.cw_offset);

  // Select codeword specific parameters.
  modulation = pdu.codewords[i_cw].modulation;

  cb_size = segment_buffer_.get_segment_length();

  // Initialize the codeblock processing control variables.
  next_i_cb_enq = start_i_cb_;
  next_i_cb_deq = start_i_cb_;
  start_i_cb    = start_i_cb_;
  last_i_cb     = start_i_cb_ + cb_batch_len - 1;
  enqueue       = true;
  dequeue       = false;
  nof_dequeued  = 0;

  // Save the pointer to the input data.
  transport_block = data;

  // Save the pointer to the segment buffer.
  segment_buffer = &segment_buffer_;

  return *this;
}

void pdsch_block_processor_hw_impl::enqueue_codeblocks()
{
  // Reserve a hardware-queue for the current encoding operation.
  if (next_i_cb_enq == start_i_cb) {
    encoder->reserve_queue();
  }

  // Prepare codeblock data.
  cb_data.resize(cb_size.value());

  // Enqueue a batch of new CBs into the hardware accelerator.
  bool enqueued = true;
  while (enqueued && (next_i_cb_enq < last_i_cb + 1)) {
    // Retrieve segment description.
    const codeblock_metadata cb_metadata = segment_buffer->get_cb_metadata(next_i_cb_enq);

    // Copy codeblock data, including TB and/or CB CRC if applicable, as well as filler and zero padding bits.
    segment_buffer->read_codeblock(cb_data, transport_block, next_i_cb_enq);

    // Set the encoding parameters as required by the hardware-accelerated PDSCH encoder.
    hal::hw_pdsch_encoder_configuration hw_cfg = {};
    set_hw_enc_configuration(hw_cfg, cb_metadata, next_i_cb_enq);

    // Try to enqueue a new hardware-accelerated PDSCH encoding operation.
    unsigned segment_length =
        static_cast<unsigned>(segment_buffer->get_segment_length().value()) - hw_cfg.nof_filler_bits;
    unsigned segment_length_bytes = static_cast<unsigned>(ceil(static_cast<float>(segment_length) / 8.0));
    enqueued = encoder->enqueue_operation(cb_data.get_buffer().first(segment_length_bytes), {}, next_i_cb_enq);
    if (enqueued) {
      // Increment the enqueued codeblock counter.
      ++next_i_cb_enq;
    }
  }
}

bool pdsch_block_processor_hw_impl::dequeue_codeblock(bool force_dequeue)
{
  // Rate Matching output length.
  unsigned rm_length = segment_buffer->get_rm_length(next_i_cb_deq);

  // Number of symbols.
  unsigned nof_symbols = rm_length / get_bits_per_symbol(modulation);

  // Try to dequeue one rate matched CB. If requested, iterate until success.
  temp_codeblock.resize(rm_length);
  bool dequeued = false;
  while (!dequeued) {
    // Try to dequeue one hardware-accelerated PUSCH decoding operation. No unpacking is required.
    dequeued = encoder->dequeue_operation(temp_codeblock.get_buffer(), {}, next_i_cb_deq);
    if (!dequeued && !force_dequeue) {
      break;
    } else if (dequeued) {
      // Increment the dequeued codeblock counter.
      ++next_i_cb_deq;

      // Apply scrambling sequence in-place.
      scrambler->apply_xor(temp_codeblock, temp_codeblock);

      // Apply modulation.
      codeblock_symbols = span<ci8_t>(temp_codeblock_symbols).first(nof_symbols);
      modulator->modulate(codeblock_symbols, temp_codeblock, modulation);
    }
  }

  // Free the hardware-queue utilized by the completed encoding operation.
  if (next_i_cb_deq == last_i_cb + 1) {
    encoder->free_queue();
  }

  return dequeued;
}

span<const ci8_t> pdsch_block_processor_hw_impl::pop_symbols(unsigned block_size)
{
  // Process a new codeblock if the buffer with codeblock symbols is empty.
  if (codeblock_symbols.empty()) {
    // Process one CB, while enqueueing as many CBs as possible, until all CBs are processed.
    bool new_cb_data = ((next_i_cb_enq == last_i_cb + 1) && (next_i_cb_deq == last_i_cb + 1));
    while (!new_cb_data) {
      // Enqueue a new batch of codeblocks into the hardware accelerator.
      if (enqueue && (next_i_cb_enq < last_i_cb + 1)) {
        enqueue_codeblocks();
        // No more CBs can be enqueued into the hardware accelerator.
        enqueue      = false;
        dequeue      = true;
        nof_dequeued = 0;
      } else if (next_i_cb_enq == last_i_cb + 1) {
        dequeue      = true;
        nof_dequeued = 0;
      }

      // Dequeue one codeblock from the hardware accelerator.
      bool dequeued = false;
      if (dequeue && (next_i_cb_deq < last_i_cb + 1)) {
        dequeued = dequeue_codeblock(nof_dequeued == 0);

        // No more CBs can be dequeued from the hardware accelerator.
        if (!dequeued) {
          enqueue = true;
          dequeue = false;
        }
      }
      if (dequeued) {
        // Increment dequeued codeblock counter.
        ++nof_dequeued;
        new_cb_data = true;
      }
    }
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

unsigned pdsch_block_processor_hw_impl::get_max_block_size() const
{
  if (!codeblock_symbols.empty()) {
    return codeblock_symbols.size();
  }

  if (next_i_cb_deq != segment_buffer->get_nof_codeblocks()) {
    unsigned rm_length       = segment_buffer->get_rm_length(next_i_cb_deq);
    unsigned bits_per_symbol = get_bits_per_symbol(modulation);
    return rm_length / bits_per_symbol;
  }

  return 0;
}

bool pdsch_block_processor_hw_impl::empty() const
{
  return codeblock_symbols.empty() && (next_i_cb_deq == last_i_cb + 1);
}

void pdsch_block_processor_hw_impl::set_hw_enc_configuration(hal::hw_pdsch_encoder_configuration& hw_cfg,
                                                             const codeblock_metadata&            cb_metadata,
                                                             unsigned                             cb_index)
{
  // Operation mode. CB mode is forced in this case.
  hw_cfg.cb_mode      = true;
  hw_cfg.nof_segments = 1;

  // Modulation.
  hw_cfg.modulation = modulation;

  // Redundancy version.
  hw_cfg.rv = cb_metadata.tb_common.rv;

  // Number of segments. Always 1 segment only.
  hw_cfg.nof_segments = 1;

  // CB and TB attachment is handled during segmentation.
  hw_cfg.attach_cb_crc = false;
  hw_cfg.attach_tb_crc = false;

  // Accelerator output data does not need to be unpacked.
  hw_cfg.do_unpack = false;

  // LDPC lifting size.
  hw_cfg.lifting_size = cb_metadata.tb_common.lifting_size;

  // Base graph index and length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  unsigned N              = 0;
  hw_cfg.base_graph_index = cb_metadata.tb_common.base_graph;
  if (hw_cfg.base_graph_index == ldpc_base_graph_type::BG1) {
    N = hw_cfg.lifting_size * 66;
  } else {
    N = hw_cfg.lifting_size * 50;
  }
  hw_cfg.Ncb  = N;
  hw_cfg.Nref = cb_metadata.tb_common.Nref;

  // Number of filler bits in this segment.
  hw_cfg.nof_filler_bits = cb_metadata.cb_specific.nof_filler_bits;

  // Rate-matched length of this segment in bits.
  hw_cfg.rm_length = cb_metadata.cb_specific.rm_length;

  // Set configuration in the HW accelerated encoder.
  encoder->configure_operation(hw_cfg, cb_index);
}
