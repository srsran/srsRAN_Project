/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Zero Forcing equalizer algorithm for a MISO 1 X 2 channel.

#pragma once

#include "channel_equalizer_zf_impl.h"

namespace srsgnb {

/// \brief Implementation of a Zero Forcing equalizer algorithm for a MISO 1 X 2 channel.
/// \param[out] eq_symbols   Resultant equalized symbols.
/// \param[out] noise_vars   Noise variances after equalization.
/// \param[in]  ch_symbols   Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates Channel estimation coefficients.
/// \param[in]  tx_scaling   Transmission gain scaling factor.
void equalize_zf_1x2(equalizer_symbol_list&          eq_symbols,
                     equalizer_noise_var_list&       noise_vars,
                     const equalizer_ch_symbol_list& ch_symbols,
                     const channel_estimate&         ch_estimates,
                     float                           tx_scaling);
} // namespace srsgnb