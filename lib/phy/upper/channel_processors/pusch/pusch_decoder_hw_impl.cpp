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

#include "pusch_decoder_hw_impl.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/rx_buffer.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

// Number of bits in one byte.
static constexpr unsigned BITS_PER_BYTE = 8;

// Maximum TBS that implies a 16-bit CRC.
static constexpr unsigned MAX_BITS_CRC16 = 3824;

// Computes the TB size in bits including the CRC. The CRC is accounted for only when there are multiple codeblocks.
// Otherwise, one the TB consists of a single codeblock, tb_and_crc_size = tb_size. The input is the TB size (in bits)
// and the number of codeblocks.
static unsigned get_tb_and_crc_size(unsigned tb_size, unsigned nof_cbs)
{
  unsigned tb_and_crc_size = tb_size;
  // If only one codeblock is transmitted, the CRC is taken into account by the decoder. If more than one codeblock is
  // transmitted, there is an extra CRC of length 24 bits.
  if (nof_cbs > 1) {
    tb_and_crc_size += LONG_CRC_LENGTH;
  }

  return tb_and_crc_size;
};

// Returns, in order, the codeblock length, the message length and the number of data bits.
// The message length is the number of systematic bits of the codeblock. This includes data and, if applicable, CRC,
// zero padding and filler bits.
static std::tuple<unsigned, unsigned, unsigned> get_cblk_bit_breakdown(const codeblock_metadata& cb_meta)
{
  // Get codeblock length, without rate matching.
  unsigned cb_length = cb_meta.cb_specific.full_length;

  // Compute the message length.
  // BG1 has rate 1/3 and BG2 has rate 1/5.
  constexpr unsigned INVERSE_BG1_RATE = 3;
  constexpr unsigned INVERSE_BG2_RATE = 5;
  unsigned           inverse_rate =
      (cb_meta.tb_common.base_graph == ldpc_base_graph_type::BG1) ? INVERSE_BG1_RATE : INVERSE_BG2_RATE;
  unsigned msg_length = cb_length / inverse_rate;

  // Number of data bits (no CRC, no filler bits - may contain zero-padding).
  unsigned nof_data_bits = msg_length - cb_meta.cb_specific.nof_crc_bits - cb_meta.cb_specific.nof_filler_bits;

  return {cb_length, msg_length, nof_data_bits};
}

static std::tuple<unsigned, hal::hw_dec_cb_crc_type> set_crc_type(unsigned nof_cbs, unsigned tb_size)
{
  unsigned                crc_len  = 16;
  hal::hw_dec_cb_crc_type crc_type = hal::hw_dec_cb_crc_type::CRC16;

  // CBs in a segmented TB always use CRC24B.
  if (nof_cbs > 1) {
    crc_len  = 24;
    crc_type = hal::hw_dec_cb_crc_type::CRC24B;
    // Large CBs in non-segmented TBs use CRC24A.
  } else if (tb_size > MAX_BITS_CRC16) {
    crc_len  = 24;
    crc_type = hal::hw_dec_cb_crc_type::CRC24A;
  }

  return {crc_len, crc_type};
}

pusch_decoder_buffer& pusch_decoder_hw_impl::new_data(span<uint8_t>                       transport_block_,
                                                      unique_rx_buffer                    softbuffer_,
                                                      pusch_decoder_notifier&             notifier,
                                                      const pusch_decoder::configuration& cfg)
{
  transport_block = transport_block_;
  softbuffer      = std::move(softbuffer_);
  result_notifier = &notifier;
  current_config  = cfg;
  softbits_count  = 0;
  return *this;
}

span<log_likelihood_ratio> pusch_decoder_hw_impl::get_next_block_view(unsigned block_size)
{
  // Makes sure the block size does not overflow the buffer.
  srsran_assert(softbits_count + block_size <= softbits_buffer.size(),
                "The sum of current buffer number of elements (i.e., {}) and the block size (i.e., {}), exceeds the "
                "total number of elements of the buffer (i.e., {}).",
                softbits_count,
                block_size,
                softbits_buffer.size());

  return span<log_likelihood_ratio>(softbits_buffer).subspan(softbits_count, block_size);
}

