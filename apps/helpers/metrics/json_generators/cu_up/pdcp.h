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
