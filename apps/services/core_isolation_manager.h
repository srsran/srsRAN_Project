/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
