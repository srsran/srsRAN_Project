/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "port_channel_estimator_helpers.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/conversion.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/subtract.h"
#include "srsran/support/compiler.h"
#include "srsran/support/transform_optional.h"

using namespace srsran;

#ifndef SRSRAN_HAS_ENTERPRISE

/// \brief Estimates the noise energy of one hop.
///
/// The layers in the given range are assumed to transmit DM-RS on the same resources.
/// \param[in] pilots                DM-RS pilots.
/// \param[in] rx_pilots             Received samples corresponding to DM-RS pilots.
/// \param[in] estimates             Estimated channel frequency response.
/// \param[in] beta                  DM-RS-to-data amplitude gain (linear scale).
/// \param[in] dmrs_mask             Boolean mask identifying the OFDM symbols carrying DM-RS within the slot.
/// \param[in] cfo                   Carrier frequency offset.
/// \param[in] symbol_start_epochs   Cumulative duration of all CPs in the slot.
/// \param[in] compensate_cfo        Boolean flag to activate the CFO compensation.
/// \param[in] first_hop_symbol      Index of the first OFDM symbol of the current hop, within the slot.
/// \param[in] last_hop_symbol       Index of the last OFDM symbol of the current hop (not included), within the slot.
/// \param[in] hop_offset            Number of OFDM symbols containing DM-RS pilots in the previous hop (set to 0 if the
///                                  current hop is the first/only one).
/// \return The noise energy for the current hop.
static float estimate_noise(const dmrs_symbol_list&                   pilots,
                            const dmrs_symbol_list&                   rx_pilots,
                            const re_buffer_reader<cf_t>&             estimates,
                            float                                     beta,
                            const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                            std::optional<float>                      cfo,
                            span<const float>                         symbol_start_epochs,
                            bool                                      compensate_cfo,
                            unsigned                                  first_hop_symbol,
                            unsigned                                  last_hop_symbol,
                            unsigned                                  hop_offset);

void port_channel_estimator_average_impl::do_compute(channel_estimate&           estimate,
                                                     const resource_grid_reader& grid,
                                                     unsigned                    port,
                                                     const dmrs_symbol_list&     pilots,
                                                     const configuration&        cfg)
{
  srsran_assert(cfg.dmrs_pattern.size() == 1,
                "Only one UL layer supported in this version, requested {}.",
                cfg.dmrs_pattern.size());

  re_measurement_dimensions symbols_size = pilots.size();

  symbols_size.nof_slices = 1;

  // Prepare symbol destination.
  rx_pilots.resize(symbols_size);

  // Compute the cumulative duration of all CPs for the given subcarrier spacing.
  initialize_symbol_start_epochs(cfg.cp, cfg.scs);

  constexpr unsigned layer0 = 0;

  // Initialize metrics.
  rsrp             = 0;
  epre             = 0;
  noise_var        = 0;
  time_alignment_s = 0;
  cfo_normalized   = std::nullopt;

  // compute_hop updates rsrp, epre, niose_var, time_alignment_s, and cfo_normalized.
  compute_hop(estimate, grid, port, pilots, cfg, /*hop=*/0);
  if (cfg.dmrs_pattern[0].hopping_symbol_index.has_value()) {
    compute_hop(estimate, grid, port, pilots, cfg, /*hop=*/1);
    time_alignment_s /= 2.0F;
  }

  unsigned nof_dmrs_pilots = symbols_size.nof_subc * symbols_size.nof_symbols;
  rsrp /= static_cast<float>(nof_dmrs_pilots);
  epre /= static_cast<float>(nof_dmrs_pilots);

  noise_var /= static_cast<float>(nof_dmrs_pilots - 1);

  // Bound the noise variance from below.
  float min_noise_variance = rsrp / convert_dB_to_power(MAX_SINR_DB);
  noise_var                = std::max(min_noise_variance, noise_var);

  srsran_assert(cfg.scaling > 0, "The DM-RS to data scaling factor should be a positive number.");
  // Compute the estimated data received power by scaling the RSRP.
  float datarp = rsrp / cfg.scaling / cfg.scaling;

  // Report stats.
  if (compensate_cfo && cfo_normalized.has_value()) {
    // Apply CFO to the estimated channel.
    float cfo = cfo_normalized.value();
    for (unsigned i_symbol = cfg.first_symbol, last_symbol = cfg.first_symbol + cfg.nof_symbols;
         i_symbol != last_symbol;
         ++i_symbol) {
      span<cbf16_t> symbol_ch_estimate = estimate.get_symbol_ch_estimate(i_symbol, port, layer0);
      srsvec::sc_prod(
          symbol_ch_estimate, std::polar(1.0F, TWOPI * symbol_start_epochs[i_symbol] * cfo), symbol_ch_estimate);
    }
  }

  estimate.set_rsrp(rsrp, port, layer0);
  estimate.set_epre(epre, port);
  estimate.set_time_alignment(phy_time_unit::from_seconds(time_alignment_s), port, layer0);
  estimate.set_cfo_Hz(cfo_normalized.has_value()
                          ? std::optional<float>(cfo_normalized.value() * scs_to_khz(cfg.scs) * 1000)
                          : std::nullopt,
                      port,
                      layer0);

  // Write the noise variance in the channel estimate result.
  estimate.set_noise_variance(noise_var, port);

  estimate.set_snr((noise_var != 0) ? datarp / noise_var : 1000, port);
}

