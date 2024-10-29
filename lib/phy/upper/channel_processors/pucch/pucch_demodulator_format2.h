/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUCCH Format 2 demodulator declaration.

#pragma once

#include "srsran/adt/to_array.h"
#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/phy/upper/equalization/dynamic_ch_est_list.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/pucch/pucch_constants.h"

namespace srsran {

/// PUCCH Format 2 demodulator.
class pucch_demodulator_format2
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pucch_demodulator_format2(std::unique_ptr<channel_equalizer>       equalizer_,
                            std::unique_ptr<demodulation_mapper>     demapper_,
                            std::unique_ptr<pseudo_random_generator> descrambler_) :
    equalizer(std::move(equalizer_)),
    demapper(std::move(demapper_)),
    descrambler(std::move(descrambler_)),
    ch_estimates(pucch_constants::MAX_NOF_RE, MAX_PORTS, 1)
  {
    srsran_assert(equalizer, "Invalid pointer to channel_equalizer object.");
    srsran_assert(demapper, "Invalid pointer to demodulation_mapper object.");
    srsran_assert(descrambler, "Invalid pointer to pseudo_random_generator object.");
  }

  /// Demodulates a PUCCH Format 2 transmission. See \ref pucch_demodulator for more details.
  void demodulate(span<log_likelihood_ratio>                      llr,
                  const resource_grid_reader&                     grid,
                  const channel_estimate&                         estimates,
                  const pucch_demodulator::format2_configuration& config);

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
  void get_data_re_ests(const resource_grid_reader&                     resource_grid,
                        const channel_estimate&                         channel_ests,
                        const pucch_demodulator::format2_configuration& config);

  /// Channel equalization component, also in charge of combining contributions of all receive antenna ports.
  std::unique_ptr<channel_equalizer> equalizer;
  /// Demodulation mapper component: transforms channel symbols into log-likelihood ratios (i.e., soft bits).
  std::unique_ptr<demodulation_mapper> demapper;
  /// Descrambler component.
  std::unique_ptr<pseudo_random_generator> descrambler;

  /// \brief Buffer used to transfer channel modulation symbols from the resource grid to the equalizer.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and receive port.
  static_re_buffer<MAX_PORTS, pucch_constants::MAX_NOF_RE, cbf16_t> ch_re;

  /// \brief Buffer used to store channel modulation resource elements at the equalizer output.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and transmit layer.
  static_vector<cf_t, pucch_constants::MAX_NOF_RE> eq_re;

  /// \brief Buffer used to transfer symbol noise variances at the equalizer output.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and transmit layer.
  static_vector<float, pucch_constants::MAX_NOF_RE> eq_noise_vars;

  /// \brief Buffer used to transfer channel estimation coefficients from the channel estimate to the equalizer.
  /// \remark The channel estimation coefficients are arranged in three dimensions, i.e., resource element, receive port
  /// and transmit layer.
  dynamic_ch_est_list ch_estimates;

  /// Buffer used to transfer noise variance estimates from the channel estimate to the equalizer.
  std::array<float, MAX_PORTS> noise_var_estimates;

  /// Temporary channel estimates for an OFDM symbol.
  std::array<cf_t, MAX_RB * NRE> temp_ests_symbol;
  /// PRB mask indicating the used PRB within the resource grid.
  bounded_bitset<MAX_RB> prb_mask;
};

} // namespace srsran
