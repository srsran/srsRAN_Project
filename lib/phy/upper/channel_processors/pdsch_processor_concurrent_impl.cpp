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

#include "pdsch_processor_concurrent_impl.h"
#include "pdsch_processor_validator_impl.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/support/event_tracing.h"

using namespace srsran;

void pdsch_processor_concurrent_impl::process(resource_grid_mapper&                                        mapper_,
                                              pdsch_processor_notifier&                                    notifier_,
                                              static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data_,
                                              const pdsch_processor::pdu_t&                                pdu_)
{
  // Saves inputs.
  save_inputs(mapper_, notifier_, data_, pdu_);

  // Makes sure the PDU is valid.
  pdsch_processor_validator_impl::assert_pdu(config);

  // Set the number of asynchronous tasks. It counts as CB processing and DM-RS generation.
  async_task_counter = 2;

  // Process DM-RS concurrently.
  auto dmrs_task = [this]() { process_dmrs(); };
  if (!executor.execute(dmrs_task)) {
    dmrs_task();
  }

  // Fork codeblock processing tasks.
  fork_cb_batches();
}

void pdsch_processor_concurrent_impl::save_inputs(resource_grid_mapper&     mapper_,
                                                  pdsch_processor_notifier& notifier_,
                                                  static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data_,
                                                  const pdsch_processor::pdu_t&                                pdu)
{
  using namespace units::literals;

  // Save process parameter inputs.
  mapper   = &mapper_;
  notifier = &notifier_;
  data     = data_.front();
  config   = pdu;

  // Codeword index is fix.
  static constexpr unsigned i_cw = 0;

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = config.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = compute_nof_data_re(config);

  // Calculate the total number of the chanel modulated symbols.
  nof_ch_symbols = nof_layers * nof_re_pdsch;

  // Calculate scrambling initial state.
  scrambler->init((static_cast<unsigned>(config.rnti) << 15U) + (i_cw << 14U) + config.n_id);

  // Calculate transport block size.
  tbs = units::bytes(data.size()).to_bits();

  // Calculate number of codeblocks.
  nof_cb = ldpc::compute_nof_codeblocks(tbs, config.ldpc_base_graph);

  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  unsigned nof_short_segments = nof_cb - (nof_re_pdsch % nof_cb);

  // Compute number of CRC bits for the transport block.
  units::bits nof_tb_crc_bits = ldpc::compute_tb_crc_size(tbs);

  // Compute number of CRC bits for each codeblock.
  units::bits nof_cb_crc_bits = (nof_cb > 1) ? 24_bits : 0_bits;

  // Calculate the total number of bits including transport block and codeblock CRC.
  units::bits nof_tb_bits_out = tbs + nof_tb_crc_bits + units::bits(nof_cb_crc_bits * nof_cb);

  // Compute the number of information bits that is assigned to a codeblock.
  cb_info_bits = units::bits(divide_ceil(nof_tb_bits_out.value(), nof_cb)) - nof_cb_crc_bits;

  unsigned lifting_size = ldpc::compute_lifting_size(tbs, config.ldpc_base_graph, nof_cb);
  segment_length        = ldpc::compute_codeblock_size(config.ldpc_base_graph, lifting_size);

  modulation_scheme modulation = config.codewords.front().modulation;
  units::bits       bits_per_symbol(get_bits_per_symbol(modulation));

  units::bits full_codeblock_size = ldpc::compute_full_codeblock_size(config.ldpc_base_graph, segment_length);
  units::bits cw_length           = units::bits(nof_re_pdsch * nof_layers * bits_per_symbol);
  zero_pad                        = (cb_info_bits + nof_cb_crc_bits) * nof_cb - nof_tb_bits_out;

  // Prepare codeblock metadata.
  cb_metadata.tb_common.base_graph        = config.ldpc_base_graph;
  cb_metadata.tb_common.lifting_size      = static_cast<ldpc::lifting_size_t>(lifting_size);
  cb_metadata.tb_common.rv                = config.codewords.front().rv;
  cb_metadata.tb_common.mod               = modulation;
  cb_metadata.tb_common.Nref              = units::bytes(config.tbs_lbrm_bytes).to_bits().value();
  cb_metadata.tb_common.cw_length         = cw_length.value();
  cb_metadata.cb_specific.full_length     = full_codeblock_size.value();
  cb_metadata.cb_specific.rm_length       = 0;
  cb_metadata.cb_specific.nof_filler_bits = (segment_length - cb_info_bits - nof_cb_crc_bits).value();
  cb_metadata.cb_specific.cw_offset       = 0;
  cb_metadata.cb_specific.nof_crc_bits    = nof_tb_crc_bits.value();

  // Calculate RM length for each codeblock.
  rm_length.resize(nof_cb);
  cw_offset.resize(nof_cb);
  re_offset.resize(nof_cb);
  unsigned re_count_sum = 0;
  for (unsigned i_cb = 0; i_cb != nof_cb; ++i_cb) {
    // Calculate RM length in RE.
    unsigned rm_length_re = divide_ceil(nof_re_pdsch, nof_cb);
    if (i_cb < nof_short_segments) {
      rm_length_re = nof_re_pdsch / nof_cb;
    }

    // Convert RM length from RE to bits.
    rm_length[i_cb] = rm_length_re * nof_layers * bits_per_symbol;

    // Set and increment CW offset.
    cw_offset[i_cb] = re_count_sum * nof_layers * bits_per_symbol;

    // Set RE offset for the resource mapper.
    re_offset[i_cb] = re_count_sum;

    // Increment RE count.
    re_count_sum += rm_length_re;
  }
  srsran_assert(re_count_sum * nof_layers * bits_per_symbol == cw_length,
                "RM length sum (i.e., {}) must be equal to the codeword length (i.e., {}).",
                units::bits(re_count_sum * nof_layers * bits_per_symbol),
                cw_length);

  // Get the PRB allocation mask.
  const bounded_bitset<MAX_RB> prb_allocation_mask =
      config.freq_alloc.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb);

  // First symbol used in this transmission.
  unsigned start_symbol_index = config.start_symbol_index;

  // Calculate the end symbol index (excluded) and assert it does not exceed the slot boundary.
  unsigned end_symbol_index = config.start_symbol_index + config.nof_symbols;

  srsran_assert(end_symbol_index <= MAX_NSYMB_PER_SLOT,
                "The time allocation of the transmission ({}:{}) exceeds the slot boundary.",
                start_symbol_index,
                end_symbol_index);

  // PDSCH OFDM symbol mask.
  symbol_slot_mask symbols;
  symbols.fill(start_symbol_index, end_symbol_index);

  // Allocation pattern for the mapper.
  allocation.clear();
  re_pattern pdsch_pattern;

  // Reserved REs, including DM-RS and CSI-RS.
  reserved = re_pattern_list(config.reserved);

  // Get DM-RS RE pattern.
  re_pattern dmrs_pattern = config.dmrs.get_dmrs_pattern(
      config.bwp_start_rb, config.bwp_size_rb, config.nof_cdm_groups_without_data, config.dmrs_symbol_mask);

  // Merge DM-RS RE pattern into the reserved RE patterns.
  reserved.merge(dmrs_pattern);

  // Set PDSCH allocation pattern.
  pdsch_pattern.prb_mask = prb_allocation_mask;
  pdsch_pattern.re_mask  = ~re_prb_mask();
  pdsch_pattern.symbols  = symbols;
  allocation.merge(pdsch_pattern);

  // Calculate modulation scaling.
  float scaling = convert_dB_to_amplitude(-config.ratio_pdsch_data_to_sss_dB);
  scaling *= cb_processor_pool->get().get_scaling(config.codewords.front().modulation);

  // Apply scaling to the precoding matrix.
  precoding = config.precoding;
  precoding *= scaling;
}

