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
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/srsvec/sc_prod.h"
#include <numeric>

using namespace srsgnb;

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
        default:; // Do nothing
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
        default:; // Do nothing
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
      default:; // Do nothing
    }
  }
  srsran_assert(false, "Invalid case nof_symbols={} and m_prime={}", config.nof_symbols, m_prime);
  return 0;
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
  srsran_assert(!r_uv.empty(), "low PAPR sequence not implemented for the specified u, v and alpha");

  // Get orthogonal sequence.
  cf_t w_i_m = occ->get_sequence_value(cfg.n_pucch, pucch_config.time_domain_occ, cfg.m);

  // Compute z(n) = w(i) * r_uv(n).
  srsvec::sc_prod(r_uv, w_i_m, sequence);
}

void dmrs_pucch_processor_format1_impl::mapping(span<cf_t>                  ce,
                                                const resource_grid_reader& grid,
                                                unsigned                    start_prb,
                                                unsigned                    symbol) const
{
  grid.get(ce, 0, symbol, start_prb * NRE);
}

void dmrs_pucch_processor_format1_impl::estimate(channel_estimate&                     estimate,
                                                 const resource_grid_reader&           grid,
                                                 const dmrs_pucch_processor::config_t& config)
{
  unsigned u, v;
  // Compute group sequence.
  helper.compute_group_sequence(config.group_hopping, config.n_id, u, v);

  // Array for the channel estimates.
  std::array<std::array<cf_t, NRE>, 2U * PUCCH_FORMAT1_N_MAX> ce = {};
  // Array for measurements
  std::array<cf_t, 2U * PUCCH_FORMAT1_N_MAX> corr = {};

  // Total number of processed PUCCH DMRS symbols.
  unsigned n_pucch_sum = 0;
  // First symbol index.
  unsigned l_prime = config.start_symbol_index;

  // Clause 6.4.1.3.1.2 specifies l = 0, 2, 4...
  for (unsigned m_prime = 0, l = 0; m_prime < (config.intra_slot_hopping ? 2 : 1); m_prime++) {
    // Get number of symbols carrying DMRS
    unsigned n_pucch = dmrs_pucch_symbols(config, m_prime);

    srsran_assert(n_pucch > 0, "Error getting number of symbols");

    // Get the starting PRB.
    unsigned starting_prb = (m_prime == 0) ? config.starting_prb : config.second_hop_prb;

    // For each symbol carrying DM-RS.
    for (unsigned m = 0; m != n_pucch; m++, l += 2) {
      unsigned symbol = l_prime + l;

      std::array<cf_t, NRE> sequence     = {};
      std::array<cf_t, NRE> slot_symbols = {};
      // Aggregate parameters and generate sequence.
      sequence_generation_config cfg = {.u = u, .v = v, .m = m, .n_pucch = n_pucch};
      sequence_generation(sequence, config, cfg, symbol);

      // Get DMRS symbols from the grid.
      mapping(slot_symbols, grid, starting_prb, symbol);

      // Perform estimation (calculate least square estimates for this symbol).
      srsvec::prod_conj(slot_symbols, sequence, ce[n_pucch_sum]);

      n_pucch_sum++;
    }
  }

  // Perform measurements.
  float rsrp = 0.0F;
  float epre = 0.0F;
  for (unsigned m = 0; m < n_pucch_sum; ++m) {
    // Compute RSRP.
    cf_t correlation = std::accumulate(std::begin(ce[m]), std::end(ce[m]), cf_t(0));
    corr[m]          = correlation / static_cast<float>(NRE);
    rsrp += std::norm(corr[m]);

    // Compute EPRE.
    cf_t avg_power = 0;
    std::for_each(std::begin(ce[m]), std::end(ce[m]), [&avg_power](cf_t ce) {
      // Conjugate dot-product.
      avg_power += (ce * std::conj(ce));
    });
    epre += std::real(avg_power) / static_cast<float>(NRE);
  }

  // Average measurements.
  rsrp /= n_pucch_sum;
  epre /= n_pucch_sum;

  // Set power measures.
  rsrp = std::min(rsrp, epre);
  estimate.set_rsrp(rsrp, 0);
  estimate.set_epre(epre, 0);
  float noise_var = std::max(epre - rsrp, 1e-6F);
  estimate.set_noise_variance(noise_var, 0);
  estimate.set_snr(rsrp / noise_var, 0);

  span<cf_t> ce_span = estimate.get_path_ch_estimate_wr(0);
  // Interpolates between DMRS symbols.
  for (uint32_t m = 0; m < n_pucch_sum; ++m) {
    span<cf_t> ce_span_rb = ce_span.subspan(m * NRE, NRE);

    if (m != n_pucch_sum - 1) {
      // If it is not the last symbol with DMRS, average between.
      srsvec::add(ce[m], ce[m + 1], ce_span_rb);
      srsvec::sc_prod(ce_span_rb, 0.5F, ce_span_rb);
    } else if (m != 0) {
      // Extrapolate for the last if more than 1 are provided.
      srsvec::sc_prod(ce[m], 3.0F, ce_span_rb);
      // Subtraction ce[m] - ce[m - 1].
      std::transform(
          std::begin(ce_span_rb), std::end(ce_span_rb), std::begin(ce[m - 1]), std::begin(ce_span_rb), std::minus<>());
      srsvec::sc_prod(ce_span_rb, 0.5F, ce_span_rb);
    } else {
      // Simply copy the estimated channel.
      srsvec::copy(ce_span_rb, ce[m]);
    }
  }
}
