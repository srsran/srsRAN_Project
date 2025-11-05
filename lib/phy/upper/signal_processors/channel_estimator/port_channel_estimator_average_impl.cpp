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
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/simd.h"
#include "srsran/srsvec/subtract.h"
#include "srsran/support/transform_optional.h"

using namespace srsran;

static const unsigned& MAX_LAYERS   = port_channel_estimator_average_impl::MAX_LAYERS;
static const unsigned& MAX_V_PILOTS = port_channel_estimator_average_impl::MAX_V_PILOTS;

/// \brief Gets the time-domain characteristics of the DM-RS pattern for the current hop, common to all layers.
/// \param[in] cfg  The channel estimator configuration.
/// \param[in] hop  The current hop index (0 or 1).
/// \return A four-tuple with the following entries:
///   - a boolean mask of the OFDM symbols carrying DM-RS in the slot;
///   - the first OFDM symbol carrying DM-RS in the current hop;
///   - the last OFDM symbol carrying DM-RS in the current hop;
///   - the number of OFDM symbols carrying DM-RS in the current hop.
static std::tuple<const bounded_bitset<MAX_NSYMB_PER_SLOT>&, unsigned, unsigned, unsigned>
extract_common_pattern(const port_channel_estimator::configuration& cfg, unsigned hop);

/// \brief Replaces pairs of elements by their average in all slices.
///
/// Last step of the LSE estimation of the channel coefficients at the REs carrying DM-RS. It cancels the contribution
/// of the DM-RS of the other layer multiplexed in the same CDM group. (For the orthogonality of the two DM-RS to hold,
/// the channel is assumed constant over the two averaged REs.)
static void average_pairs(re_measurement<cf_t>& values);

/// \brief Estimates the noise energy of one hop and a given range of layers.
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
/// \param[in] start_layer           First layer in the considered range.
/// \param[in] stop_layer            Last layer (excluded) in the considered range.
/// \return The noise energy for the current hop.
static float estimate_noise(const dmrs_symbol_list&                   pilots,
                            const dmrs_symbol_list&                   rx_pilots,
                            const re_measurement<cf_t>&               estimates,
                            float                                     beta,
                            const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                            std::optional<float>                      cfo,
                            span<const float>                         symbol_start_epochs,
                            bool                                      compensate_cfo,
                            unsigned                                  first_hop_symbol,
                            unsigned                                  last_hop_symbol,
                            unsigned                                  hop_offset,
                            unsigned                                  start_layer,
                            unsigned                                  stop_layer);

/// \brief Prepares auxiliary buffers.
///
/// Resizes the static buffers and assign slices to the modular buffers based on the smoothing strategy, the number of
/// transmission layers and the number of pilots in a DM-RS symbol.
static void setup_auxiliary_buffers(
    modular_re_measurement<cf_t, port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS>& pilots_lse,
    static_re_measurement<cf_t,
                          MAX_RB * NRE + 2 * MAX_V_PILOTS,
                          port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS,
                          MAX_LAYERS>& enlarged_pilots_lse,
    modular_re_measurement<cf_t, port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS>&
                                                     filtered_pilots_lse,
    static_re_measurement<cf_t,
                          MAX_RB * NRE + 2 * MAX_V_PILOTS,
                          port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS,
                          MAX_LAYERS>&               enlarged_filtered_pilots_lse,
    static_re_buffer<MAX_LAYERS, MAX_RB * NRE>&      pilot_products,
    port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
    port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
    unsigned                                         nof_tx_layers,
    unsigned                                         nof_dmrs_symbols,
    unsigned                                         nof_symbol_pilots);

/// \brief Interpolates two vectors.
/// \param[out] out   Interpolation result.
/// \param[in] first  First vector.
/// \param[in] second Second vector.
/// \param[in] weight Interpolation weight.
static void simd_vector_interpolate(span<cbf16_t> out, span<const cf_t> first, span<const cf_t> second, float weight);

