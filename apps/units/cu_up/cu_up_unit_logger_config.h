/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsran {

/// Configuration of logging functionalities.
struct cu_up_unit_logger_config {
  std::string cu_level   = "warning";
  std::string gtpu_level = "warning";
  std::string pdcp_level = "warning";
  std::string f1u_level  = "warning";
  std::string sdap_level = "warning";
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
};

} // namespace srsran
