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

/// \file
/// \brief PUCCH Format 4 demodulator declaration.

#pragma once

#include "srsran/adt/to_array.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/phy/upper/equalization/dynamic_ch_est_list.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/pucch/pucch_constants.h"

namespace srsran {

/// PUCCH Format 4 demodulator.
class pucch_demodulator_format4
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pucch_demodulator_format4(std::unique_ptr<channel_equalizer>       equalizer_,
                            std::unique_ptr<demodulation_mapper>     demapper_,
                            std::unique_ptr<pseudo_random_generator> descrambler_,
                            std::unique_ptr<transform_precoder>      precoder_) :
    equalizer(std::move(equalizer_)),
    demapper(std::move(demapper_)),
    descrambler(std::move(descrambler_)),
    precoder(std::move(precoder_))
  {
    srsran_assert(equalizer, "Invalid pointer to channel_equalizer object.");
    srsran_assert(demapper, "Invalid pointer to demodulation_mapper object.");
    srsran_assert(descrambler, "Invalid pointer to pseudo_random_generator object.");
    srsran_assert(precoder, "Invalid pointer to transform precoder object.");
  }

  /// Demodulates a PUCCH Format 4 transmission. See \ref pucch_demodulator for more details.
  void demodulate(span<log_likelihood_ratio>                      llr,
                  const resource_grid_reader&                     grid,
                  const channel_estimate&                         estimates,
                  const pucch_demodulator::format4_configuration& config);

private:
  /// \brief Channel equalizer.
  ///
  /// Combines the contributions of all receive antenna ports, equalizing the channels.
  std::unique_ptr<channel_equalizer> equalizer;
  /// \brief Demodulation mapper.
  ///
  /// Transforms channel symbols into log-likelihood ratios (i.e., soft  bits).
  std::unique_ptr<demodulation_mapper> demapper;
  /// Descrambler component.
  std::unique_ptr<pseudo_random_generator> descrambler;
  /// Transform precoder component.
  std::unique_ptr<transform_precoder> precoder;

  /// \brief Buffer used to store channel modulation resource elements at the equalizer output.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and transmit layer.
  static_vector<cf_t, pucch_constants::MAX_NOF_RE> eq_re;

  /// \brief Buffer used to transfer symbol noise variances at the equalizer output.
  /// \remark The symbols are arranged in two dimensions, i.e., resource element and transmit layer.
  static_vector<float, pucch_constants::MAX_NOF_RE> eq_noise_vars;

  /// \brief Reverts the block-wise spreading applied to PUCCH Format 4, as per TS38.211 Section 6.3.2.6.3.
  /// \param[out] original      Destination buffer for the original symbols.
  /// \param[out] noise_vars    Destination buffer for the noise variances.
  /// \param[in]  eq_re         Input buffer for the spread symbols.
  /// \param[in]  eq_noise_vars Input buffer for the noise variances.
  /// \param[in]  config        PUCCH Format 4 configuration parameters.
  static void inverse_blockwise_spreading(span<cf_t>                                      original,
                                          span<float>                                     noise_vars,
                                          span<const cf_t>                                eq_re,
                                          span<const float>                               eq_noise_vars,
                                          const pucch_demodulator::format4_configuration& config);
};

} // namespace srsran
