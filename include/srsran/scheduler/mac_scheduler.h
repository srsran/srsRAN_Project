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

#include "scheduler_configurator.h"
#include "scheduler_dl_buffer_state_indication_handler.h"
#include "scheduler_feedback_handler.h"
#include "scheduler_paging_handler.h"
#include "scheduler_slot_handler.h"

namespace srsran {

class mac_scheduler : public scheduler_configurator,
                      public scheduler_ue_configurator,
                      public scheduler_feedback_handler,
                      public scheduler_slot_handler,
                      public scheduler_dl_buffer_state_indication_handler,
                      public scheduler_paging_handler
{
public:
  virtual ~mac_scheduler() = default;
};

} // namespace srsran
