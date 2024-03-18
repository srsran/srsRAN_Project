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

#include "pusch_decoder_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/rx_buffer.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

// Select the CRC for the decoder based on the TBS and the number of codeblocks.
crc_calculator* select_crc(pusch_decoder_impl::sch_crc& crcs, unsigned tbs, unsigned nof_blocks)
{
  if (nof_blocks > 1) {
    return crcs.crc24B.get();
  }
  // From here, we have only 1 codeblock.
  if (tbs > MAX_BITS_CRC16) {
    return crcs.crc24A.get();
  }
  // One short codeblock.
  return crcs.crc16.get();
}

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

// Computes the number of entire CBs present in the CW buffer, given the CB segmentation information and the number of
// available CW softbits.
static unsigned
get_nof_available_codeblocks(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
                             unsigned                                                 nof_cw_softbits)
{
  unsigned i_cb = 0;
  for (unsigned nof_cbs = described_codeblocks.size(); i_cb != nof_cbs; ++i_cb) {
    unsigned i_cb_end = described_codeblocks[i_cb].second.cb_specific.cw_offset +
                        described_codeblocks[i_cb].second.cb_specific.rm_length;

    // Stop counting codeblocks if not all softbits of the current codeblock are available.
    if (i_cb_end > nof_cw_softbits) {
      break;
    }
  }
  return i_cb;
}

pusch_decoder_buffer& pusch_decoder_impl::new_data(span<uint8_t>                       transport_block_,
                                                   unique_rx_buffer                    unique_rm_buffer_,
                                                   pusch_decoder_notifier&             notifier,
                                                   const pusch_decoder::configuration& cfg)
{
  internal_states previous_state = current_state.exchange(internal_states::collecting);
  srsran_assert(previous_state == internal_states::idle,
                "Invalid state. It was expected to be {} but it was {}.",
                to_string(internal_states::idle),
                to_string(previous_state));

  transport_block  = transport_block_;
  unique_rm_buffer = std::move(unique_rm_buffer_);
  result_notifier  = &notifier;
  current_config   = cfg;
  softbits_count   = 0;
  codeblock_llrs.clear();

  // Unset the expected number of UL-SCH softbits.
  nof_ulsch_softbits.reset();

  // Compute segmentation configuration.
  segmentation_config.base_graph = current_config.base_graph;
  segmentation_config.rv         = current_config.rv;
  segmentation_config.mod        = current_config.mod;
  segmentation_config.Nref       = current_config.Nref;
  segmentation_config.nof_layers = current_config.nof_layers;

  // Set the CB counters.
  unsigned tb_size     = transport_block.size() * BITS_PER_BYTE;
  nof_codeblocks       = ldpc::compute_nof_codeblocks(units::bits(tb_size), segmentation_config.base_graph);
  cb_task_counter      = nof_codeblocks;
  available_cb_counter = 0;

  srsran_assert(nof_codeblocks == unique_rm_buffer->get_nof_codeblocks(),
                "Wrong number of codeblocks {} (expected {}).",
                unique_rm_buffer->get_nof_codeblocks(),
                nof_codeblocks);

  // Select CRC calculator for inner codeblock checks.
  block_crc = select_crc(crc_set, tb_size, nof_codeblocks);

  // Reset CRCs if new data is flagged.
  span<bool> cb_crcs = unique_rm_buffer->get_codeblocks_crc();
  if (current_config.new_data) {
    srsvec::zero(cb_crcs);
  }

  return *this;
}

span<log_likelihood_ratio> pusch_decoder_impl::get_next_block_view(unsigned block_size)
{
  internal_states state = current_state.load();
  srsran_assert(state == internal_states::collecting,
                "Invalid state. It was expected to be {} but it was {}.",
                to_string(internal_states::collecting),
                state);

  // Makes sure the block size does not overflow the buffer.
  srsran_assert(softbits_count + block_size <= softbits_buffer.size(),
                "The sum of current buffer number of elements (i.e., {}) and the block size (i.e., {}), exceeds the "
                "total number of elements of the buffer (i.e., {}).",
                softbits_count,
                block_size,
                softbits_buffer.size());

  return span<log_likelihood_ratio>(softbits_buffer).subspan(softbits_count, block_size);
}

