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

#include "srsran/rrc/meas_types.h"

namespace srsran {
namespace srs_cu_cp {

/// Validates the provided config(s).
bool validate_config(const rrc_meas_trigger_quant_offset& config);
bool validate_config(const rrc_cond_event_a3& config);

} // namespace srs_cu_cp
} // namespace srsran
