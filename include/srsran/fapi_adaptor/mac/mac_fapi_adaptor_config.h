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

#include "srsran/fapi_adaptor/precoding_matrix_mapper.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <memory>

namespace srsran {

class task_executor;

namespace fapi {
class slot_message_gateway;
class slot_last_message_notifier;
} // namespace fapi

namespace fapi_adaptor {

/// MAC&ndash;FAPI sector adaptor configuration.
struct mac_fapi_sector_adaptor_config {
  /// Base station sector identifier.
  unsigned sector_id;
  /// Cell number of resource blocks.
  unsigned cell_nof_prbs;
  /// Subcarrier spacing as per TS38.331 Section 6.2.2.
  subcarrier_spacing scs;
  /// Log level.
  srslog::basic_levels log_level;
  /// Number of slots the L2 is ahead the L1.
  unsigned l2_nof_slots_ahead;
};

/// MAC&ndash;FAPI sector adaptor dependencies.
struct mac_fapi_sector_adaptor_dependencies {
  /// Slot-specific FAPI message gateway.
  fapi::slot_message_gateway* gateway;
  /// Slot-specific last message notifier.
  fapi::slot_last_message_notifier* last_msg_notifier;
  /// Precoding matrix mapper.
  std::unique_ptr<precoding_matrix_mapper> pm_mapper;
  /// UCI Part2 mapper.
  std::unique_ptr<uci_part2_correspondence_mapper> part2_mapper;
  /// FAPI message bufferer task executor.
  std::optional<task_executor*> bufferer_task_executor;
};

/// MAC&ndash;FAPI adaptor configuration.
struct mac_fapi_adaptor_config {
  std::vector<mac_fapi_sector_adaptor_config> sectors;
};

/// MAC&ndash;FAPI adaptor dependencies.
struct mac_fapi_adaptor_dependencies {
  std::vector<mac_fapi_sector_adaptor_dependencies> sectors;
};

} // namespace fapi_adaptor
} // namespace srsran