void pusch_decoder_impl::set_nof_softbits(units::bits nof_softbits)
{
  internal_states state = current_state.load();
  srsran_assert(state == internal_states::collecting,
                "Invalid state. It was expected to be {} but it was {}.",
                to_string(internal_states::collecting),
                to_string(state));

  nof_ulsch_softbits.emplace(nof_softbits);

  unsigned modulation_order = get_bits_per_symbol(current_config.mod);
  srsran_assert(nof_ulsch_softbits->value() % modulation_order == 0,
                "The number of soft bits (i.e., {}) must be multiple of the modulation order (i.e., {}).",
                nof_ulsch_softbits->value(),
                modulation_order);

  // Set derived parameters.
  segmentation_config.nof_ch_symbols = nof_ulsch_softbits->value() / modulation_order;

  // Select view of LLRs.
  span<const log_likelihood_ratio> llrs =
      span<const log_likelihood_ratio>(softbits_buffer).first(nof_ulsch_softbits->value());

  // Recall that the TB is in packed format.
  unsigned tb_size = transport_block.size() * BITS_PER_BYTE;

  // Generate segmentation information and CB views.
  segmenter->segment(codeblock_llrs, llrs, tb_size, segmentation_config);

  // All information about the CW segments should be available.
  srsran_assert(
      nof_codeblocks == codeblock_llrs.size(),
      "The number of described CW segments, i.e., {}, does not match the expected number of codeblocks, i.e., {}.",
      codeblock_llrs.size(),
      nof_codeblocks);

  // Compute the number of currently available codeblocks.
  available_cb_counter = get_nof_available_codeblocks(codeblock_llrs, softbits_count);

  // If there are available codeblocks, start as many decoding tasks as possible.
  if (available_cb_counter > 0) {
    // Fork tasks for new codeblocks.
    for (unsigned i_cb = 0; i_cb != available_cb_counter; ++i_cb) {
      fork_codeblock_task(i_cb);
    }
  }
}

void pusch_decoder_impl::on_new_softbits(span<const log_likelihood_ratio> softbits)
{
  internal_states state = current_state.load();
  srsran_assert(state == internal_states::collecting,
                "Invalid state. It was expected to be {} but it was {}.",
                to_string(internal_states::collecting),
                state);

  span<log_likelihood_ratio> block = get_next_block_view(softbits.size());

  // Copy only if the soft bits do not match.
  if (block.data() != softbits.data()) {
    srsvec::copy(block, softbits);
  }

  softbits_count += softbits.size();

  if (nof_ulsch_softbits.has_value()) {
    // Codeblocks that have already been dispatched to the decoder.
    unsigned dispatched_codeblocks = available_cb_counter;

    // Update the number of currently available codeblocks.
    available_cb_counter = get_nof_available_codeblocks(codeblock_llrs, softbits_count);

    // Fork tasks for new codeblocks.
    for (unsigned i_cb = dispatched_codeblocks, i_cb_end = available_cb_counter; i_cb != i_cb_end; ++i_cb) {
      fork_codeblock_task(i_cb);
    }
  }
}

void pusch_decoder_impl::on_end_softbits()
{
  // Verify the number of bits match with the configured one.
  srsran_assert(!nof_ulsch_softbits.has_value() || (nof_ulsch_softbits.value() == units::bits(softbits_count)),
                "The number of UL-SCH softbits, i.e., {}, does not match the expected value, i.e., {}.",
                softbits_count,
                nof_ulsch_softbits.value());

  unsigned modulation_order = get_bits_per_symbol(current_config.mod);
  srsran_assert(softbits_count % modulation_order == 0,
                "The number of soft bits (i.e., {}) must be multiple of the modulation order (i.e., {}).",
                softbits_count,
                modulation_order);

  // Skip processing if all codeblock decoding tasks have already been dispatched. This should be the case if the
  // number of CW softbits has been provided by calling set_nof_sofbits.
  if (available_cb_counter == nof_codeblocks) {
    srsran_assert(nof_ulsch_softbits->value() == softbits_count,
                  "The number of provided softbits (i.e. {}), does not match the expected number (i.e. {}).",
                  softbits_count,
                  nof_ulsch_softbits->value());

    // Try to transition from collecting to decoding.
    internal_states expected_state = internal_states::collecting;
    if (!current_state.compare_exchange_strong(expected_state, internal_states::decoding)) {
      // If the previous state was not collecting, the only valid state is decoded.
      internal_states state = current_state.load();
      srsran_assert(state == internal_states::decoded,
                    "Invalid state. It was expected to be {} but it was {}.",
                    to_string(internal_states::decoded),
                    to_string(state));

      // In this case, all the codeblocks have already been decoded. Notify the completion.
      join_and_notify();
    }

    // All codeblocks have already been forked. There is nothing else to do.
    return;
  }

  // Transition to decoding.
  internal_states previous_state = current_state.exchange(internal_states::decoding);
  srsran_assert(previous_state == internal_states::collecting,
                "Invalid state. It was expected to be {} but it was {}.",
                to_string(internal_states::collecting),
                to_string(previous_state));

  srsran_assert(!nof_ulsch_softbits.has_value(),
                "The number of CW softbits has been provided and not all CB decoding tasks have been dispatched.");

  // Compute segmentation.
  span<const log_likelihood_ratio> llrs = span<const log_likelihood_ratio>(softbits_buffer).first(softbits_count);

  // Recall that the TB is in packed format.
  unsigned tb_size                   = transport_block.size() * BITS_PER_BYTE;
  segmentation_config.nof_ch_symbols = softbits_count / modulation_order;
  segmenter->segment(codeblock_llrs, llrs, tb_size, segmentation_config);

  // All information about the CW segments should be available.
  srsran_assert(
      nof_codeblocks == codeblock_llrs.size(),
      "The number of described CW segments, i.e., {}, does not match the expected number of codeblocks, i.e., {}.",
      codeblock_llrs.size(),
      nof_codeblocks);

  // Iterate for each remaining code block.
  for (unsigned cb_id = 0; cb_id != nof_codeblocks; ++cb_id) {
    fork_codeblock_task(cb_id);
  }
}