unsigned pdsch_processor_concurrent_impl::compute_nof_data_re(const pdu_t& config)
{
  // Get PRB mask.
  bounded_bitset<MAX_RB> prb_mask = config.freq_alloc.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb);

  // Get number of PRB.
  unsigned nof_prb = prb_mask.count();

  // Calculate the number of RE allocated in the grid.
  unsigned nof_grid_re = nof_prb * NRE * config.nof_symbols;

  // Generate DM-RS pattern.
  re_pattern dmrs_pattern = config.dmrs.get_dmrs_pattern(
      config.bwp_start_rb, config.bwp_size_rb, config.nof_cdm_groups_without_data, config.dmrs_symbol_mask);

  // Calculate the number of RE used by DM-RS. It assumes it does not overlap with reserved elements.
  unsigned nof_grid_dmrs = nof_prb * dmrs_pattern.re_mask.count() * dmrs_pattern.symbols.count();

  // Calculate the number of reserved resource elements.
  unsigned nof_reserved_re =
      config.reserved.get_inclusion_count(config.start_symbol_index, config.nof_symbols, prb_mask);

  // Subtract the number of reserved RE from the number of allocated RE.
  srsran_assert(nof_grid_re > nof_reserved_re,
                "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
                nof_grid_re,
                nof_reserved_re);
  return nof_grid_re - nof_reserved_re - nof_grid_dmrs;
}

