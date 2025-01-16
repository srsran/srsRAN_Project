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

#include "srsran/support/units.h"

namespace srsran {

/// Notification that the SIB1 PDU content was updated.
struct sib1_pdu_update_request {
  du_cell_index_t cell_index;
  unsigned        pdu_version;
  units::bytes    payload_size;
};

/// Interface used to notify new SIB1 or SI message updates to the scheduler.
class scheduler_sys_info_handler
{
public:
  virtual ~scheduler_sys_info_handler() = default;

  /// Handle SIB1 PDU content update.
  virtual void handle_sib1_update_request(const sib1_pdu_update_request& req) = 0;
};

} // namespace srsran
