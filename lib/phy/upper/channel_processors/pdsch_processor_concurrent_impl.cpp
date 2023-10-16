/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_processor_concurrent_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/ran/dmrs.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/event_tracing.h"
#include <thread>

using namespace srsran;

void pdsch_processor_concurrent_impl::map(span<const srsran::ci8_t> codeword)
{
  // Build resource grid mapper adaptor.
  resource_grid_mapper::symbol_buffer_adapter buffer(codeword);

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

  // Calculate modulation scaling.
  float scaling = convert_dB_to_amplitude(-config.ratio_pdsch_data_to_sss_dB);
  scaling *= cb_processor_pool->get().get_scaling(config.codewords.front().modulation);

  // Apply scaling to the precoding matrix.
  precoding_configuration precoding = config.precoding;
  precoding *= scaling;

  // Map into the resource grid.
  mapper->map(buffer, allocation, reserved, precoding);

  // Decrement asynchronous task counter.
  if (async_task_counter.fetch_sub(1) == 1) {
    // Notify end of the processing.
    notifier->on_finish_processing();
  }
}

void pdsch_processor_concurrent_impl::process(resource_grid_mapper&                                        mapper_,
                                              pdsch_processor_notifier&                                    notifier_,
                                              static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data_,
                                              const pdsch_processor::pdu_t&                                pdu_)
{
  // Codeword index is fix.
  static constexpr unsigned i_cw = 0;

  // Saves inputs.
  save_inputs(mapper_, notifier_, data_, pdu_);

  // Makes sure the PDU is valid.
  assert_pdu();

  // Set the number of asynchronous tasks. It counts as CB processing and DM-RS generation.
  async_task_counter = 2;

  // Process DM-RS concurrently.
  auto dmrs_task = [this]() { process_dmrs(); };
  if (!executor.execute(dmrs_task)) {
    dmrs_task();
  }

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = config.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = compute_nof_data_re(config);

  // Calculate scrambling initial state.
  scrambler->init((static_cast<unsigned>(config.rnti) << 15U) + (i_cw << 14U) + config.n_id);

  // Select codeword specific parameters.
  unsigned          rv         = config.codewords[i_cw].rv;
  modulation_scheme modulation = config.codewords[i_cw].modulation;

  // Prepare segmenter configuration.
  segmenter_config encoder_config;
  encoder_config.base_graph     = config.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = config.tbs_lbrm_bytes * 8;
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = nof_re_pdsch * nof_layers;

  // Clear the segmentation buffer.
  d_segments.clear();

  // Segmentation (it includes CRC attachment for the entire transport block and each individual segment).
  segmenter->segment(d_segments, data, encoder_config);

  // Prepare data view to modulated symbols.
  span<ci8_t> codeword = span<ci8_t>(temp_codeword).first(nof_layers * nof_re_pdsch);

  // Fork codeblock processing tasks.
  fork_cb_batches(codeword);
}

void pdsch_processor_concurrent_impl::save_inputs(resource_grid_mapper&     mapper_,
                                                  pdsch_processor_notifier& notifier_,
                                                  static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data_,
                                                  const pdsch_processor::pdu_t&                                pdu)
{
  mapper   = &mapper_;
  notifier = &notifier_;
  data     = data_.front();
  config   = pdu;
}

