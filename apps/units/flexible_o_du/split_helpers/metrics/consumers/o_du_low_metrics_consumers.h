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