void port_channel_estimator_average_impl::do_compute(channel_estimate&           estimate,
                                                     const resource_grid_reader& grid,
                                                     unsigned                    port,
                                                     const dmrs_symbol_list&     pilots,
                                                     const configuration&        cfg)
{
  re_measurement_dimensions symbols_size    = pilots.size();
  unsigned                  nof_layers      = symbols_size.nof_slices;
  unsigned                  nof_dmrs_pilots = symbols_size.nof_subc * symbols_size.nof_symbols;

  unsigned nof_cdm        = divide_ceil(nof_layers, 2);
  symbols_size.nof_slices = nof_cdm;

  // Prepare symbol destination.
  rx_pilots.resize(symbols_size);

  // Compute the cumulative duration of all CPs for the given subcarrier spacing.
  initialize_symbol_start_epochs(cfg.cp, cfg.scs);

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

  rsrp /= static_cast<float>(nof_dmrs_pilots * nof_layers);
  epre /= static_cast<float>(nof_dmrs_pilots);

  noise_var /= static_cast<float>(nof_dmrs_pilots * nof_cdm - 1);

  // Bound the noise variance from below.
  float min_noise_variance = rsrp / convert_dB_to_power(MAX_SINR_DB);
  noise_var                = std::max(min_noise_variance, noise_var);

  srsran_assert(cfg.scaling > 0, "The DM-RS to data scaling factor should be a positive number.");

  // Write EPRE and noise variance in the channel estimate result.
  estimate.set_epre(epre, port);
  estimate.set_noise_variance(noise_var, port);

  unsigned nof_tx_layers = cfg.dmrs_pattern.size();
  // Compute the estimated data received power by scaling the RSRP.
  float datarp = rsrp * nof_tx_layers / cfg.scaling / cfg.scaling;

  // Determine the linear SNR measurement. The linear SNR must result to zero if the noise variance is zero, NaN or
  // infinity.
  float snr = 0;
  if (std::isnormal(noise_var)) {
    snr = datarp / noise_var;
  }

  estimate.set_snr(snr, port);

  // Report stats for each layer.
  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    if (compensate_cfo && cfo_normalized.has_value()) {
      // Apply CFO to the estimated channel.
      float cfo = cfo_normalized.value();
      for (unsigned i_symbol = cfg.first_symbol, last_symbol = cfg.first_symbol + cfg.nof_symbols;
           i_symbol != last_symbol;
           ++i_symbol) {
        span<cbf16_t> symbol_ch_estimate = estimate.get_symbol_ch_estimate(i_symbol, port, i_layer);
        srsvec::sc_prod(
            symbol_ch_estimate, symbol_ch_estimate, std::polar(1.0F, TWOPI * symbol_start_epochs[i_symbol] * cfo));
      }
    }

    estimate.set_rsrp(rsrp, port, i_layer);
    estimate.set_time_alignment(phy_time_unit::from_seconds(time_alignment_s), port, i_layer);
    estimate.set_cfo_Hz(cfo_normalized.has_value()
                            ? std::optional<float>(cfo_normalized.value() * scs_to_khz(cfg.scs) * 1000)
                            : std::nullopt,
                        port,
                        i_layer);
  }
}

