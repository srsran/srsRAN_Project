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
/// \brief Channel equalizer implementation for the Zero Forcing and the Minimum Mean Square Error methods.

#pragma once

#include "srsran/phy/upper/equalization/channel_equalizer.h"
#include "srsran/phy/upper/equalization/channel_equalizer_algorithm_type.h"

namespace srsran {

/// Channel equalizer implementation for the Zero Forcing and the MMSE algorithms.
class channel_equalizer_generic_impl : public channel_equalizer
{
public:
  /// Default constructor.
  explicit channel_equalizer_generic_impl(channel_equalizer_algorithm_type type_) : type(type_) {}

  // See interface for documentation.
  void equalize(span<cf_t>         eq_symbols,
                span<float>        eq_noise_vars,
                const re_list&     ch_symbols,
                const ch_est_list& ch_estimates,
                span<const float>  noise_var_estimates,
                float              tx_scaling) override;

private:
  channel_equalizer_algorithm_type type;
};

} // namespace srsran