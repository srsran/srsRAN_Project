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

#include "pdsch_processor_flexible_impl.h"
#include "pdsch_processor_helpers.h"
#include "pdsch_processor_validator_impl.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/ran/ptrs/ptrs_pattern.h"
#include "srsran/support/rtsan.h"
#include "srsran/support/tracing/event_tracing.h"

using namespace srsran;

/// \brief Looks at the output of the validator and, if unsuccessful, fills \c msg with the error message.
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

void pdsch_processor_flexible_impl::process(resource_grid_writer&                                           grid_,
                                            pdsch_processor_notifier&                                       notifier_,
                                            static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data_,
                                            const pdsch_processor::pdu_t&                                   pdu_)
{
  // Makes sure the PDU is valid.
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, pdsch_processor_validator_impl().is_valid(pdu_)), "{}", msg);

  // Initialize transmission and save inputs.
  initialize_new_transmission(grid_, notifier_, std::move(data_), pdu_);

  if (!async_proc) {
    trace_point process_pdsch_tp = l1_dl_tracer.now();

    // Synchronous CB processing.
    sync_pdsch_cb_processing();

    l1_dl_tracer << trace_event("sync_pdsch_cb_processing", process_pdsch_tp);
  } else {
    // Calculate modulation scaling.
    float scaling = convert_dB_to_amplitude(-config.ratio_pdsch_data_to_sss_dB);

    // Apply modulation mapper scaling to the precoding matrix.
    scaling *= modulation_mapper::get_modulation_scaling(config.codewords.front().modulation);
    precoding = config.precoding;
    precoding *= scaling;

    // Set the number of asynchronous tasks. It counts as CB processing and DM-RS generation.
    async_task_counter = 2;

    // Add PT-RS to the asynchronous tasks.
    if (config.ptrs) {
      ++async_task_counter;

      // Process PT-RS concurrently.
      auto ptrs_task = [this]() noexcept SRSRAN_RTSAN_NONBLOCKING {
        auto execute_on_exit = make_scope_exit([this]() {
          // Decrement asynchronous task counter.
          if (async_task_counter.fetch_sub(1) == 1) {
            // Notify end of the processing.
            notifier->on_finish_processing();
          }
        });

        auto ptrs_generator = ptrs_generator_pool->get();
        if (!ptrs_generator) {
          logger.error("Failed to retrieve PT-RS generator (async).");
          return;
        }

        pdsch_process_ptrs(*grid, *ptrs_generator, config);
      };

      bool success_ptrs = executor.defer(ptrs_task);
      if (!success_ptrs) {
        ptrs_task();
      }
    }

    // Process DM-RS concurrently.
    auto dmrs_task = [this]() noexcept SRSRAN_RTSAN_NONBLOCKING {
      auto execute_on_exit = make_scope_exit([this]() {
        // Decrement asynchronous task counter.
        if (async_task_counter.fetch_sub(1) == 1) {
          // Notify end of the processing.
          notifier->on_finish_processing();
        }
      });

      auto dmrs_generator = dmrs_generator_pool->get();
      if (!dmrs_generator) {
        logger.error("Failed to retrieve DM-RS generator (async).");
        return;
      }

      pdsch_process_dmrs(*grid, *dmrs_generator, config);
    };

    bool success_dmrs = executor.defer(dmrs_task);
    if (!success_dmrs) {
      dmrs_task();
    }

    // Fork codeblock processing tasks.
    fork_cb_batches();
  }
}

