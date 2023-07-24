/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "port_channel_estimator_average_impl.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/convolution.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

/// Coefficients of a raised cosine FIR filter with roll-off 0.2, 3-symbol span, 10 samples per symbol (total 31
/// samples). The filter will be applied to a frequency-domain signal to attenuate time components for t > 1/10 Ts, with
/// Ts the symbol time (recall that the CP is ~1/14 Ts).
static constexpr unsigned                             MAX_FILTER_LENGTH = 31;
static constexpr std::array<float, MAX_FILTER_LENGTH> RC_FILTER         = {
            -0.0641253, -0.0660711, -0.0611526, -0.0485918, -0.0281126, 0.0000000,  0.0348830, 0.0751249,
            0.1188406,  0.1637874,  0.2075139,  0.2475302,  0.2814857,  0.3073415,  0.3235207, 0.3290274,
            0.3235207,  0.3073415,  0.2814857,  0.2475302,  0.2075139,  0.1637874,  0.1188406, 0.0751249,
            0.0348830,  0.0000000,  -0.0281126, -0.0485918, -0.0611526, -0.0660711, -0.0641253};

namespace {
/// \brief Provides access to customized raised-cosine filters.
///
/// Besides the FIR coefficients, the class also provides access to a correction term to be applied to the tails of the
/// output signal to compensate for the truncation.
class filter_type
{
public:
  /// \brief Creates a customized raised-cosine filter by resampling and renormalizing the coefficients in RC_FILTER.
  ///
  /// The filter is tuned to work with DM-RS placed on nof_rbs resource blocks every other stride-th resource element.
  filter_type(unsigned nof_rbs, unsigned stride)
  {
    // Span at most 3 RBs.
    nof_rbs = std::min(nof_rbs, 3U);
    // Number of coefficients at full rate.
    unsigned nof_coefs     = nof_rbs * 10 + 1;
    unsigned nof_coefs_out = nof_coefs / 2 / stride;
    // Index of the first coefficient copied from the general filter to the customized one.
    unsigned n_first = MAX_FILTER_LENGTH / 2 - nof_coefs_out * stride;
    // Number of coefficients after downsampling.
    nof_coefs_out = 2 * nof_coefs_out + 1;

    rc_filter       = rc_filter.first(nof_coefs_out);
    tail_correction = tail_correction.first(nof_coefs_out);
    unsigned n      = n_first;
    float    total  = 0;
    for (unsigned i_coef = 0; i_coef != nof_coefs_out; ++i_coef) {
      rc_filter[i_coef] = RC_FILTER[n];
      total += rc_filter[i_coef];
      tail_correction[i_coef] = 1.0F / total;
      n += stride;
    }
    // Normalize the filter so that the sum of its coefficients is 1 and the tail correction coefficients accordingly.
    srsvec::sc_prod(rc_filter, 1 / total, rc_filter);
    srsvec::sc_prod(tail_correction, total, tail_correction);

    tail_correction = tail_correction.subspan(nof_coefs_out / 2, nof_coefs_out / 2);
  }

  // Customized raised-cosine FIR coefficients.
  span<float> rc_filter = {filter_coefs};
  // Tail correction coefficients.
  span<float> tail_correction = {correction_coefs};

private:
  // Auxiliary buffers.
  std::array<float, MAX_FILTER_LENGTH>     filter_coefs     = {};
  std::array<float, MAX_FILTER_LENGTH / 2> correction_coefs = {};
};
} // namespace

/// \brief Extracts channel observations corresponding to DM-RS pilots from the resource grid for one layer, one hop
/// and for the selected port.
/// \param[out] rx_symbols  Symbol buffer destination.
/// \param[in]  grid        Resource grid.
/// \param[in]  port        Port index.
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
/// \param[in] hop_symbols Number of OFDM symbols containing DM-RS pilots in the current hop.
/// \param[in] hop_offset  Number of OFDM symbols containing DM-RS pilots in the previous hop (set to 0 if the current
///                        hop is the first/only one).
/// \param[in] i_layer     Index of the selected layer.
/// \return The noise energy for the current hop.
static float estimate_noise(const dmrs_symbol_list& pilots,
                            const dmrs_symbol_list& rx_pilots,
                            span<const cf_t>        estimates,
                            float                   beta,
                            unsigned                hop_symbols,
                            unsigned                hop_offset,
                            unsigned                i_layer);

/// \brief Estimates the time alignment based on one hop.
///
/// \param[in] pilots_lse The estimated channel (only for REs carrying DM-RS).
/// \param[in] pattern    DM-RS pattern for the current layer.
/// \param[in] hop        Intra-slot frequency hopping index: 0 for first position (before hopping), 1 for second
///                       position (after hopping).
/// \param[in] idft       Inverse DFT processor.
/// \return The estimated time alignment as a number of samples (the sampling frequency is given by the DFT processor).
static float estimate_time_alignment(span<const cf_t>                                  pilots_lse,
                                     const port_channel_estimator::layer_dmrs_pattern& pattern,
                                     unsigned                                          hop,
                                     dft_processor*                                    idft);