void port_channel_estimator_average_impl::compute_hop(srsran::channel_estimate&           estimate,
                                                      const srsran::resource_grid_reader& grid,
                                                      unsigned                            port,
                                                      const dmrs_symbol_list&             pilots,
                                                      const configuration&                cfg,
                                                      unsigned                            hop)
{
  constexpr unsigned layer0    = 0;
  constexpr unsigned one_layer = 1;

  srsran_assert((hop == 0) || cfg.dmrs_pattern[layer0].hopping_symbol_index.has_value(),
                "Frequency hopping requested but not configured.");

  const layer_dmrs_pattern& pattern = cfg.dmrs_pattern[layer0];

  // Extract symbols from resource grid.
  unsigned nof_dmrs_symbols = extract_layer_hop_rx_pilots(rx_pilots, grid, port, cfg, hop);
  srsran_assert(nof_dmrs_symbols != 0, "No DM-RS symbols were found for layer {}.", layer0);

  unsigned first_symbol = ((hop == 1) && pattern.hopping_symbol_index.has_value())
                              ? pattern.hopping_symbol_index.value()
                              : cfg.first_symbol;
  unsigned last_symbol = ((hop == 0) && pattern.hopping_symbol_index.has_value()) ? pattern.hopping_symbol_index.value()
                                                                                  : cfg.first_symbol + cfg.nof_symbols;

  for (unsigned i_dmrs = 0; i_dmrs != nof_dmrs_symbols; ++i_dmrs) {
    epre += srsvec::average_power(rx_pilots.get_symbol(i_dmrs, 0)) * rx_pilots.get_symbol(i_dmrs, 0).size();
  }

  unsigned hop_offset = 0;
  if (hop == 1) {
    hop_offset = pilots.size().nof_symbols - nof_dmrs_symbols;
  }

  // Number of pilots in a single OFDM symbol (carrying DM-RS).
  unsigned nof_symbol_pilots = pilots.size().nof_subc;

  // Auxiliary buffers for pilot computations.
  pilot_products.resize(1, nof_symbol_pilots);
  pilots_lse.resize(1, nof_symbol_pilots);

  static_re_buffer<one_layer, MAX_RB * NRE + 2 * MAX_V_PILOTS> enlarged_filtered_pilots_lse;
  modular_re_buffer<cf_t, one_layer>                           filtered_pilots_lse(one_layer, nof_symbol_pilots);

  if (fd_smoothing_strategy == port_channel_estimator_fd_smoothing_strategy::filter) {
    enlarged_pilots_lse.resize(one_layer, nof_symbol_pilots + 2 * MAX_V_PILOTS);
    enlarged_filtered_pilots_lse.resize(one_layer, nof_symbol_pilots + 2 * MAX_V_PILOTS);

    pilots_lse.set_slice(layer0, enlarged_pilots_lse.get_slice(layer0).subspan(MAX_V_PILOTS, nof_symbol_pilots));
    filtered_pilots_lse.set_slice(
        layer0, enlarged_filtered_pilots_lse.get_slice(layer0).subspan(MAX_V_PILOTS, nof_symbol_pilots));
  } else {
    // Enlarging is only necessary with the filter smoothing strategy.
    enlarged_pilots_lse.resize(one_layer, nof_symbol_pilots);
    enlarged_filtered_pilots_lse.resize(one_layer, nof_symbol_pilots);

    pilots_lse.set_slice(layer0, enlarged_pilots_lse.get_slice(layer0));
    filtered_pilots_lse.set_slice(layer0, enlarged_filtered_pilots_lse.get_slice(layer0));
  }

  float beta_scaling = cfg.scaling;

  //  Preprocess the pilots and compute the hop contribution to the CFO. Recall that this method updates
  //  pilot_products and pilots_lse.
  std::optional<float> cfo_hop = preprocess_pilots_and_estimate_cfo(
      pilots, pattern.symbols, first_symbol, last_symbol, hop_offset, layer0, layer0 + 1);
  if (cfo_hop.has_value()) {
    cfo_normalized = evaluate_or(
        cfo_normalized, cfo_hop.value(), [](float a, float b) { return (a + b) / 2.0F; }, cfo_hop.value());
  }

  // Average and apply DM-RS-to-data gain.
  float total_scaling = 1.0F / (static_cast<float>(nof_dmrs_symbols) * beta_scaling);
  srsvec::sc_prod(pilots_lse.get_slice(layer0), total_scaling, pilots_lse.get_slice(layer0));

  const bounded_bitset<MAX_RB>& hop_rb_mask      = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;
  interpolator::configuration   interpolator_cfg = configure_interpolator(pattern.re_pattern);

  // Apply a smoothing strategy to remove some noise ("high-time" components). Note that pilots_lse and
  // filtered_pilots_lse have some empty space at both sides, which we include with the enlarged version.
  apply_fd_smoothing(enlarged_filtered_pilots_lse.get_slice(layer0),
                     enlarged_pilots_lse.get_slice(layer0),
                     hop_rb_mask.count(),
                     interpolator_cfg.stride,
                     fd_smoothing_strategy);

  rsrp += srsvec::average_power(filtered_pilots_lse.get_slice(layer0)) * filtered_pilots_lse.get_slice(layer0).size() *
          beta_scaling * beta_scaling * static_cast<float>(nof_dmrs_symbols);

  // Extract RB mask lowest and highest RB. Also, determine if the allocation is contiguous.
  int      lowest_rb  = hop_rb_mask.find_lowest();
  int      highest_rb = hop_rb_mask.find_highest();
  unsigned rb_count   = hop_rb_mask.count();
  srsran_assert(highest_rb >= lowest_rb, "Invalid hop RB mask.");
  bool is_contiguous = (static_cast<unsigned>(highest_rb + 1 - lowest_rb) == rb_count);

  // Interpolate frequency domain.
  unsigned   nof_re  = rb_count * NRE;
  span<cf_t> ce_freq = span<cf_t>(freq_response).first(nof_re);
  freq_interpolator->interpolate(ce_freq, filtered_pilots_lse.get_slice(layer0), interpolator_cfg);

  // Convert interpolation result to complex BFloat16.
  span<cbf16_t> ce_freq_cbf16 = span<cbf16_t>(freq_response_cbf16).first(nof_re);
  srsvec::convert(ce_freq_cbf16, ce_freq);

  // Map frequency response to channel estimates.
  for (unsigned i_symbol = first_symbol; i_symbol != last_symbol; ++i_symbol) {
    span<cbf16_t> symbol_fr_resp = estimate.get_symbol_ch_estimate(i_symbol, port, layer0);

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

  time_alignment_s += estimate_time_alignment(filtered_pilots_lse, pattern, hop, cfg.scs, *ta_estimator);

  noise_var += estimate_noise(pilots,
                              rx_pilots,
                              filtered_pilots_lse,
                              beta_scaling,
                              pattern.symbols,
                              cfo_hop,
                              symbol_start_epochs,
                              compensate_cfo,
                              first_symbol,
                              last_symbol,
                              hop_offset);
}

std::optional<float> port_channel_estimator_average_impl::preprocess_pilots_and_estimate_cfo(
    const dmrs_symbol_list&                   pilots,
    const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
    unsigned                                  first_hop_symbol,
    unsigned                                  last_hop_symbol,
    unsigned                                  hop_offset,
    unsigned /* unused */,
    unsigned /* unused */)
{
  constexpr unsigned layer0 = 0;

  // Number of OFDM symbols carrying DM-RS in the current hop.
  unsigned nof_dmrs_symbols = dmrs_mask.slice(first_hop_symbol, last_hop_symbol).count();

  // Match received and transmitted pilots in the first DM-RS symbol.
  srsvec::prod_conj(
      rx_pilots.get_symbol(0, layer0), pilots.get_symbol(hop_offset, layer0), pilots_lse.get_slice(layer0));

  if (nof_dmrs_symbols == 1) {
    return std::nullopt;
  }

  // If we have more than one DM-RS symbol, we can estimate the CFO.
  unsigned i_dmrs_0 = dmrs_mask.find_lowest(first_hop_symbol, last_hop_symbol);
  unsigned i_dmrs_1 = dmrs_mask.find_lowest(i_dmrs_0 + 1, dmrs_mask.size());

  cf_t noisy_phase_acc = 0;

  // Match received and transmitted pilots in the second DM-RS symbol.
  srsvec::prod_conj(
      rx_pilots.get_symbol(1, layer0), pilots.get_symbol(hop_offset + 1, layer0), pilot_products.get_slice(layer0));

  noisy_phase_acc += srsvec::dot_prod(pilot_products.get_slice(layer0), pilots_lse.get_slice(layer0));

  float noisy_phase = std::arg(noisy_phase_acc);
  float cfo         = noisy_phase / TWOPI / (symbol_start_epochs[i_dmrs_1] - symbol_start_epochs[i_dmrs_0]);

  // Compensate the CFO in the first two DM-RS symbols and combine them.
  if (compensate_cfo) {
    srsvec::sc_prod(pilots_lse.get_slice(layer0),
                    std::polar(1.0F, -TWOPI * symbol_start_epochs[i_dmrs_0] * cfo),
                    pilots_lse.get_slice(layer0));
    srsvec::sc_prod(pilot_products.get_slice(layer0),
                    std::polar(1.0F, -TWOPI * symbol_start_epochs[i_dmrs_1] * cfo),
                    pilot_products.get_slice(layer0));
  }
  srsvec::add(pilots_lse.get_slice(layer0), pilot_products.get_slice(layer0), pilots_lse.get_slice(layer0));

  // If there are other DM-RS symbols in the hop, match them with the corresponding transmitted symbols, compensate the
  // CFO and combine with the previous DM-RS symbols.
  if (i_dmrs_1 < last_hop_symbol) {
    auto combine_pilots = [&, i_dmrs = 2](size_t i_symbol) mutable {
      srsvec::prod_conj(rx_pilots.get_symbol(i_dmrs, 0),
                        pilots.get_symbol(hop_offset + i_dmrs, layer0),
                        pilot_products.get_slice(layer0));
      if (compensate_cfo) {
        srsvec::sc_prod(pilot_products.get_slice(layer0),
                        std::polar(1.0F, -TWOPI * symbol_start_epochs[i_symbol] * cfo),
                        pilot_products.get_slice(layer0));
      }
      srsvec::add(pilots_lse.get_slice(layer0), pilot_products.get_slice(layer0), pilots_lse.get_slice(layer0));
      ++i_dmrs;
    };

    dmrs_mask.for_each(i_dmrs_1 + 1, last_hop_symbol, combine_pilots);
  }

  return cfo;
}

// In this version of the code, the preprocess_pilots_and_estimate_cfo method takes care of compensating the CFO.
void port_channel_estimator_average_impl::
    compensate_cfo_and_accumulate( // NOLINT(readability-convert-member-functions-to-static)
        const dmrs_symbol_list& /* unused */,
        const bounded_bitset<MAX_NSYMB_PER_SLOT>& /* unused */,
        unsigned /* unused */,
        unsigned /* unused */,
        std::optional<float> /* unused */)
{
  srsran_assertion_failure("Function not implemented.");
}

static float estimate_noise(const dmrs_symbol_list&                   pilots,
                            const dmrs_symbol_list&                   rx_pilots,
                            const re_buffer_reader<cf_t>&             estimates,
                            float                                     beta,
                            const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                            std::optional<float>                      cfo,
                            span<const float>                         symbol_start_epochs,
                            bool                                      compensate_cfo,
                            unsigned                                  first_hop_symbol,
                            unsigned                                  last_hop_symbol,
                            unsigned                                  hop_offset)
{
  static constexpr unsigned one_layer = 1;
  static constexpr unsigned layer0    = 0;

  // Deduce the number of RE to process.
  unsigned nof_re = estimates.get_nof_re();

  // Scale channel estimates.
  static_re_buffer<one_layer, MAX_RB * NRE> scaled_estimates(one_layer, nof_re);
  srsvec::sc_prod(estimates.get_slice(layer0), beta, scaled_estimates.get_slice(layer0));

  // Temporary data buffers.
  static_re_buffer<1, MAX_RB * NRE> temp_buffer(1, nof_re);

  // Noise energy accumulator for each OFDM symbol containing DM-RS.
  float noise_energy = 0.0F;

  auto estimate_noise_symbol = [&, i_dmrs = 0](size_t i_symbol) mutable {
    // Select temporary buffers.
    span<cf_t> predicted_obs = temp_buffer.get_slice(layer0);
    span<cf_t> noise_samples = temp_buffer.get_slice(layer0);

    // Get original symbol pilots.
    span<const cf_t> symbol_pilots = pilots.get_symbol(hop_offset + i_dmrs, layer0);

    // Apply estimated channel response to the original symbol pilots.
    srsvec::prod(scaled_estimates.get_slice(layer0), symbol_pilots, predicted_obs);

    // Compensate for CFO only if present.
    if (compensate_cfo && cfo.has_value()) {
      srsvec::sc_prod(
          predicted_obs, std::polar(1.0F, TWOPI * symbol_start_epochs[i_symbol] * cfo.value()), predicted_obs);
    }

    // Estimate receiver error as the subtraction of the regenerated symbols to the received pilots.
    span<const cf_t> symbol_rx_pilots = rx_pilots.get_symbol(i_dmrs, 0);
    srsvec::subtract(noise_samples, symbol_rx_pilots, predicted_obs);

    // Accumulate received power.
    noise_energy += srsvec::average_power(noise_samples) * noise_samples.size();
    ++i_dmrs;
  };

  // Process each OFDM containing DM-RS.
  dmrs_mask.for_each(first_hop_symbol, last_hop_symbol, estimate_noise_symbol);

  return noise_energy;
}

#endif // SRSRAN_HAS_ENTERPRISE

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
