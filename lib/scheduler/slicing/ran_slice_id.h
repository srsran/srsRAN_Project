/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/strong_type.h"
#include <cstdint>

namespace srsran {

/// RAN slice identifier that should be unique for a given cell,PLMN,S-NSSAI.
struct ran_slice_id_tag {};
using ran_slice_id_t = strong_type<uint8_t, struct ran_slice_id_tag, strong_increment_decrement, strong_equality>;

} // namespace srsran
