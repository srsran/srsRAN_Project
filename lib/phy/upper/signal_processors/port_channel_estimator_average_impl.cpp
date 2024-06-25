/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/srsvec/conversion.h"
#include "srsran/srsvec/convolution.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/mean.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/unwrap.h"
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

/// Create an array indexing the virtual pilots.
template <size_t N>
static constexpr std::array<int, N> create_index_array()
{
  std::array<int, port_channel_estimator_average_impl::MAX_V_PILOTS> out;
  int                                                                count = 0;
  for (int& v : out) {
    v = count++;
  }
  return out;
}
static const std::array<int, port_channel_estimator_average_impl::MAX_V_PILOTS> indices =
    create_index_array<port_channel_estimator_average_impl::MAX_V_PILOTS>();

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
  std::array<float, MAX_FILTER_LENGTH>     filter_coefs;
  std::array<float, MAX_FILTER_LENGTH / 2> correction_coefs;
};
} // namespace

/// \brief Computes some virtual pilots to improve estimation at the edges.
///
/// \param[out] out       Computed virtual pilots.
/// \param[in]  in_abs    Modulus of the true pilots used for extrapolation.
/// \param[in]  in_arg    Phase angle of the true pilots used for extrapolation - it is assumed unwrapped.
/// \param[in]  is_start  Direction flag: If true, virtual pilots are computed to precede the true ones. If false,
///                       virtual pilots will follow the true ones.
static void compute_v_pilots(span<cf_t> out, span<const float> in_abs, span<const float> in_arg, bool is_start);

/// Adds virtual pilots at both ends of \c old_pilots (\c old_pilots is assumed a centered subspan of new_pilots).
static void add_v_pilots(span<cf_t> new_pilots, span<const cf_t> old_pilots);

/// \brief Applies frequency domain smoothing strategy.
/// \param[out] enlarged_filtered_pilots_out   Smoothed pilots estimates.
/// \param[in]  enlarged_pilots_in             Pilots estimates.
/// \param[in]  nof_rb                         Number of resource blocks.
/// \param[in]  stride                         Reference signals stride in frequency domain.
/// \param[in]  fd_smoothing_strategy          Frequency domain smoothing strategy.
static void apply_fd_smoothing(span<cf_t>                                   enlarged_filtered_pilots_out,
                               span<cf_t>                                   enlarged_pilots_in,
                               unsigned                                     nof_rb,
                               unsigned                                     stride,
                               port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy);

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
/// \param[in] pilots            DM-RS pilots.
/// \param[in] rx_pilots         Received samples corresponding to DM-RS pilots.
/// \param[in] estimates         Estimated channel frequency response.
/// \param[in] beta              DM-RS-to-data amplitude gain (linear scale).
/// \param[in] dmrs_mask         Boolean mask identifying the OFDM symbols carrying DM-RS within the slot.
/// \param[in] scs               Subcarrier spacing.
/// \param[in] cfo               Carrier frequency offset.
/// \param[in] symbol_start_epochs   Cumulative duration of all CPs in the slot.
/// \param[in] compensate_cfo    Boolean flag to activate the CFO compensation.
/// \param[in] first_hop_symbol  Index of the first OFDM symbol of the current hop, within the slot.
/// \param[in] last_hop_symbol   Index of the last OFDM symbol of the current hop (not included), within the slot.
/// \param[in] hop_symbols       Number of OFDM symbols containing DM-RS pilots in the current hop.
/// \param[in] hop_offset        Number of OFDM symbols containing DM-RS pilots in the previous hop (set to 0 if the
///                              current hop is the first/only one).
/// \param[in] i_layer           Index of the selected layer.
/// \return The noise energy for the current hop.
static float estimate_noise(const dmrs_symbol_list&                   pilots,
                            const dmrs_symbol_list&                   rx_pilots,
                            span<const cf_t>                          estimates,
                            float                                     beta,
                            const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                            const subcarrier_spacing&                 scs,
                            std::optional<float>                      cfo,
                            span<const float>                         symbol_start_epochs,
                            bool                                      compensate_cfo,
                            unsigned                                  first_hop_symbol,
                            unsigned                                  last_hop_symbol,
                            unsigned                                  hop_symbols,
                            unsigned                                  hop_offset,
                            unsigned                                  i_layer);

