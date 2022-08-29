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
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/prod.h"

using namespace srsgnb;

void port_channel_estimator_average_impl::compute(channel_estimate&           estimate,
                                                  const resource_grid_reader& grid,
                                                  unsigned                    port,
                                                  const dmrs_symbol_list&     symbols,
                                                  span<const dmrs_pattern>    pattern,
                                                  const configuration&        cfg)
{
  // Prepare symbol destination.
  temp_symbols.resize(symbols.size());

  // Extract symbols from resource grid.
  extract_symbols(temp_symbols, grid, port, pattern, cfg);

  // Compute least-square estimates and store in them in temp_symbols.
  for (unsigned i_layer = 0; i_layer != cfg.nof_tx_layers; ++i_layer) {
    srsvec::prod_conj(temp_symbols.get_slice(i_layer), symbols.get_slice(i_layer), temp_symbols.get_slice(i_layer));
  }

  // Interpolate.
  interpolate(estimate, temp_symbols, port, cfg.nof_tx_layers);
}

void port_channel_estimator_average_impl::extract_symbols(dmrs_symbol_list&           symbol_buffer,
                                                          const resource_grid_reader& grid,
                                                          unsigned                    port,
                                                          span<const dmrs_pattern>    pattern,
                                                          const configuration&        cfg)
{
  // For each layer...
  for (unsigned i_layer = 0; i_layer != cfg.nof_tx_layers; ++i_layer) {
    // Prepare RE mask, common for all symbols carrying DM-RS.
    std::array<bool, MAX_RB* NRE> temp_re_mask = {};
    span<bool>                    re_mask      = span<bool>(temp_re_mask).first(cfg.rb_mask.size() * NRE);

    // For each RB, copy RE pattern mask.
    pattern[i_layer].rb_mask.for_each(0, pattern[i_layer].rb_mask.size(), [&](unsigned rb_index) {
      srsvec::copy(re_mask.subspan(rb_index * NRE, NRE), pattern[i_layer].re_pattern);
    });

    // For each OFDM symbol in the transmission...
    for (unsigned symbol_index      = cfg.first_symbol,
                  symbol_index_end  = cfg.first_symbol + cfg.nof_symbols,
                  dmrs_symbol_index = 0;
         symbol_index != symbol_index_end;
         ++symbol_index) {
      // Skip if the symbol does not carry DMRS.
      if (!pattern[i_layer].symbols[symbol_index]) {
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

void port_channel_estimator_average_impl::interpolate(channel_estimate&       estimate,
                                                      const dmrs_symbol_list& lse,
                                                      unsigned                port,
                                                      unsigned                nof_tx_layers)
{
  // For each layer...
  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    // Select layer LSE.
    span<const cf_t> lse_layer = lse.get_slice(i_layer);

    // Average LSE.
    cf_t average =
        std::accumulate(lse_layer.begin(), lse_layer.end(), cf_t(0.0)) / static_cast<float>(lse_layer.size());

    // Get channel estimates view for the port and layer.
    span<cf_t> ch_estimate = estimate.get_path_ch_estimate(port, i_layer);

    // Fill the channel estimates with the average estimated channel.
    std::fill(ch_estimate.begin(), ch_estimate.end(), average);
  }
}
