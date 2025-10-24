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

#include "du_ue_controller_impl.h"
#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/execute_on_blocking.h"

using namespace srsran;
using namespace srs_du;

static const char* get_rlf_cause_str(rlf_cause cause)
{
  switch (cause) {
    case rlf_cause::max_mac_kos_reached:
      return "MAC max KOs reached";
    case rlf_cause::max_rlc_retxs_reached:
      return "RLC max ReTxs reached";
    case rlf_cause::rlc_protocol_failure:
      return "RLC protocol failure";
    default:
      break;
  }
  return "unknown";
}

namespace {

/// Adapter between MAC and DU manager RLF detection handler.
class mac_rlf_du_adapter final : public mac_ue_radio_link_notifier
{
public:
  mac_rlf_du_adapter(du_ue_index_t ue_index_, du_ue_manager_repository& ue_db_, task_executor& ctrl_exec_) :
    ue_index(ue_index_), ue_db(ue_db_), ctrl_exec(ctrl_exec_)
  {
  }

  void on_rlf_detected() override
  {
    // Dispatch RLF handling to DU manager execution context.
    bool dispatched = ctrl_exec.execute([ue_idx = ue_index, ue_db_ptr = &ue_db]() {
      // Note: The UE might have already been deleted by the time this method is called, so we need to check if it still
      // exists.
      du_ue* u = ue_db_ptr->find_ue(ue_idx);
      if (u == nullptr) {
        return;
      }
      u->handle_rlf_detection(rlf_cause::max_mac_kos_reached);
    });
    if (not dispatched) {
      logger.warning("ue={}: Failed to dispatch RLF detection handling", fmt::underlying(ue_index));
    }
  }

  void on_crnti_ce_received() override
  {
    bool dispatched = ctrl_exec.execute([ue_idx = ue_index, ue_db_ptr = &ue_db]() {
      du_ue* u = ue_db_ptr->find_ue(ue_idx);
      if (u == nullptr) {
        return;
      }
      u->handle_crnti_ce_detection();
    });
    if (not dispatched) {
      logger.warning("ue={}: Failed to dispatch RLF detection handling", fmt::underlying(ue_index));
    }
  }

private:
  du_ue_index_t             ue_index;
  du_ue_manager_repository& ue_db;
  task_executor&            ctrl_exec;
  srslog::basic_logger&     logger = srslog::fetch_basic_logger("DU-MNG");
};

/// Adapter between RLC and DU manager RLF detection handler.
class rlc_rlf_du_adapter : public rlc_tx_upper_layer_control_notifier
{
public:
  rlc_rlf_du_adapter(du_ue_index_t ue_index_, du_ue_manager_repository& ue_db_, task_executor& ctrl_exec_) :
    ue_index(ue_index_), ue_db(ue_db_), ctrl_exec(ctrl_exec_)
  {
  }

  void on_protocol_failure() override { dispatch_impl(rlf_cause::rlc_protocol_failure); }
  void on_max_retx() override { dispatch_impl(rlf_cause::max_rlc_retxs_reached); }

private:
  void dispatch_impl(rlf_cause cause)
  {
    bool prev_value = rlf_triggered.exchange(true, std::memory_order_relaxed);
    if (prev_value) {
      // RLF already dispatched. Do not bother to dispatch an RLF due RLC failure again.
      return;
    }
    // Dispatch RLF handling to DU manager execution context.
    bool dispatched = ctrl_exec.execute([ue_idx = ue_index, ue_db_ptr = &ue_db, cause]() {
      // Note: The UE might have already been deleted by the time this method is called, so we need to check if it still
      // exists.
      du_ue* u = ue_db_ptr->find_ue(ue_idx);
      if (u == nullptr) {
        return;
      }
      u->handle_rlf_detection(cause);
    });
    if (not dispatched) {
      logger.warning("ue={}: Failed to dispatch RLF detection handling", fmt::underlying(ue_index));
      rlf_triggered.store(false, std::memory_order_relaxed);
    }
  }

  du_ue_index_t             ue_index;
  du_ue_manager_repository& ue_db;
  task_executor&            ctrl_exec;
  std::atomic<bool>         rlf_triggered{false};
  srslog::basic_logger&     logger = srslog::fetch_basic_logger("DU-MNG");
};

} // namespace

// -------------

class srsran::srs_du::du_ue_controller_impl::rlf_state_machine
{
public:
  rlf_state_machine(du_ue_controller_impl& ue_ctx_, const du_manager_params& cfg_, du_ue_manager_repository& ue_db_) :
    ue_ctx(ue_ctx_), cfg(cfg_), release_timer(cfg.services.timers.create_unique_timer(cfg.services.du_mng_exec))
  {
  }