void port_channel_estimator_average_impl::compute_hop(srsran::channel_estimate&           estimate,
                                                      const srsran::resource_grid_reader& grid,
                                                      unsigned                            port,
                                                      const dmrs_symbol_list&             pilots,
                                                      const configuration&                cfg,
                                                      unsigned                            hop)
{
  unsigned nof_tx_layers = cfg.dmrs_pattern.size();
  srsran_assert(
      nof_tx_layers <= MAX_LAYERS, "The number of Tx layers is {}, max {} supported.", nof_tx_layers, MAX_LAYERS);

  float beta_scaling = cfg.scaling;

  // Get the time-domain characteristics of the DM-RS pattern, common to all layers.
  //  - pattern_symbols: mask of OFDM symbols carrying DM-RS in the slot
  //  - first_symbol: first OFDM symbol carrying DM-RS in the current hop
  //  - last_symbol: last OFDM symbol carrying DM-RS in the current hop
  //  - nof_dmrs_symbols: number of OFDM symbols carrying DM-RS in the current hop.
  auto [pattern_symbols, first_symbol, last_symbol, nof_dmrs_symbols] = extract_common_pattern(cfg, hop);

  // Number of pilots in a single OFDM symbol (carrying DM-RS).
  unsigned nof_symbol_pilots = pilots.size().nof_subc;

  unsigned nof_lse_symbols = 1;
  if (td_interpolation_strategy != port_channel_estimator_td_interpolation_strategy::average) {
    nof_lse_symbols = nof_dmrs_symbols;
  }

  // Auxiliary buffers for pilot computations. Calling the setup_auxiliary_buffers function will resize
  // enlarged_pilots_lse, enlarged_filtered_pilots_lse and pilot_products, and will assign slices to the modular buffers
  // pilots_lse and filtered_pilots_lse.
  static_re_measurement<cf_t, MAX_NOF_PILOTS_SYMBOL, MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS> enlarged_filtered_pilots_lse(
      {.nof_subc = nof_symbol_pilots, .nof_symbols = nof_lse_symbols, .nof_slices = nof_tx_layers});
  modular_re_measurement<cf_t, MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS> filtered_pilots_lse(enlarged_filtered_pilots_lse);
  pilots_lse.resize({.nof_subc = nof_symbol_pilots, .nof_symbols = nof_dmrs_symbols, .nof_slices = nof_tx_layers});

  setup_auxiliary_buffers(pilots_lse,
                          enlarged_pilots_lse,
                          filtered_pilots_lse,
                          enlarged_filtered_pilots_lse,
                          pilot_products,
                          fd_smoothing_strategy,
                          td_interpolation_strategy,
                          nof_tx_layers,
                          nof_dmrs_symbols,
                          nof_symbol_pilots);

  std::optional<float> cfo_hop = std::nullopt;

  // We process layers in groups of two, since the DM-RS for layer 2n and layer 2n+1 are mapped onto the same REs.
  for (unsigned i_layer = 0; i_layer < nof_tx_layers; i_layer += 2U) {
    srsran_assert((hop == 0) || cfg.dmrs_pattern[i_layer].hopping_symbol_index.has_value(),
                  "Frequency hopping requested but not configured.");

    // Extract symbols from resource grid.
    extract_layer_hop_rx_pilots(rx_pilots, grid, port, cfg, hop, i_layer);

    unsigned i_cdm = i_layer / 2;
    for (unsigned i_dmrs = 0; i_dmrs != nof_dmrs_symbols; ++i_dmrs) {
      epre += srsvec::average_power(rx_pilots.get_symbol(i_dmrs, i_cdm)) * rx_pilots.get_symbol(i_dmrs, i_cdm).size();
    }

    unsigned hop_offset = 0;
    if (hop == 1) {
      hop_offset = pilots.size().nof_symbols - nof_dmrs_symbols;
    }

    unsigned stop_layer = (i_layer < nof_tx_layers - 1) ? i_layer + 2 : i_layer + 1;

    //  Preprocess the pilots and compute the hop contribution to the CFO. Recall that this method updates
    //  pilot_products and pilots_lse.
    std::optional<float> cfo_hop_cdm = preprocess_pilots_and_estimate_cfo(
        pilots, pattern_symbols, first_symbol, last_symbol, hop_offset, i_layer, stop_layer);
    if (cfo_hop_cdm.has_value()) {
      cfo_hop = evaluate_or(cfo_hop, cfo_hop_cdm.value(), std::plus(), cfo_hop_cdm.value());
    }
  }

  cfo_hop = transform_optional(cfo_hop, std::divides(), static_cast<float>(divide_ceil(nof_tx_layers, 2)));
  if (cfo_hop.has_value()) {
    cfo_normalized =
        evaluate_or(cfo_normalized, cfo_hop.value(), [](float a, float b) { return (a + b) / 2.0F; }, cfo_hop.value());
  }

  //  Compensate the CFO. Recall that this method updates
  //  pilot_products and pilots_lse.
  compensate_cfo_and_accumulate(pilots, pattern_symbols, first_symbol, last_symbol, cfo_hop);

  // Process pilot estimates in frequency and time domain for each layer.
  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    const layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

    // Select hop resource block mask.
    const auto& hop_rb_mask = (hop == 0) ? pattern.rb_mask : pattern.rb_mask2;

    // Extract RB mask lowest and highest RB. Also, determine if the allocation is contiguous.
    int      lowest_rb  = hop_rb_mask.find_lowest();
    int      highest_rb = hop_rb_mask.find_highest();
    unsigned rb_count   = hop_rb_mask.count();
    srsran_assert(highest_rb >= lowest_rb, "Invalid hop RB mask.");
    bool is_contiguous = (static_cast<unsigned>(highest_rb + 1 - lowest_rb) == rb_count);

    // Prepare frequency domain interpolation.
    interpolator::configuration interpolator_cfg = configure_interpolator(pattern.re_pattern);
    unsigned                    nof_re           = rb_count * NRE;
    freq_response.resize(nof_lse_symbols, nof_re);

    // Calculate total scaling accounting for the symbol accumulation and the DM-RS-to-data gain.
    float total_scaling = 1.0F / beta_scaling;
    if (td_interpolation_strategy == port_channel_estimator_td_interpolation_strategy::average) {
      total_scaling /= static_cast<float>(nof_dmrs_symbols);
    }

    // Apply frequency domain processing.
    for (unsigned i_symbol = 0; i_symbol != nof_lse_symbols; ++i_symbol) {
      srsvec::sc_prod(
          pilots_lse.get_symbol(i_symbol, i_layer), pilots_lse.get_symbol(i_symbol, i_layer), total_scaling);

      // Apply a smoothing strategy to remove some noise ("high-time" components). Note that pilots_lse and
      // filtered_pilots_lse have some empty space at both sides, which we include with the enlarged version.
      apply_fd_smoothing(enlarged_filtered_pilots_lse.get_symbol(i_symbol, i_layer),
                         enlarged_pilots_lse.get_symbol(i_symbol, i_layer),
                         hop_rb_mask.count(),
                         interpolator_cfg.stride,
                         fd_smoothing_strategy);

      // Energy of the DM-RS in the current symbol and layer.
      float avg = srsvec::average_power(filtered_pilots_lse.get_symbol(i_symbol, i_layer)) *
                  filtered_pilots_lse.get_symbol(i_symbol, i_layer).size();

      // Normalization factor: the factor nof_dmrs_symbols / nof_lse_symbols accounts for whether the
      // symbols carrying DM-RS have been combined or not (i.e., td_interpolation_strategy is average or interpolate,
      // respectively), while beta_scaling^2 accounts for the data-to-DM-RS scaling.
      float power_normalization_factor =
          beta_scaling * beta_scaling * static_cast<float>(nof_dmrs_symbols) / static_cast<float>(nof_lse_symbols);
      avg *= power_normalization_factor;
      rsrp += avg;

      // Interpolate frequency response for this symbol.
      freq_interpolator->interpolate(
          freq_response.get_slice(i_symbol), filtered_pilots_lse.get_symbol(i_symbol, i_layer), interpolator_cfg);
    }

    // Map frequency response to channel estimates.
    for (unsigned i_symbol = first_symbol; i_symbol != last_symbol;
         ++i_symbol) { // Map frequency response to channel estimates.
      // Select frequency domain channel estimate.
      span<cbf16_t> symbol_fr_resp = estimate.get_symbol_ch_estimate(i_symbol, port, i_layer);

      // Straight process if the allocation is contiguous.
      if (is_contiguous) {
        apply_td_domain_strategy(symbol_fr_resp.subspan(lowest_rb * NRE, nof_re),
                                 pattern.symbols,
                                 freq_response,
                                 first_symbol,
                                 last_symbol,
                                 i_symbol);
        continue;
      }

      // Otherwise copy each PRB.
      unsigned i_prb_ce = 0;
      hop_rb_mask.for_each(
          0, hop_rb_mask.size(), [&, first_symbol_ = first_symbol, last_symbol_ = last_symbol](unsigned i_prb) {
            modular_re_buffer<cf_t, MAX_NOF_DMRS_SYMBOLS> prb_freq_response(freq_response, i_prb_ce * NRE, NRE);
            apply_td_domain_strategy(symbol_fr_resp.subspan(lowest_rb * NRE, nof_re),
                                     pattern.symbols,
                                     prb_freq_response,
                                     first_symbol_,
                                     last_symbol_,
                                     i_symbol);
            ++i_prb_ce;
          });
    }
  }

  for (unsigned i_layer = 0; i_layer < nof_tx_layers; i_layer += 2U) {
    const layer_dmrs_pattern& pattern = cfg.dmrs_pattern[i_layer];

    unsigned stop_layer = (i_layer < nof_tx_layers - 1) ? i_layer + 2 : i_layer + 1;

    unsigned hop_offset = 0;
    if (hop == 1) {
      hop_offset = pilots.size().nof_symbols - nof_dmrs_symbols;
    }

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
                                hop_offset,
                                i_layer,
                                stop_layer);
  }

  time_alignment_s +=
      estimate_time_alignment(filtered_pilots_lse, cfg.dmrs_pattern.front(), hop, cfg.scs, *ta_estimator);
}

