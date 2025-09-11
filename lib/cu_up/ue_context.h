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

#pragma once

#include "cu_up_ue_logger.h"
#include "ngu_session_manager.h"
#include "pdu_session_manager.h"
#include "pdu_session_manager_impl.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/support/async/execute_on_blocking.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include <map>
#include <utility>

namespace srsran::srs_cu_up {

constexpr uint16_t UE_TASK_QUEUE_SIZE = 1024;

/// \brief UE context setup configuration
struct ue_context_cfg {
  security::sec_as_config                          security_info;
  activity_notification_level_t                    activity_level;
  std::optional<std::chrono::seconds>              ue_inactivity_timeout;
  std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos;
  uint64_t                                         ue_dl_aggregate_maximum_bit_rate;
};

/// \brief Context for a UE within the CU-UP with storage for all active PDU sessions.
class ue_context : public pdu_session_manager_ctrl
{
public:
  ue_context(ue_index_t                          index_,
             ue_context_cfg                      cfg_,
             e1ap_interface&                     e1ap_,
             const n3_interface_config&          n3_config_,
             const cu_up_test_mode_config&       test_mode_config_,
             std::unique_ptr<ue_executor_mapper> ue_exec_mapper_,
             fifo_async_task_scheduler&          task_sched_,
             timer_factory                       ue_dl_timer_factory_,
             timer_factory                       ue_ul_timer_factory_,
             timer_factory                       ue_ctrl_timer_factory_,
             f1u_cu_up_gateway&                  f1u_gw_,
             ngu_session_manager&                ngu_session_mngr_,
             gtpu_teid_pool&                     n3_teid_allocator_,
             gtpu_teid_pool&                     f1u_teid_allocator_,
             gtpu_demux_ctrl&                    gtpu_rx_demux_,
             dlt_pcap&                           gtpu_pcap) :
    task_sched(task_sched_),
    ue_exec_mapper(std::move(ue_exec_mapper_)),
    index(index_),
    cfg(std::move(cfg_)),
    logger("CU-UP", {index_}),
    e1ap(e1ap_),
    pdu_session_manager(index,
                        cfg.qos,
                        cfg.security_info,
                        n3_config_,
                        test_mode_config_,
                        logger,
                        cfg.ue_dl_aggregate_maximum_bit_rate,
                        ue_inactivity_timer,
                        ue_dl_timer_factory_,
                        ue_ul_timer_factory_,
                        ue_ctrl_timer_factory_,
                        e1ap_,
                        f1u_gw_,
                        ngu_session_mngr_,
                        n3_teid_allocator_,
                        f1u_teid_allocator_,
                        gtpu_rx_demux_,
                        ue_exec_mapper->dl_pdu_executor(),
                        ue_exec_mapper->ul_pdu_executor(),
                        ue_exec_mapper->ctrl_executor(),
                        ue_exec_mapper->crypto_executor(),
                        gtpu_pcap),
    ue_dl_timer_factory(ue_dl_timer_factory_),
    ue_ul_timer_factory(ue_ul_timer_factory_),
    ue_ctrl_timer_factory(ue_ctrl_timer_factory_)
  {
    if (cfg.activity_level == activity_notification_level_t::ue) {
      if (not cfg.ue_inactivity_timeout.has_value()) {
        report_error(
            "Failed to create UE context. Activity notification level is UE, but no UE inactivity timer configured\n");
      }
      ue_inactivity_timer = ue_ctrl_timer_factory.create_timer();
      ue_inactivity_timer.set(*cfg.ue_inactivity_timeout,
                              [this](timer_id_t /*tid*/) { on_ue_inactivity_timer_expired(); });
      ue_inactivity_timer.run();
    }
  }
  ~ue_context() override = default;

