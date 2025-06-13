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
#include "srsran/support/timers.h"
#include <string>

namespace srsran {

struct pdcp_tx_metrics_container;
struct pdcp_rx_metrics_container;

namespace app_helpers {
namespace json_generators {

/// Generates a nlohmann JSON object that codifies the given PDCP metrics.
nlohmann::json generate(const pdcp_tx_metrics_container& tx,
                        const pdcp_rx_metrics_container& rx,
                        double                           tx_cpu_usage,
                        double                           rx_cpu_usage,
                        timer_duration                   metrics_period);

/// Generates a string in JSON format that codifies the given PDCP metrics.
std::string generate_string(const pdcp_tx_metrics_container& tx,
                            const pdcp_rx_metrics_container& rx,
                            double                           tx_cpu_usage,
                            double                           rx_cpu_usage,
                            timer_duration                   metrics_period,
                            int                              indent = -1);

} // namespace json_generators
} // namespace app_helpers
} // namespace srsran
