/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "external/nlohmann/json.hpp"
#include <string>

namespace srsran {

namespace srs_du {
struct du_metrics_report;
}

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given O-DU high metrics.
nlohmann::json generate(const srs_du::du_metrics_report& metrics);

/// Generates a string in JSON format that codifies the given O-DU high metrics.
std::string generate_string(const srs_du::du_metrics_report& metrics, int indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
