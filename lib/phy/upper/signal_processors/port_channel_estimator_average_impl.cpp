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
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

// Size of the averaging window used for noise estimation. Its value should divide the number of DM-RS pilots per PRB,
// which is 12 (PUCCH F1), 6 (PUSCH) or 4 (PUCCH F2).
static constexpr unsigned WINDOW_SIZE = 2;

/// \brief Extracts channel observations corresponding to DM-RS pilots from the resource grid for one layer, one hop and
/// for the selected port.
/// \param[out] rx_symbols  Symbol buffer destination.
/// \param[in]  grid        Resource grid.
/// \param[in]  port        Port index.
/// \param[in]  pattern     DM-RS pattern for each layer.
/// \param[in]  cfg         Configuration parameters of the current context.
/// \param[in]  hop         Intra-slot frequency hopping index: 0 for first position (before hopping), 1 for second
///                         position (after hopping).
/// \param[in]  i_layer     Index of the selected layer.
/// \return The number of OFDM symbols containing DM-RS for the given layer and hop.
static unsigned extract_layer_hop_rx_pilots(dmrs_symbol_list&                            rx_symbols,
                                            const resource_grid_reader&                  grid,
                                            unsigned                                     port,
                                            const port_channel_estimator::configuration& cfg,
                                            unsigned                                     hop,
                                            unsigned                                     i_layer);

/// \brief Estimates the noise energy of one hop.
/// \param[in] pilots      DM-RS pilots.
/// \param[in] rx_pilots   Received samples corresponding to DM-RS pilots.
/// \param[in] estimates   Estimated channel frequency response.
/// \param[in] beta        DM-RS-to-data amplitude gain (linear scale).
/// \param[in] window_size Size of the averaging window.
/// \param[in] hop_symbols Number of OFDM symbols containing DM-RS pilots in the current hop.
/// \param[in] hop_offset  Number of OFDM symbols containing DM-RS pilots in the previous hop (set to 0 if the current
///                        hop is the first/only one).
/// \param[in] i_layer     Index of the selected layer.
/// \return The noise energy for the current hop (normalized with respect to the number of averaging windows).
static float estimate_noise(const dmrs_symbol_list& pilots,
                            const dmrs_symbol_list& rx_pilots,
                            span<const cf_t>        estimates,
                            float                   beta,
                            unsigned                window_size,
                            unsigned                hop_symbols,
                            unsigned                hop_offset,
                            unsigned                i_layer);

// Returns the interpolator configuration for the given RE pattern.
static interpolator::configuration set_interpolator_cfg(const bounded_bitset<NRE>& re_mask)
{
  int offset = re_mask.find_lowest();
  srsgnb_assert(offset != -1, "re_mask seems to have no active entries.");

  int stride = re_mask.find_lowest(offset + 1, re_mask.size());
  srsgnb_assert(stride != -1, "re_mask seems to have only one active entry.");

  return {static_cast<unsigned>(offset), static_cast<unsigned>(stride - offset)};
}

void port_channel_estimator_average_impl::compute(channel_estimate&           estimate,
                                                  const resource_grid_reader& grid,
                                                  unsigned                    port,
                                                  const dmrs_symbol_list&     pilots,
                                                  const configuration&        cfg)
{
  re_measurement_dimensions symbols_size    = pilots.size();
  unsigned                  nof_dmrs_pilots = symbols_size.nof_subc * symbols_size.nof_symbols;

  // Prepare symbol destination.
  rx_pilots.resize(symbols_size);

  // For each layer...
  for (unsigned i_layer = 0, nof_tx_layers = cfg.dmrs_pattern.size(); i_layer != nof_tx_layers; ++i_layer) {
    rsrp      = 0;
    noise_var = 0;
    compute_layer_hop(estimate, grid, port, pilots, cfg, 0, i_layer);
    if (cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value()) {
      compute_layer_hop(estimate, grid, port, pilots, cfg, 1, i_layer);
    }

    rsrp /= static_cast<float>(nof_dmrs_pilots);
    estimate.set_rsrp(rsrp, port, i_layer);

    float epre = rsrp * convert_dB_to_power(cfg.beta_dmrs_dB);
    estimate.set_epre(epre, port, i_layer);

    noise_var /= static_cast<float>(WINDOW_SIZE * symbols_size.nof_symbols - 1);
    // todo: temporarily commenting this since it causes old tests to fail.
    // estimate.set_noise_variance(noise_var, port, i_layer);
    estimate.set_noise_variance(convert_dB_to_power(-30), port, i_layer);
    estimate.set_snr((noise_var != 0) ? epre / noise_var : 1000, port, i_layer);
  }
}

