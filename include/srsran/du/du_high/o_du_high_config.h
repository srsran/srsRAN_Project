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
#include "srsran/e2/e2ap_configuration.h"

namespace srsran {

class e2_du_metrics_interface;
class e2_connection_client;

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
  /// E2AP configuration.
  e2ap_configuration e2ap_config;
};

/// O-RAN DU high sector dependencies. Contains the dependencies of one sector.
struct o_du_high_sector_dependencies {
  fapi::slot_message_gateway*       gateway           = nullptr;
  fapi::slot_last_message_notifier* last_msg_notifier = nullptr;
  /// FAPI message bufferer executor.
  std::optional<task_executor*> fapi_executor;
};

/// O-RAN DU high dependencies.
struct o_du_high_dependencies {
  std::vector<o_du_high_sector_dependencies> sectors;
  du_high_dependencies                       du_hi;
  e2_connection_client*                      e2_client          = nullptr;
  e2_du_metrics_interface*                   e2_du_metric_iface = nullptr;
};

} // namespace srs_du
} // namespace srsran