std::optional<float> port_channel_estimator_average_impl::preprocess_pilots_and_estimate_cfo(
    const dmrs_symbol_list&                   pilots,
    const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
    unsigned                                  first_hop_symbol,
    unsigned                                  last_hop_symbol,
    unsigned                                  hop_offset,
    unsigned                                  start_layer,
    unsigned                                  stop_layer)
{
  srsran_assert((stop_layer == start_layer + 1) || (stop_layer == start_layer + 2),
                "Layers must be processed either independently on in pairs, required {{{}, ..., {}}}.",
                start_layer,
                stop_layer - 1);
  bool need_average = (stop_layer - start_layer > 1);

  unsigned i_cdm = start_layer / 2;

  // Number of OFDM symbols carrying DM-RS in the current hop.
  unsigned nof_dmrs_symbols = dmrs_mask.slice(first_hop_symbol, last_hop_symbol).count();

  for (unsigned i_layer = start_layer; i_layer != stop_layer; ++i_layer) {
    // Match received and transmitted pilots in the first DM-RS symbol.
    srsvec::prod_conj(
        pilots_lse.get_symbol(0, i_layer), rx_pilots.get_symbol(0, i_cdm), pilots.get_symbol(hop_offset, i_layer));
  }

  if (nof_dmrs_symbols == 1) {
    if (need_average) {
      average_pairs(pilots_lse);
    }
    return std::nullopt;
  }

  // If we have more than one DM-RS symbol, we can estimate the CFO.
  unsigned i_dmrs_0 = dmrs_mask.find_lowest(first_hop_symbol, last_hop_symbol);
  unsigned i_dmrs_1 = dmrs_mask.find_lowest(i_dmrs_0 + 1, dmrs_mask.size());

  cf_t noisy_phase_acc = 0;

  for (unsigned i_layer = start_layer; i_layer != stop_layer; ++i_layer) {
    // Select destination of temporary pilots LSE.
    span<cf_t> temp_pilot_products = pilot_products.get_slice(i_layer);
    if (td_interpolation_strategy != port_channel_estimator_td_interpolation_strategy::average) {
      temp_pilot_products = pilots_lse.get_symbol(1, i_layer);
    }

    // Match received and transmitted pilots in the second DM-RS symbol.
    srsvec::prod_conj(temp_pilot_products, rx_pilots.get_symbol(1, i_cdm), pilots.get_symbol(hop_offset + 1, i_layer));

    noisy_phase_acc += srsvec::dot_prod(temp_pilot_products, pilots_lse.get_symbol(0, i_layer));
  }

  float noisy_phase = std::arg(noisy_phase_acc);
  float cfo         = noisy_phase / TWOPI / (symbol_start_epochs[i_dmrs_1] - symbol_start_epochs[i_dmrs_0]);

  return cfo;
}

