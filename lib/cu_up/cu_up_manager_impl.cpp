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

#include "cu_up_manager_impl.h"
#include "cu_up_manager_helpers.h"
#include "routines/cu_up_bearer_context_modification_routine.h"
#include "routines/cu_up_test_mode_routines.h"
#include "srsran/support/async/execute_on_blocking.h"

using namespace srsran;
using namespace srs_cu_up;

/// Helper functions
static ue_manager_config generate_ue_manager_config(const n3_interface_config&    n3_config,
                                                    const cu_up_test_mode_config& test_mode_config)
{
  return {n3_config, test_mode_config};
}

static ue_manager_dependencies generate_ue_manager_dependencies(const cu_up_manager_impl_dependencies& dependencies,
                                                                srslog::basic_logger&                  logger)
{
  return {dependencies.e1ap,
          dependencies.timers,
          dependencies.f1u_gateway,
          dependencies.ngu_session_mngr,
          dependencies.ngu_demux,
          dependencies.n3_teid_allocator,
          dependencies.f1u_teid_allocator,
          dependencies.exec_mapper,
          dependencies.gtpu_pcap,
          logger};
}

cu_up_manager_impl::cu_up_manager_impl(const cu_up_manager_impl_config&       config,
                                       const cu_up_manager_impl_dependencies& dependencies) :
  qos(config.qos),
  n3_cfg(config.n3_cfg),
  test_mode_cfg(config.test_mode_cfg),
  ngu_demux(dependencies.ngu_demux),
  exec_mapper(dependencies.exec_mapper),
  timers(dependencies.timers),
  cu_up_task_scheduler(dependencies.cu_up_task_scheduler)
{
  /// > Create UE manager
  ue_mng = std::make_unique<ue_manager>(generate_ue_manager_config(n3_cfg, test_mode_cfg),
                                        generate_ue_manager_dependencies(dependencies, logger));
}

async_task<void> cu_up_manager_impl::stop()
{
  return ue_mng->stop();
}

void cu_up_manager_impl::schedule_cu_up_async_task(async_task<void> task)
{
  cu_up_task_scheduler.schedule(std::move(task));
}

void cu_up_manager_impl::schedule_ue_async_task(ue_index_t ue_index, async_task<void> task)
{
  ue_mng->schedule_ue_async_task(ue_index, std::move(task));
}

e1ap_bearer_context_setup_response
cu_up_manager_impl::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg)
{
  e1ap_bearer_context_setup_response response = {};
  response.ue_index                           = INVALID_UE_INDEX;
  response.success                            = false;

  // 1. Create new UE context
  ue_context_cfg ue_cfg = {};
  fill_sec_as_config(ue_cfg.security_info, msg.security_info);
  ue_cfg.activity_level                   = msg.activity_notif_level;
  ue_cfg.ue_inactivity_timeout            = msg.ue_inactivity_timer;
  ue_cfg.qos                              = qos;
  ue_cfg.ue_dl_aggregate_maximum_bit_rate = msg.ue_dl_aggregate_maximum_bit_rate;
  ue_context* ue_ctxt                     = ue_mng->add_ue(ue_cfg);
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return response;
  }
  ue_ctxt->get_logger().log_info("UE created");

  // 2. Handle bearer context setup request
  for (const auto& pdu_session : msg.pdu_session_res_to_setup_list) {
    pdu_session_setup_result result = ue_ctxt->setup_pdu_session(pdu_session);
    if (result.success) {
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, result);
    } else {
      e1ap_pdu_session_resource_failed_item res_failed_item;

      res_failed_item.pdu_session_id = result.pdu_session_id;
      res_failed_item.cause          = result.cause;

      response.pdu_session_resource_failed_list.emplace(result.pdu_session_id, res_failed_item);
    }
  }

  // 3. Create response
  response.ue_index = ue_ctxt->get_index();
  response.success  = true;
  return response;
}

async_task<e1ap_bearer_context_modification_response>
cu_up_manager_impl::handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return launch_async([](coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) {
      CORO_BEGIN(ctx);
      e1ap_bearer_context_modification_response res;
      res.success = false;
      CORO_RETURN(res);
    });
  }
  return execute_and_continue_on_blocking(ue_ctxt->ue_exec_mapper->ctrl_executor(),
                                          exec_mapper.ctrl_executor(),
                                          timers,
                                          launch_async<cu_up_bearer_context_modification_routine>(*ue_ctxt, msg));
}

async_task<void>
cu_up_manager_impl::handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("ue={}: Discarding E1 Bearer Context Release Command. UE context not found",
                 fmt::underlying(msg.ue_index));
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  ue_ctxt->get_logger().log_debug("Received E1 Bearer Context Release Command");

  return ue_mng->remove_ue(msg.ue_index);
}

async_task<void> cu_up_manager_impl::enable_test_mode()
{
  return launch_async<cu_up_enable_test_mode_routine>(test_mode_cfg, *this, ngu_demux);
}

async_task<void> cu_up_manager_impl::disable_test_mode()
{
  return launch_async<cu_up_disable_test_mode_routine>(*this, *ue_mng);
}

void cu_up_manager_impl::trigger_enable_test_mode()
{
  if (test_mode_cfg.attach_detach_period.count() != 0) {
    test_mode_ue_timer = timers.create_unique_timer(exec_mapper.ctrl_executor());
    test_mode_ue_timer.set(test_mode_cfg.attach_detach_period,
                           [this](timer_id_t /**/) { schedule_cu_up_async_task(enable_test_mode()); });
    test_mode_ue_timer.run();
  }
}

void cu_up_manager_impl::trigger_disable_test_mode()
{
  if (test_mode_cfg.attach_detach_period.count() != 0) {
    test_mode_ue_timer = timers.create_unique_timer(exec_mapper.ctrl_executor());
    test_mode_ue_timer.set(test_mode_cfg.attach_detach_period,
                           [this](timer_id_t /**/) { schedule_cu_up_async_task(disable_test_mode()); });
    test_mode_ue_timer.run();
  }
}
