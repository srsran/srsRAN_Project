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
#include "srsran/ran/dmrs.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace srsran;

void pdsch_processor_concurrent_impl::process(resource_grid_mapper&                                        mapper,
                                              static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                                              const pdsch_processor::pdu_t&                                pdu)
{
  assert_pdu(pdu);

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH in the grid. Common for all codewords.
  unsigned nof_re_pdsch = compute_nof_data_re(pdu);

  // Select codeword specific parameters.
  unsigned          rv         = pdu.codewords[0].rv;
  modulation_scheme modulation = pdu.codewords[0].modulation;
  span<uint8_t>     codeword =
      span<uint8_t>(temp_codeword).first(nof_re_pdsch * nof_layers * get_bits_per_symbol(modulation));

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
  segmenter->segment(d_segments, data[0], encoder_config);

  // Fork codeblock processing tasks.
  bounded_bitset<MAX_NOF_SEGMENTS> cb_mask(d_segments.size());
  for (unsigned i_cb = 0, i_cb_end = d_segments.size(); i_cb != i_cb_end; ++i_cb) {
    auto encode_task = [this, &cb_mask, codeword, i_cb]() {
      // Select segment description.
      const described_segment& descr_seg = d_segments[i_cb];

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

      unsigned offset    = descr_seg.get_metadata().cb_specific.cw_offset;
      unsigned rm_length = descr_seg.get_metadata().cb_specific.rm_length;

      span<uint8_t> codeblock = codeword.subspan(offset, rm_length);

      // Process codeblock.
      cb_processor.process(codeblock, descr_seg);

      // Update the codeblock mask.
      {
        std::unique_lock<std::mutex> lock(cb_count_mutex);
        cb_mask.set(i_cb);
        cb_count_cvar.notify_all();
      }
    };
    if (not executor.execute(encode_task)) {
      // Skip the CB encoding if the task could not be enqueued.
      {
        std::unique_lock<std::mutex> lock(cb_count_mutex);
        cb_mask.set(i_cb);
        cb_count_cvar.notify_all();
      }
    }
  }

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

  // Prepare packed encoded codeword.
  temp_packed_codeword.resize(codeword.size());

  // Pack codeblocks as they are completed.
  unsigned nof_packed_bits = 0;
  unsigned highest_i_cb    = 0;
  do {
    std::unique_lock<std::mutex> lock(cb_count_mutex);

    // Find the lowest unprocessed codeblock.
    int pos = cb_mask.find_lowest(highest_i_cb, cb_mask.size(), false);

    // If an unprocessed codeblock is found and matches the previous one, wait for notification and continue.
    if ((pos > 0) && (static_cast<unsigned>(pos) == highest_i_cb)) {
      cb_count_cvar.wait(lock);
      continue;
    }

    // Otherwise, the lock is no longer necessary.
    lock.unlock();

    // If all blocks have been processed, finish packing.
    if (pos < 0) {
      bit_buffer chunk_view = temp_packed_codeword.last(codeword.size() - nof_packed_bits);
      srsvec::bit_pack(chunk_view, codeword.subspan(nof_packed_bits, codeword.size() - nof_packed_bits));
      nof_packed_bits = codeword.size();
      continue;
    }

    // Some new blocks have been processed. Pack up to the previous byte.
    highest_i_cb             = pos;
    unsigned cw_offset_bytes = d_segments[pos].get_metadata().cb_specific.cw_offset / 8;
    unsigned count_bytes     = cw_offset_bytes - nof_packed_bits / 8;

    bit_buffer chunk_view = temp_packed_codeword.last(codeword.size() - nof_packed_bits).first(count_bytes * 8);
    srsvec::bit_pack(chunk_view, codeword.subspan(nof_packed_bits, count_bytes * 8));

    // Update the number of packed bits.
    nof_packed_bits = cw_offset_bytes * 8;

  } while (nof_packed_bits != codeword.size());

  // Prepare modulator configuration.
  pdsch_modulator::config_t modulator_config;
  modulator_config.rnti                        = pdu.rnti;
  modulator_config.bwp_size_rb                 = pdu.bwp_size_rb;
  modulator_config.bwp_start_rb                = pdu.bwp_start_rb;
  modulator_config.modulation1                 = pdu.codewords[0].modulation;
  modulator_config.modulation2                 = modulation_scheme::BPSK;
  modulator_config.freq_allocation             = pdu.freq_alloc;
  modulator_config.start_symbol_index          = pdu.start_symbol_index;
  modulator_config.nof_symbols                 = pdu.nof_symbols;
  modulator_config.dmrs_symb_pos               = pdu.dmrs_symbol_mask;
  modulator_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  modulator_config.n_id                        = pdu.n_id;
  modulator_config.scaling                     = convert_dB_to_amplitude(-pdu.ratio_pdsch_data_to_sss_dB);
  modulator_config.reserved                    = pdu.reserved;
  modulator_config.precoding                   = pdu.precoding;

  // Prepare encoded codewords.
  static_vector<bit_buffer, pdsch_constants::MAX_NOF_CODEWORDS> codewords = {temp_packed_codeword};

  // Actual modulation.
  modulator->modulate(mapper, codewords, modulator_config);
}

void pdsch_processor_concurrent_impl::assert_pdu(const pdsch_processor::pdu_t& pdu) const
{
  // Deduce parameters from the PDU.
  unsigned         nof_layers       = pdu.precoding.get_nof_layers();
  unsigned         nof_codewords    = (nof_layers > 4) ? 2 : 1;
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
                "The transmission with time allocation {}:{} exceeds the slot boundary of {} symbols.",
                pdu.start_symbol_index,
                pdu.nof_symbols,
                nof_symbols_slot);
  srsran_assert(pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb),
                "Invalid BWP configuration {}:{} for the given frequency allocation {}.",
                pdu.bwp_start_rb,
                pdu.bwp_size_rb,
                pdu.freq_alloc);
  srsran_assert(pdu.dmrs == dmrs_type::TYPE1, "Only DM-RS Type 1 is currently supported.");
  srsran_assert(pdu.freq_alloc.is_contiguous(), "Only contiguous allocation is currently supported.");
  srsran_assert(
      pdu.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(dmrs_config),
      "The number of CDM groups without data (i.e., {}) must not exceed the maximum given by the type (i.e., {}).",
      pdu.nof_cdm_groups_without_data,
      get_max_nof_cdm_groups_without_data(dmrs_config));
  srsran_assert(nof_layers != 0, "No transmit layers are active.");
  srsran_assert(nof_layers <= 4, "Only 1 to 4 layers are currently supported. {} layers requested.", nof_layers);

  srsran_assert(pdu.codewords.size() == nof_codewords,
                "Expected {} codewords and got {} for {} layers.",
                nof_codewords,
                pdu.codewords.size(),
                nof_layers);
  srsran_assert(pdu.tbs_lbrm_bytes > 0 && pdu.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, lesser than or equal to {} bytes",
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

  // Subtract the number of reserved RE to the number of allocated RE.
  srsran_assert(nof_grid_re > nof_reserved_re,
                "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
                nof_grid_re,
                nof_reserved_re);
  return nof_grid_re - nof_reserved_re;
}