void port_channel_estimator_average_impl::compensate_cfo_and_accumulate(
    const dmrs_symbol_list&                   pilots,
    const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
    unsigned                                  first_hop_symbol,
    unsigned                                  last_hop_symbol,
    std::optional<float>                      cfo)
{
  if (!cfo.has_value()) {
    // Nothing to do.
    return;
  }

  unsigned nof_layers = pilots.size().nof_slices;

  // If CFO has value, then there are at least 2 OFDM symbols carrying DM-RS.
  unsigned i_dmrs_0 = dmrs_mask.find_lowest(first_hop_symbol, last_hop_symbol);
  unsigned i_dmrs_1 = dmrs_mask.find_lowest(i_dmrs_0 + 1, dmrs_mask.size());

  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    // Select destination of temporary pilots LSE.
    span<cf_t> temp_pilot_products = pilot_products.get_slice(i_layer);
    if (td_interpolation_strategy != port_channel_estimator_td_interpolation_strategy::average) {
      temp_pilot_products = pilots_lse.get_symbol(1, i_layer);
    }

    if (compensate_cfo && cfo.has_value()) {
      srsvec::sc_prod(pilots_lse.get_symbol(0, i_layer),
                      pilots_lse.get_symbol(0, i_layer),
                      std::polar(1.0F, -TWOPI * symbol_start_epochs[i_dmrs_0] * cfo.value()));
      srsvec::sc_prod(temp_pilot_products,
                      temp_pilot_products,
                      std::polar(1.0F, -TWOPI * symbol_start_epochs[i_dmrs_1] * cfo.value()));
    }

    if (td_interpolation_strategy == port_channel_estimator_td_interpolation_strategy::average) {
      srsvec::add(pilots_lse.get_symbol(0, i_layer), temp_pilot_products, pilots_lse.get_symbol(0, i_layer));
    }
  }

  unsigned hop_offset = dmrs_mask.slice(0, first_hop_symbol).count();

  // For each DM-RS symbol in the hop, compensate the CFO and combine with the previous DM-RS symbols.
  auto combine_pilots = [&, i_dmrs = 2](size_t i_symbol) mutable {
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      unsigned i_cdm = i_layer / 2;

      // Select destination of temporary pilots LSE.
      span<cf_t> temp_pilot_products = pilot_products.get_slice(i_layer);
      if (td_interpolation_strategy != port_channel_estimator_td_interpolation_strategy::average) {
        temp_pilot_products = pilots_lse.get_symbol(i_dmrs, i_layer);
      }

      srsvec::prod_conj(
          temp_pilot_products, rx_pilots.get_symbol(i_dmrs, i_cdm), pilots.get_symbol(hop_offset + i_dmrs, i_layer));
      if (compensate_cfo && cfo.has_value()) {
        srsvec::sc_prod(temp_pilot_products,
                        temp_pilot_products,
                        std::polar(1.0F, -TWOPI * symbol_start_epochs[i_symbol] * cfo.value()));
      }

      if (td_interpolation_strategy == port_channel_estimator_td_interpolation_strategy::average) {
        srsvec::add(pilots_lse.get_symbol(0, i_layer), temp_pilot_products, pilots_lse.get_symbol(0, i_layer));
      }
    }
    ++i_dmrs;
  };

  dmrs_mask.for_each(i_dmrs_1 + 1, last_hop_symbol, combine_pilots);

  if (nof_layers > 1) {
    average_pairs(pilots_lse);
  }
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

