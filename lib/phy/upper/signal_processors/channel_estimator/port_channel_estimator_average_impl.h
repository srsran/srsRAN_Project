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

#pragma once

#include "srsran/phy/support/interpolator.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "srsran/phy/upper/signal_processors/channel_estimator/port_channel_estimator.h"
#include "srsran/phy/upper/signal_processors/channel_estimator/port_channel_estimator_parameters.h"

namespace srsran {

/// \brief Port channel estimator that averages all OFDM symbols containing DM-RS.
///
/// This estimator considers the channel constant in time over the entire slot. As for the frequency domain, the channel
/// coefficients corresponding to subcarriers containing DM-RS pilots are LSE-estimated, while the remaining
/// coefficients are estimated by interpolating the previous ones.
class port_channel_estimator_average_impl : public port_channel_estimator
{
public:
  /// Maximum supported number of layers.
  static constexpr unsigned MAX_LAYERS = 4;

  /// \brief Maximum SINR in decibels.
  ///
  /// The SINR is bounded above to avoid a zero noise variance.
  static constexpr float MAX_SINR_DB = 100;

  /// Maximum number of virtual pilots used for estimation at the edges.
  static constexpr unsigned MAX_V_PILOTS = 12;

  /// Maximum number of pilots per OFDM symbol.
  static constexpr unsigned MAX_NOF_PILOTS_SYMBOL = MAX_RB * NRE + 2 * MAX_V_PILOTS;

  /// Maximum number of OFDM symbols that contain DM-RS in a transmission.
  static constexpr unsigned MAX_NOF_DMRS_SYMBOLS = pusch_constants::MAX_NOF_DMRS_SYMBOLS;

  /// Constructor - Sets the internal interpolator and inverse DFT processor of size \c DFT_SIZE.
  port_channel_estimator_average_impl(std::unique_ptr<interpolator>                    interp,
                                      std::unique_ptr<time_alignment_estimator>        ta_estimator_,
                                      port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy_,
                                      port_channel_estimator_td_interpolation_strategy td_interpolation_strategy_,
                                      bool                                             compensate_cfo_ = true) :
    fd_smoothing_strategy(fd_smoothing_strategy_),
    td_interpolation_strategy(td_interpolation_strategy_),
    compensate_cfo(compensate_cfo_),
    freq_interpolator(std::move(interp)),
    ta_estimator(std::move(ta_estimator_))
  {
    srsran_assert(freq_interpolator, "Invalid interpolator.");
    srsran_assert(ta_estimator, "Invalid TA estimator.");
  }

  // See interface for documentation.
  void compute(channel_estimate&           estimate,
               const resource_grid_reader& grid,
               unsigned                    port,
               const dmrs_symbol_list&     pilots,
               const configuration&        cfg) override
  {
    do_compute(estimate, grid, port, pilots, cfg);
  }

private:
  /// Actual implementation of the \c compute public method.
  void do_compute(channel_estimate&           estimate,
                  const resource_grid_reader& grid,
                  unsigned                    port,
                  const dmrs_symbol_list&     pilots,
                  const configuration&        cfg);

  /// Specializes \ref compute for one hop.
  void compute_hop(channel_estimate&           estimate,
                   const resource_grid_reader& grid,
                   unsigned                    port,
                   const dmrs_symbol_list&     pilots,
                   const configuration&        cfg,
                   unsigned                    hop);

  /// \brief Preprocesses the pilots and computes the CFO.
  ///
  /// For the current hop, the function does the following:
  /// - matches the received pilots with the expected ones (element-wise multiplication with complex conjugate);
  /// - estimates the CFO (if the number of OFDM symbols with pilots is at least 2).
  /// \param[in]  pilots            Transmitted pilots.
  /// \param[in]  dmrs_mask         Boolean mask identifying the OFDM symbols carrying DM-RS within the slot.
  /// \param[in]  first_hop_symbol  Index of the first OFDM symbol of the current hop, within the slot.
  /// \param[in]  last_hop_symbol   Index of the last OFDM symbol of the current hop (not included), within the slot.
  /// \param[in]  hop_offset        Number of OFDM symbols carrying DM-RS in the previous hop.
  /// \param[in]  start_layer       Index of the first transmission layer to be preprocessed.
  /// \param[in]  stop_layer        Index of the last transmission layer (not included) to be preprocessed.
  /// \return A contribution to the CFO estimate. CFO is empty if the hop has only one OFDM symbol carrying DM-RS.
  ///
  /// \warning This method updates the content of the buffers \c pilots_lse and \c pilot_products.
  std::optional<float> preprocess_pilots_and_estimate_cfo(const dmrs_symbol_list&                   pilots,
                                                          const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                                                          unsigned                                  first_hop_symbol,
                                                          unsigned                                  last_hop_symbol,
                                                          unsigned                                  hop_offset,
                                                          unsigned                                  start_layer,
                                                          unsigned                                  stop_layer);

