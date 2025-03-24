/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/du_types.h"
#include "srsran/support/units.h"

namespace srsran {

/// Identifier for the version of the system information scheduling information.
using si_version_type = unsigned;

/// Identifier of a SI message.
using si_message_index_type = unsigned;

/// Notification that the SIB1 PDU content was updated.
struct si_scheduling_update_request {
  /// Cell index specific to the update of the SI scheduling.
  du_cell_index_t cell_index;
  /// SI version number. Monotocally increasing with each update.
  si_version_type version;
  /// SIB1 payload length.
  units::bytes sib1_len;
};

/// Interface used to notify new SIB1 or SI message updates to the scheduler.
class scheduler_sys_info_handler
{
public:
  virtual ~scheduler_sys_info_handler() = default;

  /// Handle cell system information scheduling update.
  virtual void handle_si_update_request(const si_scheduling_update_request& req) = 0;
};

} // namespace srsran
