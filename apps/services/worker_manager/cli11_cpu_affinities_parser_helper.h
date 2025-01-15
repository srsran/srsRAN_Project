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

#include <string>

namespace srsran {

struct os_sched_affinity_bitmask;

void parse_affinity_mask(os_sched_affinity_bitmask& mask, const std::string& value, const std::string& property_name);

} // namespace srsran
