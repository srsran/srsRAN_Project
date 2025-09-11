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

#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_metrics.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <map>

namespace srsran {
namespace srs_cu_cp {

inline std::chrono::milliseconds get_current_time()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

struct rrc_connection_establishment_metrics {
  rrc_connection_establishment_counter_with_cause attempted_rrc_connection_establishments;
  rrc_connection_establishment_counter_with_cause successful_rrc_connection_establishments;
};

struct rrc_connection_reestablishment_metrics {
  unsigned attempted_rrc_connection_reestablishments                     = 0;
  unsigned successful_rrc_connection_reestablishments_with_ue_context    = 0;
  unsigned successful_rrc_connection_reestablishments_without_ue_context = 0;
};

class rrc_du_metrics_aggregator : public rrc_du_metrics_collector
{
public:
  rrc_du_metrics_aggregator();
  ~rrc_du_metrics_aggregator() = default;

  void aggregate_successful_rrc_setup();

  void aggregate_successful_rrc_release();

  void aggregate_attempted_connection_establishment(establishment_cause_t cause);

  void aggregate_successful_connection_establishment(establishment_cause_t cause);

  void aggregate_attempted_connection_reestablishment();

  void aggregate_successful_connection_reestablishment(bool with_ue_context);

  void collect_metrics(rrc_du_metrics& metrics) override;

private:
  /// Aggregator for RRC connection related metrics.
  struct rrc_connection_metrics_aggregator {
    rrc_connection_metrics_aggregator() = default;

    void reset()
    {
      rrc_connections_with_time.clear();
      rrc_connections_with_time.emplace(get_current_time(), current_rrc_connections);
    }

    void add_rrc_connection()
    {
      std::chrono::milliseconds now = get_current_time();
      current_rrc_connections++;
      rrc_connections_with_time[now] = current_rrc_connections;
    }

    void remove_rrc_connection()
    {
      std::chrono::milliseconds now = get_current_time();
      current_rrc_connections--;
      rrc_connections_with_time[now] = current_rrc_connections;
    }

    unsigned get_mean_nof_rrc_connections()
    {
      if (rrc_connections_with_time.size() < 2) {
        // No need to calculate weighted mean.
        return rrc_connections_with_time.begin()->second;
      }

      if (rrc_connections_with_time.size() == 2 && rrc_connections_with_time.begin()->second == 0) {
        // Only one measurement has been made (the map is initialized with a zero value).
        return rrc_connections_with_time.end()->second;
      }

      // Add current value to the map to count all actual measurements (the last value of the map will be ignored).
      auto now                       = get_current_time();
      rrc_connections_with_time[now] = current_rrc_connections;

      long long weighted_sum = 0;
      long long weight_sum   = 0;

      auto it         = rrc_connections_with_time.begin();
      auto prev_time  = it->first;
      auto prev_value = it->second;
      ++it;

      for (; it != rrc_connections_with_time.end(); ++it) {
        auto current_time  = it->first;
        auto current_value = it->second;

        // Calculate time difference (weight).
        auto time_diff = (current_time - prev_time).count();

        // Update weighted sum and total weight.
        weighted_sum += static_cast<long long>(prev_value) * time_diff;
        weight_sum += time_diff;

        // Update previous time and value.
        prev_time  = current_time;
        prev_value = current_value;
      }

      // Avoid division by zero.
      if (weight_sum == 0) {
        return 0;
      }

      return std::floor(static_cast<double>(weighted_sum) / weight_sum);
    }

    unsigned get_max_nof_rrc_connections()
    {
      // If the map is empty, no RRC connections have been established.
      if (rrc_connections_with_time.empty()) {
        return 0;
      }

      auto max_it = std::max_element(rrc_connections_with_time.begin(),
                                     rrc_connections_with_time.end(),
                                     [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });

      return max_it->second;
    }

    std::map<std::chrono::milliseconds, unsigned> rrc_connections_with_time;
    unsigned                                      current_rrc_connections = 0;
  };

  rrc_connection_metrics_aggregator      connection_metrics;
  rrc_connection_establishment_metrics   connection_establishment_metrics;
  rrc_connection_reestablishment_metrics connection_reestablishment_metrics;
};

} // namespace srs_cu_cp
} // namespace srsran