void port_channel_estimator_average_impl::apply_td_domain_strategy(span<cbf16_t>                 estimated_rg,
                                                                   const symbol_slot_mask&       dmrs_mask,
                                                                   const re_buffer_reader<cf_t>& freq_response_dmrs,
                                                                   unsigned                      hop_first_symbol,
                                                                   unsigned                      hop_last_symbol,
                                                                   unsigned                      i_symbol) const
{
  // If the time-domain strategy is average, skipp all interpolation.
  if (td_interpolation_strategy == port_channel_estimator_td_interpolation_strategy::average) {
    srsvec::convert(estimated_rg, freq_response_dmrs.get_slice(0));
    return;
  }

  // Deduce number of DM-RS symbols.
  unsigned nof_dmrs_symbols = freq_response_dmrs.get_nof_slices();
  // Find nearest DM-RS symbol before this symbol within the hop.
  int dmrs_symbol_before = dmrs_mask.find_highest(hop_first_symbol, i_symbol);
  // Find nearest DM-RS symbol after this symbol.
  int dmrs_symbol_after = dmrs_mask.find_lowest(i_symbol, hop_last_symbol);

  // No DM-RS symbol is earlier, select the first DM-RS symbol.
  if (dmrs_symbol_before == -1) {
    // Search a second DM-RS symbol.
    int second_dmrs_symbol = dmrs_mask.find_lowest(dmrs_symbol_after + 1, hop_last_symbol);

    // If there is no other DM-RS symbol, then use the same estimated channel.
    if (second_dmrs_symbol == -1) {
      srsvec::convert(estimated_rg, freq_response_dmrs.get_slice(0));
      return;
    }

    // Otherwise, interpolate between the two first OFDM symbols containing DM-RS.
    dmrs_symbol_before = dmrs_symbol_after;
    dmrs_symbol_after  = second_dmrs_symbol;
  }

  // No DM-RS symbol is after, select the last DM-RS symbol.
  if (dmrs_symbol_after == -1) {
    // Search a second last DM-RS symbol.
    int second_last_dmrs_symbol = dmrs_mask.find_highest(hop_first_symbol, dmrs_symbol_before);

    // If there is no other DM-RS symbol, then use the same estimated channel.
    if (second_last_dmrs_symbol == -1) {
      srsvec::convert(estimated_rg, freq_response_dmrs.get_slice(nof_dmrs_symbols - 1));
      return;
    }

    // Otherwise, interpolate between the two last OFDM symbols containing DM-RS.
    dmrs_symbol_after  = dmrs_symbol_before;
    dmrs_symbol_before = second_last_dmrs_symbol;
  }

  // Interpolate between OFDM symbols that contain DM-RS.
  int      diff_dmrs_before = static_cast<int>(i_symbol) - dmrs_symbol_before;
  float    weight = static_cast<float>(diff_dmrs_before) / static_cast<float>(dmrs_symbol_after - dmrs_symbol_before);
  unsigned i_dmrs_symbol = dmrs_mask.slice(hop_first_symbol, dmrs_symbol_before).count();

  simd_vector_interpolate(estimated_rg,
                          freq_response_dmrs.get_slice(i_dmrs_symbol),
                          freq_response_dmrs.get_slice(i_dmrs_symbol + 1),
                          weight);
}

static std::tuple<const bounded_bitset<MAX_NSYMB_PER_SLOT>&, unsigned, unsigned, unsigned>
extract_common_pattern(const port_channel_estimator::configuration& cfg, unsigned hop)
{
  const port_channel_estimator::layer_dmrs_pattern& pattern = cfg.dmrs_pattern[0];

  unsigned first_symbol = ((hop == 1) && pattern.hopping_symbol_index.has_value())
                              ? pattern.hopping_symbol_index.value()
                              : cfg.first_symbol;
  unsigned last_symbol = ((hop == 0) && pattern.hopping_symbol_index.has_value()) ? pattern.hopping_symbol_index.value()
                                                                                  : cfg.first_symbol + cfg.nof_symbols;

  unsigned nof_dmrs_symbols = pattern.symbols.slice(first_symbol, last_symbol).count();
  srsran_assert(nof_dmrs_symbols != 0, "No DM-RS symbols were found.");

  return {pattern.symbols, first_symbol, last_symbol, nof_dmrs_symbols};
}

