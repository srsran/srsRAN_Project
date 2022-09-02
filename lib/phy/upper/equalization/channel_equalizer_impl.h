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

#include "srsgnb/phy/upper/equalization/channel_equalizer.h"

namespace srsgnb {

class channel_equalizer_impl : public channel_equalizer
{
public:
  /// Default constructor.
  channel_equalizer_impl()
  {
    // Do nothing.
  }

  void equalize(equalizer_symbol_list&          mod_symbols,
                equalizer_noise_var_list&       noise_vars,
                const equalizer_ch_symbol_list& ch_symbols,
                const channel_estimate&         ch_estimates,
                float                           tx_scaling) override;
};

} // namespace srsgnb