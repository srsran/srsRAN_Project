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

#include "amf_connection_manager.h"
#include "../cu_cp_impl_interface.h"
#include "../routine_managers/cu_cp_routine_manager.h"
#include "../routines/amf_connection_setup_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_manager::amf_connection_manager(cu_cp_routine_manager&       routine_manager_,
                                               const ngap_configuration&    ngap_cfg_,
                                               cu_cp_ngap_control_notifier& ngap_ctrl_notifier_) :
  routine_manager(routine_manager_), ngap_cfg(ngap_cfg_), ngap_ctrl_notifier(ngap_ctrl_notifier_)
{
}

void amf_connection_manager::connect_to_amf(std::promise<bool>* completion_signal)
{
  // Schedules setup routine to be executed in sequence with other CU-CP procedures.
  routine_manager.schedule_async_task(
      launch_async([this, p = completion_signal](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Launch procedure to initiate AMF connection.
        CORO_AWAIT_VALUE(bool success, launch_async<amf_connection_setup_routine>(ngap_cfg, ngap_ctrl_notifier));

        // Handle result of NG setup.
        handle_connection_setup_result(success);

        // Signal through the promise the result of the connection setup.
        if (p != nullptr) {
          p->set_value(success);
        }

        CORO_RETURN();
      }));
}

void amf_connection_manager::handle_connection_setup_result(bool success)
{
  // Update AMF connection handler state.
  amf_connected = success;
}
