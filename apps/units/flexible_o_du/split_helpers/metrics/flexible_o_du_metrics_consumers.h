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

#include "apps/services/metrics/metrics_consumer.h"
#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

struct ru_metrics;

/// Log consumer for the flexible O-DU metrics.
class flexible_o_du_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit flexible_o_du_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_), verbose(false)
  {
    srsran_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
  const bool           verbose;
};

/// JSON handler for the O-RU metrics.
class o_ru_metrics_handler_json
{
public:
  o_ru_metrics_handler_json(srslog::log_channel& log_chan_, span<const pci_t> pci_sector_map_) :
    log_chan(log_chan_), pci_sector_map(pci_sector_map_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

private:
  srslog::log_channel& log_chan;
  span<const pci_t>    pci_sector_map;
};

/// JSON consumer for the flexible O-DU metrics.
class flexible_o_du_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  flexible_o_du_metrics_consumer_json(srslog::log_channel& log_chan_, std::vector<pci_t> pci_sector_map_) :
    log_chan(log_chan_), pci_sector_map(std::move(pci_sector_map_)), oru_metrics_handler(log_chan, pci_sector_map)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel&      log_chan;
  const std::vector<pci_t>  pci_sector_map;
  o_ru_metrics_handler_json oru_metrics_handler;
};

} // namespace srsran
