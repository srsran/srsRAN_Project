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

#pragma once

#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/support/interpolator.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"

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

  /// Constructor - Sets the internal interpolator and inverse DFT processor of size \c DFT_SIZE.
  port_channel_estimator_average_impl(std::unique_ptr<interpolator> interp, std::unique_ptr<dft_processor> idft_proc) :
    freq_interpolator(std::move(interp)), idft(std::move(idft_proc))
  {
    srsran_assert(freq_interpolator, "Invalid interpolator.");
    srsran_assert(idft->get_direction() == dft_processor::direction::INVERSE,
                  "The port channel estimator requires an inverse (not direct) DFT processor.");
    srsran_assert(idft->get_size() == DFT_SIZE,
                  "The port channel estimator requires an iDFT of size {}, provided {}.",
                  static_cast<unsigned>(DFT_SIZE),
                  idft->get_size());
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

  /// \brief Interpolator.
  ///
  /// When DM-RS pilots do not occupy all REs in an OFDM symbol, the interpolator is used to estimate the channel of the
  /// REs without pilots.
  std::unique_ptr<interpolator> freq_interpolator;

  /// Inverse DFT processor of size \c DFT_SIZE.
  std::unique_ptr<dft_processor> idft;

  /// Buffer of received signal samples corresponding to pilots.
  dmrs_symbol_list rx_pilots;

  /// Buffer of frequency response coefficients.
  std::array<cf_t, MAX_RB * NRE> freq_response;

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
};

} // namespace srsran
