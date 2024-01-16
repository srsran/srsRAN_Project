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
/// \brief Zero Forcing equalizer algorithm for a MIMO 2 X 2 channel.

#pragma once

#include "channel_equalizer_zf_impl.h"

namespace srsran {

/// \brief Implementation of a Zero Forcing equalizer algorithm for a MIMO 2 X 2 channel.
/// \param[out] eq_symbols   Resultant equalized symbols.
/// \param[out] noise_vars   Noise variances after equalization.
/// \param[in]  ch_symbols   Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates Channel estimation coefficients.
/// \param[in]  noise_var_est Estimated noise variance. It is assumed to be the same for each receive port.
/// \param[in]  tx_scaling   Transmission gain scaling factor.
void equalize_zf_2x2(channel_equalizer::re_list&           eq_symbols,
                     channel_equalizer::noise_var_list&    noise_vars,
                     const channel_equalizer::re_list&     ch_symbols,
                     const channel_equalizer::ch_est_list& ch_estimates,
                     float                                 noise_var_est,
                     float                                 tx_scaling);
} // namespace srsran