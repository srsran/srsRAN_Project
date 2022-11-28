/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/support/interpolator.h"
#include "srsgnb/phy/upper/signal_processors/port_channel_estimator.h"

namespace srsgnb {

/// \brief Port channel estimator that averages all OFDM symbols containing DM-RS.
///
/// This estimator considers the channel constant in time over the entire slot. As for the frequency domain, the channel
/// coefficients corresponding to subcarriers containing DM-RS pilots are LSE-estimated, while the remaining
/// coefficients are estimated by interpolating the previous ones.
class port_channel_estimator_average_impl : public port_channel_estimator
{
public:
  /// Constructor - Sets the internal interpolator.
  explicit port_channel_estimator_average_impl(std::unique_ptr<interpolator> interp) :
    freq_interpolator(std::move(interp))
  {
    srsgnb_assert(freq_interpolator, "Invalid interpolator.");
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

  /// Buffer of received signal samples corresponding to pilots.
  dmrs_symbol_list rx_pilots;

  /// Buffer of frequency response coefficients.
  std::array<cf_t, MAX_RB * NRE> freq_response;

  /// Estimated RSRP value (single layer).
  float rsrp = 0;

  /// Estimated noise variance (single layer).
  float noise_var = 0;
};

} // namespace srsgnb