static void setup_auxiliary_buffers(
    modular_re_measurement<cf_t, port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS>& pilots_lse,
    static_re_measurement<cf_t,
                          port_channel_estimator_average_impl::MAX_NOF_PILOTS_SYMBOL,
                          port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS,
                          MAX_LAYERS>& enlarged_pilots_lse,
    modular_re_measurement<cf_t, port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS>&
                                                     filtered_pilots_lse,
    static_re_measurement<cf_t,
                          port_channel_estimator_average_impl::MAX_NOF_PILOTS_SYMBOL,
                          port_channel_estimator_average_impl::MAX_NOF_DMRS_SYMBOLS,
                          MAX_LAYERS>&               enlarged_filtered_pilots_lse,
    static_re_buffer<MAX_LAYERS, MAX_RB * NRE>&      pilot_products,
    port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
    port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
    unsigned                                         nof_tx_layers,
    unsigned                                         nof_dmrs_symbols,
    unsigned                                         nof_symbol_pilots)
{
  unsigned nof_lse_symbols = 1;
  if (td_interpolation_strategy != port_channel_estimator_td_interpolation_strategy::average) {
    nof_lse_symbols = nof_dmrs_symbols;
  }

  pilots_lse.resize({.nof_subc = nof_symbol_pilots, .nof_symbols = nof_dmrs_symbols, .nof_slices = nof_tx_layers});
  pilot_products.resize(nof_tx_layers, nof_symbol_pilots);

  if (fd_smoothing_strategy == port_channel_estimator_fd_smoothing_strategy::filter) {
    enlarged_pilots_lse.resize({.nof_subc    = nof_symbol_pilots + 2 * MAX_V_PILOTS,
                                .nof_symbols = nof_lse_symbols,
                                .nof_slices  = nof_tx_layers});
    enlarged_filtered_pilots_lse.resize({.nof_subc    = nof_symbol_pilots + 2 * MAX_V_PILOTS,
                                         .nof_symbols = nof_lse_symbols,
                                         .nof_slices  = nof_tx_layers});

    pilots_lse.assign(enlarged_pilots_lse, MAX_V_PILOTS, nof_symbol_pilots);
    filtered_pilots_lse.assign(enlarged_filtered_pilots_lse, MAX_V_PILOTS, nof_symbol_pilots);
  } else {
    // Enlarging is only necessary with the filter smoothing strategy.
    enlarged_pilots_lse.resize(
        {.nof_subc = nof_symbol_pilots, .nof_symbols = nof_lse_symbols, .nof_slices = nof_tx_layers});
    enlarged_filtered_pilots_lse.resize(
        {.nof_subc = nof_symbol_pilots, .nof_symbols = nof_lse_symbols, .nof_slices = nof_tx_layers});
    pilots_lse.assign(enlarged_pilots_lse);
    filtered_pilots_lse.assign(enlarged_filtered_pilots_lse);
  }
}

static void average_pairs(re_measurement<cf_t>& values)
{
  unsigned nof_slices  = values.size().nof_slices;
  unsigned nof_values  = values.size().nof_subc;
  unsigned nof_symbols = values.size().nof_symbols;

  for (unsigned i_slice = 0; i_slice != nof_slices; ++i_slice) {
    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      span<cf_t> values_local = values.get_symbol(i_symbol, i_slice);
      for (unsigned i_value = 0; i_value < nof_values; i_value += 2) {
        cf_t av = (values_local[i_value] + values_local[i_value + 1]) / 2.0F;

        values_local[i_value]     = av;
        values_local[i_value + 1] = av;
      }
      // If there are an odd number of values, the last one is left unchanged.
    }
  }
}

