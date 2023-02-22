/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

/// \brief Adapter interface between MAC and scheduler that allows the srsGNB MAC to configure different scheduler
/// implementations.
/// Note: This adapter interface makes scheduler configuration methods awaitable.
class mac_scheduler_configurator
{
public:
  virtual ~mac_scheduler_configurator() = default;

  virtual void add_cell(const mac_cell_creation_request& msg) = 0;

  virtual void remove_cell(du_cell_index_t cell_index) = 0;

  virtual async_task<bool> handle_ue_creation_request(const mac_ue_create_request_message& msg) = 0;

  virtual async_task<bool> handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) = 0;

  /// \brief Removes UE from MAC scheduler in an asynchronous manner.
  /// The scheduler shouldn't allocate more grants directed at the UE being removed after this procedure is complete.
  virtual async_task<bool> handle_ue_removal_request(const mac_ue_delete_request_message& msg) = 0;

  virtual sched_configuration_notifier& get_sched_notifier() = 0;
};

} // namespace srsran
