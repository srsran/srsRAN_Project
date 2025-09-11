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

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/units/flexible_o_du/split_helpers/metrics/consumers/o_du_low_metrics_consumers.h"
#include "apps/units/flexible_o_du/split_helpers/metrics/consumers/ru_metrics_consumers.h"

namespace srsran {

struct ru_metrics;

/// Log consumer for the split 6 flexible O-DU low metrics.
class split6_flexible_o_du_low_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  split6_flexible_o_du_low_metrics_consumer_log(srslog::log_channel&     log_chan,
                                                std::vector<pci_t>       pci_sector_map_,
                                                bool                     verbose,
                                                std::chrono::nanoseconds symbol_duration) :
    pci_sector_map(std::move(pci_sector_map_)),
    odu_low_metrics_handler(log_chan, pci_sector_map, verbose),
    ru_metrics_handler(log_chan, pci_sector_map, symbol_duration)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  const std::vector<pci_t>      pci_sector_map;
  o_du_low_metrics_consumer_log odu_low_metrics_handler;
  ru_metrics_consumer_log       ru_metrics_handler;
};

/// JSON consumer for the flexible O-DU metrics.
class split6_flexible_o_du_low_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  split6_flexible_o_du_low_metrics_consumer_json(srslog::log_channel&     log_chan,
                                                 std::vector<pci_t>       pci_sector_map_,
                                                 std::chrono::nanoseconds symbol_duration) :
    pci_sector_map(std::move(pci_sector_map_)),
    odu_low_metrics_handler(log_chan, pci_sector_map),
    ru_metrics_handler(log_chan, pci_sector_map, symbol_duration)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  const std::vector<pci_t>       pci_sector_map;
  o_du_low_metrics_consumer_json odu_low_metrics_handler;
  ru_metrics_consumer_json       ru_metrics_handler;
};

} // namespace srsran