  /// \brief Compensates the CFO.
  ///
  /// For the current hop:
  /// - compensates the CFO for all pilots;
  /// - accumulates all the matched, CFO-compensated received pilots from all OFDM symbols carrying DM-RS.
  /// \param[in]  pilots            Transmitted pilots.
  /// \param[in]  dmrs_mask         Boolean mask identifying the OFDM symbols carrying DM-RS within the slot.
  /// \param[in]  first_hop_symbol  Index of the first OFDM symbol of the current hop, within the slot.
  /// \param[in]  last_hop_symbol   Index of the last OFDM symbol of the current hop (not included), within the slot.
  /// \param[in]  cfo               Estimated CFO.
  ///
  /// \warning This method updates the content of the buffers \c pilots_lse and \c pilot_products.
  void compensate_cfo_and_accumulate(const dmrs_symbol_list&                   pilots,
                                     const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                                     unsigned                                  first_hop_symbol,
                                     unsigned                                  last_hop_symbol,
                                     std::optional<float>                      cfo);

  /// \brief Computes the starting time of the symbols inside a slot for the given subcarrier spacing.
  ///
  /// The symbol starting time is computed from the start of the slot and is expressed in units of OFDM symbol duration.
  void initialize_symbol_start_epochs(cyclic_prefix cp, subcarrier_spacing scs);

  /// \brief Applies the time domain interpolation strategy for a given OFDM symbol within the hop transmission.
  /// \param[out] destination      Estimated resource grid OFDM symbol for a single channel.
  /// \param[in]  dmrs_mask        Time-domain DM-RS mask.
  /// \param[in]  freq_response    Frequency-domain channel estimates for the given channel for each of the OFDM symbols
  ///                              containing DM-RS.
  /// \param[in]  hop_first_symbol Start symbol index for the hop within the slot.
  /// \param[in]  hop_last_symbol  Last symbol index for the hop within the slot.
  /// \param[in]  i_symbol         OFDM symbol index within the slot to calculate.
  void apply_td_domain_strategy(span<cbf16_t>                 destination,
                                const symbol_slot_mask&       dmrs_mask,
                                const re_buffer_reader<cf_t>& freq_response,
                                unsigned                      hop_first_symbol,
                                unsigned                      hop_last_symbol,
                                unsigned                      i_symbol) const;

  /// Frequency domain smoothing strategy.
  port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy;

  /// Time domain smoothing strategy.
  port_channel_estimator_td_interpolation_strategy td_interpolation_strategy;

  /// Boolean flag for activating CFO compensation (active when true).
  bool compensate_cfo;

  /// \brief Interpolator.
  ///
  /// When DM-RS pilots do not occupy all REs in an OFDM symbol, the interpolator is used to estimate the channel of the
  /// REs without pilots.
  std::unique_ptr<interpolator> freq_interpolator;

  /// Time alignment estimator.
  std::unique_ptr<time_alignment_estimator> ta_estimator;

  /// Buffer of received signal samples corresponding to pilots.
  dmrs_symbol_list rx_pilots;

  /// Auxiliary buffer for processing the pilots.
  static_re_buffer<MAX_LAYERS, MAX_RB * NRE> pilot_products;

  /// Second auxiliary buffer for processing the pilots.
  static_re_measurement<cf_t, MAX_NOF_PILOTS_SYMBOL, MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS> enlarged_pilots_lse;
  modular_re_measurement<cf_t, MAX_NOF_DMRS_SYMBOLS, MAX_LAYERS>                       pilots_lse;

  /// Buffer of frequency response coefficients.
  static_re_buffer<MAX_NOF_DMRS_SYMBOLS, MAX_RB * NRE, cf_t> freq_response;

  /// Buffer for the OFDM symbols starting epochs within the current slot.
  std::array<float, MAX_NSYMB_PER_SLOT> aux_symbol_start_epochs;
  /// View on the used part of the symbols starting epochs buffer \c aux_symbol_start_epochs.
  span<float> symbol_start_epochs;

  /// Estimated RSRP value (single layer).
  float rsrp = 0;

  /// \brief Observed average DM-RS EPRE.
  ///
  /// \remark The EPRE is defined as the average received power (including noise) across all REs carrying DM-RS.
  float epre = 0;

  /// Estimated noise variance (single layer).
  float noise_var = 0;

  /// Estimated time alignment in seconds.
  float time_alignment_s = 0;

  /// Estimated CFO, normalized with respect to the subcarrier spacing.
  std::optional<float> cfo_normalized = std::nullopt;
};

} // namespace srsran
