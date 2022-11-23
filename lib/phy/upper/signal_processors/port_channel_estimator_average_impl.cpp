/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "port_channel_estimator_average_impl.h"
#include "srsgnb/srsvec/add.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/prod.h"

using namespace srsgnb;

// \brief Extracts DM-RS symbols from the resource grid for one layer, one hop and for the selected port.
// \param[out] symbols  Symbol buffer destination.
// \param[in]  grid     Resource grid.
// \param[in]  port     Port index.
// \param[in]  pattern  DM-RS pattern for each layer.
// \param[in]  cfg      Configuration parameters of the current context.
// \param[in]  i_layer  Index of the selected layer.
// \param[in]  hop      Intra-slot frequency hopping index: 0 for first position (before hopping), 1 for second position
//                      (after hopping).
// \return The number of ODFM symbols containing DM-RS for the given layer and hop.
static unsigned extract_layer_hop_symbols(dmrs_symbol_list&                            symbols,
                                          const resource_grid_reader&                  grid,
                                          unsigned                                     port,
                                          const port_channel_estimator::configuration& cfg,
                                          unsigned                                     i_layer,
                                          unsigned                                     hop);

static interpolator::configuration set_interpolator_cfg(span<const bool> re_mask)
{
  span<const bool>::iterator offset_it = std::find(re_mask.begin(), re_mask.end(), true);
  srsgnb_assert(offset_it != re_mask.end(), "re_mask seems to have no active entries.");
  span<const bool>::iterator stride_it = std::find(offset_it + 1, re_mask.end(), true);
  srsgnb_assert(stride_it != re_mask.end(), "re_mask seems to have only one active entry.");

  return {static_cast<unsigned>(offset_it - re_mask.begin()), static_cast<unsigned>(stride_it - offset_it)};
}

void port_channel_estimator_average_impl::compute(channel_estimate&           estimate,
                                                  const resource_grid_reader& grid,
                                                  unsigned                    port,
                                                  const dmrs_symbol_list&     symbols,
                                                  const configuration&        cfg)
{
  re_measurement_dimensions symbols_size    = symbols.size();
  unsigned                  nof_dmrs_pilots = symbols_size.nof_subc * symbols_size.nof_symbols;

  // Prepare symbol destination.
  temp_symbols.resize(symbols_size);

  // For each layer...
  for (unsigned i_layer = 0, nof_tx_layers = cfg.dmrs_pattern.size(); i_layer != nof_tx_layers; ++i_layer) {
    // todo: proper noise variance estimation.
    estimate.set_noise_variance(convert_dB_to_power(-30), port, i_layer);

    rsrp_tmp = 0;
    compute_layer_hop(estimate, grid, port, symbols, cfg, i_layer, 0);
    if (cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value()) {
      compute_layer_hop(estimate, grid, port, symbols, cfg, i_layer, 1);
    }
    rsrp_tmp /= static_cast<float>(nof_dmrs_pilots);
    estimate.set_rsrp(rsrp_tmp, port, i_layer);
    estimate.set_epre(rsrp_tmp * convert_dB_to_power(cfg.beta_dmrs), port, i_layer);
  }
}

