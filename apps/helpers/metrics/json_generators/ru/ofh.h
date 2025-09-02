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

namespace ofh {
struct metrics;
}

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given Open Fronthaul low metrics.
nlohmann::json
generate(const ofh::metrics& metrics, span<const pci_t> pci_sector_map, std::chrono::nanoseconds symbol_duration);

/// Generates a string in JSON format that codifies the given Open Fronthaul low metrics.
std::string generate_string(const ofh::metrics&      metrics,
                            span<const pci_t>        pci_sector_map,
                            std::chrono::nanoseconds symbol_duration,
                            int                      indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