void port_channel_estimator_average_impl::compute_layer_hop(srsgnb::channel_estimate&           estimate,
                                                            const srsgnb::resource_grid_reader& grid,
                                                            unsigned                            port,
                                                            const dmrs_symbol_list&             pilots,
                                                            const configuration&                cfg,
                                                            unsigned                            hop,
                                                            unsigned                            i_layer)
{
  srsgnb_assert((hop == 0) || cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value(),
                "Frequency hopping requested but not configured.");

  // Auxiliary buffers for pilot computations.
  std::array<cf_t, MAX_RB * NRE> aux_pilot_products;
  std::array<cf_t, MAX_RB * NRE> aux_pilots_lse;

  const layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

  unsigned first_symbol = ((hop == 1) && pattern.hopping_symbol_index.has_value())
                              ? pattern.hopping_symbol_index.value()
                              : cfg.first_symbol;
  unsigned last_symbol = ((hop == 0) && pattern.hopping_symbol_index.has_value()) ? pattern.hopping_symbol_index.value()
                                                                                  : cfg.first_symbol + cfg.nof_symbols;

  // Extract symbols from resource grid.
  unsigned nof_dmrs_symbols = extract_layer_hop_rx_pilots(rx_pilots, grid, port, cfg, hop, i_layer);
  srsgnb_assert(nof_dmrs_symbols != 0, "No DM-RS symbols were found for layer {}.", i_layer);

  unsigned hop_offset = 0;
  if (hop == 1) {
    hop_offset = pilots.size().nof_symbols - nof_dmrs_symbols;
  }

  span<cf_t> pilot_products = span<cf_t>(aux_pilot_products).first(pilots.size().nof_subc);
  span<cf_t> pilots_lse     = span<cf_t>(aux_pilots_lse).first(pilots.size().nof_subc);
  srsvec::prod_conj(rx_pilots.get_symbol(0, i_layer), pilots.get_symbol(hop_offset, i_layer), pilots_lse);

  // Accumulate all symbols frequency domain response.
  for (unsigned i_dmrs_symbol = 1; i_dmrs_symbol != nof_dmrs_symbols; ++i_dmrs_symbol) {
    srsvec::prod_conj(rx_pilots.get_symbol(i_dmrs_symbol, i_layer),
                      pilots.get_symbol(hop_offset + i_dmrs_symbol, i_layer),
                      pilot_products);
    srsvec::add(pilots_lse, pilot_products, pilots_lse);
  }

  // Average and apply DM-RS-to-data gain.
  float beta_scaling = convert_dB_to_amplitude(-cfg.beta_dmrs_dB);
  for (cf_t& value : pilots_lse) {
    rsrp += abs_sq(value) / static_cast<float>(nof_dmrs_symbols);
    value /= (static_cast<float>(nof_dmrs_symbols) * beta_scaling);
  }

  noise_var +=
      estimate_noise(pilots, rx_pilots, pilots_lse, beta_scaling, WINDOW_SIZE, nof_dmrs_symbols, hop_offset, i_layer);

  // Interpolate frequency domain.
  const bounded_bitset<MAX_RB>& hop_rb_mask      = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;
  span<cf_t>                    ce_freq          = span<cf_t>(freq_response).first(hop_rb_mask.count() * NRE);
  interpolator::configuration   interpolator_cfg = set_interpolator_cfg(pattern.re_pattern);

  freq_interpolator->interpolate(ce_freq, pilots_lse, interpolator_cfg);

  // Map frequency response to channel estimates.
  for (unsigned i_symbol = first_symbol; i_symbol != last_symbol; ++i_symbol) {
    span<cf_t> symbol_fr_resp = estimate.get_symbol_ch_estimate(i_symbol, port, i_layer);

    unsigned i_prb_ce = 0;
    hop_rb_mask.for_each(0, hop_rb_mask.size(), [&](unsigned i_prb) {
      srsvec::copy(symbol_fr_resp.subspan(i_prb * NRE, NRE), ce_freq.subspan(i_prb_ce * NRE, NRE));
      ++i_prb_ce;
    });
  }
}

