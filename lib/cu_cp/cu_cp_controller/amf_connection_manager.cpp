/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amf_connection_manager.h"
#include "../cu_cp_impl_interface.h"
#include "../routine_managers/cu_cp_routine_manager.h"
#include "../routines/amf_connection_setup_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_manager::amf_connection_manager(cu_cp_routine_manager&    routine_manager_,
                                               const ngap_configuration& ngap_cfg_,
                                               ngap_connection_manager&  ngap_conn_mng_) :
  routine_manager(routine_manager_), ngap_cfg(ngap_cfg_), ngap_conn_mng(ngap_conn_mng_)
{
}

void amf_connection_manager::connect_to_amf(std::promise<bool>* completion_signal)
{
  // Schedules setup routine to be executed in sequence with other CU-CP procedures.
  routine_manager.schedule_async_task(
      launch_async([this, p = completion_signal](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Launch procedure to initiate AMF connection.
        CORO_AWAIT_VALUE(bool success, launch_async<amf_connection_setup_routine>(ngap_cfg, ngap_conn_mng));

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
