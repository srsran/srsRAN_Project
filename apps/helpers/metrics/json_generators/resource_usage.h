#pragma once

#include "external/nlohmann/json.hpp"
#include <string>

namespace srsran {

struct resource_usage_metrics;

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given resource usage metrics.
nlohmann::json generate(const resource_usage_metrics& metrics);

/// Generates a string in JSON format that codifies the given resource usage metrics.
std::string generate_string(const resource_usage_metrics& metrics, int indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
