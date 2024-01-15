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

#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"

namespace srsran {

/// DL buffer state for a given RLC bearer.
struct dl_buffer_state_indication_message {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  unsigned      bs;
};

/// Scheduler interface to push DL buffer state updates for a given RLC bearer.
class scheduler_dl_buffer_state_indication_handler
{
public:
  virtual ~scheduler_dl_buffer_state_indication_handler() = default;

  /// Forward DL buffer state update to scheduler.
  virtual void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) = 0;
};

} // namespace srsran
