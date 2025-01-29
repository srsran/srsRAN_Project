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
