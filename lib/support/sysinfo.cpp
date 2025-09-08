/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