void pusch_decoder_hw_impl::on_new_softbits(span<const log_likelihood_ratio> softbits)
{
  span<log_likelihood_ratio> block = get_next_block_view(softbits.size());

  // Copy only if the soft bits do not match.
  if (block.data() != softbits.data()) {
    srsvec::copy(block, softbits);
  }

  softbits_count += softbits.size();
}

void pusch_decoder_hw_impl::on_end_softbits()
{
  unsigned modulation_order = get_bits_per_symbol(current_config.mod);
  srsran_assert(softbits_count % modulation_order == 0,
                "The number of soft bits (i.e., {}) must be multiple of the modulation order (i.e., {}).\n",
                softbits_count,
                modulation_order);

  segmenter_config segmentation_config;
  segmentation_config.base_graph     = current_config.base_graph;
  segmentation_config.rv             = current_config.rv;
  segmentation_config.mod            = current_config.mod;
  segmentation_config.Nref           = current_config.Nref;
  segmentation_config.nof_layers     = current_config.nof_layers;
  segmentation_config.nof_ch_symbols = softbits_count / modulation_order;

  // Select view of LLRs.
  span<const log_likelihood_ratio> llrs = span<const log_likelihood_ratio>(softbits_buffer).first(softbits_count);

  // Recall that the TB is in packed format.
  unsigned tb_size = transport_block.size() * BITS_PER_BYTE;
  codeblock_llrs.clear();
  segmenter->segment(codeblock_llrs, llrs, tb_size, segmentation_config);

  unsigned nof_cbs = codeblock_llrs.size();
  srsran_assert(nof_cbs == softbuffer->get_nof_codeblocks(),
                "Wrong number of codeblocks {} (expected {}).",
                softbuffer->get_nof_codeblocks(),
                nof_cbs);

  unsigned tb_and_crc_size = get_tb_and_crc_size(tb_size, nof_cbs);
  tmp_tb_bits.resize(tb_and_crc_size);

  // Set the CB CRC type to handle it correctly.
  unsigned                crc_len;
  hal::hw_dec_cb_crc_type crc_type;
  std::tie(crc_len, crc_type) = set_crc_type(nof_cbs, tb_size);

  // Reset CRCs if new data is flagged.
  span<bool> cb_crcs = softbuffer->get_codeblocks_crc();
  if (current_config.new_data) {
    srsvec::zero(cb_crcs);
  }

  // Initialize decoder status.
  pusch_decoder_result stats = {};

  stats.nof_codeblocks_total = nof_cbs;
  stats.ldpc_decoder_stats.reset();
  unsigned last_enqueued_cb_id = 0, last_dequeued_cb_id = 0, enq_tb_offset = 0, deq_tb_offset = 0;
  bool     all_enqueued = false, all_dequeued = false;
  bool     external_harq = decoder->is_external_harq_supported();

  // Validate that all CBs have been succesfully enqueued and dequeued.
  while (!all_enqueued || !all_dequeued) {
    // Enqueue each CB in the TB
    bool enqueued = false;

    for (unsigned cb_id = last_enqueued_cb_id, last_cb_id = nof_cbs; cb_id != last_cb_id; ++cb_id) {
      // Update the pointer to the CB to be currently enqueued.
      last_enqueued_cb_id = cb_id;
      const auto& cb_llrs = codeblock_llrs[cb_id].first;
      const auto& cb_meta = codeblock_llrs[cb_id].second;
      srsran_assert(cb_llrs.size() == cb_meta.cb_specific.rm_length, "Wrong rate-matched codeblock length.");

      // Get codeblock length, without rate matching, the message length and the number of data bits (no CRC, no filler
      // bits - may contain zero-padding).
      unsigned cb_length = 0, msg_length = 0, nof_data_bits = 0;
      std::tie(cb_length, msg_length, nof_data_bits) = get_cblk_bit_breakdown(cb_meta);

      // Number of TB bits still "empty".
      unsigned free_tb_bits = tb_and_crc_size - enq_tb_offset;
      // Avoid including zero-padding in the TB.
      unsigned nof_new_bits = std::min(free_tb_bits, nof_data_bits);

      if (!cb_crcs[cb_id]) {
        // Get the unique absolute identifier of the current codeblock.
        absolute_cb_ids[cb_id] = softbuffer->get_absolute_codeblock_id(cb_id);

        // Set the decoding parameters of the CB as required by the hardware-accelerated PUSCH decoder.
        set_hw_dec_configuration(nof_cbs,
                                 cb_meta.cb_specific.rm_length,
                                 static_cast<unsigned>(cb_meta.tb_common.lifting_size),
                                 nof_new_bits,
                                 cb_meta.cb_specific.nof_filler_bits,
                                 crc_len,
                                 crc_type,
                                 cb_id);

        // Get the LLRs from previous transmissions, if any, or a clean buffer.
        span<log_likelihood_ratio> rm_buffer = {};
        if (!external_harq) {
          rm_buffer = softbuffer->get_codeblock_soft_bits(cb_id, cb_length);
        }

        span<const int8_t> cb_llrs_i8 =
            span<const int8_t>(reinterpret_cast<const int8_t*>(cb_llrs.data()), cb_llrs.size());

        span<const int8_t> rm_buffer_i8 =
            span<const int8_t>(reinterpret_cast<const int8_t*>(rm_buffer.data()), rm_buffer.size());

        // Enqueue the hardware-accelerated PUSCH decoding operation.
        enqueued = decoder->enqueue_operation(cb_llrs_i8, rm_buffer_i8, cb_id);
        // Exit the enqueing loop in case the operation couldn't be enqueued.
        if (!enqueued) {
          break;
        }
      }

      // Update the enqueuing offset within the TB.
      enq_tb_offset += nof_new_bits;
      // Enqueue a single operation when using the host memory to implement the HARQ buffer.
      if (!external_harq) {
        break;
      }
    }
    // The loop is exited when all CBs in the TB have been enqueued, the queue is full or because enqueuing failed for a
    // given CB.
    if (enqueued) {
      // Update the enqueuing point to ensure that each CB is only enqueued once.
      if (last_enqueued_cb_id == nof_cbs - 1) {
        ++last_enqueued_cb_id;
        all_enqueued = true;
        // Force updating of the last enqueued CB when using the host memory to implement the HARQ buffer.
      } else if (!external_harq) {
        ++last_enqueued_cb_id;
      }
    }

    // Reset the dequeuing control variables.
    unsigned num_dequeued = 0;
    bool     dequeued     = false;

    // Dequeue each CB in the TB
    for (unsigned cb_id = last_dequeued_cb_id, last_cb_id = last_enqueued_cb_id; cb_id != last_cb_id; ++cb_id) {
      // Update the pointer to the CB to be currently dequeued.
      last_dequeued_cb_id = cb_id;
      const auto& cb_llrs = codeblock_llrs[cb_id].first;
      const auto& cb_meta = codeblock_llrs[cb_id].second;
      srsran_assert(cb_llrs.size() == cb_meta.cb_specific.rm_length, "Wrong rate-matched codeblock length.");

      // Get codeblock length, without rate matching, the message length and the number of data bits (no CRC, no filler
      // bits - may contain zero-padding).
      unsigned cb_length = 0, msg_length = 0, nof_data_bits = 0;
      std::tie(cb_length, msg_length, nof_data_bits) = get_cblk_bit_breakdown(cb_meta);

      // Get data bits from previous transmissions, if any.
      // Messages are written on a dedicated buffer associated to the softbuffer. By doing this, we keep the decoded
      // message in memory and we don't need to compute it again if there is a retransmission.
      bit_buffer message = softbuffer->get_codeblock_data_bits(cb_id, msg_length);

      // Number of TB bits still "empty".
      unsigned free_tb_bits = tb_and_crc_size - deq_tb_offset;
      // Avoid including zero-padding in the TB.
      unsigned nof_new_bits = std::min(free_tb_bits, nof_data_bits);

      if (!cb_crcs[cb_id]) {
        // Get the LLRs from previous transmissions, if any, or a clean buffer.
        span<log_likelihood_ratio> codeblock = {};
        if (!external_harq) {
          codeblock = softbuffer->get_codeblock_soft_bits(cb_id, cb_length);
        }

        // Convert buffer to 8-bit integer.
        span<int8_t> codeblock_i8 = span<int8_t>(reinterpret_cast<int8_t*>(codeblock.data()), codeblock.size());

        // Make sure at least one operation is dequeued.
        dequeued = false;
        while (!dequeued) {
          // Dequeue the hardware-accelerated PUSCH decoding operation (updates the softbuffer too).
          dequeued = decoder->dequeue_operation(message.get_buffer(), codeblock_i8, cb_id);
          if (!dequeued) {
            if (num_dequeued > 0) {
              break;
            }
          } else {
            ++num_dequeued;

            // Check the CB decoding operation success.
            check_hw_results(stats, cb_crcs, cb_id, crc_type, message.first(nof_new_bits + crc_len));
          }
        }

        // Exit the dequeueing loop in case no more operations can be dequeued.
        if (!dequeued) {
          break;
        }
      }

      // Copy the decoded code block into the transport block buffer.
      srsvec::copy_offset(tmp_tb_bits, deq_tb_offset, message, 0, nof_new_bits);

      // Update the dequeuing offset within the TB.
      deq_tb_offset += nof_new_bits;
    }
    // The loop is exited when all enqueued CBs have been dequeued or because dequeuing failed for a given CB.
    if (dequeued) {
      // Dequeueing of a given CB only has to take place once.
      ++last_dequeued_cb_id;
      if (last_dequeued_cb_id == nof_cbs) {
        all_dequeued = true;
      }
    }
  }
  srsran_assert(deq_tb_offset == tb_and_crc_size, "All TB bits should be filled at this point.");

  copy_tb_and_notify(stats, nof_cbs, cb_crcs);
}

