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

#include "du_high/du_high_config.h"
#include "e2/o_du_high_e2_config.h"
#include "fapi/fapi_config.h"

namespace srsran {

/// O-DU high unit configuration.
struct o_du_high_unit_config {
  du_high_parsed_config du_high_cfg;
  o_du_high_e2_config   e2_cfg;
  fapi_unit_config      fapi_cfg;
};

} // namespace srsran
