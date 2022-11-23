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

/// Port channel estimator that averages all DM-RS.
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
               const dmrs_symbol_list&     symbols,
               const configuration&        cfg) override;

private:
  /// Specializes \ref compute for one layer and one hop.
  void compute_layer_hop(channel_estimate&           estimate,
                         const resource_grid_reader& grid,
                         unsigned                    port,
                         const dmrs_symbol_list&     symbols,
                         const configuration&        cfg,
                         unsigned                    layer,
                         unsigned                    hop);

  /// \brief Interpolator.
  ///
  /// When DM-RS pilots do not occupy all REs in an OFDM symbol, the interpolator estimates the channel of the REs
  /// without pilots.
  std::unique_ptr<interpolator> freq_interpolator;

  /// Temporary storage of symbols.
  dmrs_symbol_list temp_symbols;

  /// Temporary frequency response.
  std::array<cf_t, MAX_RB * NRE> temp_ce_freq;

  /// Temporary RSRP value.
  float rsrp_tmp = 0;
};

} // namespace srsgnb