// Returns the interpolator configuration for the given RE pattern.
static interpolator::configuration configure_interpolator(const bounded_bitset<NRE>& re_mask)
{
  int offset = re_mask.find_lowest();
  srsran_assert(offset != -1, "re_mask seems to have no active entries.");

  int stride = re_mask.find_lowest(offset + 1, re_mask.size());
  srsran_assert(stride != -1, "re_mask seems to have only one active entry.");

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
    rsrp             = 0;
    epre             = 0;
    noise_var        = 0;
    time_alignment_s = 0;

    // compute_layer_hop updates rsrp, epre, niose_var and time_alignment_s.
    compute_layer_hop(estimate, grid, port, pilots, cfg, 0, i_layer);
    if (cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value()) {
      compute_layer_hop(estimate, grid, port, pilots, cfg, 1, i_layer);
      time_alignment_s /= 2.0F;
    }

    rsrp /= static_cast<float>(nof_dmrs_pilots);
    epre /= static_cast<float>(nof_dmrs_pilots);
    time_alignment_s /= (DFT_SIZE * scs_to_khz(cfg.scs) * 1000.0F);

    estimate.set_rsrp(rsrp, port, i_layer);
    estimate.set_epre(epre, port, i_layer);
    estimate.set_time_alignment(phy_time_unit::from_seconds(time_alignment_s), port, i_layer);

    noise_var /= static_cast<float>(nof_dmrs_pilots - 1);
    estimate.set_noise_variance(noise_var, port, i_layer);

    srsran_assert(cfg.scaling > 0, "The DM-RS to data scaling factor should be a positive number.");
    // Compute the estimated data received power by scaling the RSRP.
    float datarp = rsrp / cfg.scaling / cfg.scaling;
    estimate.set_snr((noise_var != 0) ? datarp / noise_var : 1000, port, i_layer);
  }
}

void port_channel_estimator_average_impl::compute_layer_hop(srsran::channel_estimate&           estimate,
                                                            const srsran::resource_grid_reader& grid,
                                                            unsigned                            port,
                                                            const dmrs_symbol_list&             pilots,
                                                            const configuration&                cfg,
                                                            unsigned                            hop,
                                                            unsigned                            i_layer)
{
  srsran_assert((hop == 0) || cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value(),
                "Frequency hopping requested but not configured.");

  // Auxiliary buffers for pilot computations.
  std::array<cf_t, MAX_RB * NRE> aux_pilot_products;
  std::array<cf_t, MAX_RB * NRE> aux_pilots_lse;
  std::array<cf_t, MAX_RB * NRE> aux_pilots_lse_rc;

  const layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

  unsigned first_symbol = ((hop == 1) && pattern.hopping_symbol_index.has_value())
                              ? pattern.hopping_symbol_index.value()
                              : cfg.first_symbol;
  unsigned last_symbol = ((hop == 0) && pattern.hopping_symbol_index.has_value()) ? pattern.hopping_symbol_index.value()
                                                                                  : cfg.first_symbol + cfg.nof_symbols;

  // Extract symbols from resource grid.
  unsigned nof_dmrs_symbols = extract_layer_hop_rx_pilots(rx_pilots, grid, port, cfg, hop, i_layer);
  srsran_assert(nof_dmrs_symbols != 0, "No DM-RS symbols were found for layer {}.", i_layer);

  unsigned hop_offset = 0;
  if (hop == 1) {
    hop_offset = pilots.size().nof_symbols - nof_dmrs_symbols;
  }

  span<cf_t> pilot_products = span<cf_t>(aux_pilot_products).first(pilots.size().nof_subc);
  span<cf_t> pilots_lse     = span<cf_t>(aux_pilots_lse).first(pilots.size().nof_subc);
  srsvec::prod_conj(rx_pilots.get_symbol(0, i_layer), pilots.get_symbol(hop_offset, i_layer), pilots_lse);

  epre += std::real(srsvec::dot_prod(rx_pilots.get_symbol(0, i_layer), rx_pilots.get_symbol(0, i_layer)));

  // Accumulate all symbols frequency domain response.
  for (unsigned i_dmrs_symbol = 1; i_dmrs_symbol != nof_dmrs_symbols; ++i_dmrs_symbol) {
    srsvec::prod_conj(rx_pilots.get_symbol(i_dmrs_symbol, i_layer),
                      pilots.get_symbol(hop_offset + i_dmrs_symbol, i_layer),
                      pilot_products);
    srsvec::add(pilots_lse, pilot_products, pilots_lse);

    epre += std::real(
        srsvec::dot_prod(rx_pilots.get_symbol(i_dmrs_symbol, i_layer), rx_pilots.get_symbol(i_dmrs_symbol, i_layer)));
  }

  // Average and apply DM-RS-to-data gain.
  float beta_scaling  = cfg.scaling;
  float total_scaling = 1.0F / (static_cast<float>(nof_dmrs_symbols) * beta_scaling);
  srsvec::sc_prod(pilots_lse, total_scaling, pilots_lse);

  const bounded_bitset<MAX_RB>& hop_rb_mask      = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;
  interpolator::configuration   interpolator_cfg = configure_interpolator(pattern.re_pattern);

  // Apply a low-pass filter to remove some noise ("high-time" components).
  filter_type rc(hop_rb_mask.count(), interpolator_cfg.stride);
  span<cf_t>  filtered_pilots_lse = span<cf_t>(aux_pilots_lse_rc).first(pilots_lse.size());
  srsvec::convolution_same(filtered_pilots_lse, pilots_lse, rc.rc_filter);

  // Compensate tails (note that we take the reverse iterator for the back tail).
  auto tail_front = filtered_pilots_lse.begin();
  auto tail_back  = filtered_pilots_lse.rbegin();
  for (const auto cc : rc.tail_correction) {
    *tail_front++ *= cc;
    *tail_back++ *= cc;
  }

  rsrp += std::real(srsvec::dot_prod(filtered_pilots_lse, filtered_pilots_lse)) * beta_scaling * beta_scaling *
          static_cast<float>(nof_dmrs_symbols);

  noise_var +=
      estimate_noise(pilots, rx_pilots, filtered_pilots_lse, beta_scaling, nof_dmrs_symbols, hop_offset, i_layer);

  time_alignment_s += estimate_time_alignment(filtered_pilots_lse, pattern, hop, idft.get());

  // Interpolate frequency domain.
  span<cf_t> ce_freq = span<cf_t>(freq_response).first(hop_rb_mask.count() * NRE);
  freq_interpolator->interpolate(ce_freq, filtered_pilots_lse, interpolator_cfg);

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
    srsran_assert(layer_dmrs_symbols.empty(),
                  "The DM-RS buffer is not completed. {} samples have not been read.",
                  layer_dmrs_symbols.size());
  }

  return dmrs_symbol_index;
}