void pusch_decoder_impl::fork_codeblock_task(unsigned cb_id)
{
  auto cb_process_task = [this, cb_id]() {
    span<const log_likelihood_ratio> cb_llrs = codeblock_llrs[cb_id].first;
    const codeblock_metadata&        cb_meta = codeblock_llrs[cb_id].second;
    srsran_assert(cb_llrs.size() == cb_meta.cb_specific.rm_length, "Wrong rate-matched codeblock length.");

    // Get codeblock length, without rate matching, the message length and the number of data bits (no CRC, no filler
    // bits - may contain zero-padding).
    unsigned cb_length = 0, msg_length = 0, nof_data_bits = 0;
    std::tie(cb_length, msg_length, nof_data_bits) = get_cblk_bit_breakdown(cb_meta);

    // Get data bits from previous transmissions, if any.
    // Messages are written on a dedicated buffer associated to the softbuffer. By doing this, we keep the decoded
    // message in memory and we don't need to compute it again if there is a retransmission.
    bit_buffer message = unique_rm_buffer->get_codeblock_data_bits(cb_id, msg_length);

    // Get the LLRs from previous transmissions, if any, or a clean buffer.
    span<log_likelihood_ratio> rm_buffer = unique_rm_buffer->get_codeblock_soft_bits(cb_id, cb_length);

    span<bool> cb_crcs = unique_rm_buffer->get_codeblocks_crc();

    // Code block processing task.
    trace_point tp = l1_tracer.now();

    // Check current CRC status.
    if (cb_crcs[cb_id]) {
      // Dematch the new LLRs and combine them with the ones from previous transmissions. We do this everytime,
      // including when the CRC for the codeblock is OK (from previous retransmissions), because we may need to
      // decode it again if, eventually, we find out that the CRC of the entire transport block is KO.
      decoder_pool->get().rate_match(rm_buffer, cb_llrs, current_config.new_data, cb_meta);

      if (cb_task_counter.fetch_sub(1) == 1) {
        join_and_notify();
      }
      return;
    }

    // Try to decode.
    optional<unsigned> nof_iters = decoder_pool->get().decode(message,
                                                              rm_buffer,
                                                              cb_llrs,
                                                              current_config.new_data,
                                                              block_crc->get_generator_poly(),
                                                              current_config.use_early_stop,
                                                              current_config.nof_ldpc_iterations,
                                                              cb_meta);

    if (nof_iters.has_value()) {
      // If successful decoding, flag the CRC, record number of iterations and copy bits to the TB buffer.
      cb_crcs[cb_id] = true;
      cb_stats.push_blocking(nof_iters.value());
    } else {
      cb_stats.push_blocking(current_config.nof_ldpc_iterations);
    }

    if (cb_task_counter.fetch_sub(1) == 1) {
      join_and_notify();
    }

    l1_tracer << trace_event("cb_decode", tp);
  };

  // Execute task asynchronously if an executor is available and the number of codeblocks is larger than one.
  bool enqueued = false;
  if ((executor != nullptr) && (nof_codeblocks > 1)) {
    enqueued = executor->execute(cb_process_task);
  }

  // Process task synchronously if is not successfully enqueued.
  if (!enqueued) {
    cb_process_task();
  }
}