void pdsch_processor_flexible_impl::initialize_new_transmission(
    resource_grid_writer&                                           grid_,
    pdsch_processor_notifier&                                       notifier_,
    static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data_,
    const pdsch_processor::pdu_t&                                   pdu)
{
  using namespace units::literals;

  // Save process parameter inputs.
  grid     = &grid_;
  notifier = &notifier_;
  data     = std::move(data_.front());
  config   = pdu;

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = config.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = pdsch_compute_nof_data_re(config);

  // Calculate transport block size.
  units::bits tbs = units::bytes(data.get_buffer().size()).to_bits();

  // Calculate number of codeblocks.
  nof_cb = ldpc::compute_nof_codeblocks(tbs, config.ldpc_base_graph);

  modulation_scheme modulation = config.codewords.front().modulation;

  // Calculate rate match buffer size.
  units::bits Nref = ldpc::compute_N_ref(config.tbs_lbrm, nof_cb);

  // Initialize the segmenter.
  segmenter_config segmenter_cfg;
  segmenter_cfg.base_graph     = config.ldpc_base_graph;
  segmenter_cfg.rv             = config.codewords.front().rv;
  segmenter_cfg.mod            = modulation;
  segmenter_cfg.Nref           = Nref.value();
  segmenter_cfg.nof_layers     = nof_layers;
  segmenter_cfg.nof_ch_symbols = nof_layers * nof_re_pdsch;

  // Initialize the segmenter.
  segment_buffer = &segmenter->new_transmission(data.get_buffer(), segmenter_cfg);

  // The processing of this transmission is synchronous if the number of codeblocks is smaller than the batch size.
  async_proc = (nof_cb > max_nof_codeblocks_per_batch);

  if (async_proc) {
    units::bits bits_per_symbol(get_bits_per_symbol(modulation));

    // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
    // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
    // rounded up.
    unsigned nof_short_segments = segment_buffer->get_nof_short_segments();

    units::bits cw_length = segment_buffer->get_cw_length();

    // Calculate RE offset for each codeblock.
    re_offset.resize(nof_cb);
    unsigned re_count_sum = 0;
    for (unsigned i_cb = 0; i_cb != nof_cb; ++i_cb) {
      // Calculate RM length in RE.
      unsigned rm_length_re = divide_ceil(nof_re_pdsch, nof_cb);
      if (i_cb < nof_short_segments) {
        rm_length_re = nof_re_pdsch / nof_cb;
      }

      // Set RE offset for the resource mapper.
      re_offset[i_cb] = re_count_sum;

      // Increment RE count.
      re_count_sum += rm_length_re;
    }
    srsran_assert(re_count_sum * nof_layers * bits_per_symbol == cw_length,
                  "RM length sum (i.e., {}) must be equal to the codeword length (i.e., {}).",
                  units::bits(re_count_sum * nof_layers * bits_per_symbol),
                  cw_length);
  }

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

  // Prepare the allocation pattern for the resource grid mapper.
  allocation.bwp        = {pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb};
  allocation.freq_alloc = pdu.freq_alloc;
  allocation.time_alloc = {pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols};

  // Reserved REs, including DM-RS and CSI-RS.
  reserved = re_pattern_list(config.reserved);

  // Get DM-RS RE pattern.
  re_pattern dmrs_pattern = config.dmrs.get_dmrs_pattern(
      config.bwp_start_rb, config.bwp_size_rb, config.nof_cdm_groups_without_data, config.dmrs_symbol_mask);

  // Merge DM-RS RE pattern into the reserved RE patterns.
  reserved.merge(dmrs_pattern);
}