/// \brief Estimates the time alignment based on one hop.
///
/// \param[in] pilots_lse   The estimated channel (only for REs carrying DM-RS).
/// \param[in] pattern      DM-RS pattern for the current layer.
/// \param[in] hop          Intra-slot frequency hopping index: 0 for first position (before hopping), 1 for second
///                         position (after hopping).
/// \param[in] scs          Subcarrier spacing.
/// \param[in] ta_estimator Time alignment estimator.
/// \return The estimated time alignment as a number of samples (the sampling frequency is given by the DFT processor).
static float estimate_time_alignment(span<const cf_t>                                  pilots_lse,
                                     const port_channel_estimator::layer_dmrs_pattern& pattern,
                                     unsigned                                          hop,
                                     subcarrier_spacing                                scs,
                                     time_alignment_estimator&                         ta_estimator);

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

  // Compute the cumulative duration of all CPs for the given subcarrier spacing.
  initialize_symbol_start_epochs(cfg.cp, cfg.scs);

  // For each layer...
  for (unsigned i_layer = 0, nof_tx_layers = cfg.dmrs_pattern.size(); i_layer != nof_tx_layers; ++i_layer) {
    rsrp             = 0;
    epre             = 0;
    noise_var        = 0;
    time_alignment_s = 0;
    cfo_normalized   = std::nullopt;

    // compute_layer_hop updates rsrp, epre, niose_var, time_alignment_s, and cfo_normalized.
    compute_layer_hop(estimate, grid, port, pilots, cfg, /*hop=*/0, i_layer);
    if (cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value()) {
      compute_layer_hop(estimate, grid, port, pilots, cfg, /*hop=*/1, i_layer);
      time_alignment_s /= 2.0F;
    }

    if (compensate_cfo && cfo_normalized.has_value()) {
      // Apply CFO to the estimated channel.
      float cfo = cfo_normalized.value();
      for (unsigned i_symbol = cfg.first_symbol, last_symbol = cfg.first_symbol + cfg.nof_symbols;
           i_symbol != last_symbol;
           ++i_symbol) {
        span<cbf16_t> symbol_ch_estimate = estimate.get_symbol_ch_estimate(i_symbol, port, i_layer);
        srsvec::sc_prod(
            symbol_ch_estimate, std::polar(1.0F, TWOPI * symbol_start_epochs[i_symbol] * cfo), symbol_ch_estimate);
      }
    }

    rsrp /= static_cast<float>(nof_dmrs_pilots);
    epre /= static_cast<float>(nof_dmrs_pilots);

    estimate.set_rsrp(rsrp, port, i_layer);
    estimate.set_epre(epre, port, i_layer);
    estimate.set_time_alignment(phy_time_unit::from_seconds(time_alignment_s), port, i_layer);
    estimate.set_cfo_Hz(cfo_normalized.has_value()
                            ? std::optional<float>(cfo_normalized.value() * scs_to_khz(cfg.scs) * 1000)
                            : std::nullopt,
                        port,
                        i_layer);

    noise_var /= static_cast<float>(nof_dmrs_pilots - 1);

    // Bound the noise variance from below.
    float min_noise_variance = rsrp / convert_dB_to_power(MAX_SINR_DB);
    noise_var                = std::max(min_noise_variance, noise_var);

    // Write the noise variance in the channel estimate result.
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
  std::array<cf_t, MAX_RB * NRE + 2 * MAX_V_PILOTS> aux_pilots_lse_rc;

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

  pilot_products = span<cf_t>(aux_pilot_products).first(pilots.size().nof_subc);
  pilots_lse     = span<cf_t>(aux_pilots_lse).subspan(MAX_V_PILOTS, pilots.size().nof_subc);

  // Preprocess the pilots and compute hop contributions to the EPRE (first ouput) and to the CFO (second output, if it
  // can be estimated). Recall that this method updates pilot_products and pilots_lse.
  std::pair<float, std::optional<float>> hop_results =
      preprocess_pilots_and_cfo(pilots, pattern.symbols, cfg.scs, first_symbol, last_symbol, hop_offset, i_layer);
  epre += hop_results.first;
  std::optional<float> cfo_hop = hop_results.second;
  if (cfo_hop.has_value()) {
    if (cfo_normalized.has_value()) {
      cfo_normalized = (cfo_normalized.value() + cfo_hop.value()) / 2;
    } else {
      cfo_normalized = cfo_hop.value();
    }
  }

  // Average and apply DM-RS-to-data gain.
  float beta_scaling  = cfg.scaling;
  float total_scaling = 1.0F / (static_cast<float>(nof_dmrs_symbols) * beta_scaling);
  srsvec::sc_prod(pilots_lse, total_scaling, pilots_lse);

  const bounded_bitset<MAX_RB>& hop_rb_mask      = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;
  interpolator::configuration   interpolator_cfg = configure_interpolator(pattern.re_pattern);

  // Apply a smoothing strategy to remove some noise ("high-time" components). Note that pilots_lse and
  // filtered_pilots_lse have some empty space at both sides, which we include with the enlarged version.
  span<cf_t> filtered_pilots_lse          = span<cf_t>(aux_pilots_lse_rc).subspan(MAX_V_PILOTS, pilots_lse.size());
  span<cf_t> enlarged_pilots_lse          = span<cf_t>(aux_pilots_lse).first(pilots_lse.size() + 2 * MAX_V_PILOTS);
  span<cf_t> enlarged_filtered_pilots_lse = span<cf_t>(aux_pilots_lse_rc).first(pilots_lse.size() + 2 * MAX_V_PILOTS);
  if (fd_smoothing_strategy != port_channel_estimator_fd_smoothing_strategy::filter) {
    // Enlarging is only necessary with the filter smoothing strategy.
    enlarged_pilots_lse          = pilots_lse;
    enlarged_filtered_pilots_lse = filtered_pilots_lse;
  }
  apply_fd_smoothing(enlarged_filtered_pilots_lse,
                     enlarged_pilots_lse,
                     hop_rb_mask.count(),
                     interpolator_cfg.stride,
                     fd_smoothing_strategy);

  rsrp += srsvec::average_power(filtered_pilots_lse) * filtered_pilots_lse.size() * beta_scaling * beta_scaling *
          static_cast<float>(nof_dmrs_symbols);

  noise_var += estimate_noise(pilots,
                              rx_pilots,
                              filtered_pilots_lse,
                              beta_scaling,
                              pattern.symbols,
                              cfg.scs,
                              cfo_hop,
                              symbol_start_epochs,
                              compensate_cfo,
                              first_symbol,
                              last_symbol,
                              nof_dmrs_symbols,
                              hop_offset,
                              i_layer);

  time_alignment_s += estimate_time_alignment(filtered_pilots_lse, pattern, hop, cfg.scs, *ta_estimator);

  // Extract RB mask lowest and highest RB. Also, determine if the allocation is contiguous.
  int      lowest_rb  = hop_rb_mask.find_lowest();
  int      highest_rb = hop_rb_mask.find_highest();
  unsigned rb_count   = hop_rb_mask.count();
  srsran_assert(highest_rb >= lowest_rb, "Invalid hop RB mask.");
  bool is_contiguous = (static_cast<unsigned>(highest_rb + 1 - lowest_rb) == rb_count);

  // Interpolate frequency domain.
  unsigned   nof_re  = rb_count * NRE;
  span<cf_t> ce_freq = span<cf_t>(freq_response).first(nof_re);
  freq_interpolator->interpolate(ce_freq, filtered_pilots_lse, interpolator_cfg);

  // Convert interpolation result to complex BFloat16.
  span<cbf16_t> ce_freq_cbf16 = span<cbf16_t>(freq_response_cbf16).first(nof_re);
  srsvec::convert(ce_freq_cbf16, ce_freq);

  // Map frequency response to channel estimates.
  for (unsigned i_symbol = first_symbol; i_symbol != last_symbol; ++i_symbol) {
    span<cbf16_t> symbol_fr_resp = estimate.get_symbol_ch_estimate(i_symbol, port, i_layer);

    if (is_contiguous) {
      srsvec::copy(symbol_fr_resp.subspan(lowest_rb * NRE, nof_re), ce_freq.first(nof_re));
      continue;
    }

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

void port_channel_estimator_average_impl::initialize_symbol_start_epochs(cyclic_prefix cp, subcarrier_spacing scs)
{
  unsigned nof_symbols_slot = get_nsymb_per_slot(cp);
  symbol_start_epochs       = span<float>(aux_symbol_start_epochs).first(nof_symbols_slot);

  // Compute cumulative duration of CPs.
  symbol_start_epochs[0] = cp.get_length(0, scs).to_seconds() * scs_to_khz(scs) * 1000;
  for (unsigned i_cp = 1; i_cp != nof_symbols_slot; ++i_cp) {
    symbol_start_epochs[i_cp] =
        symbol_start_epochs[i_cp - 1] + cp.get_length(i_cp, scs).to_seconds() * scs_to_khz(scs) * 1000 + 1.0F;
  }
}

std::pair<float, std::optional<float>>
port_channel_estimator_average_impl::preprocess_pilots_and_cfo(const dmrs_symbol_list&                   pilots,
                                                               const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                                                               const subcarrier_spacing&                 scs,
                                                               unsigned first_hop_symbol,
                                                               unsigned last_hop_symbol,
                                                               unsigned hop_offset,
                                                               unsigned i_layer)
{
  // Number of OFDM symbols carrying DM-RS in the current hop.
  unsigned nof_dmrs_symbols = dmrs_mask.slice(first_hop_symbol, last_hop_symbol).count();

  // Match received and transmitted pilots in the first DM-RS symbol and compute EPRE contribution.
  srsvec::prod_conj(rx_pilots.get_symbol(0, i_layer), pilots.get_symbol(hop_offset, i_layer), pilots_lse);
  float epre_hop = srsvec::average_power(rx_pilots.get_symbol(0, i_layer)) * rx_pilots.get_symbol(0, i_layer).size();

  if (nof_dmrs_symbols == 1) {
    return {epre_hop, std::nullopt};
  }

  // Match received and transmitted pilots in the second DM-RS symbol and compute EPRE contribution.
  srsvec::prod_conj(rx_pilots.get_symbol(1, i_layer), pilots.get_symbol(hop_offset + 1, i_layer), pilot_products);
  epre_hop += srsvec::average_power(rx_pilots.get_symbol(1, i_layer)) * rx_pilots.get_symbol(1, i_layer).size();

  // Use the first two DM-RS symbols to estimate the CFO.
  unsigned i_dmrs_0 = dmrs_mask.find_lowest(first_hop_symbol, last_hop_symbol);
  unsigned i_dmrs_1 = dmrs_mask.find_lowest(i_dmrs_0 + 1, dmrs_mask.size());

  float noisy_phase = std::arg(srsvec::dot_prod(pilot_products, pilots_lse));

  float cfo = noisy_phase / TWOPI / (symbol_start_epochs[i_dmrs_1] - symbol_start_epochs[i_dmrs_0]);

  // Compensate the CFO in the first two DM-RS symbols and combine them.
  if (compensate_cfo) {
    srsvec::sc_prod(pilots_lse, std::polar(1.0F, -TWOPI * symbol_start_epochs[i_dmrs_0] * cfo), pilots_lse);
    srsvec::sc_prod(pilot_products, std::polar(1.0F, -TWOPI * symbol_start_epochs[i_dmrs_1] * cfo), pilot_products);
  }
  srsvec::add(pilots_lse, pilot_products, pilots_lse);

  // If there are other DM-RS symbols in the hop, match them with the corresponding transmitted symbols, compensate the
  // CFO and combine with the previous DM-RS symbols.
  if (i_dmrs_1 < last_hop_symbol) {
    auto combine_pilots = [&, i_dmrs = 2](size_t i_symbol) mutable {
      srsvec::prod_conj(
          rx_pilots.get_symbol(i_dmrs, i_layer), pilots.get_symbol(hop_offset + i_dmrs, i_layer), pilot_products);
      if (compensate_cfo) {
        srsvec::sc_prod(pilot_products, std::polar(1.0F, -TWOPI * symbol_start_epochs[i_symbol] * cfo), pilot_products);
      }
      srsvec::add(pilots_lse, pilot_products, pilots_lse);
      epre_hop +=
          srsvec::average_power(rx_pilots.get_symbol(i_dmrs, i_layer)) * rx_pilots.get_symbol(i_dmrs, i_layer).size();
      ++i_dmrs;
    };

    dmrs_mask.for_each(i_dmrs_1 + 1, last_hop_symbol, combine_pilots);
  }

  return {epre_hop, cfo};
}

static float estimate_noise(const dmrs_symbol_list&                   pilots,
                            const dmrs_symbol_list&                   rx_pilots,
                            span<const cf_t>                          estimates,
                            float                                     beta,
                            const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                            const subcarrier_spacing&                 scs,
                            std::optional<float>                      cfo,
                            span<const float>                         symbol_start_epochs,
                            bool                                      compensate_cfo,
                            unsigned                                  first_hop_symbol,
                            unsigned                                  last_hop_symbol,
                            unsigned                                  hop_symbols,
                            unsigned                                  hop_offset,
                            unsigned                                  i_layer)
{
  std::array<cf_t, MAX_RB * NRE> avg_estimates_buffer;
  std::array<cf_t, MAX_RB * NRE> predicted_obs_buffer;

  span<cf_t> scaled_estimates = span<cf_t>(avg_estimates_buffer).first(estimates.size());

  srsvec::sc_prod(estimates, cf_t(-beta, 0), scaled_estimates);

  span<cf_t> predicted_obs = span<cf_t>(predicted_obs_buffer).first(estimates.size());
  float      noise_energy  = 0.0F;

  if (compensate_cfo && cfo.has_value()) {
    auto noise_cfo = [&, i_dmrs = 0](size_t i_symbol) mutable {
      span<const cf_t> symbol_pilots    = pilots.get_symbol(hop_offset + i_dmrs, i_layer);
      span<const cf_t> symbol_rx_pilots = rx_pilots.get_symbol(i_dmrs, i_layer);

      srsvec::prod(scaled_estimates, symbol_pilots, predicted_obs);
      srsvec::sc_prod(
          predicted_obs, std::polar(1.0F, TWOPI * symbol_start_epochs[i_symbol] * cfo.value()), predicted_obs);
      srsvec::add(predicted_obs, symbol_rx_pilots, predicted_obs);

      noise_energy += srsvec::average_power(predicted_obs) * predicted_obs.size();
      ++i_dmrs;
    };

    dmrs_mask.for_each(first_hop_symbol, last_hop_symbol, noise_cfo);
  } else {
    auto noise_no_cfo = [&, i_dmrs = 0](size_t i_symbol) mutable {
      span<const cf_t> symbol_pilots    = pilots.get_symbol(hop_offset + i_dmrs, i_layer);
      span<const cf_t> symbol_rx_pilots = rx_pilots.get_symbol(i_dmrs, i_layer);

      srsvec::prod(scaled_estimates, symbol_pilots, predicted_obs);
      srsvec::add(predicted_obs, symbol_rx_pilots, predicted_obs);

      noise_energy += srsvec::average_power(predicted_obs) * predicted_obs.size();
      ++i_dmrs;
    };
    dmrs_mask.for_each(first_hop_symbol, last_hop_symbol, noise_no_cfo);
  }

  return noise_energy;
}

static float estimate_time_alignment(span<const cf_t>                                  pilots_lse,
                                     const port_channel_estimator::layer_dmrs_pattern& pattern,
                                     unsigned                                          hop,
                                     subcarrier_spacing                                scs,
                                     time_alignment_estimator&                         ta_estimator)
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

  return ta_estimator.estimate(pilots_lse, re_mask, scs).time_alignment;
}

static void apply_fd_smoothing(span<cf_t>                                   enlarged_filtered_pilots_out,
                               span<cf_t>                                   enlarged_pilots_in,
                               unsigned                                     nof_rb,
                               unsigned                                     stride,
                               port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy)
{
  static constexpr unsigned MAX_V_PILOTS = port_channel_estimator_average_impl::MAX_V_PILOTS;

  switch (fd_smoothing_strategy) {
    case port_channel_estimator_fd_smoothing_strategy::mean:
      std::fill(
          enlarged_filtered_pilots_out.begin(), enlarged_filtered_pilots_out.end(), srsvec::mean(enlarged_pilots_in));
      break;
    case port_channel_estimator_fd_smoothing_strategy::filter: {
      // Generate a low pass filter.
      filter_type rc(nof_rb, stride);

      // Recover data part of extended_pilots_lse
      span<const cf_t> pilots_lse =
          enlarged_pilots_in.subspan(MAX_V_PILOTS, enlarged_pilots_in.size() - 2 * MAX_V_PILOTS);
      // Create virtual pilots.
      unsigned nof_v_pilots = std::min<unsigned>(MAX_V_PILOTS, rc.rc_filter.size() / 2);
      if (nof_rb == 1) {
        nof_v_pilots = pilots_lse.size() / nof_rb;
      }
      span<cf_t> enlarged_pilots =
          enlarged_pilots_in.subspan(MAX_V_PILOTS - nof_v_pilots, pilots_lse.size() + 2 * nof_v_pilots);
      add_v_pilots(enlarged_pilots, pilots_lse);

      span<cf_t> enlarged_filtered_pilots =
          enlarged_filtered_pilots_out.subspan(MAX_V_PILOTS - nof_v_pilots, pilots_lse.size() + 2 * nof_v_pilots);
      // Apply filter.
      srsvec::convolution_same(enlarged_filtered_pilots, enlarged_pilots, rc.rc_filter);
    } break;

    case port_channel_estimator_fd_smoothing_strategy::none:
    default:
      // No strategy.
      srsvec::copy(enlarged_filtered_pilots_out, enlarged_pilots_in);
      break;
  }
}

static void add_v_pilots(span<cf_t> new_pilots, span<const cf_t> old_pilots)
{
  std::array<float, port_channel_estimator_average_impl::MAX_V_PILOTS> aux_base_abs;
  std::array<float, port_channel_estimator_average_impl::MAX_V_PILOTS> aux_base_arg;

  unsigned nof_pilots   = old_pilots.size();
  unsigned nof_v_pilots = new_pilots.size() - nof_pilots;
  srsran_assert((nof_v_pilots >= 2) && ((nof_v_pilots & 1U) == 0U),
                "The total number of virtual pilots should be an even number not lower than 2, provided {}.",
                nof_v_pilots);
  nof_v_pilots /= 2;

  // Compute virtual pilots at the beginning of the band.
  span<const cf_t> base_pilots = old_pilots.first(nof_v_pilots);
  span<float>      base_abs    = span<float>(aux_base_abs).first(nof_v_pilots);
  std::transform(base_pilots.begin(), base_pilots.end(), base_abs.begin(), [](cf_t a) { return std::abs(a); });

  span<float> base_arg = span<float>(aux_base_arg).first(nof_v_pilots);
  srsvec::unwrap_arguments(base_arg, base_pilots);
  compute_v_pilots(new_pilots.first(nof_v_pilots), base_abs, base_arg, /* is_start = */ true);

  // Compute virtual pilots at the end of the band.
  base_pilots = old_pilots.last(nof_v_pilots);
  std::transform(base_pilots.begin(), base_pilots.end(), base_abs.begin(), [](cf_t a) { return std::abs(a); });

  srsvec::unwrap_arguments(base_arg, base_pilots);
  compute_v_pilots(new_pilots.last(nof_v_pilots), base_abs, base_arg, /* is start = */ false);
}

static void compute_v_pilots(span<cf_t> out, span<const float> in_abs, span<const float> in_arg, bool is_start)
{
  unsigned nof_v_pilots = out.size();
  srsran_assert(nof_v_pilots < 13, "The maximum number of virtual pilots is 12, requested {}.", nof_v_pilots);
  srsran_assert(in_abs.size() == nof_v_pilots,
                "Input and output should have the same size, provided {} and {}.",
                in_abs.size(),
                nof_v_pilots);
  srsran_assert(in_arg.size() == nof_v_pilots,
                "Input and output should have the same size, provided {} and {}.",
                in_arg.size(),
                nof_v_pilots);
  span<const int> used_indices = span<const int>(indices).first(nof_v_pilots);

  float mean_x    = static_cast<float>(nof_v_pilots * (nof_v_pilots - 1)) / 2.0F / nof_v_pilots;
  float norm_x_sq = static_cast<float>((nof_v_pilots - 1) * nof_v_pilots * (2 * nof_v_pilots - 1)) / 6.0F;

  float mean_abs  = srsvec::mean(in_abs);
  float slope_abs = srsvec::dot_prod(in_abs, used_indices, 0.0F);
  slope_abs -= mean_x * mean_abs * nof_v_pilots;
  slope_abs /= (norm_x_sq - nof_v_pilots * mean_x * mean_x);

  float intercept_abs = mean_abs - slope_abs * mean_x;

  float mean_arg  = srsvec::mean(in_arg);
  float slope_arg = srsvec::dot_prod(in_arg, used_indices, 0.0F);
  slope_arg -= mean_x * mean_arg * nof_v_pilots;
  slope_arg /= (norm_x_sq - nof_v_pilots * mean_x * mean_x);

  float intercept_arg = mean_arg - slope_arg * mean_x;

  int v_offset = -nof_v_pilots;
  if (!is_start) {
    v_offset = nof_v_pilots;
  }

  for (unsigned i_pilot = 0; i_pilot != nof_v_pilots; ++i_pilot) {
    int i_virtual = static_cast<int>(i_pilot) + v_offset;
    out[i_pilot]  = std::polar(slope_abs * i_virtual + intercept_abs, slope_arg * i_virtual + intercept_arg);
  }
}
