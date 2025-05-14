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

#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"

namespace srsran {

namespace srs_du {
struct o_du_low_metrics;
}

/// JSON handler for the O-DU low metrics.
class o_du_low_metrics_consumer_json
{
public:
  o_du_low_metrics_consumer_json(srslog::log_channel& log_chan_, span<const pci_t> pci_sector_map_) :
    log_chan(log_chan_), pci_sector_map(pci_sector_map_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // Handles the O-RU metrics.
  void handle_metric(const srs_du::o_du_low_metrics& metric);

private:
  srslog::log_channel& log_chan;
  span<const pci_t>    pci_sector_map;
};

/// Logger consumer for the O-DU low metrics.
class o_du_low_metrics_consumer_log
{
public:
  o_du_low_metrics_consumer_log(srslog::log_channel& log_chan_, span<const pci_t> pci_sector_map_, bool verbose_) :
    log_chan(log_chan_), verbose(verbose_), pci_sector_map(pci_sector_map_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // Handles the O-RU metrics.
  void handle_metric(const srs_du::o_du_low_metrics& metric);

private:
  srslog::log_channel& log_chan;
  const bool           verbose;
  span<const pci_t>    pci_sector_map;
};

} // namespace srsran
