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

#include "split_8_o_du_unit_config.h"

namespace srsran {

/// Validates the given split 8 O-RAN DU unit configuration. Returns true on success, false otherwise.
bool validate_split_8_o_du_unit_config(const split_8_o_du_unit_config&  config,
                                       const os_sched_affinity_bitmask& available_cpus);

} // namespace srsran
