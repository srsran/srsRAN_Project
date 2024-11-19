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

#include "srsran/du/du_high/du_high_configuration.h"

namespace srsran {

namespace fapi {
class slot_message_gateway;
class slot_last_message_notifier;
} // namespace fapi

namespace srs_du {

/// FAPI configuration for the O-RAN DU high.
struct o_du_high_fapi_config {
  srslog::basic_levels log_level;
  unsigned             l2_nof_slots_ahead;
};

/// Base O-DU high configuration.
struct o_du_high_config {
  /// Configuration of the DU-high that comprises the MAC, RLC and F1 layers.
  srs_du::du_high_configuration du_hi;
  /// O-RAN DU high FAPI configuration.
  o_du_high_fapi_config fapi;
};

/// O-RAN DU high sector dependencies. Contains the dependencies of one sector.
struct o_du_high_sector_dependencies {
  fapi::slot_message_gateway*       gateway           = nullptr;
  fapi::slot_last_message_notifier* last_msg_notifier = nullptr;
  /// FAPI buffered executor.
  std::optional<task_executor*> fapi_executor;
};

/// O-RAN DU high dependencies.
struct o_du_high_dependencies {
  std::vector<o_du_high_sector_dependencies> sectors;
};

} // namespace srs_du
} // namespace srsran
