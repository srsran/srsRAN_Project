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

#include "pdsch_processor_concurrent_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/ran/dmrs.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include <mutex>
#include <thread>

using namespace srsran;

void pdsch_processor_concurrent_impl::map(resource_grid_mapper&   mapper,
                                          const re_buffer_reader& data_re,
                                          const pdu_t&            config)
{
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
  re_pattern_list allocation;
  re_pattern      pdsch_pattern;

  // Reserved REs, including DM-RS and CSI-RS.
  re_pattern_list reserved(config.reserved);

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

  // Map into the resource grid.
  mapper.map(data_re, allocation, reserved, config.precoding);
}

void pdsch_processor_concurrent_impl::process(resource_grid_mapper&                                        mapper,
                                              static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                                              const pdsch_processor::pdu_t&                                pdu)
{
  // Codeword index.
  static constexpr unsigned i_cw = 0;

  assert_pdu(pdu);

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = compute_nof_data_re(pdu);

  // Calculate scrambling initial state.
  scrambler->init((static_cast<unsigned>(pdu.rnti) << 15U) + (i_cw << 14U) + pdu.n_id);

  // Select codeword specific parameters.
  unsigned          rv         = pdu.codewords[i_cw].rv;
  modulation_scheme modulation = pdu.codewords[i_cw].modulation;

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
  segmenter->segment(d_segments, data[i_cw], encoder_config);

  // Prepare data view to modulated symbols.
  temp_re.resize(nof_layers, nof_re_pdsch);

  unsigned nof_cb = d_segments.size();

  unsigned nof_cb_batches = std::min(nof_cb, static_cast<unsigned>(cb_processor_pool.size()) * 4);
  unsigned cb_batch_size  = divide_ceil(nof_cb, nof_cb_batches);

  // Fork codeblock processing tasks.
  unsigned cb_batch_count = 0;
  for (unsigned i_cb_batch = 0, i_cb = 0; i_cb_batch != nof_cb_batches; ++i_cb_batch, i_cb += cb_batch_size) {
    // Limit batch size for the last batch.
    cb_batch_size = std::min(nof_cb - i_cb, cb_batch_size);

    // Extract scrambling initial state for the next bit.
    pseudo_random_generator::state_s c_init = scrambler->get_state();

    // Select segment description.
    span<const described_segment> segments = span<const described_segment>(d_segments).subspan(i_cb, cb_batch_size);

    auto encode_task = [this, &cb_batch_count, segments, c_init, nof_layers]() mutable {
      // Global count of threads.
      static std::atomic<unsigned> global_count = {0};
      // Local thread index.
      thread_local unsigned thread_index = global_count++;

      // Make sure the thread index does not exceed the codeblock processor pool.
      srsran_assert(thread_index < cb_processor_pool.size(),
                    "Insufficient number of processors, i.e., {}, for thread index {}.",
                    cb_processor_pool.size(),
                    thread_index);

      // Select codeblock processor.
      pdsch_codeblock_processor& cb_processor = *cb_processor_pool[thread_index];

      // For each segment...
      for (const described_segment& descr_seg : segments) {
        // Process codeblock.
        c_init = cb_processor.process(temp_re, descr_seg, c_init, nof_layers);
      }

      // Increment code block batch counter.
      {
        std::unique_lock<std::mutex> lock(cb_count_mutex);
        ++cb_batch_count;
        cb_count_cvar.notify_all();
      }
    };

    // Try to execute task asynchronously.
    bool successful = executor.execute(encode_task);

    // Increment code block batch counter if the task was not successfully executed.
    if (!successful) {
      std::unique_lock<std::mutex> lock(cb_count_mutex);
      ++cb_batch_count;
      cb_count_cvar.notify_all();
    }

    // Advance scrambling sequence for the next batch.
    for (const described_segment& descr_seg : segments) {
      scrambler->advance(descr_seg.get_metadata().cb_specific.rm_length);
    }
  }

  // Process DM-RS while the codeblocks are being processed.
  process_dmrs(mapper, pdu);

  // Wait for the asynchronous codeblock processing to finish.
  {
    std::unique_lock<std::mutex> lock(cb_count_mutex);
    cb_count_cvar.wait(lock, [&cb_batch_count, &nof_cb_batches]() { return cb_batch_count == nof_cb_batches; });
  }

  // Map the process data.
  map(mapper, temp_re, pdu);
}

