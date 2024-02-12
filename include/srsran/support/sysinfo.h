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

#include "srsran/srslog/logger.h"

namespace srsran {

struct os_sched_affinity_bitmask;

/// \brief Check whether the CPU scaling governor is set to performance.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs scaling governor information.
bool check_cpu_governor(srslog::basic_logger& logger);

/// \brief Check whether the DRM KMS polling is set.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs for the DRM KMS polling information.
bool check_drm_kms_polling(srslog::basic_logger& logger);

/// \brief Configures cgroups.
///
/// \param[in] isol_cpus Set of CPUs to be isolated for the gNB app.
/// \return True if we were able to configure cgroups through the sysfs.
bool configure_cgroups(const srsran::os_sched_affinity_bitmask& isol_cpus);

/// \brief Removes cgroups created by the gNB app.
void cleanup_cgroups();

} // namespace srsran
