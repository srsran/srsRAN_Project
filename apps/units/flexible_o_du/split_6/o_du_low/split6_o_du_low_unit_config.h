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

#pragma once

#include "apps/units/flexible_o_du/o_du_low/du_low_config.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config.h"
#include <variant>

namespace srsran {

/// O-RAN DU low Split 6 unit configuration.
struct split6_o_du_low_unit_config {
  unsigned du_report_period = 1000;
  /// Start jitter in milliseconds.
  unsigned start_time_jitter_ms = 0;
  /// FAPI log level.
  srslog::basic_levels fapi_level = srslog::basic_levels::warning;
  /// DU low configuration.
  du_low_unit_config du_low_cfg;
  /// Radio Unit configuration.
  std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config> ru_cfg = {ru_sdr_unit_config{}};
};

} // namespace srsran
