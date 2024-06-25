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

#pragma once

#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/support/interpolator.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator_parameters.h"

namespace srsran {

/// \brief Port channel estimator that averages all OFDM symbols containing DM-RS.
///
/// This estimator considers the channel constant in time over the entire slot. As for the frequency domain, the channel
/// coefficients corresponding to subcarriers containing DM-RS pilots are LSE-estimated, while the remaining
/// coefficients are estimated by interpolating the previous ones.
class port_channel_estimator_average_impl : public port_channel_estimator
{
public:
  /// \brief Size of the internal inverse Discrete Fourier Transform.
  ///
  /// The inverse DFT is used to estimate the time alignment. A DFT size of 4096 points allows of a resolution of 16.3
  /// and 8.1 nanoseconds with a subcarrier spacing of 15 kHz and 30 kHz, respectively.
  static constexpr unsigned DFT_SIZE = 4096;
  /// \brief Maximum SINR in decibels.
  ///
  /// The SINR is bounded above to avoid a zero noise variance.
  static constexpr float MAX_SINR_DB = 100;

  /// Maximum number of virtual pilots used for estimation at the edges.
  static constexpr unsigned MAX_V_PILOTS = 12;

  /// Constructor - Sets the internal interpolator and inverse DFT processor of size \c DFT_SIZE.
  port_channel_estimator_average_impl(std::unique_ptr<interpolator>                interp,
                                      std::unique_ptr<time_alignment_estimator>    ta_estimator_,
                                      port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy_,
                                      bool                                         compensate_cfo_ = true) :
    fd_smoothing_strategy(fd_smoothing_strategy_),
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
               const configuration&        cfg) override;

private:
  /// Specializes \ref compute for one layer and one hop.
  void compute_layer_hop(channel_estimate&           estimate,
                         const resource_grid_reader& grid,
                         unsigned                    port,
                         const dmrs_symbol_list&     pilots,
                         const configuration&        cfg,
                         unsigned                    hop,
                         unsigned                    layer);

  /// \brief Preprocesses the pilots and computes the CFO.
  ///
  /// For the current hop, the function does the following:
  /// - matches the received pilots with the expected ones (element-wise multiplication with complex conjugate);
  /// - estimates the CFO (if the number of OFDM symbols with pilots is at least 2);
  /// - compensates the CFO for all pilots;
  /// - accumulates all the matched, CFO-compensated received pilots.
  /// \param[in]  rx_pilots         Received pilots.
  /// \param[in]  pilots            Transmitted pilots.
  /// \param[in]  dmrs_mask         Boolean mask identifying the OFDM symbols carrying DM-RS within the slot.
  /// \param[in]  scs               Subcarrier spacing.
  /// \param[in]  symbol_start_epochs   Cumulative duration of all CPs in the slot.
  /// \param[in]  compensate_cfo    Boolean flag to activate the CFO compensation.
  /// \param[in]  first_hop_symbol  Index of the first OFDM symbol of the current hop, within the slot.
  /// \param[in]  last_hop_symbol   Index of the last OFDM symbol of the current hop (not included), within the slot.
  /// \param[in]  hop_offset        Number of OFDM symbols carrying DM-RS in the previous hop.
  /// \param[in]  i_layer           The considered transmission layer.
  /// \return A contribution to the EPRE and CFO estimates. CFO is empty if the hop has only one OFDM symbol carrying
  /// DM-RS.
  ///
  /// \warning This method updates the content of the buffers \c pilots_lse and \c pilot_products.
  std::pair<float, std::optional<float>> preprocess_pilots_and_cfo(const dmrs_symbol_list&                   pilots,
                                                                   const bounded_bitset<MAX_NSYMB_PER_SLOT>& dmrs_mask,
                                                                   const subcarrier_spacing&                 scs,
                                                                   unsigned first_hop_symbol,
                                                                   unsigned last_hop_symbol,
                                                                   unsigned hop_offset,
                                                                   unsigned i_layer);

  /// \brief Computes the starting time of the symbols inside a slot for the given subcarrier spacing.
  ///
  /// The symbol starting time is computed from the start of the slot and is expressed in units of OFDM symbol duration.
  void initialize_symbol_start_epochs(cyclic_prefix cp, subcarrier_spacing scs);

  /// Frequency domain smoothing strategy.
  port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy;

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
  std::array<cf_t, MAX_RB * NRE> aux_pilot_products;
  /// View on the used part of the pilot buffer \c aux_pilot_products.
  span<cf_t> pilot_products;

  /// Second auxiliary buffer for processing the pilots.
  std::array<cf_t, MAX_RB * NRE + 2 * MAX_V_PILOTS> aux_pilots_lse;
  /// View on the used part of the pilot buffer \c aux_pilots_lse.
  span<cf_t> pilots_lse;

  /// Buffer of frequency response coefficients.
  std::array<cf_t, MAX_RB * NRE> freq_response;
  /// Buffer of frequency response coefficients in \c bfloat16.
  std::array<cbf16_t, MAX_RB * NRE> freq_response_cbf16;

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
