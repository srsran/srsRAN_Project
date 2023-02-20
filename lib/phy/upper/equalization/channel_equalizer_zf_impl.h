/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Channel equalizer implementation using the Zero Forcing method.

#pragma once

#include "srsgnb/phy/upper/equalization/channel_equalizer.h"

namespace srsran {

/// Channel equalizer implementation using the Zero Forcing algorithm.
class channel_equalizer_zf_impl : public channel_equalizer
{
public:
  /// Default constructor.
  explicit channel_equalizer_zf_impl() = default;

  // See interface for documentation.
  void equalize(re_list&           eq_symbols,
                noise_var_list&    eq_noise_vars,
                const re_list&     ch_symbols,
                const ch_est_list& ch_estimates,
                span<const float>  noise_var_estimates,
                float              tx_scaling) override;
};

} // namespace srsran