  void handle_rlf_detection(rlf_cause cause)
  {
    if (not is_handling_new_rlfs()) {
      // Either the RLF has been triggered or the UE is already being destroyed.
      return;
    }

    if (release_timer.is_running()) {
      // In case the RLF has already been triggered, check if RLF cause can be promoted in severity.

      if (cause != rlf_cause::max_mac_kos_reached and current_cause == rlf_cause::max_mac_kos_reached) {
        // RLFs due to RLC failures take priority over RLF due to MAC KOs, as they cannot be recovered from.
        current_cause = cause;
        stop_ue_drb_activity();
      }
      return;
    }

    // The release timer is not running yet. We need to store the cause and start the timer.
    current_cause    = cause;
    auto timeout_val = get_release_timeout();
    logger.warning("ue={} rnti={}: RLF detected with cause \"{}\". Timer of {} msec to release UE started...",
                   ue_ctx.ue_index,
                   ue_ctx.rnti,
                   get_rlf_cause_str(cause),
                   timeout_val.count());

    // Start timer.
    release_timer.set(timeout_val, [this](timer_id_t tid) { trigger_ue_release(); });
    release_timer.run();

    // Stop traffic in case of RLC-initiated RLF.
    if (cause != rlf_cause::max_mac_kos_reached) {
      stop_ue_drb_activity();
    }
  }

  void handle_crnti_ce_detection()
  {
    if (release_timer.is_running() and current_cause == rlf_cause::max_mac_kos_reached) {
      // If the RLF was not due to MAC KOs, a C-RNTI CE is not enough to cancel the RLF.
      release_timer.stop();
      logger.info("ue={}: RLF timer reset. Cause: C-RNTI CE was received for the UE", fmt::underlying(ue_ctx.ue_index));
    }
  }

  bool deactivate()
  {
    if (release_timer.is_valid()) {
      release_timer.reset();
      return true;
    }
    return false;
  }

private:
  bool is_handling_new_rlfs() const
  {
    return release_timer.is_valid() and current_cause != rlf_cause::rlc_protocol_failure and
           current_cause != rlf_cause::max_rlc_retxs_reached;
  }

  std::chrono::milliseconds get_release_timeout() const
  {
    // Note: Between an RLF being detected and the UE being released, the DU manager will wait for enough time to allow
    // the UE to perform C-RNTI CE (t310) and/or RRC re-establishment (t311).
    // Note: When the UE is initially created, it does not yet have a SRB2+DRB configured, so we do not need to account
    // for the t311 in the RLF timer, as the UE won't try to do RRC re-establishment at that stage.
    const auto& ue_timers = cfg.ran.cells[ue_ctx.pcell_index].ue_timers_and_constants;

    bool has_srb2_and_drb = ue_ctx.bearers.srbs().contains(srb_id_t::srb2) and not ue_ctx.bearers.drbs().empty();
    return has_srb2_and_drb ? ue_timers.t310 + ue_timers.t311 : ue_timers.t310;
  }

  void trigger_ue_release()
  {
    logger.info("ue={}: RLF timer expired with cause=\"{}\". Requesting a UE release...",
                fmt::underlying(ue_ctx.ue_index),
                get_rlf_cause_str(*current_cause));

    // Request UE release to the CU.
    using cause_type = f1ap_ue_context_release_request::cause_type;
    cause_type cause = *current_cause == rlf_cause::max_mac_kos_reached ? cause_type::rlf_mac : cause_type::rlf_rlc;
    cfg.f1ap.ue_mng.handle_ue_context_release_request(srs_du::f1ap_ue_context_release_request{ue_ctx.ue_index, cause});

    // Stop handling of RLFs, so that no new RLFs are triggered after the UE is scheduled for release.
    deactivate();
  }

  void stop_ue_drb_activity()
  {
    // We are at a point of no return for this UE. Disable DRB traffic (not RLFs because we still the RLF timer to
    // trigger).
    // Note: We use an async task rather than just an execute call, to ensure that this task is not dispatched after
    // the UE has already been deleted.
    ue_ctx.schedule_async_task(ue_ctx.handle_rb_stop_request(false));
  }

  du_ue_controller_impl&   ue_ctx;
  const du_manager_params& cfg;
  srslog::basic_logger&    logger = srslog::fetch_basic_logger("DU-MNG");

  std::optional<rlf_cause> current_cause;
  unique_timer             release_timer;
};

// -------------

du_ue_controller_impl::du_ue_controller_impl(const du_ue_context&         context_,
                                             du_ue_manager_repository&    ue_db_,
                                             const du_manager_params&     cfg_,
                                             ue_ran_resource_configurator ue_ran_res_) :
  du_ue(context_, std::move(ue_ran_res_)),
  ue_db(ue_db_),
  cfg(cfg_),
  rlf_handler(std::make_unique<rlf_state_machine>(*this, cfg, ue_db)),
  mac_rlf_notifier(std::make_unique<mac_rlf_du_adapter>(ue_index, ue_db, cfg.services.du_mng_exec)),
  rlc_rlf_notifier(std::make_unique<rlc_rlf_du_adapter>(ue_index, ue_db, cfg.services.du_mng_exec))
{
}

