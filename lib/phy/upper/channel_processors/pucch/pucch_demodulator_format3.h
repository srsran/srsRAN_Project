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
/// \brief PUCCH Format 3 demodulator declaration.

#pragma once

#include "srsran/adt/to_array.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/phy/upper/equalization/dynamic_ch_est_list.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/pucch/pucch_constants.h"

namespace srsran {

/// PUCCH Format 3 demodulator.
class pucch_demodulator_format3
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pucch_demodulator_format3(std::unique_ptr<channel_equalizer>       equalizer_,
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

  /// Demodulates a PUCCH Format 3 transmission. See \ref pucch_demodulator for more details.
  void demodulate(span<log_likelihood_ratio>                      llr,
                  const resource_grid_reader&                     grid,
                  const channel_estimate&                         estimates,
                  const pucch_demodulator::format3_configuration& config);

private:
  /// Channel equalization component, also in charge of combining contributions of all receive antenna ports.
  std::unique_ptr<channel_equalizer> equalizer;
  /// Demodulation mapper component: transforms channel symbols into log-likelihood ratios (i.e., soft bits).
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
};

} // namespace srsran
