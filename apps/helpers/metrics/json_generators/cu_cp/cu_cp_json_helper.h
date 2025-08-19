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
#include "srsran/cu_cp/cu_cp_metrics_notifier.h"
#include "srsran/support/timers.h"
#include <string>

namespace srsran {

struct cu_cp_metrics_report;

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given CU-CP metrics.
nlohmann::json generate(const cu_cp_metrics_report& report);

/// Generates a string in JSON format that codifies the given CU-CP metrics.
std::string generate_string(const cu_cp_metrics_report& report, int indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
