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

#include "srsran/srslog/logger.h"

namespace srsran {

/// Configuration of logging functionalities.
struct cu_up_unit_logger_config {
  srslog::basic_levels cu_level   = srslog::basic_levels::warning;
  srslog::basic_levels e1ap_level = srslog::basic_levels::warning;
  srslog::basic_levels gtpu_level = srslog::basic_levels::warning;
  srslog::basic_levels pdcp_level = srslog::basic_levels::warning;
  srslog::basic_levels f1u_level  = srslog::basic_levels::warning;
  srslog::basic_levels sdap_level = srslog::basic_levels::warning;
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Enable JSON generation for the E1AP Tx and Rx PDUs.
  bool e1ap_json_enabled = false;
};

} // namespace srsran