void pdsch_processor_concurrent_impl::assert_pdu(const pdsch_processor::pdu_t& pdu) const
{
  // Deduce parameters from the PDU.
  unsigned         nof_layers       = pdu.precoding.get_nof_layers();
  unsigned         nof_symbols_slot = get_nsymb_per_slot(pdu.cp);
  dmrs_config_type dmrs_config = (pdu.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  srsran_assert(pdu.dmrs_symbol_mask.size() == nof_symbols_slot,
                "The DM-RS symbol mask size (i.e., {}), must be equal to the number of symbols in the slot (i.e., {}).",
                pdu.dmrs_symbol_mask.size(),
                nof_symbols_slot);
  srsran_assert(pdu.dmrs_symbol_mask.any(), "The number of OFDM symbols carrying DM-RS RE must be greater than zero.");
  srsran_assert(
      static_cast<unsigned>(pdu.dmrs_symbol_mask.find_lowest(true)) >= pdu.start_symbol_index,
      "The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to or greater than the first symbol "
      "allocated to transmission (i.e., {}).",
      pdu.dmrs_symbol_mask.find_lowest(true),
      pdu.start_symbol_index);
  srsran_assert(static_cast<unsigned>(pdu.dmrs_symbol_mask.find_highest(true)) <
                    (pdu.start_symbol_index + pdu.nof_symbols),
                "The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than or equal to the last "
                "symbol allocated to transmission (i.e., {}).",
                pdu.dmrs_symbol_mask.find_highest(true),
                pdu.start_symbol_index + pdu.nof_symbols - 1);
  srsran_assert((pdu.start_symbol_index + pdu.nof_symbols) <= nof_symbols_slot,
                "The transmission with time allocation [{}, {}) exceeds the slot boundary of {} symbols.",
                pdu.start_symbol_index,
                pdu.start_symbol_index + pdu.nof_symbols,
                nof_symbols_slot);
  srsran_assert(pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb),
                "Invalid BWP configuration [{}, {}) for the given frequency allocation {}.",
                pdu.bwp_start_rb,
                pdu.bwp_start_rb + pdu.bwp_size_rb,
                pdu.freq_alloc);
  srsran_assert(pdu.dmrs == dmrs_type::TYPE1, "Only DM-RS Type 1 is currently supported.");
  srsran_assert(pdu.freq_alloc.is_contiguous(), "Only contiguous allocation is currently supported.");
  srsran_assert(pdu.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(dmrs_config),
                "The number of CDM groups without data (i.e., {}) must not exceed the maximum supported by the DM-RS "
                "type (i.e., {}).",
                pdu.nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(dmrs_config));
  srsran_assert(nof_layers != 0, "No transmit layers are active.");
  srsran_assert(nof_layers <= 4, "Only 1 to 4 layers are currently supported. {} layers requested.", nof_layers);

  srsran_assert(pdu.codewords.size() == 1, "Only one codeword is currently supported.");
  srsran_assert(pdu.tbs_lbrm_bytes > 0 && pdu.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, less than or equal to {} bytes",
                pdu.tbs_lbrm_bytes,
                ldpc::MAX_CODEBLOCK_SIZE / 8);
}

unsigned pdsch_processor_concurrent_impl::compute_nof_data_re(const pdu_t& pdu)
{
  // Copy reserved RE and merge DMRS pattern.
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

void pdsch_processor_concurrent_impl::process_dmrs(resource_grid_mapper& mapper, const pdu_t& pdu)
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