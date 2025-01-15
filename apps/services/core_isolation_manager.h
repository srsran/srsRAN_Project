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

#include "srsran/support/sysinfo.h"

namespace srsran {
namespace app_services {

/// \brief Core isolation manager.
///
/// Isolates CPU cores used by the application using cgroups.
class core_isolation_manager
{
  bool is_core_isolation_enabled = false;

public:
  ~core_isolation_manager()
  {
    // Clean the cgroups if they were configured.
    if (is_core_isolation_enabled) {
      cleanup_cgroups();
    }
  }

  /// Configure cgroups to isolate the cores from the given isolated_cpus.
  bool isolate_cores(const os_sched_affinity_bitmask& isolated_cpus)
  {
    is_core_isolation_enabled = configure_cgroups(isolated_cpus);

    return is_core_isolation_enabled;
  }
};

} // namespace app_services
} // namespace srsran