du_ue_controller_impl::~du_ue_controller_impl() {}

void du_ue_controller_impl::disable_rlf_detection()
{
  if (rlf_handler->deactivate()) {
    logger.debug("ue={}: Disabled RLF detection", fmt::underlying(ue_index));
  }
}

async_task<void> du_ue_controller_impl::handle_activity_stop_request(bool stop_srbs)
{
  // Disable RLF detections.
  disable_rlf_detection();

  return handle_rb_stop_request(stop_srbs);
}

async_task<void> du_ue_controller_impl::handle_rb_stop_request(bool stop_srbs)
{
  // Disable RBs
  if (stop_srbs) {
    logger.debug("ue={}: Stopping SRB and DRB traffic...", fmt::underlying(ue_index));
  } else {
    logger.debug("ue={}: Stopping DRB traffic...", fmt::underlying(ue_index));
  }

  // Disconnect bearers from within the UE execution context.
  return run_in_ue_executor([this, stop_srbs]() {
    // > Disconnect all DRBs.
    for (auto& drb_pair : bearers.drbs()) {
      du_ue_drb& drb = *drb_pair.second;
      drb.stop();
    }

    // > Disconnect SRBs.
    if (stop_srbs) {
      for (du_ue_srb& srb : bearers.srbs()) {
        srb.stop();
      }
    }

    if (stop_srbs) {
      logger.info("ue={}: SRB and DRB traffic stopped", fmt::underlying(ue_index));
    } else {
      logger.info("ue={}: DRB traffic stopped", fmt::underlying(ue_index));
    }
  });
}

async_task<void> du_ue_controller_impl::handle_drb_stop_request(span<const drb_id_t> drbs_to_stop)
{
  if (drbs_to_stop.empty()) {
    return launch_no_op_task();
  }

  // Cannot pass span to executor, otherwise, we will have a dangling pointer.
  std::vector<drb_id_t> drbs_to_stop_cpy(drbs_to_stop.begin(), drbs_to_stop.end());

  return run_in_ue_executor([this, drbs_to_stop_cpy = std::move(drbs_to_stop_cpy)]() {
    // > Subset of DRBs will be stopped.
    auto& ue_drbs = bearers.drbs();
    for (drb_id_t drb_id : drbs_to_stop_cpy) {
      auto it = ue_drbs.find(drb_id);
      if (it == ue_drbs.end()) {
        logger.warning("ue={}: Failed to stop {} activity. Cause: DRB not found", fmt::underlying(ue_index), drb_id);
        continue;
      }
      it->second->stop();
      logger.debug("ue={}: DRB {} traffic was stopped", fmt::underlying(ue_index), drb_id);
    }
  });
}

void du_ue_controller_impl::handle_rlf_detection(rlf_cause cause)
{
  rlf_handler->handle_rlf_detection(cause);
}

void du_ue_controller_impl::handle_crnti_ce_detection()
{
  rlf_handler->handle_crnti_ce_detection();
}

async_task<void> du_ue_controller_impl::run_in_ue_executor(unique_task task)
{
  auto log_dispatch_retry = [this]() {
    logger.warning("ue={}: Postpone dispatching of control task to executor. Cause: Task queue is full",
                   fmt::underlying(ue_index));
  };

  return launch_async([this, task = std::move(task), log_dispatch_retry](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Sync with UE control executor to run provided task.
    CORO_AWAIT(
        defer_on_blocking(cfg.services.ue_execs.ctrl_executor(ue_index), cfg.services.timers, log_dispatch_retry));
    task();

    // Sync with remaining UE executors, as there might be still pending tasks dispatched to those.
    // TODO: use when_all awaiter
    CORO_AWAIT(defer_on_blocking(
        cfg.services.ue_execs.mac_ul_pdu_executor(ue_index), cfg.services.timers, log_dispatch_retry));
    CORO_AWAIT(defer_on_blocking(
        cfg.services.ue_execs.f1u_dl_pdu_executor(ue_index), cfg.services.timers, log_dispatch_retry));

    // Sync with rlc-lower executor, as there might be some timer stop pending.
    CORO_AWAIT(defer_on_blocking(
        cfg.services.cell_execs.rlc_lower_executor(pcell_index), cfg.services.timers, log_dispatch_retry));

    // Return back to DU manager executor.
    CORO_AWAIT(defer_on_blocking(cfg.services.du_mng_exec, cfg.services.timers, log_dispatch_retry));

    CORO_RETURN();
  });
}
