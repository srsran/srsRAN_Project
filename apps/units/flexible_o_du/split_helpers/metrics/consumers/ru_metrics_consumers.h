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
  ru_metrics_consumer_json(srslog::log_channel& log_chan_, span<const pci_t> pci_sector_map_) :
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

/// Logger consumer for the O-RU metrics.
class ru_metrics_consumer_log
{
public:
  ru_metrics_consumer_log(srslog::log_channel& log_chan_, span<const pci_t> pci_sector_map_) :
    log_chan(log_chan_), verbose(true), pci_sector_map(pci_sector_map_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // Handles the O-RU metrics.
  void handle_metric(const ru_metrics& metric);

private:
  srslog::log_channel& log_chan;
  const bool           verbose;
  span<const pci_t>    pci_sector_map;
};

/// STDOUT handler for the O-RU metrics.
class ru_metrics_handler_stdout
{
  /// Maximum number of metric lines in STDOUT without a header, ie, print header every
  /// MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER lines.
  static constexpr unsigned MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER = 10;

public:
  explicit ru_metrics_handler_stdout(span<const pci_t> pci_sector_map_) : pci_sector_map(pci_sector_map_) {}

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
  unsigned          nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER;
  span<const pci_t> pci_sector_map;
};

} // namespace srsran
