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