void pusch_decoder_hw_impl::check_hw_results(pusch_decoder_result&   stats,
                                             span<bool>              cb_crcs,
                                             unsigned                cb_id,
                                             hal::hw_dec_cb_crc_type crc_type,
                                             bit_buffer              data)
{
  hal::hw_pusch_decoder_outputs hw_out = {};
  decoder->read_operation_outputs(hw_out, cb_id, absolute_cb_ids[cb_id]);
  // CRC24B is always checked by the accelerator.
  if (crc_type != hal::hw_dec_cb_crc_type::CRC24B) {
    // CRC24A and CRC16 are not checked by the accelerator.
    if (crc_type == hal::hw_dec_cb_crc_type::CRC24A) {
      hw_out.CRC_pass = (crc_set.crc24A->calculate(data) == 0);
    } else {
      hw_out.CRC_pass = (crc_set.crc16->calculate(data) == 0);
    }
  }

  cb_crcs[cb_id] = hw_out.CRC_pass;
  stats.ldpc_decoder_stats.update(hw_out.nof_ldpc_iterations);
}

void pusch_decoder_hw_impl::copy_tb_and_notify(pusch_decoder_result& stats, unsigned nof_cbs, span<bool> cb_crcs)
{
  stats.tb_crc_ok = false;
  if (nof_cbs == 1) {
    // When only one codeblock, the CRC of codeblock and transport block are the same.
    stats.tb_crc_ok = cb_crcs[0];
    if (stats.tb_crc_ok) {
      srsvec::copy(transport_block, tmp_tb_bits.get_buffer().first(transport_block.size()));
      // Free the HARQ context entries.
      for (unsigned cb_idx = 0, cb_idx_end = nof_cbs; cb_idx != cb_idx_end; ++cb_idx) {
        decoder->free_harq_context_entry(absolute_cb_ids[cb_idx]);
      }
    }
  } else if (std::all_of(cb_crcs.begin(), cb_crcs.end(), [](bool a) { return a; })) {
    // When more than one codeblock, we need to check the global transport block CRC. Note that there is no need to
    // compute the CRC if any of the codeblocks was not decoded correctly.
    srsvec::copy(transport_block, tmp_tb_bits.get_buffer().first(transport_block.size()));

    if (crc_set.crc24A->calculate(tmp_tb_bits) == 0) {
      stats.tb_crc_ok = true;
      // Free the HARQ context entries.
      for (unsigned cb_idx = 0, cb_idx_end = nof_cbs; cb_idx != cb_idx_end; ++cb_idx) {
        decoder->free_harq_context_entry(absolute_cb_ids[cb_idx]);
      }
    } else {
      // If the checksum is wrong, then at least one of the codeblocks is a false negative. Reset all of them.
      softbuffer->reset_codeblocks_crc();
    }
  }

  // Release soft buffer if the CRC is OK, otherwise unlock.
  if (stats.tb_crc_ok) {
    softbuffer.release();
  } else {
    softbuffer.unlock();
  }

  // In case there are multiple codeblocks and at least one has a corrupted codeblock CRC, nothing to do.

  // Finally report decoding result.
  result_notifier->on_sch_data(stats);
}

