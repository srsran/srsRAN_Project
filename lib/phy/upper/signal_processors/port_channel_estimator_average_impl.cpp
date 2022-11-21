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
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

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
  // Prepare symbol destination.
  temp_symbols.resize(symbols.size());

  // Extract symbols from resource grid.
  extract_symbols(temp_symbols, grid, port, cfg);

  // Compute least-square estimates and store in them in temp_symbols.
  for (unsigned i_layer = 0, i_layer_end = cfg.dmrs_pattern.size(); i_layer != i_layer_end; ++i_layer) {
    srsvec::prod_conj(temp_symbols.get_slice(i_layer), symbols.get_slice(i_layer), temp_symbols.get_slice(i_layer));
    estimate.set_noise_variance(convert_dB_to_power(-30), port, i_layer);
  }

  // For each layer...
  unsigned nof_tx_layers = cfg.dmrs_pattern.size();
  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    // Select first DM-RS symbol LSE.
    span<cf_t> symbol_lse_0 = temp_symbols.get_symbol(0, i_layer);

    // Accumulate all symbols frequency domain response.
    for (unsigned i_dmrs_symbol = 1, i_dmrs_symbol_end = symbols.size().nof_symbols; i_dmrs_symbol != i_dmrs_symbol_end;
         ++i_dmrs_symbol) {
      span<cf_t> symbol_lse = temp_symbols.get_symbol(i_dmrs_symbol, i_layer);
      srsvec::add(symbol_lse_0, symbol_lse, symbol_lse_0);
    }

    // Average.
    for (cf_t& value : symbol_lse_0) {
      value /= static_cast<float>(symbols.size().nof_symbols);
    }

    // Interpolate frequency domain.
    span<cf_t>                  ce_freq = span<cf_t>(temp_ce_freq).first(cfg.dmrs_pattern[0].rb_mask.count() * NRE);
    interpolator::configuration interpolator_cfg = set_interpolator_cfg(cfg.dmrs_pattern[0].re_pattern);

    freq_interpolator->interpolate(ce_freq, symbol_lse_0, interpolator_cfg);

    // Map frequency response to channel estimates.
    for (unsigned i_symbol = cfg.first_symbol, i_symbol_end = cfg.first_symbol + cfg.nof_symbols;
         i_symbol != i_symbol_end;
         ++i_symbol) {
      span<cf_t> symbol_ce = estimate.get_symbol_ch_estimate(i_symbol, port, i_layer);

      unsigned i_prb_ce = 0;
      cfg.dmrs_pattern[0].rb_mask.for_each(0, cfg.dmrs_pattern[0].rb_mask.size(), [&](unsigned i_prb) {
        srsvec::copy(symbol_ce.subspan(i_prb * NRE, NRE), ce_freq.subspan(i_prb_ce * NRE, NRE));
        ++i_prb_ce;
      });
    }
  }
}

void port_channel_estimator_average_impl::extract_symbols(dmrs_symbol_list&           symbol_buffer,
                                                          const resource_grid_reader& grid,
                                                          unsigned                    port,
                                                          const configuration&        cfg)
{
  // For each layer...
  for (unsigned i_layer = 0, i_layer_end = cfg.dmrs_pattern.size(); i_layer != i_layer_end; ++i_layer) {
    // Select DM-RS pattern.
    const layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

    // Prepare RE mask, common for all symbols carrying DM-RS.
    std::array<bool, MAX_RB* NRE> temp_re_mask = {};
    span<bool>                    re_mask      = span<bool>(temp_re_mask).first(pattern.rb_mask.size() * NRE);

    // For each RB, copy RE pattern mask.
    pattern.rb_mask.for_each(0, pattern.rb_mask.size(), [&](unsigned rb_index) {
      srsvec::copy(re_mask.subspan(rb_index * NRE, NRE), pattern.re_pattern);
    });

    // Prepare RE mask, common for all symbols carrying DM-RS after the first hop.
    std::array<bool, MAX_RB* NRE> temp_re_mask2 = {};
    span<bool>                    re_mask2      = span<bool>(temp_re_mask2).first(pattern.rb_mask2.size() * NRE);

    // For each RB, copy RE pattern mask after the first hop.
    pattern.rb_mask.for_each(0, pattern.rb_mask.size(), [&](unsigned rb_index) {
      srsvec::copy(re_mask.subspan(rb_index * NRE, NRE), pattern.re_pattern);
    });

    // For each OFDM symbol in the transmission...
    for (unsigned symbol_index      = cfg.first_symbol,
                  symbol_index_end  = cfg.first_symbol + cfg.nof_symbols,
                  dmrs_symbol_index = 0;
         symbol_index != symbol_index_end;
         ++symbol_index) {
      // Skip if the symbol does not carry DM-RS.
      if (!pattern.symbols[symbol_index]) {
        continue;
      }

      // Select symbol buffer for the selected layer and symbol.
      span<cf_t> dmrs_symbols_layer = symbol_buffer.get_symbol(dmrs_symbol_index++, i_layer);

      // Get DM-RS symbols from the resource grid.
      dmrs_symbols_layer = grid.get(dmrs_symbols_layer, cfg.rx_ports[port], symbol_index, 0, re_mask);

      // The DMRS symbol buffer must be complete.
      srsgnb_assert(dmrs_symbols_layer.empty(),
                    "The DMRS buffer is not completed. {} samples have not been read.",
                    dmrs_symbols_layer.size());
    }
  }
}
