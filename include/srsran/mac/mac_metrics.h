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

#include "srsran/ran/pci.h"
#include "srsran/ran/slot_point.h"
#include <chrono>
#include <vector>

namespace srsran {

/// Metrics relative to a specific MAC cell.
struct mac_dl_cell_metric_report {
  struct latency_report {
    std::chrono::nanoseconds min;
    std::chrono::nanoseconds max;
    std::chrono::nanoseconds average;
    slot_point               max_slot;
  };
  /// Physical cell id.
  pci_t pci;
  /// Number of slots considered in this report.
  unsigned nof_slots;
  /// Slot duration.
  std::chrono::nanoseconds slot_duration;
  /// Description of the wall clock latency of the MAC at handling slot indications.
  latency_report wall_clock_latency;
  /// Description of the time spent by the MAC in user mode when handling slot indications.
  latency_report user_time;
  /// Description of the time spent by the MAC in kernel mode when handling slot indications.
  latency_report sys_time;
  /// \brief Description of the wall clock latency between the lower layers signalling a slot indication and the MAC
  /// starting to handle it.
  latency_report slot_ind_handle_latency;
  /// Number of voluntary context switches.
  unsigned count_voluntary_context_switches;
  /// Number of involuntary context switches.
  unsigned count_involuntary_context_switches;
};

struct mac_dl_metric_report {
  std::vector<mac_dl_cell_metric_report> cells;
};

/// \brief Structure holding the metrics of a MAC layer.
struct mac_metric_report {
  /// Metrics of the MAC DL.
  mac_dl_metric_report dl;
};

} // namespace srsran