void pusch_decoder_impl::join_and_notify()
{
  // Transition to decoded if the current state is collecting. In this case, skip notifying.
  internal_states expected_state_collecting = internal_states::collecting;
  if (current_state.compare_exchange_strong(expected_state_collecting, internal_states::decoded)) {
    return;
  }

  unsigned   nof_cbs = codeblock_llrs.size();
  span<bool> cb_crcs = unique_rm_buffer->get_codeblocks_crc();

  // Initialize decoder status.
  pusch_decoder_result stats;
  stats.tb_crc_ok            = false;
  stats.nof_codeblocks_total = nof_cbs;
  stats.ldpc_decoder_stats.reset();

  // Calculate statistics.
  optional<unsigned> cb_nof_iter = cb_stats.try_pop();
  while (cb_nof_iter.has_value()) {
    stats.ldpc_decoder_stats.update(cb_nof_iter.value());
    cb_nof_iter = cb_stats.try_pop();
  }

  if (nof_cbs == 1) {
    // When only one codeblock, the CRC of codeblock and transport block are the same.
    stats.tb_crc_ok = cb_crcs[0];

    // Copy the code block only nif the CRC is OK.
    if (stats.tb_crc_ok) {
      const bit_buffer cb_data = unique_rm_buffer->get_codeblock_data_bits(0, transport_block.size() * BITS_PER_BYTE);
      srsvec::copy(transport_block, cb_data.get_buffer());
    }
  } else if (std::all_of(cb_crcs.begin(), cb_crcs.end(), [](bool a) { return a; })) {
    // When more than one codeblock, we need to check the global transport block CRC. Note that there is no need to
    // compute the CRC if any of the codeblocks was not decoded correctly.
    unsigned tb_checksum = concatenate_codeblocks();

    bit_buffer tb_data = bit_buffer::from_bytes(transport_block);
    if (crc_set.crc24A->calculate(tb_data) == tb_checksum) {
      stats.tb_crc_ok = true;
    } else {
      // If the checksum is wrong, then at least one of the codeblocks is a false negative. Reset all of them.
      unique_rm_buffer->reset_codeblocks_crc();
    }
  }

  // Release soft buffer if the CRC is OK, otherwise unlock.
  if (stats.tb_crc_ok) {
    unique_rm_buffer.release();
  } else {
    unique_rm_buffer.unlock();
  }

  // In case there are multiple codeblocks and at least one has a corrupted codeblock CRC, nothing to do.

  // Finally report decoding result.
  result_notifier->on_sch_data(stats);

  // Transition back to idle.
  internal_states previous_state = current_state.exchange(internal_states::idle);
  srsran_assert((previous_state == internal_states::decoding) || (previous_state == internal_states::decoded),
                "Invalid state. It expected to be {} or {} but it was {}.",
                to_string(internal_states::decoding),
                to_string(internal_states::decoded),
                to_string(previous_state));
}

unsigned pusch_decoder_impl::concatenate_codeblocks()
{
  unsigned   nof_cbs = codeblock_llrs.size();
  bit_buffer tb_data = bit_buffer::from_bytes(transport_block);

  // Transport block write position. Bit index where the code block is copied.
  unsigned tb_offset = 0;

  unsigned tb_checksum = 0;

  for (unsigned cb_id = 0; cb_id != nof_cbs; ++cb_id) {
    const span<const log_likelihood_ratio>& cb_llrs = codeblock_llrs[cb_id].first;
    const codeblock_metadata&               cb_meta = codeblock_llrs[cb_id].second;
    srsran_assert(cb_llrs.size() == cb_meta.cb_specific.rm_length, "Wrong rate-matched codeblock length.");

    // Get codeblock length, without rate matching, the message length and the number of data bits (no CRC, no filler
    // bits - may contain zero-padding).
    unsigned cb_length = 0, msg_length = 0, nof_data_bits = 0;
    std::tie(cb_length, msg_length, nof_data_bits) = get_cblk_bit_breakdown(cb_meta);

    // Number of TB bits still "empty".
    unsigned free_tb_bits = tb_data.size() - tb_offset;
    // Avoid including zero-padding in the TB.
    unsigned nof_new_bits = std::min(free_tb_bits, nof_data_bits);

    // Get code block data from the buffer.
    bit_buffer cb_data = unique_rm_buffer->get_codeblock_data_bits(cb_id, nof_data_bits);

    // Copy the decoded code block into the transport block buffer.
    srsvec::copy_offset(tb_data, tb_offset, cb_data, 0, nof_new_bits);

    // Pack checksum if it is the last code block.
    if (cb_id == nof_cbs - 1) {
      for (unsigned i_byte = 0; i_byte != 3; ++i_byte) {
        tb_checksum = (tb_checksum << 8U) | cb_data.extract(nof_new_bits + i_byte * 8, 8);
      }
    }

    // Increment transport block offset.
    tb_offset += nof_new_bits;
  }

  srsran_assert(tb_offset == tb_data.size(), "All TB bits should be filled at this point.");

  return tb_checksum;
}
