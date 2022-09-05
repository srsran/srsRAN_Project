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

class channel_equalizer_zf_impl : public channel_equalizer
{
public:
  /// Default constructor.
  channel_equalizer_zf_impl()
  {
    // Do nothing.
  }

  /// See interface for documentation.
  void equalize(equalizer_symbol_list&          mod_symbols,
                equalizer_noise_var_list&       noise_vars,
                const equalizer_ch_symbol_list& ch_symbols,
                const channel_estimate&         ch_estimates,
                float                           tx_scaling) override;

private:
  /// Implementation of a Zero Forcing equalizer algorithm for the scalar, SISO case.
  void equalize_1x1_zf(equalizer_symbol_list&          mod_symbols,
                       equalizer_noise_var_list&       noise_vars,
                       const equalizer_ch_symbol_list& ch_symbols,
                       const channel_estimate&         ch_estimates,
                       float                           tx_scaling);

  /// Implementation of a Zero Forcing equalizer algorithm for the 1 X 2 case.
  void equalize_1x2_zf(equalizer_symbol_list&          mod_symbols,
                       equalizer_noise_var_list&       noise_vars,
                       const equalizer_ch_symbol_list& ch_symbols,
                       const channel_estimate&         ch_estimates,
                       float                           tx_scaling);

  /// Implementation of a Zero Forcing equalizer algorithm for the 2 X 2 case.
  void equalize_2x2_zf(equalizer_symbol_list&          mod_symbols,
                       equalizer_noise_var_list&       noise_vars,
                       const equalizer_ch_symbol_list& ch_symbols,
                       const channel_estimate&         ch_estimates,
                       float                           tx_scaling);
};

} // namespace srsgnb