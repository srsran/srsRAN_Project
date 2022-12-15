/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pucch_processor_format1_impl.h"
#include "pucch_helper.h"
#include "srsgnb/srsvec/add.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

// DM-RS symbol allocation pattern for PUCCH Format 1, as defined in TS38.211 Section 6.4.1.3.1.2.
static const bounded_bitset<MAX_NSYMB_PER_SLOT> pucch_format1_dmrs_symb_mask =
    {true, false, true, false, true, false, true, false, true, false, true, false, true, false};

// Implements TS 38.211 table 6.4.1.3.1.1-1: Number of DM-RS symbols and the corresponding N_PUCCH.
static unsigned dmrs_pucch_symbols(const dmrs_pucch_processor::config_t& config, unsigned m_prime)
{
  if (config.intra_slot_hopping) {
    if (m_prime == 0) {
      switch (config.nof_symbols) {
        case 4:
        case 5:
          return 1;
        case 6:
        case 7:
        case 8:
        case 9:
          return 2;
        case 10:
        case 11:
        case 12:
        case 13:
          return 3;
        case 14:
          return 4;
        default:; // Do nothing.
      }
    } else {
      switch (config.nof_symbols) {
        case 4:
        case 6:
          return 1;
        case 5:
        case 7:
        case 8:
        case 10:
          return 2;
        case 9:
        case 11:
        case 12:
        case 14:
          return 3;
        case 13:
          return 4;
        default:; // Do nothing.
      }
    }
  } else if (m_prime == 0) {
    switch (config.nof_symbols) {
      case 4:
        return 2;
      case 5:
      case 6:
        return 3;
      case 7:
      case 8:
        return 4;
      case 9:
      case 10:
        return 5;
      case 11:
      case 12:
        return 6;
      case 13:
      case 14:
        return 7;
      default:; // Do nothing.
    }
  }
  srsgnb_assert(false, "Invalid case nof_symbols={} and m_prime={}", config.nof_symbols, m_prime);
  return 0;
}

void dmrs_pucch_processor_format1_impl::generate_dmrs_pattern(layer_dmrs_pattern& mask, const config_t& config)
{
  // All RE within the PRB are allocated to DM-RS.
  mask.re_pattern = ~bounded_bitset<NRE>(NRE);

  // Set PRB allocation. Recall that PUCCH Format 1 occupies a single PRB.
  mask.rb_mask.resize(config.starting_prb + 1);
  mask.rb_mask.set(config.starting_prb);

  if (config.intra_slot_hopping) {
    // Set second hop PRB allocation.
    mask.rb_mask2.resize(config.second_hop_prb + 1);
    mask.rb_mask2.set(config.second_hop_prb);

    // Set the hopping symbol index, indicating the start of the second hop. In case of a PUCCH allocation with an odd
    // number of symbols, the second hop is one symbol larger than the first one. See TS38.211 Table 6.3.2.4.1-1.
    mask.hopping_symbol_index = config.start_symbol_index + (config.nof_symbols / 2);
  }

  // Set used symbols. DM-RS occupies the even-numbered symbols (0, 2, ...) of the PUCCH allocation, starting from the
  // first symbol allocated to PUCCH Format 1. See TS38.211 Section 6.4.1.3.1.2.
  mask.symbols.resize(get_nsymb_per_slot(config.cp));
  pucch_format1_dmrs_symb_mask.for_each(0, config.nof_symbols, [&mask, &config](unsigned bitpos) {
    mask.symbols.set(config.start_symbol_index + bitpos);
  });
}

void dmrs_pucch_processor_format1_impl::sequence_generation(span<srsgnb::cf_t>                    sequence,
                                                            const dmrs_pucch_processor::config_t& pucch_config,
                                                            const sequence_generation_config&     cfg,
                                                            unsigned                              symbol) const
{
  // Compute alpha index.
  unsigned alpha_idx = helper.get_alpha_index(
      pucch_config.slot, pucch_config.cp, pucch_config.n_id, symbol, pucch_config.initial_cyclic_shift, 0);

  // Get r_uv sequence from the sequence collection.
  span<const cf_t> r_uv = sequence_collection->get(cfg.u, cfg.v, alpha_idx);
  srsgnb_assert(!r_uv.empty(), "low PAPR sequence not implemented for the specified u, v and alpha");

  // Get orthogonal sequence.
  cf_t w_i_m = occ.get_sequence_value(cfg.n_pucch, pucch_config.time_domain_occ, cfg.m);

  // Compute z(n) = w(i) * r_uv(n).
  srsvec::sc_prod(r_uv, w_i_m, sequence);
}

void dmrs_pucch_processor_format1_impl::estimate(channel_estimate&                     estimate,
                                                 const resource_grid_reader&           grid,
                                                 const dmrs_pucch_processor::config_t& config)
{
  unsigned nof_rx_ports = config.ports.size();

  // Prepare DM-RS symbol buffer dimensions. Recall that PUCCH Format 1 occupies a single PRB.
  re_measurement_dimensions dims;
  dims.nof_subc = NRE;
  if (config.intra_slot_hopping) {
    // Consider the number of DM-RS symbols of both hops.
    dims.nof_symbols = dmrs_pucch_symbols(config, 0) + dmrs_pucch_symbols(config, 1);
  } else {
    dims.nof_symbols = dmrs_pucch_symbols(config, 0);
  }
  dims.nof_slices = PUCCH_MAX_LAYERS;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  // Resize channel estimate.
  estimate.resize(
      {config.starting_prb + 1, config.start_symbol_index + config.nof_symbols, nof_rx_ports, PUCCH_MAX_LAYERS});

  // Generate the DM-RS allocation pattern.
  generate_dmrs_pattern(temp_pattern, config);

  unsigned u, v;
  // Compute group sequence.
  helper.compute_group_sequence(config.group_hopping, config.n_id, u, v);

  // Index to write into the DM-RS buffer.
  unsigned i_dmrs_symb = 0;

  // Generate DM-RS sequences for even-numbered symbols within the PUCCH allocation, as per TS38.211
  // Section 6.4.1.3.1.
  for (unsigned i_hop = 0, i_symb = config.start_symbol_index; i_hop < (config.intra_slot_hopping ? 2 : 1); ++i_hop) {
    // Get number of symbols carrying DM-RS in the current hop
    unsigned nof_dmrs_symb_hop = dmrs_pucch_symbols(config, i_hop);

    srsgnb_assert(nof_dmrs_symb_hop > 0, "Error getting number of DM-RS symbols");

    // For each symbol in the current hop carrying DM-RS.
    for (unsigned i_dmrs_symb_hop = 0; i_dmrs_symb_hop != nof_dmrs_symb_hop; ++i_dmrs_symb_hop, i_symb += 2) {
      // Aggregate parameters and generate sequence.
      sequence_generation_config cfg = {.u = u, .v = v, .m = i_dmrs_symb_hop, .n_pucch = nof_dmrs_symb_hop};
      sequence_generation(temp_symbols.get_symbol(i_dmrs_symb++, 0), config, cfg, i_symb);
    }
  }

  // Prepare channel estimator configuration.
  port_channel_estimator::configuration est_cfg = {};
  est_cfg.dmrs_pattern.emplace_back(temp_pattern);
  est_cfg.scs          = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.first_symbol = config.start_symbol_index;
  est_cfg.nof_symbols  = config.nof_symbols;
  est_cfg.rx_ports     = config.ports;

  // Perform estimation for each receive port.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    ch_estimator->compute(estimate, grid, i_port, temp_symbols, est_cfg);
  }
}
