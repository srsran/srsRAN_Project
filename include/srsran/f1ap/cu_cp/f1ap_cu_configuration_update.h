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

#include "srsran/f1ap/common/interface_management.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// Common type F1AP GNB-CU Configuration Update, see TS 38.473, section 9.2.1.10.
struct f1ap_gnb_cu_configuration_update {
  std::vector<f1ap_cell_to_activate>   cells_to_be_activated_list;
  std::vector<f1ap_cell_to_deactivate> cells_to_be_deactivated_list;

  std::string gnb_cu_name;

  // TODO: Add missing optional fields.
};

/// Common type F1AP GNB-CU Configuration Update Response. Success is true for a GNB-CU Configuration Update Acknowledge
/// (see TS 38.473, section 9.2.1.11), GNB-CU Configuration Update Failure (see TS 38.473, section 9.2.1.12) otherwise.
struct f1ap_gnb_cu_configuration_update_response {
  bool success = false;

  // GNB CU Configuration Update Acknowledge.
  std::vector<f1ap_cell_failed_to_activate> cells_failed_to_be_activated_list;

  // TODO: Add missing optional fields.

  // GNB CU Configuration Update Failure.
  std::optional<f1ap_cause_t> cause;

  // TODO: Add missing optional fields.
};

} // namespace srs_cu_cp
} // namespace srsran