void pdsch_processor_concurrent_impl::assert_pdu() const
{
  // Deduce parameters from the config.
  unsigned         nof_layers       = config.precoding.get_nof_layers();
  unsigned         nof_symbols_slot = get_nsymb_per_slot(config.cp);
  dmrs_config_type dmrs_config = (config.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  srsran_assert(config.dmrs_symbol_mask.size() == nof_symbols_slot,
                "The DM-RS symbol mask size (i.e., {}), must be equal to the number of symbols in the slot (i.e., {}).",
                config.dmrs_symbol_mask.size(),
                nof_symbols_slot);
  srsran_assert(config.dmrs_symbol_mask.any(),
                "The number of OFDM symbols carrying DM-RS RE must be greater than zero.");
  srsran_assert(
      static_cast<unsigned>(config.dmrs_symbol_mask.find_lowest(true)) >= config.start_symbol_index,
      "The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to or greater than the first symbol "
      "allocated to transmission (i.e., {}).",
      config.dmrs_symbol_mask.find_lowest(true),
      config.start_symbol_index);
  srsran_assert(static_cast<unsigned>(config.dmrs_symbol_mask.find_highest(true)) <
                    (config.start_symbol_index + config.nof_symbols),
                "The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than or equal to the last "
                "symbol allocated to transmission (i.e., {}).",
                config.dmrs_symbol_mask.find_highest(true),
                config.start_symbol_index + config.nof_symbols - 1);
  srsran_assert((config.start_symbol_index + config.nof_symbols) <= nof_symbols_slot,
                "The transmission with time allocation [{}, {}) exceeds the slot boundary of {} symbols.",
                config.start_symbol_index,
                config.start_symbol_index + config.nof_symbols,
                nof_symbols_slot);
  srsran_assert(config.freq_alloc.is_bwp_valid(config.bwp_start_rb, config.bwp_size_rb),
                "Invalid BWP configuration [{}, {}) for the given frequency allocation {}.",
                config.bwp_start_rb,
                config.bwp_start_rb + config.bwp_size_rb,
                config.freq_alloc);
  srsran_assert(config.dmrs == dmrs_type::TYPE1, "Only DM-RS Type 1 is currently supported.");
  srsran_assert(config.freq_alloc.is_contiguous(), "Only contiguous allocation is currently supported.");
  srsran_assert(config.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(dmrs_config),
                "The number of CDM groups without data (i.e., {}) must not exceed the maximum supported by the DM-RS "
                "type (i.e., {}).",
                config.nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(dmrs_config));
  srsran_assert(nof_layers != 0, "No transmit layers are active.");
  srsran_assert(nof_layers <= 4, "Only 1 to 4 layers are currently supported. {} layers requested.", nof_layers);

  srsran_assert(config.codewords.size() == 1, "Only one codeword is currently supported.");
  srsran_assert(config.tbs_lbrm_bytes > 0 && config.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, less than or equal to {} bytes",
                config.tbs_lbrm_bytes,
                ldpc::MAX_CODEBLOCK_SIZE / 8);
}

unsigned pdsch_processor_concurrent_impl::compute_nof_data_re(const pdu_t& config)
{
  // Copy reserved RE and merge DMRS pattern.
  re_pattern_list reserved_re = config.reserved;
  reserved_re.merge(config.dmrs.get_dmrs_pattern(
      config.bwp_start_rb, config.bwp_size_rb, config.nof_cdm_groups_without_data, config.dmrs_symbol_mask));

  // Generate allocation mask.
  bounded_bitset<MAX_RB> prb_mask = config.freq_alloc.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb);

  // Calculate the number of RE allocated in the grid.
  unsigned nof_grid_re = config.freq_alloc.get_nof_rb() * NRE * config.nof_symbols;

  // Calculate the number of reserved resource elements.
  unsigned nof_reserved_re = reserved_re.get_inclusion_count(config.start_symbol_index, config.nof_symbols, prb_mask);

  // Subtract the number of reserved RE from the number of allocated RE.
  srsran_assert(nof_grid_re > nof_reserved_re,
                "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
                nof_grid_re,
                nof_reserved_re);
  return nof_grid_re - nof_reserved_re;
}

void pdsch_processor_concurrent_impl::fork_cb_batches(span<ci8_t> codeword)
{
  // Minimum number of codeblocks per batch.
  unsigned min_cb_batch_size = 4;

  // Get total number of segments.
  unsigned nof_cb = d_segments.size();

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

    // Select segment description.
    span<const described_segment> segments = span<const described_segment>(d_segments).subspan(i_cb, cb_batch_size);

    auto async_task = [this, segments, c_init, codeword]() mutable {
      // Select codeblock processor from the pool.
      pdsch_codeblock_processor& processor = cb_processor_pool->get();

      // For each segment...
      for (const described_segment& descr_seg : segments) {
        // Process codeblock.
        c_init = processor.process(codeword, descr_seg, c_init);
      }

      // Decrement code block batch counter.
      if (cb_batch_counter.fetch_sub(1) == 1) {
        map(codeword);
      }
    };

    // Try to execute task asynchronously.
    bool successful = executor.execute(async_task);

    // Execute task locally if it was not enqueued.
    if (!successful) {
      async_task();
    }

    // Skip advancing pseudo-random sequence state for the last batch.
    if (i_cb_batch == nof_cb_batches - 1) {
      continue;
    }

    // Advance scrambling sequence for the next batch.
    for (const described_segment& descr_seg : segments) {
      scrambler->advance(descr_seg.get_metadata().cb_specific.rm_length);
    }
  }
}

void pdsch_processor_concurrent_impl::process_dmrs()
{
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
  dmrs->map(*mapper, dmrs_config);

  // Decrement asynchronous task counter.
  if (async_task_counter.fetch_sub(1) == 1) {
    // Notify end of the processing.
    notifier->on_finish_processing();
  }
}
