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

#include "srsran/srslog/logger.h"
#include <fstream>
#include <string>
#include <thread>

#pragma once

/// \brief Check whether the CPU scaling governor is set to performance.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs scaling governor information.
inline bool check_cpu_governor(srslog::basic_logger& logger)
{
  unsigned int n_cpus        = std::thread::hardware_concurrency();
  std::string  filename_base = "/sys/devices/system/cpu/cpu";
  for (unsigned int i = 0; i < n_cpus; ++i) {
    std::string   filename = filename_base + std::to_string(i) + "/cpufreq/scaling_governor";
    std::ifstream input(filename);
    if (input.fail()) {
      logger.warning("Could not check scaling governor. filename={} error=\"{}\"", filename, strerror(errno));
      return false;
    }
    std::string gov;
    std::getline(input, gov);
    if (input.fail()) {
      logger.warning("Could not check scaling governor. filename={} error=\"{}\"", filename, strerror(errno));
      return false;
    }
    if (gov == "performance") {
      logger.debug("CPU{} scaling governor is set to performance", i);
    } else {
      logger.warning(
          "CPU{} scaling governor is not set to performance, which may hinder performance. You can set it to "
          "performance using the "
          "\"srsran_performance\" script",
          i);
    }
  }
  return true;
}

/// \brief Check whether the DRM KMS polling is set.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs for the DRM KMS polling information.
inline bool check_drm_kms_polling(srslog::basic_logger& logger)
{
  std::string   filename = "/sys/module/drm_kms_helper/parameters/poll";
  std::ifstream input(filename);
  if (input.fail()) {
    logger.warning("Could not check DRM KMS polling. filename={} error=\"{}\"", filename, strerror(errno));
    return false;
  }
  std::string polling;
  std::getline(input, polling);
  if (input.fail()) {
    logger.warning("Could not check DRM KMS polling. filename={} error=\"{}\"", filename, strerror(errno));
    return false;
  }
  if (polling == "N") {
    logger.debug("DRM KMS polling is disabled");
  } else {
    logger.warning("DRM KMS polling is enabled, which may hinder performance. You can disable it using the "
                   "\"srsran_performance\" script");
  }
  return true;
}

/// \brief Configures cgroups.
///
/// \param[in] isol_cpus Set of CPUs to be isolated for the gNB app.
/// \param[in] os_cpus   Set of CPUs for other operating system processes.
/// \return True if we were able to configure cgroups through the sysfs.
bool configure_cgroups(const std::string& isol_cpus, const std::string& os_cpus);

/// \brief Removes cgroups created by the gNB app.
void cleanup_cgroups();
