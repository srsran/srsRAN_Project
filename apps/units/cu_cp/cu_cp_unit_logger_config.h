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

/// Configuration of logging functionalities.
struct cu_cp_unit_logger_config {
  srslog::basic_levels cu_level   = srslog::basic_levels::warning;
  srslog::basic_levels f1ap_level = srslog::basic_levels::warning;
  srslog::basic_levels pdcp_level = srslog::basic_levels::warning;
  srslog::basic_levels rrc_level  = srslog::basic_levels::warning;
  srslog::basic_levels ngap_level = srslog::basic_levels::warning;
  srslog::basic_levels sec_level  = srslog::basic_levels::warning;
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Enable JSON generation for the F1AP Tx and Rx PDUs.
  bool f1ap_json_enabled = false;
};

} // namespace srsran
