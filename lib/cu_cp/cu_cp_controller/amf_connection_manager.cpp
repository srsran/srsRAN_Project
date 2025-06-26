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

#include "amf_connection_manager.h"
#include "../cu_cp_impl_interface.h"
#include "../routines/amf_connection_removal_routine.h"
#include "../routines/amf_connection_setup_routine.h"
#include "../routines/amf_reconnection_routine.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/support/synchronization/baton.h"
#include <chrono>
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_manager::amf_connection_manager(ngap_repository&                ngaps_,
                                               cu_cp_amf_reconnection_handler& cu_cp_notifier_,
                                               timer_manager&                  timers_,
                                               task_executor&                  cu_cp_exec_,
                                               common_task_scheduler&          common_task_sched_) :
  ngaps(ngaps_),
  cu_cp_notifier(cu_cp_notifier_),
  timers(timers_),
  cu_cp_exec(cu_cp_exec_),
  common_task_sched(common_task_sched_),
  logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void amf_connection_manager::connect_to_amf(std::promise<bool>* completion_signal)
{
  // Schedules setup routine to be executed in sequence with other CU-CP procedures.
  common_task_sched.schedule_async_task(
      launch_async([this, success = false, p = completion_signal](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Launch procedure to initiate AMF connection.
        CORO_AWAIT_VALUE(success, start_amf_connection_setup(ngaps, amfs_connected));

        // Signal through the promise the result of the connection setup.
        if (p != nullptr) {
          p->set_value(success);
        }

        CORO_RETURN();
      }));
}

async_task<void> amf_connection_manager::disconnect_amf()
{
  if (ngaps.get_ngaps().empty() or amfs_connected.empty()) {
    logger.error("No NGAP interface available to disconnect from AMF");
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  return start_amf_connection_removal(ngaps, amfs_connected);
}

void amf_connection_manager::handle_amf_connection_loss(amf_index_t amf_index)
{
  amfs_connected.erase(amf_index);
}

void amf_connection_manager::reconnect_to_amf(amf_index_t               amf_index,
                                              ue_manager*               ue_mng,
                                              std::chrono::milliseconds amf_reconnection_retry_time)
{
  if (ngaps.find_ngap(amf_index) == nullptr) {
    logger.debug("AMF index {} for reconnection not found", amf_index);
    return;
  }

  ngaps.get_ngap_task_scheduler().handle_amf_async_task(
      amf_index,
      launch_async([this, amf_index, success = bool{false}, ue_mng, amf_reconnection_retry_time](
                       coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        CORO_AWAIT_VALUE(success,
                         start_amf_reconnection(*ngaps.find_ngap(amf_index),
                                                timer_factory{timers, cu_cp_exec},
                                                amf_reconnection_retry_time));

        if (success) {
          // Update PLMN lookups in NGAP repository after successful reconnection.
          ngaps.update_plmn_lookup(amf_index);
          ue_mng->remove_blocked_plmns(ngaps.find_ngap(amf_index)->get_ngap_context().get_supported_plmns());
          amfs_connected.emplace(amf_index, true);
          // Notrify CU-CP about the successful reconnection.
          cu_cp_notifier.handle_amf_reconnection(amf_index);
        } else {
          logger.info("Failed to reconnect to AMF index {}", amf_index);
        }

        CORO_RETURN();
      }));
}

void amf_connection_manager::stop()
{
  if (stopped) {
    return;
  }

  baton               stop_baton;
  scoped_baton_sender signal_stop{stop_baton};

  // Stop and delete AMF connections.
  while (not cu_cp_exec.defer([this, signal_stop = std::move(signal_stop)]() mutable {
    common_task_sched.schedule_async_task(
        launch_async([this, signal_stop = std::move(signal_stop)](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          // Disconnect AMF connection.
          CORO_AWAIT(disconnect_amf());

          // AMF disconnection successfully finished.
          // Dispatch main async task loop destruction via defer so that the current coroutine ends successfully.
          while (not cu_cp_exec.defer([signal_stop = std::move(signal_stop)]() mutable { signal_stop.post(); })) {
            logger.warning("Unable to stop AMF Manager. Retrying...");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }

          CORO_RETURN();
        }));
  })) {
    logger.warning("Failed to dispatch AMF stop task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Wait for AMF stop to complete.
  stop_baton.wait();

  stopped = true;
}

bool amf_connection_manager::is_amf_connected(plmn_identity plmn) const
{
  amf_index_t amf_index = plmn_to_amf_index(plmn);
  if (amf_index == amf_index_t::invalid) {
    return false;
  }

  return is_amf_connected(amf_index);
}

bool amf_connection_manager::is_amf_connected(amf_index_t amf_index) const
{
  const auto& amf_connected = amfs_connected.find(amf_index);
  if (amf_connected == amfs_connected.end()) {
    return false;
  }

  return amf_connected->second.load(std::memory_order_relaxed);
}

void amf_connection_manager::handle_connection_setup_result(amf_index_t amf_index, bool success)
{
  // Update AMF connection handler state.
  amfs_connected.emplace(amf_index, success);
}

amf_index_t amf_connection_manager::plmn_to_amf_index(plmn_identity plmn) const
{
  for (const auto& [amf_index, ngap] : ngaps.get_ngaps()) {
    for (auto& supported_plmn : ngap->get_ngap_context().get_supported_plmns()) {
      if (plmn == supported_plmn) {
        return amf_index;
      }
    }
  }

  return amf_index_t::invalid;
}
