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

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/upper/equalization/channel_equalizer.h"

namespace srsran {

void equalize_zf_3x4(span<cf_t>                            eq_symbols,
                     span<float>                           noise_vars,
                     const re_buffer_reader<cbf16_t>&      ch_symbols,
                     const channel_equalizer::ch_est_list& ch_estimates_,
                     float                                 noise_var_est,
                     float                                 tx_scaling);

} // namespace srsran
