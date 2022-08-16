/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_processor_impl.h"
#include "srsgnb/srsvec/copy.h"

using namespace srsgnb;

unsigned int pdsch_processor_impl::compute_nof_data_re(const pdu_t& pdu)
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
  srsgnb_assert(nof_grid_re > nof_reserved_re,
                "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
                nof_grid_re,
                nof_reserved_re);
  return nof_grid_re - nof_reserved_re;
}

span<const uint8_t> pdsch_processor_impl::encode(span<const uint8_t> data,
                                                 unsigned            codeword_id,
                                                 unsigned            nof_layers,
                                                 unsigned            Nre,
                                                 const pdu_t&        pdu)
{
  // Select codeword specific parameters.
  unsigned                                rv           = pdu.codewords[codeword_id].rv;
  modulation_scheme                       modulation   = pdu.codewords[codeword_id].modulation;
  std::array<uint8_t, MAX_CODEWORD_SIZE>& tmp_codeword = temp_codewords[codeword_id];

  // Prepare encoder configuration.
  segmenter_config encoder_config;
  encoder_config.base_graph     = pdu.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = pdu.tbs_lbrm_bytes * 8;
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = Nre;

  // Prepare codeword size.
  span<uint8_t> codeword = span<uint8_t>(tmp_codeword).first(Nre * nof_layers * get_bits_per_symbol(modulation));

  // Encode codeword.
  encoder->encode(codeword, data, encoder_config);

  // Return the view of the codeword.
  return codeword;
}

void pdsch_processor_impl::modulate(resource_grid_writer&           grid,
                                    span<const span<const uint8_t>> codewords,
                                    const pdu_t&                    pdu)
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
  modulator_config.scaling                     = convert_dB_to_amplitude(pdu.ratio_pdsch_data_to_sss_dB);
  modulator_config.reserved                    = pdu.reserved;
  modulator_config.ports                       = pdu.ports;

  modulator->modulate(grid, codewords, modulator_config);
}

void pdsch_processor_impl::put_dmrs(resource_grid_writer& grid, const pdu_t& pdu)
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
  dmrs_config.amplitude            = convert_dB_to_amplitude(pdu.ratio_pdsch_dmrs_to_sss_dB);
  dmrs_config.symbols_mask         = pdu.dmrs_symbol_mask;
  dmrs_config.rb_mask              = rb_mask_bitset;
  dmrs_config.ports.resize(pdu.ports.size());
  srsvec::copy(dmrs_config.ports, pdu.ports);

  dmrs->map(grid, dmrs_config);
}

void pdsch_processor_impl::process(resource_grid_writer&                                        grid,
                                   static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                                   const pdsch_processor::pdu_t&                                pdu)
{
  // Number of codewords.
  unsigned nof_codewords = (pdu.ports.size() > 4) ? 2 : 1;

  // Validate configuration.
  srsgnb_assert(pdu.codewords.size() == nof_codewords,
                "Expected {} codewords and got {} for {} ports",
                nof_codewords,
                pdu.codewords.size(),
                pdu.ports.size());
  srsgnb_assert(pdu.codewords.size() == data.size(),
                "The number of codewords ({}) does not match the number of transport blocks ({})",
                pdu.codewords.size(),
                data.size());
  srsgnb_assert(pdu.tbs_lbrm_bytes > 0 && pdu.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, lesser than or equal to {} bytes",
                pdu.tbs_lbrm_bytes,
                ldpc::MAX_CODEBLOCK_SIZE / 8);
  srsgnb_assert(pdu.nof_cdm_groups_without_data >= 1 && pdu.nof_cdm_groups_without_data <= 3,
                "Invalid number of CDM groups without data ({})",
                pdu.nof_cdm_groups_without_data);

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = pdu.ports.size();

  // Calculate the number of layers the codeword 0 is mapped to. It is the number of layers divided by the number of
  // codewords, rounding down (floor).
  unsigned nof_layers_cw0 = nof_layers / nof_codewords;

  // Calculate the number of layers the codeword 1 is mapped. It is the unused number of layers from the previous
  // codeword.
  unsigned nof_layers_cw1 = nof_layers - nof_layers_cw0;

  // Calculate the number of resource elements to map. Common for all codewords.
  unsigned Nre = compute_nof_data_re(pdu);

  // Prepare encoded codewords.
  static_vector<span<const uint8_t>, pdsch_modulator::MAX_NOF_CODEWORDS> temp_codewords;

  // Encode each codeword.
  for (unsigned codeword_id = 0; codeword_id != nof_codewords; ++codeword_id) {
    unsigned            nof_layers_cw = (codeword_id == 0) ? nof_layers_cw0 : nof_layers_cw1;
    span<const uint8_t> codeword      = encode(data[codeword_id], codeword_id, nof_layers_cw, Nre, pdu);
    temp_codewords.emplace_back(codeword);
  }

  // Modulate codewords.
  modulate(grid, temp_codewords, pdu);

  // Prepare DMRS configuration and generate.
  put_dmrs(grid, pdu);
}