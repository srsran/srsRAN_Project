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

#include "du_ue_controller_impl.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_du;

namespace {

const char* get_rlf_cause_str(rlf_cause cause)
{
  switch (cause) {
    case srsran::srs_du::rlf_cause::max_mac_kos_reached:
      return "MAC max KOs reached";
    case srsran::srs_du::rlf_cause::max_rlc_retxs_reached:
      return "RLC max ReTxs reached";
    case srsran::srs_du::rlf_cause::rlc_protocol_failure:
      return "RLC protocol failure";
    default:
      break;
  }
  return "unknown";
}

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
      logger.warning("ue={}: Failed to dispatch RLF detection handling", ue_index);
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
      logger.warning("ue={}: Failed to dispatch RLF detection handling", ue_index);
    }
  }

private:
  du_ue_index_t             ue_index;
  du_ue_manager_repository& ue_db;
  task_executor&            ctrl_exec;
  srslog::basic_logger&     logger = srslog::fetch_basic_logger("DU-MNG");
};

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
      logger.warning("ue={}: Failed to dispatch RLF detection handling", ue_index);
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
    if (not release_timer.is_valid()) {
      // Either the RLF has been triggered or the UE is already being destroyed.
      return;
    }

    if (release_timer.is_running()) {
      // In case the RLF has already been triggered, check if RLF cause can be promoted in severity.

      if (cause != rlf_cause::max_mac_kos_reached and current_cause == rlf_cause::max_mac_kos_reached) {
        // RLFs due to RLC failures take priority over RLF due to MAC KOs, as they cannot be recovered from.
        current_cause = cause;
        ue_ctx.stop_drb_traffic();
      }
      return;
    }

    // The release timer is not running yet. We need to store the cause and start the timer.
    current_cause    = cause;
    auto timeout_val = get_release_timeout();
    logger.info("ue={}: RLF detected with cause=\"{}\". Timer of {} msec to release UE started...",
                ue_ctx.ue_index,
                get_rlf_cause_str(cause),
                timeout_val.count());

    // Start timer.
    release_timer.set(timeout_val, [this](timer_id_t tid) { trigger_ue_release(); });
    release_timer.run();

    // Stop traffic in case of RLC-initiated RLF.
    if (cause != rlf_cause::max_mac_kos_reached) {
      ue_ctx.stop_drb_traffic();
    }
  }

  void handle_crnti_ce_detection()
  {
    if (release_timer.is_running() and current_cause == rlf_cause::max_mac_kos_reached) {
      // If the RLF was not due to MAC KOs, a C-RNTI CE is not enough to cancel the RLF.
      release_timer.stop();
      logger.info("ue={}: RLF timer reset. Cause: C-RNTI CE was received for the UE", ue_ctx.ue_index);
    }
  }

  void disconnect() { release_timer.reset(); }

private:
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
                ue_ctx.ue_index,
                get_rlf_cause_str(current_cause));

    // Request UE release to the CU.
    cfg.f1ap.ue_mng.handle_ue_context_release_request(
        srs_du::f1ap_ue_context_release_request{ue_ctx.ue_index, current_cause});

    // Release timer so no new RLF is triggered for the same UE, after it is scheduled for release.
    disconnect();
  }

  du_ue_controller_impl&   ue_ctx;
  const du_manager_params& cfg;
  srslog::basic_logger&    logger = srslog::fetch_basic_logger("DU-MNG");

  rlf_cause    current_cause;
  unique_timer release_timer;
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

async_task<void> du_ue_controller_impl::disconnect_notifiers()
{
  // > Disconnect RLF notifiers.
  rlf_handler->disconnect();

  // > Disconnect bearers from within the UE execution context.
  return dispatch_and_resume_on(cfg.services.ue_execs.ctrl_executor(ue_index), cfg.services.du_mng_exec, [this]() {
    // > Disconnect DRBs.
    for (auto& drb_pair : bearers.drbs()) {
      du_ue_drb& drb = *drb_pair.second;
      drb.disconnect();
    }

    // > Disconnect SRBs.
    for (du_ue_srb& srb : bearers.srbs()) {
      srb.disconnect();
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

void du_ue_controller_impl::stop_drb_traffic()
{
  // > Disconnect DRBs.
  // Note: We use an async task rather than just an execute call, to ensure that this task is not dispatched after
  // the UE has already been deleted.
  schedule_async_task(
      dispatch_and_resume_on(cfg.services.ue_execs.ctrl_executor(ue_index), cfg.services.du_mng_exec, [this]() {
        // > Disconnect DRBs.
        for (auto& drb_pair : bearers.drbs()) {
          du_ue_drb& drb = *drb_pair.second;
          drb.disconnect();
        }

        logger.info("ue={}: DRB traffic stopped", ue_index);
      }));
}
