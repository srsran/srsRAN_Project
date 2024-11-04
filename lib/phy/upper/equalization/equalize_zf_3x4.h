/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
