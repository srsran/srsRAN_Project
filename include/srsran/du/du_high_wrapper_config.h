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

#include "srsran/du_high/du_high_configuration.h"

namespace srsran {

namespace fapi {

class slot_message_gateway;
class slot_last_message_notifier;

} // namespace fapi

/// FAPI configuration for the DU high wrapper.
struct du_high_wrapper_fapi_config {
  srslog::basic_levels          log_level;
  unsigned                      l2_nof_slots_ahead;
  std::optional<task_executor*> executor;
};

/// Base DU high wrapper configuration.
struct du_high_wrapper_config {
  /// Configuration of the DU-high that comprises the MAC, RLC and F1 layers.
  srs_du::du_high_configuration du_hi;
  /// FAPI custom split 6 configuration.
  du_high_wrapper_fapi_config fapi;
};

/// DU high wrapper sector dependencies. Contains the dependencies of one sector.
struct du_high_wrapper_sector_dependencies {
  fapi::slot_message_gateway*       gateway           = nullptr;
  fapi::slot_last_message_notifier* last_msg_notifier = nullptr;
};

/// DU high wrapper dependencies.
struct du_high_wrapper_dependencies {
  std::vector<du_high_wrapper_sector_dependencies> sectors;
};

} // namespace srsran
