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

#include "srsran/cu_up/cu_up_manager.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"

namespace srsran::srs_cu_up {

/// Adapter between E1AP and CU-UP manager
class e1ap_cu_up_manager_adapter : public e1ap_cu_up_manager_notifier
{
public:
  e1ap_cu_up_manager_adapter() : logger(srslog::fetch_basic_logger("CU-UP-E1")) {}

  void connect_cu_up_manager(cu_up_manager_e1ap_interface& cu_up_handler_) { cu_up_handler = &cu_up_handler_; }

  void disconnect() { cu_up_handler = nullptr; }

  e1ap_bearer_context_setup_response
  on_bearer_context_setup_request_received(const e1ap_bearer_context_setup_request& msg) override
  {
    if (cu_up_handler == nullptr) {
      logger.warning("Could not handle context setup command, no CU-UP handler present");
      return {}; // return failure to setup bearer context
    }
    return cu_up_handler->handle_bearer_context_setup_request(msg);
  }

  async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request_received(const e1ap_bearer_context_modification_request& msg) override
  {
    if (cu_up_handler == nullptr) {
      logger.warning("Could not handle context modification command, no CU-UP handler present. ue={}", msg.ue_index);
      return {}; // return failure to modify bearer context
    }
    return cu_up_handler->handle_bearer_context_modification_request(msg);
  }

  async_task<void> on_bearer_context_release_command_received(const e1ap_bearer_context_release_command& msg) override
  {
    if (cu_up_handler == nullptr) {
      logger.warning("Could not handle context release command, no CU-UP handler present. ue={}", msg.ue_index);
      return launch_async([](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_RETURN();
      });
    }
    return cu_up_handler->handle_bearer_context_release_command(msg);
  }

  void on_schedule_ue_async_task(srs_cu_up::ue_index_t ue_index, async_task<void> task) override
  {
    if (cu_up_handler == nullptr) {
      logger.error("Could not schedule UE task, no CU-UP handler present. ue={}", ue_index);
      return;
    }
    cu_up_handler->schedule_ue_async_task(ue_index, std::move(task));
  }

private:
  cu_up_manager_e1ap_interface* cu_up_handler = nullptr;
  srslog::basic_logger&         logger;
};

} // namespace srsran::srs_cu_up