void pdsch_processor_concurrent_impl::fork_cb_batches()
{
  // Minimum number of codeblocks per batch.
  unsigned min_cb_batch_size = 4;

  // Calculate the number of batches.
  unsigned nof_cb_batches = cb_processor_pool->capacity() * 8;

  // Limit the number of batches to ensure a minimum number of CB per batch.
  unsigned max_nof_cb_batches = divide_ceil(nof_cb, min_cb_batch_size);
  nof_cb_batches              = std::min(nof_cb_batches, max_nof_cb_batches);

  // Calculate the actual number of CB per batch.
  unsigned cb_batch_size = divide_ceil(nof_cb, nof_cb_batches);

  // Set number of codeblock batches.
  cb_batch_counter = nof_cb_batches;

  for (unsigned i_cb_batch = 0, i_cb = 0; i_cb_batch != nof_cb_batches; ++i_cb_batch, i_cb += cb_batch_size) {
    // Limit batch size for the last batch.
    cb_batch_size = std::min(nof_cb - i_cb, cb_batch_size);

    // Extract scrambling initial state for the next bit.
    pseudo_random_generator::state_s c_init = scrambler->get_state();

    auto async_task = [this, cb_batch_size, c_init, i_cb]() {
      trace_point process_pdsch_tp = l1_tracer.now();

      // Select codeblock processor.
      pdsch_codeblock_processor& cb_processor = cb_processor_pool->get();

      // Save scrambling initial state.
      pseudo_random_generator::state_s scrambling_state = c_init;

      // For each segment within the batch.
      for (unsigned batch_i_cb = 0; batch_i_cb != cb_batch_size; ++batch_i_cb) {
        // Calculate the absolute codeblock index.
        unsigned absolute_i_cb = i_cb + batch_i_cb;

        // Limit the codeblock number of information bits.
        units::bits nof_info_bits = std::min<units::bits>(cb_info_bits, tbs - cb_info_bits * absolute_i_cb);

        // Set CB processor configuration.
        pdsch_codeblock_processor::configuration cb_config;
        cb_config.tb_offset    = cb_info_bits * absolute_i_cb;
        cb_config.has_cb_crc   = nof_cb > 1;
        cb_config.cb_info_size = nof_info_bits;
        cb_config.cb_size      = segment_length;
        cb_config.zero_pad     = zero_pad;
        cb_config.metadata     = cb_metadata;
        cb_config.c_init       = scrambling_state;

        // Update codeblock specific metadata fields.
        cb_config.metadata.cb_specific.cw_offset = cw_offset[absolute_i_cb].value();
        cb_config.metadata.cb_specific.rm_length = rm_length[absolute_i_cb].value();

        // Process codeblock.
        pdsch_codeblock_processor::result result = cb_processor.process(data, cb_config);

        // Build resource grid mapper adaptor.
        resource_grid_mapper::symbol_buffer_adapter buffer(result.cb_symbols);

        // Update scrambling sequence state.
        scrambling_state = result.scrambling_state;

        // Map into the resource grid.
        mapper->map(buffer, allocation, reserved, precoding, re_offset[absolute_i_cb]);
      }

      // Decrement code block batch counter.
      if (cb_batch_counter.fetch_sub(1) == 1) {
        // Decrement asynchronous task counter.
        if (async_task_counter.fetch_sub(1) == 1) {
          // Notify end of the processing.
          notifier->on_finish_processing();
        }
      }

      l1_tracer << trace_event("CB batch", process_pdsch_tp);
    };

    // Try to execute task asynchronously.
    bool successful = false;
    if (nof_cb_batches != 0) {
      successful = executor.execute(async_task);
    }

    // Execute task locally if it was not enqueued.
    if (!successful) {
      async_task();
    }

    // Advance scrambling sequence for the next batch.
    if (i_cb_batch != nof_cb_batches - 1) {
      units::bits sequence_advance_count =
          std::accumulate(rm_length.begin() + i_cb, rm_length.begin() + i_cb + cb_batch_size, units::bits(0));
      scrambler->advance(sequence_advance_count.value());
    }
  }
}

void pdsch_processor_concurrent_impl::process_dmrs()
{
  trace_point process_dmrs_tp = l1_tracer.now();

  bounded_bitset<MAX_RB> rb_mask_bitset = config.freq_alloc.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned dmrs_reference_point_k_rb = 0;
  if (config.ref_point == pdu_t::PRB0) {
    dmrs_reference_point_k_rb = config.bwp_start_rb;
  }

  // Prepare DM-RS configuration.
  dmrs_pdsch_processor::config_t dmrs_config;
  dmrs_config.slot                 = config.slot;
  dmrs_config.reference_point_k_rb = dmrs_reference_point_k_rb;
  dmrs_config.type                 = config.dmrs;
  dmrs_config.scrambling_id        = config.scrambling_id;
  dmrs_config.n_scid               = config.n_scid;
  dmrs_config.amplitude            = convert_dB_to_amplitude(-config.ratio_pdsch_dmrs_to_sss_dB);
  dmrs_config.symbols_mask         = config.dmrs_symbol_mask;
  dmrs_config.rb_mask              = rb_mask_bitset;
  dmrs_config.precoding            = config.precoding;

  // Put DM-RS.
  dmrs_generator_pool->get().map(*mapper, dmrs_config);

  l1_tracer << trace_event("process_dmrs", process_dmrs_tp);

  // Decrement asynchronous task counter.
  if (async_task_counter.fetch_sub(1) == 1) {
    // Notify end of the processing.
    notifier->on_finish_processing();
  }
}
