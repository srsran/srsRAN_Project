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

#include "srsran/du/du_high/du_high_configuration.h"

namespace srsran {

namespace fapi {
class slot_message_gateway;
class slot_last_message_notifier;
} // namespace fapi

namespace srs_du {

/// FAPI configuration for the ORAN DU high.
struct o_du_high_fapi_config {
  srslog::basic_levels log_level;
  unsigned             l2_nof_slots_ahead;
};

/// Base O-DU high configuration.
struct o_du_high_config {
  /// Configuration of the DU-high that comprises the MAC, RLC and F1 layers.
  srs_du::du_high_configuration du_hi;
  /// ORAN DU high FAPI configuration.
  o_du_high_fapi_config fapi;
};

/// ORAN DU high sector dependencies. Contains the dependencies of one sector.
struct o_du_high_sector_dependencies {
  fapi::slot_message_gateway*       gateway           = nullptr;
  fapi::slot_last_message_notifier* last_msg_notifier = nullptr;
  /// FAPI buffered executor.
  std::optional<task_executor*> fapi_executor;
};

/// ORAN DU high dependencies.
struct o_du_high_dependencies {
  std::vector<o_du_high_sector_dependencies> sectors;
};

} // namespace srs_du
} // namespace srsran
