/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  /// Constructor - Sets the internal interpolator and inverse DFT processor of size \c DFT_SIZE.
  port_channel_estimator_average_impl(std::unique_ptr<interpolator>                interp,
                                      std::unique_ptr<time_alignment_estimator>    ta_estimator_,
                                      port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy_) :
    fd_smoothing_strategy(fd_smoothing_strategy_),
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

  /// Computes the cumulative duration of all CPs (normalized with respect to the OFDM symbol time) for the given
  /// subcarrier spacing.
  void initialize_cp_cum_duration(cyclic_prefix cp, subcarrier_spacing scs);

  /// Frequency domain smoothing strategy.
  port_channel_estimator_fd_smoothing_strategy fd_smoothing_strategy;

  /// \brief Interpolator.
  ///
  /// When DM-RS pilots do not occupy all REs in an OFDM symbol, the interpolator is used to estimate the channel of the
  /// REs without pilots.
  std::unique_ptr<interpolator> freq_interpolator;

  /// Time alignment estimator.
  std::unique_ptr<time_alignment_estimator> ta_estimator;

  /// Buffer of received signal samples corresponding to pilots.
  dmrs_symbol_list rx_pilots;

  /// Buffer of frequency response coefficients.
  std::array<cf_t, MAX_RB * NRE> freq_response;

  /// Buffer of cumulative duration of CPs.
  std::array<float, MAX_NSYMB_PER_SLOT> cp_cum_help;
  /// View on the used part of the cumulative CP buffer \c cp_cum_help.
  span<float> cp_cum_duration;

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
  optional<float> cfo_normalized = nullopt;
};

} // namespace srsran
