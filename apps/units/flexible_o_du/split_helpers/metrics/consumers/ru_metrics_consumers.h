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
#include "srsran/ofh/ofh_metrics.h"
#include "srsran/ran/pci.h"

namespace srsran {

struct ru_dummy_metrics;
struct ru_generic_metrics;
struct ru_metrics;

/// JSON handler for the O-RU metrics.
class ru_metrics_consumer_json
{
public:
  ru_metrics_consumer_json(srslog::log_channel&     log_chan_,
                           span<const pci_t>        pci_sector_map_,
                           std::chrono::nanoseconds symbol_duration_) :
    symbol_duration(symbol_duration_), log_chan(log_chan_), pci_sector_map(pci_sector_map_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

private:
  const std::chrono::nanoseconds symbol_duration;
  srslog::log_channel&           log_chan;
  span<const pci_t>              pci_sector_map;
};

/// Logger consumer for the O-RU metrics.
class ru_metrics_consumer_log
{
public:
  ru_metrics_consumer_log(srslog::log_channel&     log_chan_,
                          span<const pci_t>        pci_sector_map_,
                          std::chrono::nanoseconds symbol_duration_) :
    symbol_duration(symbol_duration_), log_chan(log_chan_), verbose(true), pci_sector_map(pci_sector_map_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

private:
  const std::chrono::nanoseconds symbol_duration;
  srslog::log_channel&           log_chan;
  const bool                     verbose;
  span<const pci_t>              pci_sector_map;
};

/// STDOUT handler for the O-RU metrics.
class ru_metrics_handler_stdout
{
  /// Maximum number of metric lines in STDOUT without a header, ie, print header every
  /// MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER lines.
  static constexpr unsigned MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER = 10;

public:
  ru_metrics_handler_stdout(span<const pci_t> pci_sector_map_, std::chrono::nanoseconds symbol_duration_) :
    symbol_duration(symbol_duration_), pci_sector_map(pci_sector_map_)
  {
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

  /// Prints the header in the next metric handle.
  void print_header() { nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER; }

private:
  /// Log SDR RU metrics in STDOUT.
  void log_ru_sdr_metrics_in_stdout(const ru_generic_metrics& sdr_metrics);

  /// Log dummy RU metrics in STDOUT.
  void log_ru_dummy_metrics_in_stdout(const ru_dummy_metrics& dummy_metrics);

  /// Log OFH RU metrics in STDOUT.
  void log_ru_ofh_metrics_in_stdout(const ofh::metrics& ofh_metrics);

private:
  const std::chrono::nanoseconds symbol_duration;
  unsigned                       nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER;
  span<const pci_t>              pci_sector_map;
};

} // namespace srsran
