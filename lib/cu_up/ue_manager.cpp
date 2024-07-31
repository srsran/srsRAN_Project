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

#include "ue_manager.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_cu_up;

ue_manager::ue_manager(network_interface_config&                   net_config_,
                       n3_interface_config&                        n3_config_,
                       cu_up_test_mode_config&                     test_mode_config_,
                       e1ap_control_message_handler&               e1ap_,
                       timer_manager&                              timers_,
                       f1u_cu_up_gateway&                          f1u_gw_,
                       gtpu_tunnel_common_tx_upper_layer_notifier& gtpu_tx_notifier_,
                       gtpu_demux_ctrl&                            gtpu_rx_demux_,
                       gtpu_teid_pool&                             n3_teid_allocator_,
                       gtpu_teid_pool&                             f1u_teid_allocator_,
                       cu_up_executor_pool&                        exec_pool_,
                       task_executor&                              ctrl_executor_,
                       dlt_pcap&                                   gtpu_pcap_,
                       srslog::basic_logger&                       logger_) :
  net_config(net_config_),
  n3_config(n3_config_),
  test_mode_config(test_mode_config_),
  e1ap(e1ap_),
  f1u_gw(f1u_gw_),
  gtpu_tx_notifier(gtpu_tx_notifier_),
  gtpu_rx_demux(gtpu_rx_demux_),
  n3_teid_allocator(n3_teid_allocator_),
  f1u_teid_allocator(f1u_teid_allocator_),
  exec_pool(exec_pool_),
  ctrl_executor(ctrl_executor_),
  gtpu_pcap(gtpu_pcap_),
  timers(timers_),
  logger(logger_)
{
  // Initialize a ue task schedulers for all UE indexes.
  for (size_t i = 0; i < MAX_NOF_UES; ++i) {
    ue_task_schedulers.emplace(i, UE_TASK_QUEUE_SIZE);
  }
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.find(ue_index) != ue_db.end() ? ue_db[ue_index].get() : nullptr;
}

ue_context* ue_manager::add_ue(const ue_context_cfg& ue_cfg)
{
  if (ue_db.size() >= MAX_NOF_UES) {
    logger.error("Can't add new UE. Max number of UEs reached.");
    return nullptr;
  }

  ue_index_t new_idx = get_next_ue_index();
  if (new_idx == INVALID_UE_INDEX) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE executors
  // TODO, these should be created within the same function, so that UL, DL and CTRL executors
  // can point to the same executor.
  std::unique_ptr<ue_executor_mapper> ue_exec_mapper = exec_pool.create_ue_executor_mapper();

  // Create executor-specific timer factories
  timer_factory ue_dl_timer_factory   = {timers, ue_exec_mapper->dl_pdu_executor()};
  timer_factory ue_ul_timer_factory   = {timers, ue_exec_mapper->ul_pdu_executor()};
  timer_factory ue_ctrl_timer_factory = {timers, ue_exec_mapper->ctrl_executor()};

  // Create UE object
  std::unique_ptr<ue_context> new_ctx = std::make_unique<ue_context>(new_idx,
                                                                     ue_cfg,
                                                                     e1ap,
                                                                     net_config,
                                                                     n3_config,
                                                                     test_mode_config,
                                                                     std::move(ue_exec_mapper),
                                                                     ue_task_schedulers[new_idx],
                                                                     ue_dl_timer_factory,
                                                                     ue_ul_timer_factory,
                                                                     ue_ctrl_timer_factory,
                                                                     f1u_gw,
                                                                     n3_teid_allocator,
                                                                     f1u_teid_allocator,
                                                                     gtpu_tx_notifier,
                                                                     gtpu_rx_demux,
                                                                     gtpu_pcap);

  // add to DB
  ue_db.emplace(new_idx, std::move(new_ctx));
  return ue_db[new_idx].get();
}

async_task<void> ue_manager::remove_ue(ue_index_t ue_index)
{
  logger.debug("ue={}: Scheduling UE deletion", ue_index);
  srsran_assert(ue_db.find(ue_index) != ue_db.end(), "Remove UE called for nonexistent ue_index={}", ue_index);

  // Move UE context out from ue_db and erase the slot (from CU-UP shared ctrl executor)
  std::unique_ptr<ue_context> ue_ctxt = std::move(ue_db[ue_index]);
  ue_db.erase(ue_index);

  // Dispatch the stopping and deletion of the UE context to UE-specific ctrl executor
  return launch_async([this, ue_ctxt = std::move(ue_ctxt), ue_index](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Dispatch execution context switch.
    CORO_AWAIT(execute_on_blocking(ue_ctxt->ue_exec_mapper->ctrl_executor()));

    // Stop and delete
    CORO_AWAIT(ue_ctxt->stop());
    ue_ctxt.reset();
    logger.info("ue={}: UE removed", ue_index);

    // Continuation in the original executor.
    CORO_AWAIT(execute_on_blocking(ctrl_executor));

    CORO_RETURN();
  });
}

ue_index_t ue_manager::get_next_ue_index()
{
  // Search unallocated UE index
  for (int i = 0; i < MAX_NOF_UES; i++) {
    if (ue_db.find(static_cast<ue_index_t>(i)) == ue_db.end()) {
      return int_to_ue_index(i);
      break;
    }
  }
  return INVALID_UE_INDEX;
}

void ue_manager::schedule_ue_async_task(ue_index_t ue_index, async_task<void> task)
{
  ue_context* ue_ctx = find_ue(ue_index);
  if (ue_ctx == nullptr) {
    logger.error("Cannot schedule UE task, could not find UE. ue_index={}", ue_index);
    return;
  }
  ue_ctx->task_sched.schedule(std::move(task));
}