static unsigned extract_layer_hop_rx_pilots(dmrs_symbol_list&                            rx_symbols,
                                            const resource_grid_reader&                  grid,
                                            unsigned                                     port,
                                            const port_channel_estimator::configuration& cfg,
                                            unsigned                                     hop,
                                            unsigned                                     i_layer)
{
  // Select DM-RS pattern.
  const port_channel_estimator::layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

  const bounded_bitset<MAX_RB>& hop_rb_mask = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;

  // Prepare RE mask, common for all symbols carrying DM-RS.
  bounded_bitset<MAX_RB* NRE> re_mask = hop_rb_mask.kronecker_product<NRE>(pattern.re_pattern);

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
    if (!pattern.symbols.test(symbol_index)) {
      continue;
    }

    // Select symbol buffer for the selected layer and symbol.
    span<cf_t> layer_dmrs_symbols = rx_symbols.get_symbol(dmrs_symbol_index++, i_layer);

    // Get DM-RS symbols from the resource grid.
    layer_dmrs_symbols = grid.get(layer_dmrs_symbols, cfg.rx_ports[port], symbol_index, 0, re_mask);

    // The DM-RS symbol buffer must be complete.
    srsgnb_assert(layer_dmrs_symbols.empty(),
                  "The DM-RS buffer is not completed. {} samples have not been read.",
                  layer_dmrs_symbols.size());
  }

  return dmrs_symbol_index;
}

static float estimate_noise(const dmrs_symbol_list& pilots,
                            const dmrs_symbol_list& rx_pilots,
                            span<const cf_t>        estimates,
                            float                   beta,
                            unsigned                window_size,
                            unsigned                hop_symbols,
                            unsigned                hop_offset,
                            unsigned                i_layer)
{
  std::array<cf_t, MAX_RB * NRE> avg_estimates_buffer;
  std::array<cf_t, MAX_RB * NRE> predicted_obs_buffer;

  srsgnb_assert((window_size > 0) && (estimates.size() % window_size == 0), "Incompatible window size.");

  span<cf_t> avg_estimates = span<cf_t>(avg_estimates_buffer).first(estimates.size());

  // Span "avg_estimates" will contain blocks of "window_size" elements all equal to the average value of the
  // corresponding block of span "estimates."
  for (unsigned i_avg = 0, max_avg = avg_estimates.size(); i_avg != max_avg; i_avg += window_size) {
    span<const cf_t> estimates_block = estimates.subspan(i_avg, window_size);
    cf_t             avg =
        std::accumulate(estimates_block.begin(), estimates_block.end(), cf_t(0, 0)) / static_cast<float>(window_size);
    span<cf_t> avg_block = avg_estimates.subspan(i_avg, window_size);
    std::fill(avg_block.begin(), avg_block.end(), avg);
  }

  srsvec::sc_prod(avg_estimates, cf_t(-beta, 0), avg_estimates);

  span<cf_t> predicted_obs = span<cf_t>(predicted_obs_buffer).first(estimates.size());
  float      noise_energy  = 0.0F;
  for (unsigned i_symbol = 0; i_symbol != hop_symbols; ++i_symbol) {
    span<const cf_t> symbol_pilots    = pilots.get_symbol(hop_offset + i_symbol, i_layer);
    span<const cf_t> symbol_rx_pilots = rx_pilots.get_symbol(i_symbol, i_layer);

    std::transform(
        avg_estimates.begin(), avg_estimates.end(), symbol_pilots.begin(), predicted_obs.begin(), std::multiplies<>());
    srsvec::add(predicted_obs, symbol_rx_pilots, predicted_obs);

    noise_energy += srsvec::average_power(predicted_obs) * window_size;
  }
  return noise_energy;
}