void pusch_decoder_hw_impl::set_hw_dec_configuration(unsigned                nof_segments,
                                                     unsigned                rm_length,
                                                     unsigned                lifting_size,
                                                     unsigned                nof_segment_bits,
                                                     unsigned                nof_filler_bits,
                                                     unsigned                crc_len,
                                                     hal::hw_dec_cb_crc_type crc_type,
                                                     unsigned                cb_index)
{
  hal::hw_pusch_decoder_configuration hw_cfg = {};

  // Modulation.
  hw_cfg.modulation = current_config.mod;

  // Redundancy version.
  hw_cfg.rv = current_config.rv;

  // Number of segments.
  hw_cfg.nof_segments = nof_segments;

  // Codeword length in bits (E).
  hw_cfg.cw_length = rm_length;

  // LDPC lifting size.
  hw_cfg.lifting_size = lifting_size;

  // Base graph index and length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  unsigned N              = 0;
  hw_cfg.base_graph_index = current_config.base_graph;
  if (current_config.base_graph == ldpc_base_graph_type::BG1) {
    N = lifting_size * 66;
  } else {
    N = lifting_size * 50;
  }
  hw_cfg.Ncb  = N;
  hw_cfg.Nref = current_config.Nref;

  // Number of information bits that is assigned to a segment.
  hw_cfg.nof_segment_bits = nof_segment_bits;

  // Number of filler bits in this segment.
  hw_cfg.nof_filler_bits = nof_filler_bits;

  // Maximum number of iterations of the LDPC decoder.
  hw_cfg.max_nof_ldpc_iterations = current_config.nof_ldpc_iterations;

  // Flag for LDPC decoder early stopping: \c true to activate.
  hw_cfg.use_early_stop = current_config.use_early_stop;

  // Flag to denote new data (first HARQ transmission).
  hw_cfg.new_data = current_config.new_data;

  // CB CRC length and type.
  hw_cfg.cb_crc_len  = crc_len;
  hw_cfg.cb_crc_type = crc_type;

  // Unique absolute CB identifier.
  hw_cfg.absolute_cb_id = absolute_cb_ids[cb_index];

  // Set configuration in the HW accelerated decoder.
  decoder->configure_operation(hw_cfg, cb_index);
}
