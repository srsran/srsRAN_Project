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
#include "srsran/cu_cp/cu_cp_configuration.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_manager::amf_connection_manager(cu_cp_routine_manager&     routine_manager_,
                                               const cu_cp_configuration& cu_cp_cfg_,
                                               ngap_connection_manager&   ngap_conn_mng_) :
  routine_manager(routine_manager_), cu_cp_cfg(cu_cp_cfg_), ngap_conn_mng(ngap_conn_mng_)
{
}

void amf_connection_manager::connect_to_amf(std::promise<bool>* completion_signal)
{
  // Schedules setup routine to be executed in sequence with other CU-CP procedures.
  routine_manager.schedule_async_task(
      launch_async([this, p = completion_signal](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Launch procedure to initiate AMF connection.
        CORO_AWAIT_VALUE(bool success, launch_async<amf_connection_setup_routine>(cu_cp_cfg, ngap_conn_mng));

        // Handle result of NG setup.
        handle_connection_setup_result(success);

        // Signal through the promise the result of the connection setup.
        if (p != nullptr) {
          p->set_value(success);
        }

        CORO_RETURN();
      }));
}

async_task<void> amf_connection_manager::stop()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Run NG Removal procedure.
    // TODO

    // Launch procedure to remove AMF connection.
    CORO_AWAIT(ngap_conn_mng.handle_amf_disconnection_request());

    // Update AMF connection handler state.
    amf_connected = false;

    CORO_RETURN();
  });
}

void amf_connection_manager::handle_connection_setup_result(bool success)
{
  // Update AMF connection handler state.
  amf_connected = success;
}
