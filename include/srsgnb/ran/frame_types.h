/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstddef>

namespace srsgnb {

/// The number of OFDM symbols per slot is constant for all numerologies.
const unsigned NOF_OFDM_SYM_PER_SLOT_NORMAL_CP   = 14;
const unsigned NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP = 12;

} // namespace srsgnb
