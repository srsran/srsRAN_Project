/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pucch_processor_format2_impl.h"
#include "srsgnb/srsvec/prod.h"
#include <numeric>

using namespace srsgnb;

unsigned dmrs_pucch_processor_format2_impl::c_init(unsigned symbol, const dmrs_pucch_processor::config_t& config)
{
  uint64_t n_slot = config.slot.slot_index();
  uint64_t n_id   = config.n_id_0;

  return ((get_nsymb_per_slot(config.cp) * n_slot + symbol + 1) * (2 * n_id + 1) * pow2(17) + 2 * n_id) % pow2(31);
}

void dmrs_pucch_processor_format2_impl::sequence_generation(span<cf_t>                            sequence,
                                                            unsigned                              symbol,
                                                            unsigned                              starting_prb,
                                                            const dmrs_pucch_processor::config_t& config)
{
  // Initialise pseudo-random generator.
  prg->init(c_init(symbol, config));

  // Discard unused pilots.
  prg->advance(starting_prb * NOF_DMRS_PER_RB * 2);

  // Generate sequence.
  prg->generate(sequence.first(config.nof_prb * NOF_DMRS_PER_RB), M_SQRT1_2);
}

void dmrs_pucch_processor_format2_impl::mapping(span<cf_t>                  ce,
                                                const resource_grid_reader& grid,
                                                unsigned                    start_prb,
                                                unsigned                    nof_prb,
                                                unsigned                    symbol) const
{
  unsigned k_start = start_prb * NRE + 1;
  unsigned nof_re  = nof_prb * NRE;

  static_vector<bool, PUCCH_FORMAT2_MAX_NPRB * NRE> mask(nof_re, false);
  for (unsigned k = 0; k < nof_re; k += STRIDE) {
    mask[k] = true;
  }
  grid.get(ce, 0, symbol, k_start, mask);
}

void dmrs_pucch_processor_format2_impl::estimate(channel_estimate&                     estimate,
                                                 const resource_grid_reader&           grid,
                                                 const dmrs_pucch_processor::config_t& config)
{
  // Array for the channel estimates.
  std::array<std::array<cf_t, MAX_NOF_DMRS_PER_SYMBOL>, PUCCH_FORMAT2_MAX_NSYMB> ce = {};
  // Array for measurements.
  std::array<cf_t, PUCCH_FORMAT2_MAX_NSYMB> corr = {};

  unsigned nof_symbols = config.intra_slot_hopping ? config.nof_symbols : PUCCH_FORMAT2_MIN_NSYMB;

  unsigned l_start = config.start_symbol_index;
  unsigned l_end   = config.start_symbol_index + nof_symbols;
  unsigned nof_ref = config.nof_prb * NOF_DMRS_PER_RB;

  // For each symbol carrying DM-RS (up to 2 symbols maximum).
  for (unsigned l = l_start, i = 0; l != l_end; ++l, ++i) {
    std::array<cf_t, MAX_NOF_DMRS_PER_SYMBOL> sequence     = {};
    std::array<cf_t, MAX_NOF_DMRS_PER_SYMBOL> slot_symbols = {};

    span<cf_t> symb_span{slot_symbols.data(), nof_ref};
    span<cf_t> sequence_span{sequence.data(), nof_ref};

    unsigned start_prb = (i > 0) ? config.second_hop_prb : config.starting_prb;

    // Generate sequence.
    sequence_generation(sequence_span, l, start_prb, config);

    // Get DM-RS symbols from the grid.
    mapping(symb_span, grid, start_prb, config.nof_prb, l);

    // Perform estimation (calculate least square estimates for this symbol).
    srsvec::prod_conj(symb_span, sequence_span, {ce[i].data(), nof_ref});
  }

  // Perform measurements
  float rsrp = 0.0f;
  float epre = 0.0f;
  for (unsigned i = 0; i < nof_symbols; ++i) {
    // Compute RSRP.
    cf_t correlation = std::accumulate(std::begin(ce[i]), std::end(ce[i]), cf_t(0));
    corr[i]          = correlation / static_cast<float>(nof_ref);
    rsrp += std::norm(corr[i]);

    // Compute EPRE.
    cf_t avg_power = 0;
    std::for_each(std::begin(ce[i]), std::end(ce[i]), [&avg_power](cf_t ce) {
      // Conjugate dot-product.
      avg_power += (ce * std::conj(ce));
    });
    epre += std::real(avg_power) / static_cast<float>(nof_ref);
  }

  // Average measurements.
  rsrp /= nof_symbols;
  epre /= nof_symbols;

  // Set power measures.
  rsrp                         = std::min(rsrp, epre);
  estimate.rsrp                = rsrp;
  estimate.rsrp_dBfs           = convert_power_to_dB(rsrp);
  estimate.epre                = epre;
  estimate.epre_dBfs           = convert_power_to_dB(epre);
  estimate.noise_estimate      = std::max(epre - rsrp, 1e-6f);
  estimate.noise_estimate_dbFs = convert_power_to_dB(estimate.noise_estimate);
  estimate.snr                 = rsrp / estimate.noise_estimate;
  estimate.snr_db              = convert_power_to_dB(estimate.snr);
}