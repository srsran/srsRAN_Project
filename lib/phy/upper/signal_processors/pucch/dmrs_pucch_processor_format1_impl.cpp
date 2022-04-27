
#include "dmrs_pucch_processor_format1_impl.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

void dmrs_pucch_processor_format1_impl::sequence_generation(span<srsgnb::cf_t>                    sequence,
                                                            const dmrs_pucch_processor::config_t& pucch_config,
                                                            const seq_generation_config&          cfg,
                                                            unsigned                              symbol) const
{
  // Get Alpha index
  unsigned alpha_idx = 0;
  pucch_alpha_index(alpha_idx, pucch_config.slot, pucch_config.n_id, symbol, pucch_config.initial_cyclic_shift, 0);

  // Get r_uv sequence from the sequence collection
  span<const cf_t> r_uv = sequence_collection->get(cfg.u, cfg.v, alpha_idx);
  srsran_assert(r_uv.size() != 0, "low PAPR sequence not implemented for the specified u, v and alpha");

  // Get orthogonal sequence
  cf_t w_i_m = occ->get_sequence_value(cfg.n_pucch, pucch_config.time_domain_occ, cfg.m);

  // Compute z(n) = w(i) * r_uv(n)
  srsvec::sc_prod(r_uv, w_i_m, sequence);
}

void dmrs_pucch_processor_format1_impl::mapping(resource_grid_writer&    grid,
                                                span<const srsgnb::cf_t> sequence,
                                                unsigned                 start_prb,
                                                unsigned                 symbol) const
{
  std::array<resource_grid_coordinate, NRE> coordinates = {};
  for (unsigned i = 0; i < NRE; ++i) {
    coordinates[i].subcarrier = start_prb * NRE + i;
    coordinates[i].symbol     = symbol;
  }
  grid.put(0, coordinates, sequence);
}

void dmrs_pucch_processor_format1_impl::map(resource_grid_writer& grid, const dmrs_pucch_processor::config_t& config)
{
  unsigned u, v;
  // Get group sequence.
  pucch_group_sequence(config.group_hopping, config.n_id, u, v);

  // First symbol index.
  unsigned l_prime = config.start_symbol_index;

  // Clause 6.4.1.3.1.2 specifies l = 0, 2, 4...
  for (unsigned m_prime = 0, l = 0; m_prime < (config.intra_slot_hopping ? 2 : 1); m_prime++) {
    // Get number of symbols carrying DMRS
    unsigned n_pucch = pucch_dmrs_symbols(config, m_prime);
    srsran_assert(n_pucch > 0, "Error getting number of symbols");

    // Get the starting PRB.
    unsigned starting_prb = (m_prime == 0) ? config.starting_prb : config.second_hop_prb;

    // For each symbol carrying DM-RS
    for (unsigned m = 0; m < n_pucch; m++, l += 2) {
      unsigned symbol = l_prime + l;

      // Generate sequence.
      std::array<cf_t, NRE> sequence = {};
      seq_generation_config cfg      = {.u = u, .v = v, .m = m, .n_pucch = n_pucch};
      sequence_generation(sequence, config, cfg, symbol);

      // Map sequence in symbols.
      mapping(grid, sequence, starting_prb, symbol);
    }
  }
}