void pdsch_processor_flexible_impl::sync_pdsch_cb_processing()
{
  auto execute_on_exit = make_scope_exit([this]() {
    // No more code block tasks pending to execute, it is now safe to discard the TB buffer.
    data.release();

    // Notify end of the processing.
    notifier->on_finish_processing();
  });

  // Select codeblock processor.
  auto block_processor = block_processor_pool->get();
  if (!block_processor) {
    logger.error("Failed to retrieve PDSCH block processor.");
    return;
  }

  // Calculate modulation scaling.
  float scaling = convert_dB_to_amplitude(-config.ratio_pdsch_data_to_sss_dB);

  // Apply scaling to the precoding matrix.
  scaling *= modulation_mapper::get_modulation_scaling(config.codewords.front().modulation);
  precoding = config.precoding;
  precoding *= scaling;

  // Configure new transmission. Codeword index, start CB index and CB batch length are fixed.
  resource_grid_mapper::symbol_buffer& grid_buffer =
      block_processor->configure_new_transmission(data.get_buffer(), 0, config, *segment_buffer, 0, nof_cb);

  // Map PDSCH.
  mapper->map(*grid, grid_buffer, allocation, reserved, precoding);

  // Prepare PT-RS configuration and generate.
  if (config.ptrs) {
    auto ptrs_generator = ptrs_generator_pool->get();
    if (!ptrs_generator) {
      logger.error("Failed to retrieve PT-RS generator.");
      return;
    }

    pdsch_process_ptrs(*grid, *ptrs_generator, config);
  }

  // Process DM-RS.
  {
    auto dmrs_generator = dmrs_generator_pool->get();
    if (!dmrs_generator) {
      logger.error("Failed to retrieve DM-RS generator.");
      return;
    }

    pdsch_process_dmrs(*grid, *dmrs_generator, config);
  }
}

void pdsch_processor_flexible_impl::fork_cb_batches()
{
  // Codeword index is fix.
  static constexpr unsigned i_cw = 0;

  // Homogeneous batches of CBs will be processed per thread, unless otherwise specified.
  unsigned nof_cb_per_batch = std::max(1U, max_nof_codeblocks_per_batch);

  // Limit the number of tasks to the number of codeblock batches.
  unsigned nof_cb_batches = divide_ceil(nof_cb, nof_cb_per_batch);

  // Set number of codeblock batches.
  cb_task_counter = nof_cb_batches;

  // Spawn a task for each codeblock bactch.
  for (unsigned i_task = 0; i_task != nof_cb_batches; ++i_task) {
    // Queue batches in reverse order.
    unsigned i_batch = nof_cb_batches - 1 - i_task;

    // Create asynchronous task for the codeblock batch.
    auto async_task = [this, nof_cb_per_batch, i_batch]() noexcept SRSRAN_RTSAN_NONBLOCKING {
      // Start PDSCH codeblock batch tracing.
      trace_point cb_batch_pdsch_tp = l1_dl_tracer.now();

      // Code to execute when returning.
      auto exec_at_exit = make_scope_exit([this]() { // Decrement code block batch counter.
        if (cb_task_counter.fetch_sub(1) == 1) {
          // No more code block tasks pending to execute, it is now safe to discard the TB buffer.
          data.release();
          // Decrement asynchronous task counter.
          if (async_task_counter.fetch_sub(1) == 1) {
            // Notify end of the processing.
            notifier->on_finish_processing();
          }
        }
      });

      // Select codeblock processor.
      auto block_processor = block_processor_pool->get();
      if (!block_processor) {
        logger.error("Failed to retrieve PDSCH codeblock processor.");
        return;
      }

      // Calculate the first codeblock index within the batch.
      unsigned first_cb_index = i_batch * nof_cb_per_batch;

      // Limit batch size for the last batch.
      unsigned next_cb_batch_length = std::min(nof_cb - first_cb_index, nof_cb_per_batch);

      // Configure new transmission.
      resource_grid_mapper::symbol_buffer& grid_buffer = block_processor->configure_new_transmission(
          data.get_buffer(), i_cw, config, *segment_buffer, first_cb_index, next_cb_batch_length);

      // Map PDSCH.
      mapper->map(*grid, grid_buffer, allocation, reserved, precoding, re_offset[first_cb_index]);

      // Trace PDSCH.
      l1_dl_tracer << trace_event("CB batch", cb_batch_pdsch_tp);
    };

    // Try to execute task asynchronously.
    bool successful = false;
    if (nof_cb_batches > 1) {
      successful = executor.defer(async_task);
    }

    // Execute task locally if it was not enqueued.
    if (!successful) {
      async_task();
    }
  }
}
