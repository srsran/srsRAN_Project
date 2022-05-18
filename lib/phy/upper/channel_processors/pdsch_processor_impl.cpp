
#include "pdsch_processor_impl.h"
#include "srsgnb/srsvec/copy.h"

using namespace srsgnb;

unsigned int pdsch_processor_impl::compute_nof_re(const pdu_t& pdu)
{
  // Copy reserved RE and merge DMRS pattern.
  re_pattern_list reserved_re = pdu.reserved;
  reserved_re.merge(pdu.dmrs.get_dmrs_pattern(
      pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask));

  // Generate allocation mask.
  std::array<bool, MAX_RB> rb_mask;
  pdu.freq_alloc.get_allocation_mask(rb_mask, pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Calculate the number of resource elements to map.
  unsigned Nre = pdu.freq_alloc.get_nof_rb() * NRE * pdu.nof_symbols;

  // Calculate the number of reserved resource elements.
  unsigned Nre_reserved = reserved_re.get_inclusion_count(pdu.start_symbol_index, pdu.nof_symbols, rb_mask);

  // Subtract the number of reserved RE to the number of allocated RE.
  srsran_assert(
      Nre > Nre_reserved, "The number of reserved RE ({}) exceeds the number of PDSCH RE ({})", Nre, Nre_reserved);
  return Nre - Nre_reserved;
}

span<const uint8_t> pdsch_processor_impl::encode(span<const uint8_t> data,
                                                 unsigned            codeword_id,
                                                 unsigned            nof_layers,
                                                 unsigned            Nre,
                                                 const pdu_t&        pdu)
{
  // Select codeword specific parameters.
  unsigned                                   rv         = pdu.codewords[codeword_id].rv;
  modulation_scheme                          modulation = pdu.codewords[codeword_id].modulation;
  static_vector<uint8_t, MAX_CODEWORD_SIZE>& codeword   = codewords[codeword_id];

  // Prepare encoder configuration.
  segment_config encoder_config;
  encoder_config.base_graph     = pdu.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = pdu.tbs_lbrm_bytes * 8;
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = Nre;

  // Prepare codeword size.
  codeword.resize(Nre * nof_layers * get_bits_per_symbol(modulation));

  // Encode codeword.
  encoder->encode(codeword, data, encoder_config);

  // Return the view of the codeword.
  return codeword;
}

void pdsch_processor_impl::modulate(resource_grid_writer&            grid,
                                    span<const span<const uint8_t> > temp_codewords,
                                    const pdu_t&                     pdu)
{
  unsigned nof_codewords = temp_codewords.size();

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

  modulator->modulate(grid, temp_codewords, modulator_config);
}

void pdsch_processor_impl::put_dmrs(resource_grid_writer& grid, const pdu_t& pdu)
{
  // Generate allocation mask.
  std::array<bool, MAX_RB> rb_mask;
  pdu.freq_alloc.get_allocation_mask(rb_mask, pdu.bwp_start_rb, pdu.bwp_size_rb);

  dmrs_pdsch_processor::config_t dmrs_config;
  dmrs_config.slot                 = pdu.slot;
  dmrs_config.reference_point_k_rb = (pdu.ref_point == pdu_t::CRB0 ? pdu.bwp_start_rb : 0);
  dmrs_config.type                 = pdu.dmrs;
  dmrs_config.scrambling_id        = pdu.scrambling_id;
  dmrs_config.n_scid               = pdu.n_scid;
  dmrs_config.amplitude            = convert_dB_to_amplitude(pdu.ratio_pdsch_dmrs_to_sss_dB);
  dmrs_config.symbols_mask         = pdu.dmrs_symbol_mask;
  dmrs_config.rb_mask              = rb_mask;
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
  srsran_assert(pdu.codewords.size() == nof_codewords,
                "Expected {} codewords and got {} for {} ports",
                nof_codewords,
                pdu.codewords.size(),
                pdu.ports.size());
  srsran_assert(pdu.codewords.size() == data.size(),
                "The number of codewords ({}) does not match the number of transport blocks ({})",
                pdu.codewords.size(),
                data.size());
  srsran_assert(pdu.tbs_lbrm_bytes > 0 && pdu.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, lesser than or equal to {} bytes",
                pdu.tbs_lbrm_bytes,
                ldpc::MAX_CODEBLOCK_SIZE / 8);
  srsran_assert(pdu.nof_cdm_groups_without_data >= 1 && pdu.nof_cdm_groups_without_data <= 3,
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
  unsigned Nre = compute_nof_re(pdu);

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

std::unique_ptr<pdsch_processor> srsgnb::create_pdsch_processor(pdsch_processor_configuration& config)
{
  return std::make_unique<pdsch_processor_impl>(config);
}
