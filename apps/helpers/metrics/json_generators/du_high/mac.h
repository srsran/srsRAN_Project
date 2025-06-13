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

#include "external/nlohmann/json.hpp"

namespace srsran {

struct mac_dl_metric_report;

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given downlink MAC metrics.
nlohmann::json generate(const mac_dl_metric_report& metrics);

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
