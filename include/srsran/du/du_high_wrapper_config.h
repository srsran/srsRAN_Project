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

#include "srsran/du_high/du_high_configuration.h"

namespace srsran {

namespace fapi {

class slot_message_gateway;
class slot_last_message_notifier;

} // namespace fapi

/// FAPI configuration for the DU high wrapper.
struct du_high_wrapper_fapi_config {
  std::string              log_level;
  unsigned                 sector;
  unsigned                 l2_nof_slots_ahead;
  optional<task_executor*> executor;
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
