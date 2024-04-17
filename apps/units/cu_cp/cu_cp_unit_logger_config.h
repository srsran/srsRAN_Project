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
struct cu_cp_unit_logger_config {
  std::string cu_level   = "warning";
  std::string f1ap_level = "warning";
  std::string pdcp_level = "warning";
  std::string rrc_level  = "warning";
  std::string ngap_level = "warning";
  std::string sec_level  = "warning";
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Enable JSON generation for the F1AP Tx and Rx PDUs.
  bool f1ap_json_enabled = false;
};

} // namespace srsran
