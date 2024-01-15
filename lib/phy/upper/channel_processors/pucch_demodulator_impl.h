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

/// \file
/// \brief PUCCH demodulator implementation declaration.

#pragma once

#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pucch_demodulator.h"
#include "srsran/phy/upper/equalization/channel_equalizer.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/pucch/pucch_constants.h"

namespace srsran {

/// PUCCH demodulator implementation.
class pucch_demodulator_impl : public pucch_demodulator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pucch_demodulator_impl(std::unique_ptr<channel_equalizer>       equalizer_,
                         std::unique_ptr<demodulation_mapper>     demapper_,
                         std::unique_ptr<pseudo_random_generator> descrambler_) :
    equalizer(std::move(equalizer_)), demapper(std::move(demapper_)), descrambler(std::move(descrambler_))
  {
    srsran_assert(equalizer, "Invalid pointer to channel_equalizer object.");
    srsran_assert(demapper, "Invalid pointer to demodulation_mapper object.");
    srsran_assert(descrambler, "Invalid pointer to pseudo_random_generator object.");
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format2_configuration& config) override;

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format3_configuration& config) override;

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format4_configuration& config) override;

private:
  /// PUCCH uses a single TX layer.
  static constexpr unsigned SINGLE_TX_LAYER = 1;

  /// \brief Gets PUCCH Resource Elements and channel estimation coefficients given a PUCCH Format 2 allocation.
  ///
  /// Extracts and loads the inner buffers with the PUCCH control data RE from the provided \c resource_grid, and their
  /// corresponding channel estimates from \c channel_ests. The DM-RS RE are skipped.
  ///
  /// \param[in]  resource_grid Resource grid for the current slot.
  /// \param[in]  channel_ests  Channel estimation for the current slot.
  /// \param[in]  config        PUCCH configuration parameters.
  void get_data_re_ests(const resource_grid_reader&  resource_grid,
                        const channel_estimate&      channel_ests,
                        const format2_configuration& config);

  /// Channel equalization component, also in charge of combining contributions of all receive antenna ports.
  std::unique_ptr<channel_equalizer> equalizer;
  /// Demodulation mapper component: transforms channel symbols into log-likelihood ratios (i.e., soft bits).
  std::unique_ptr<demodulation_mapper> demapper;
  /// Descrambler component.
  std::unique_ptr<pseudo_random_generator> descrambler;

  /// \brief Buffer used to transfer channel modulation symbols from the resource grid to the equalizer.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and receive port.
  static_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                cf_t,
                pucch_constants::MAX_NOF_RE * MAX_PORTS,
                channel_equalizer::re_list::dims>
      ch_re;

  /// \brief Buffer used to store channel modulation resource elements at the equalizer output.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and transmit layer.
  static_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                cf_t,
                pucch_constants::MAX_NOF_RE,
                channel_equalizer::re_list::dims>
      eq_re;

  /// \brief Buffer used to transfer symbol noise variances at the equalizer output.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and transmit layer.
  static_tensor<std::underlying_type_t<channel_equalizer::re_list::dims>(channel_equalizer::re_list::dims::nof_dims),
                float,
                pucch_constants::MAX_NOF_RE,
                channel_equalizer::re_list::dims>
      eq_noise_vars;

  /// \brief Buffer used to transfer channel estimation coefficients from the channel estimate to the equalizer.
  /// \remark The channel estimation coefficients are arranged in three dimensions, i.e., resource element, receive port
  /// and transmit layer.
  static_tensor<std::underlying_type_t<channel_equalizer::ch_est_list::dims>(
                    channel_equalizer::ch_est_list::dims::nof_dims),
                cf_t,
                pucch_constants::MAX_NOF_RE * MAX_PORTS,
                channel_equalizer::ch_est_list::dims>
      ch_estimates;

  /// Buffer used to transfer noise variance estimates from the channel estimate to the equalizer.
  std::array<float, MAX_PORTS> noise_var_estimates;

  /// \brief Control data RE allocation pattern for PUCCH Format 2.
  ///
  /// Indicates the Resource Elements containing control data symbols within a PRB, as per TS38.211 Section 6.4.1.3.2.2.
  const re_prb_mask format2_prb_re_mask = {true, false, true, true, false, true, true, false, true, true, false, true};

  /// PRB mask indicating the used PRB within the resource grid.
  bounded_bitset<MAX_RB> prb_mask;
};

} // namespace srsran