static float estimate_noise(const dmrs_symbol_list&                   pilots,
                            const dmrs_symbol_list&                   rx_pilots,
                            const re_measurement<cf_t>&               estimates,
                            float                                     beta,
                            const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                            std::optional<float>                      cfo,
                            span<const float>                         symbol_start_epochs,
                            bool                                      compensate_cfo,
                            unsigned                                  first_hop_symbol,
                            unsigned                                  last_hop_symbol,
                            unsigned                                  hop_offset,
                            unsigned                                  start_layer,
                            unsigned                                  stop_layer)
{
  constexpr unsigned max_layers = 2;

  srsran_assert((stop_layer == start_layer + 1) || (stop_layer == start_layer + max_layers),
                "Layers must be processed either independently on in pairs, required {{{}, ..., {}}}.",
                start_layer,
                stop_layer - 1);

  // Deduce the number of RE to process.
  unsigned nof_re = estimates.size().nof_subc;

  // Deduce the number of LSE symbols.
  unsigned nof_lse_symbols = estimates.size().nof_symbols;

  // Scale channel estimates and average in time domain.
  float scaling_factor = beta / static_cast<float>(nof_lse_symbols);

  static_re_buffer<max_layers, MAX_RB * NRE> scaled_estimates(estimates.size().nof_slices, nof_re);

  for (unsigned i_layer = start_layer; i_layer != stop_layer; ++i_layer) {
    unsigned i_helper = i_layer - start_layer;
    srsvec::sc_prod(scaled_estimates.get_slice(i_helper), estimates.get_symbol(0, i_layer), scaling_factor);
    for (unsigned i_symbol = 1; i_symbol != nof_lse_symbols; ++i_symbol) {
      span<cf_t>       scaled           = scaled_estimates.get_slice(i_helper);
      span<const cf_t> estimates_symbol = estimates.get_symbol(i_symbol, i_layer);
      std::transform(estimates_symbol.begin(),
                     estimates_symbol.end(),
                     scaled.begin(),
                     scaled.begin(),
                     [scaling_factor](cf_t ce, cf_t acc) { return ce * scaling_factor + acc; });
    }
  }

  // Temporary data buffers.
  static_re_buffer<1, MAX_RB * NRE> predicted_obs_buffer(1, nof_re);
  static_re_buffer<1, MAX_RB * NRE> noise_samples_buffer(1, nof_re);

  // Noise energy accumulator for each OFDM symbol containing DM-RS.
  float noise_energy = 0.0F;

  auto estimate_noise_symbol = [&, i_dmrs = 0](size_t i_symbol) mutable {
    span<cf_t> noise_samples = noise_samples_buffer.get_slice(0);
    for (unsigned i_layer = start_layer; i_layer != stop_layer; ++i_layer) {
      unsigned i_helper = i_layer - start_layer;

      // Select temporary buffer for the pilot symbols and channel estimates product.
      span<cf_t> predicted_obs = predicted_obs_buffer.get_slice(0);

      // Skip intermediate buffer for the first process layer.
      if (i_layer == start_layer) {
        predicted_obs = noise_samples;
      }

      // Get original symbol pilots.
      span<const cf_t> symbol_pilots = pilots.get_symbol(hop_offset + i_dmrs, i_layer);

      // Apply estimated channel response to the original symbol pilots.
      srsvec::prod(predicted_obs, scaled_estimates.get_slice(i_helper), symbol_pilots);

      // Compensate for CFO only if present.
      if (compensate_cfo && cfo.has_value()) {
        srsvec::sc_prod(
            predicted_obs, predicted_obs, std::polar(1.0F, TWOPI * symbol_start_epochs[i_symbol] * cfo.value()));
      }

      // Skip addition if the intermediate buffer is skipped.
      if (predicted_obs.data() != noise_samples.data()) {
        srsvec::add(noise_samples, predicted_obs, noise_samples);
      }
    }

    // Estimate receiver error as the difference between the received pilots and the regenerated ones.
    unsigned         i_cdm            = start_layer / 2;
    span<const cf_t> symbol_rx_pilots = rx_pilots.get_symbol(i_dmrs, i_cdm);
    srsvec::subtract(noise_samples, symbol_rx_pilots, noise_samples);

    // Accumulate received power.
    noise_energy += srsvec::average_power(noise_samples) * noise_samples.size();
    ++i_dmrs;
  };

  // Process each OFDM containing DM-RS.
  dmrs_mask.for_each(first_hop_symbol, last_hop_symbol, estimate_noise_symbol);

  // Return 0 if the resultant noise is NaN or infinity.
  return std::isnormal(noise_energy) ? noise_energy : 0;
}

__attribute_noinline__ static void
simd_vector_interpolate(span<cbf16_t> out, span<const cf_t> first, span<const cf_t> second, float weight)
{
  unsigned size = out.size();
  srsran_assert(size == first.size(), "Invalid size.");
  srsran_assert(size == second.size(), "Invalid size.");

  unsigned i = 0;
#if SRSRAN_SIMD_CF_SIZE
  for (unsigned end = (size / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != end; i += SRSRAN_SIMD_CF_SIZE) {
    // Load inputs.
    simd_f_t first_0  = srsran_simd_f_loadu(reinterpret_cast<const float*>(&first[i]));
    simd_f_t first_1  = srsran_simd_f_loadu(reinterpret_cast<const float*>(&first[i + SRSRAN_SIMD_CF_SIZE / 2]));
    simd_f_t second_0 = srsran_simd_f_loadu(reinterpret_cast<const float*>(&second[i]));
    simd_f_t second_1 = srsran_simd_f_loadu(reinterpret_cast<const float*>(&second[i + SRSRAN_SIMD_CF_SIZE / 2]));

    // Calculate result.
    simd_f_t result_0 = srsran_simd_f_fma(first_0, srsran_simd_f_sub(second_0, first_0), srsran_simd_f_set1(weight));
    simd_f_t result_1 = srsran_simd_f_fma(first_1, srsran_simd_f_sub(second_1, first_1), srsran_simd_f_set1(weight));

    // Store result.
    srsran_simd_bf16_storeu(reinterpret_cast<bf16_t*>(&out[i]), result_0, result_1);
  }
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != size; ++i) {
    out[i] = to_cbf16(first[i] + (second[i] - first[i]) * weight);
  }
}