static float estimate_noise(const dmrs_symbol_list& pilots,
                            const dmrs_symbol_list& rx_pilots,
                            span<const cf_t>        estimates,
                            float                   beta,
                            unsigned                hop_symbols,
                            unsigned                hop_offset,
                            unsigned                i_layer)
{
  std::array<cf_t, MAX_RB * NRE> avg_estimates_buffer;
  std::array<cf_t, MAX_RB * NRE> predicted_obs_buffer;

  span<cf_t> scaled_estimates = span<cf_t>(avg_estimates_buffer).first(estimates.size());

  srsvec::sc_prod(estimates, cf_t(-beta, 0), scaled_estimates);

  span<cf_t> predicted_obs = span<cf_t>(predicted_obs_buffer).first(estimates.size());
  float      noise_energy  = 0.0F;
  for (unsigned i_symbol = 0; i_symbol != hop_symbols; ++i_symbol) {
    span<const cf_t> symbol_pilots    = pilots.get_symbol(hop_offset + i_symbol, i_layer);
    span<const cf_t> symbol_rx_pilots = rx_pilots.get_symbol(i_symbol, i_layer);

    srsvec::prod(scaled_estimates, symbol_pilots, predicted_obs);
    srsvec::add(predicted_obs, symbol_rx_pilots, predicted_obs);

    noise_energy += std::real(srsvec::dot_prod(predicted_obs, predicted_obs));
  }
  return noise_energy;
}

static float estimate_time_alignment(span<const cf_t>                                  pilots_lse,
                                     const port_channel_estimator::layer_dmrs_pattern& pattern,
                                     unsigned                                          hop,
                                     dft_processor*                                    idft)
{
  const bounded_bitset<MAX_RB>& hop_rb_mask = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;

  // Prepare RE mask, common for all symbols carrying DM-RS.
  bounded_bitset<MAX_RB* NRE> re_mask = hop_rb_mask.kronecker_product<NRE>(pattern.re_pattern);

  static constexpr unsigned DFT_SIZE = port_channel_estimator_average_impl::DFT_SIZE;
  srsran_assert(re_mask.size() < DFT_SIZE,
                "The resource grid size {} is larger than the port channel estimator DFT size {}.",
                re_mask.size(),
                DFT_SIZE);
  srsran_assert(pilots_lse.size() == re_mask.count(),
                "Expected {} channel estimates, provided {}.",
                re_mask.size(),
                pilots_lse.size());

  span<cf_t> channel_observed_freq = idft->get_input();
  srsvec::zero(channel_observed_freq);
  re_mask.for_each(0, re_mask.size(), [&channel_observed_freq, &pilots_lse, i_lse = 0U](unsigned i_re) mutable {
    channel_observed_freq[i_re] = pilots_lse[i_lse++];
  });

  span<const cf_t> channel_observed_time = idft->run();

  static constexpr unsigned  HALF_CP_LENGTH     = ((144 / 2) * DFT_SIZE) / 2048;
  std::pair<unsigned, float> observed_max_delay = srsvec::max_abs_element(channel_observed_time.first(HALF_CP_LENGTH));
  std::pair<unsigned, float> observed_max_advance = srsvec::max_abs_element(channel_observed_time.last(HALF_CP_LENGTH));
  if (observed_max_delay.second >= observed_max_advance.second) {
    return static_cast<float>(observed_max_delay.first);
  }
  return -static_cast<float>(HALF_CP_LENGTH - observed_max_advance.first);
}