void port_channel_estimator_average_impl::compute_layer_hop(srsgnb::channel_estimate&           estimate,
                                                            const srsgnb::resource_grid_reader& grid,
                                                            unsigned                            port,
                                                            const dmrs_symbol_list&             symbols,
                                                            const configuration&                cfg,
                                                            unsigned                            i_layer,
                                                            unsigned                            hop)
{
  srsgnb_assert((hop == 0) || cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value(),
                "Frequency hopping requested but not configured.");

  const layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

  unsigned first_symbol = ((hop == 1) && pattern.hopping_symbol_index.has_value())
                              ? pattern.hopping_symbol_index.value()
                              : cfg.first_symbol;
  unsigned last_symbol = ((hop == 0) && pattern.hopping_symbol_index.has_value()) ? pattern.hopping_symbol_index.value()
                                                                                  : cfg.first_symbol + cfg.nof_symbols;

  // Extract symbols from resource grid.
  unsigned nof_dmrs_symbols = extract_layer_hop_symbols(temp_symbols, grid, port, cfg, i_layer, hop);
  srsgnb_assert(nof_dmrs_symbols != 0, "No DM-RS symbols were found for layer {}.", i_layer);

  unsigned hop_offset = 0;
  if (hop == 1) {
    hop_offset = symbols.size().nof_symbols - nof_dmrs_symbols;
  }

  srsvec::prod_conj(temp_symbols.get_symbol(0, i_layer),
                    symbols.get_symbol(hop_offset, i_layer),
                    temp_symbols.get_symbol(0, i_layer));
  // Select first DM-RS symbol LSE.
  span<cf_t> symbol_lse_0 = temp_symbols.get_symbol(0, i_layer);

  // Accumulate all symbols frequency domain response.
  for (unsigned i_dmrs_symbol = 1; i_dmrs_symbol != nof_dmrs_symbols; ++i_dmrs_symbol) {
    srsvec::prod_conj(temp_symbols.get_symbol(i_dmrs_symbol, i_layer),
                      symbols.get_symbol(hop_offset + i_dmrs_symbol, i_layer),
                      temp_symbols.get_symbol(i_dmrs_symbol, i_layer));
    span<cf_t> symbol_lse = temp_symbols.get_symbol(i_dmrs_symbol, i_layer);
    srsvec::add(symbol_lse_0, symbol_lse, symbol_lse_0);
  }

  // Average and apply DM-RS-to-data gain.
  for (cf_t& value : symbol_lse_0) {
    rsrp_tmp += abs_sq(value) / static_cast<float>(nof_dmrs_symbols);
    value /= (static_cast<float>(nof_dmrs_symbols) * convert_dB_to_amplitude(-cfg.beta_dmrs));
  }

  const bounded_bitset<MAX_RB>& hop_rb_mask = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;

  // Interpolate frequency domain.
  span<cf_t>                  ce_freq          = span<cf_t>(temp_ce_freq).first(hop_rb_mask.count() * NRE);
  interpolator::configuration interpolator_cfg = set_interpolator_cfg(pattern.re_pattern);

  freq_interpolator->interpolate(ce_freq, symbol_lse_0, interpolator_cfg);

  // Map frequency response to channel estimates.
  for (unsigned i_symbol = first_symbol; i_symbol != last_symbol; ++i_symbol) {
    span<cf_t> symbol_ce = estimate.get_symbol_ch_estimate(i_symbol, port, i_layer);

    unsigned i_prb_ce = 0;
    hop_rb_mask.for_each(0, hop_rb_mask.size(), [&](unsigned i_prb) {
      srsvec::copy(symbol_ce.subspan(i_prb * NRE, NRE), ce_freq.subspan(i_prb_ce * NRE, NRE));
      ++i_prb_ce;
    });
  }
}

unsigned extract_layer_hop_symbols(dmrs_symbol_list&                            symbol_buffer,
                                   const resource_grid_reader&                  grid,
                                   unsigned                                     port,
                                   const port_channel_estimator::configuration& cfg,
                                   unsigned                                     i_layer,
                                   unsigned                                     hop)
{
  // Select DM-RS pattern.
  const port_channel_estimator::layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

  // Prepare RE mask, common for all symbols carrying DM-RS.
  std::array<bool, MAX_RB* NRE> temp_re_mask = {};
  span<bool>                    re_mask      = span<bool>(temp_re_mask).first(pattern.rb_mask.size() * NRE);

  const bounded_bitset<MAX_RB>& hop_rb_mask = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;
  // For each RB, copy RE pattern mask.
  hop_rb_mask.for_each(0, hop_rb_mask.size(), [&](unsigned rb_index) {
    srsvec::copy(re_mask.subspan(rb_index * NRE, NRE), pattern.re_pattern);
  });

  unsigned symbol_index      = ((hop == 1) && pattern.hopping_symbol_index.has_value())
                                   ? pattern.hopping_symbol_index.value()
                                   : cfg.first_symbol;
  unsigned symbol_index_end  = ((hop == 0) && pattern.hopping_symbol_index.has_value())
                                   ? pattern.hopping_symbol_index.value()
                                   : cfg.first_symbol + cfg.nof_symbols;
  unsigned dmrs_symbol_index = 0;
  // For each OFDM symbol in the transmission...
  for (; symbol_index != symbol_index_end; ++symbol_index) {
    // Skip if the symbol does not carry DM-RS.
    if (!pattern.symbols[symbol_index]) {
      continue;
    }

    // Select symbol buffer for the selected layer and symbol.
    span<cf_t> layer_dmrs_symbols = symbol_buffer.get_symbol(dmrs_symbol_index++, i_layer);

    // Get DM-RS symbols from the resource grid.
    layer_dmrs_symbols = grid.get(layer_dmrs_symbols, cfg.rx_ports[port], symbol_index, 0, re_mask);

    // The DM-RS symbol buffer must be complete.
    srsgnb_assert(layer_dmrs_symbols.empty(),
                  "The DM-RS buffer is not completed. {} samples have not been read.",
                  layer_dmrs_symbols.size());
  }

  return dmrs_symbol_index;
}