  /// Stop UE executors. This routine will switch between executors as required
  /// to stop/wait on events as required. At the end, it will return to the provided control executor.
  async_task<void> stop(task_executor& ctrl_executor, timer_manager& timers)
  {
    return launch_async([this, &ctrl_executor, &timers](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);

      // Switch to UE control executor. Disconnect DRBS.
      // Stop UE wide timers.
      CORO_AWAIT(execute_on_blocking(ue_exec_mapper->ctrl_executor(), timers));
      pdu_session_manager.disconnect_all_pdu_sessions();
      ue_inactivity_timer.stop();

      // Switch to UE UL executor. Flush pending UL tasks and
      // await pending UL crypto tasks.
      CORO_AWAIT(execute_on_blocking(ue_exec_mapper->ul_pdu_executor(), timers));
      CORO_AWAIT(pdu_session_manager.await_crypto_rx_all_pdu_sessions());

      // Switch to UE DL executor. Await pending DL crypto tasks.
      CORO_AWAIT(execute_on_blocking(ue_exec_mapper->dl_pdu_executor(), timers));
      CORO_AWAIT(pdu_session_manager.await_crypto_tx_all_pdu_sessions());

      // Return to UE control executor and stop UE specific executors.
      CORO_AWAIT(execute_on_blocking(ue_exec_mapper->ctrl_executor(), timers));
      CORO_AWAIT(ue_exec_mapper->stop());

      // Continuation in the original executor.
      CORO_AWAIT(execute_on_blocking(ctrl_executor, timers));

      CORO_RETURN();
    });
  }

  // security management
  void set_security_config(const security::sec_as_config& security_info)
  {
    cfg.security_info = security_info;
    pdu_session_manager.update_security_config(security_info);
  }

  void notify_pdcp_pdu_processing_stopped() { pdu_session_manager.notify_pdcp_pdu_processing_stopped(); }
  void restart_pdcp_pdu_processing() { pdu_session_manager.restart_pdcp_pdu_processing(); }

  async_task<void> await_rx_crypto_tasks() { return pdu_session_manager.await_crypto_rx_all_pdu_sessions(); }
  async_task<void> await_tx_crypto_tasks() { return pdu_session_manager.await_crypto_tx_all_pdu_sessions(); }

  // pdu_session_manager_ctrl
  pdu_session_setup_result setup_pdu_session(const e1ap_pdu_session_res_to_setup_item& session) override
  {
    return pdu_session_manager.setup_pdu_session(session);
  }
  pdu_session_modification_result modify_pdu_session(const e1ap_pdu_session_res_to_modify_item& session,
                                                     bool new_ul_tnl_info_required) override
  {
    return pdu_session_manager.modify_pdu_session(session, new_ul_tnl_info_required);
  }
  void remove_pdu_session(pdu_session_id_t pdu_session_id) override
  {
    pdu_session_manager.remove_pdu_session(pdu_session_id);
  }
  size_t get_nof_pdu_sessions() override { return pdu_session_manager.get_nof_pdu_sessions(); }

  [[nodiscard]] ue_index_t get_index() const { return index; }

  [[nodiscard]] const cu_up_ue_logger& get_logger() const { return logger; }

  fifo_async_task_scheduler& task_sched;

  std::unique_ptr<ue_executor_mapper> ue_exec_mapper;

private:
  ue_index_t      index;
  ue_context_cfg  cfg;
  cu_up_ue_logger logger;

  e1ap_interface&          e1ap;
  pdu_session_manager_impl pdu_session_manager;

  timer_factory ue_dl_timer_factory;
  timer_factory ue_ul_timer_factory;
  timer_factory ue_ctrl_timer_factory;

  unique_timer ue_inactivity_timer;

  /// Handle expired UE inactivity timer. This function is called from a timer that is run in UE executor,
  /// therefore it handovers the handling to control executor.
  void on_ue_inactivity_timer_expired()
  {
    if (!ue_exec_mapper->ctrl_executor().execute([this]() mutable {
          e1ap_bearer_context_inactivity_notification msg = {};
          msg.ue_index                                    = index;
          e1ap.handle_bearer_context_inactivity_notification(msg);
        })) {
      logger.log_warning("Could not handle expired UE inactivity handler, queue is full. ue={}",
                         fmt::underlying(index));
    }
  }
};

} // namespace srsran::srs_cu_up
