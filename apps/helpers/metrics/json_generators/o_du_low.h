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
#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"
#include <string>

namespace srsran {

namespace srs_du {
struct o_du_low_metrics;
}

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given O-DU low metrics.
nlohmann::json generate(const srs_du::o_du_low_metrics& metrics, span<const pci_t> pci_sector_map);

/// Generates a string in JSON format that codifies the given O-DU low metrics.
std::string generate_string(const srs_du::o_du_low_metrics& metrics, span<const pci_t> pci_sector_map, int indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
