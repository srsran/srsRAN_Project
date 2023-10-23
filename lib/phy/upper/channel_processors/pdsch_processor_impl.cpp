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

#include "pdsch_processor_impl.h"
#include "srsran/ran/dmrs.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

bool pdsch_processor_validator_impl::is_valid(const pdsch_processor::pdu_t& pdu) const
{
  unsigned         nof_symbols_slot = get_nsymb_per_slot(pdu.cp);
  dmrs_config_type dmrs_config = (pdu.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  // The frequency allocation is not compatible with the BWP parameters.
  if (!pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb)) {
    return false;
  }

  // The size of the DM-RS symbol mask must coincide with the number of symbols in the slot.
  if (pdu.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return false;
  }

  // The number of symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.count() == 0) {
    return false;
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < pdu.start_symbol_index) {
    return false;
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // Only DM-RS Type 1 is supported.
  if (pdu.dmrs != dmrs_type::TYPE1) {
    return false;
  }

  // Only two CDM groups without data is supported.
  if (pdu.nof_cdm_groups_without_data > get_max_nof_cdm_groups_without_data(dmrs_config)) {
    return false;
  }

  // Only contiguous allocation is currently supported.
  if (!pdu.freq_alloc.is_contiguous()) {
    return false;
  }

  // Only one to four antenna ports are currently supported.
  if ((pdu.precoding.get_nof_ports() == 0) || (pdu.precoding.get_nof_ports() > 4)) {
    return false;
  }

  // The number of layers cannot be zero or larger than the number of ports.
  if ((pdu.precoding.get_nof_layers() == 0) || (pdu.precoding.get_nof_layers() > pdu.precoding.get_nof_ports())) {
    return false;
  }

  // Only one codeword is currently supported.
  if (pdu.codewords.size() != 1) {
    return false;
  }

  // The limited buffer for rate matching size must be within limits.
  if ((pdu.tbs_lbrm_bytes == 0) || (pdu.tbs_lbrm_bytes > ldpc::MAX_CODEBLOCK_SIZE / 8)) {
    return false;
  }

  return true;
}

void pdsch_processor_impl::process(resource_grid_mapper&                                        mapper,
                                   pdsch_processor_notifier&                                    notifier,
                                   static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                                   const pdsch_processor::pdu_t&                                pdu)
{
  assert_pdu(pdu);

  // Number of layers from the precoding configuration.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // Number of codewords.
  unsigned nof_codewords = (nof_layers > 4) ? 2 : 1;

  // Calculate the number of layers codeword 0 is mapped to. It is the number of layers divided by the number of
  // codewords, rounding down (floor).
  unsigned nof_layers_cw0 = nof_layers / nof_codewords;

  // Calculate the number of layers codeword 1 is mapped to. It is the unused number of layers from the previous
  // codeword.
  unsigned nof_layers_cw1 = nof_layers - nof_layers_cw0;

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = compute_nof_data_re(pdu);

  // Prepare encoded codewords.
  static_vector<bit_buffer, pdsch_constants::MAX_NOF_CODEWORDS> codewords;

  // Encode each codeword.
  for (unsigned codeword_id = 0; codeword_id != nof_codewords; ++codeword_id) {
    unsigned          nof_layers_cw = (codeword_id == 0) ? nof_layers_cw0 : nof_layers_cw1;
    const bit_buffer& codeword      = encode(data[codeword_id], codeword_id, nof_layers_cw, nof_re_pdsch, pdu);

    codewords.emplace_back(codeword);
  }

  // Modulate codewords.
  modulate(mapper, codewords, pdu);

  // Prepare DM-RS configuration and generate.
  put_dmrs(mapper, pdu);

  // Notify the end of the processing.
  notifier.on_finish_processing();
}

void pdsch_processor_impl::assert_pdu(const pdsch_processor::pdu_t& pdu) const
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

  srsran_assert(pdu.codewords.size() == nof_codewords,
                "Expected {} codewords and got {} for {} layers.",
                nof_codewords,
                pdu.codewords.size(),
                nof_layers);
  srsran_assert(pdu.tbs_lbrm_bytes > 0 && pdu.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, less than or equal to {} bytes",
                pdu.tbs_lbrm_bytes,
                ldpc::MAX_CODEBLOCK_SIZE / 8);
}

unsigned pdsch_processor_impl::compute_nof_data_re(const pdu_t& pdu)
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

const bit_buffer& pdsch_processor_impl::encode(span<const uint8_t> data,
                                               unsigned            codeword_id,
                                               unsigned            nof_layers,
                                               unsigned            Nre,
                                               const pdu_t&        pdu)
{
  // Select codeword specific parameters.
  unsigned          rv           = pdu.codewords[codeword_id].rv;
  modulation_scheme modulation   = pdu.codewords[codeword_id].modulation;
  span<uint8_t>     tmp_codeword = temp_unpacked_codeword;

  // Prepare encoder configuration.
  segmenter_config encoder_config;
  encoder_config.base_graph     = pdu.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = pdu.tbs_lbrm_bytes * 8;
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = Nre * nof_layers;

  // Prepare codeword size.
  span<uint8_t> codeword = tmp_codeword.first(Nre * nof_layers * get_bits_per_symbol(modulation));

  // Encode codeword.
  encoder->encode(codeword, data, encoder_config);

  // Pack encoded bits.
  temp_packed_codewords[codeword_id].resize(codeword.size());
  srsvec::bit_pack(temp_packed_codewords[codeword_id], codeword);

  // Return the view of the codeword.
  return temp_packed_codewords[codeword_id];
}

void pdsch_processor_impl::modulate(resource_grid_mapper& mapper, span<const bit_buffer> codewords, const pdu_t& pdu)
{
  unsigned nof_codewords = codewords.size();

  pdsch_modulator::config_t modulator_config;
  modulator_config.rnti               = pdu.rnti;
  modulator_config.bwp_size_rb        = pdu.bwp_size_rb;
  modulator_config.bwp_start_rb       = pdu.bwp_start_rb;
  modulator_config.modulation1        = pdu.codewords[0].modulation;
  modulator_config.modulation2        = nof_codewords > 1 ? pdu.codewords[1].modulation : modulation_scheme::BPSK;
  modulator_config.freq_allocation    = pdu.freq_alloc;
  modulator_config.start_symbol_index = pdu.start_symbol_index;
  modulator_config.nof_symbols        = pdu.nof_symbols;
  modulator_config.dmrs_symb_pos      = pdu.dmrs_symbol_mask;
  modulator_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  modulator_config.n_id                        = pdu.n_id;
  modulator_config.scaling                     = convert_dB_to_amplitude(-pdu.ratio_pdsch_data_to_sss_dB);
  modulator_config.reserved                    = pdu.reserved;
  modulator_config.precoding                   = pdu.precoding;

  modulator->modulate(mapper, codewords, modulator_config);
}

void pdsch_processor_impl::put_dmrs(resource_grid_mapper& mapper, const pdu_t& pdu)
{
  bounded_bitset<MAX_RB> rb_mask_bitset = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned dmrs_reference_point_k_rb = 0;
  if (pdu.ref_point == pdu_t::PRB0) {
    dmrs_reference_point_k_rb = pdu.bwp_start_rb;
  }

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

  // Generate and map DM-RS.
  dmrs->map(mapper, dmrs_config);
}
