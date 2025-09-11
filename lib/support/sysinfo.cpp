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

#include "srsran/support/sysinfo.h"
#include <charconv>
#include <fstream>
#include <string>
#include <thread>

using namespace srsran;

bool srsran::check_cpu_governor(srslog::basic_logger& logger)
{
  static const char* filename_base = "/sys/devices/system/cpu/cpu";

  for (unsigned i = 0, e = std::thread::hardware_concurrency(); i != e; ++i) {
    std::string filename = fmt::format("{}{}{}", filename_base, i, "/cpufreq/scaling_governor");

    std::ifstream input(filename);
    if (input.fail()) {
      logger.warning("Could not check scaling governor. filename={} error={}", filename, ::strerror(errno));
      return false;
    }

    std::string gov;
    std::getline(input, gov);
    if (input.fail()) {
      logger.warning("Could not check scaling governor. filename={} error={}", filename, ::strerror(errno));
      return false;
    }

    if (gov == "performance") {
      logger.debug("CPU{} scaling governor is set to performance", i);
      continue;
    }
    logger.warning("CPU{} scaling governor is not set to performance, which may hinder performance. You can set it to "
                   "performance using the "
                   "\"srsran_performance\" script",
                   i);
  }

  return true;
}

bool srsran::check_drm_kms_polling(srslog::basic_logger& logger)
{
  static const char* filename = "/sys/module/drm_kms_helper/parameters/poll";

  std::ifstream input(filename);
  if (input.fail()) {
    logger.warning("Could not check DRM KMS polling. filename={} error={}", filename, ::strerror(errno));
    return false;
  }

  std::string polling;
  std::getline(input, polling);
  if (input.fail()) {
    logger.warning("Could not check DRM KMS polling. filename={} error={}", filename, ::strerror(errno));
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
