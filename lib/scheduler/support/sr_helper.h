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

#include "srsran/scheduler/config/serving_cell_config.h"

namespace srsran {
namespace sr_helper {

/// \brief Helpers that checks if the slot is a candidate for SR reporting for a given user.
bool is_sr_opportunity_slot(const pucch_config& pucch_cfg, slot_point sl_tx);

} // namespace sr_helper
} // namespace srsran
