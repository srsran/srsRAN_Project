/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ue_controller_impl.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_du;

namespace {

class mac_rlf_du_adapter final : public mac_ue_radio_link_notifier
{
public:
  mac_rlf_du_adapter(du_ue_controller& controller_) : controller(controller_) {}

  void on_rlf_detected() override { controller.handle_rlf_detection(rlf_cause::max_mac_kos_reached); }
  void on_crnti_ce_received() override { controller.handle_crnti_ce_detection(); }

private:
  du_ue_controller& controller;
};

class rlc_rlf_du_adapter : public rlc_tx_upper_layer_control_notifier
{
public:
  rlc_rlf_du_adapter(du_ue_controller& controller_) : controller(controller_) {}

  void on_protocol_failure() override { controller.handle_rlf_detection(rlf_cause::rlc_protocol_failure); }
  void on_max_retx() override { controller.handle_rlf_detection(rlf_cause::max_rlc_retxs_reached); }

private:
  du_ue_controller& controller;
};

} // namespace

// -------------

class srsran::srs_du::du_ue_controller_impl::rlf_state_machine
{
public:
  rlf_state_machine(du_ue& ue_ctx_, const du_manager_params& cfg_) :
    ue_ctx(ue_ctx_), cfg(cfg_), release_timer(cfg.services.timers.create_unique_timer(cfg.services.du_mng_exec))
  {
  }

  void handle_rlf_detection(rlf_cause cause)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (not is_connected_nolock()) {
      // RLF has already been triggered.
      return;
    }
    if (not release_timer.is_running()) {
      // The release timer is not running yet. We need to store the cause and start the timer.
      current_cause    = cause;
      auto timeout_val = get_release_timeout();
      release_timer.set(timeout_val, [this](timer_id_t tid) { trigger_ue_release(); });
      release_timer.run();
      logger.info(
          "ue={}: RLF detected. Timer of {} msec to release UE started...", ue_ctx.ue_index, timeout_val.count());
    } else if (cause != srs_du::rlf_cause::max_mac_kos_reached and
               current_cause == srs_du::rlf_cause::max_mac_kos_reached) {
      // RLFs due to RLC failures take priority over RLF due to MAC KOs.
      current_cause = cause;
    }
  }

  void handle_crnti_ce_detection()
  {
    bool timer_was_running = false;
    {
      std::lock_guard<std::mutex> lock(mutex);
      timer_was_running = release_timer.is_running();
      if (timer_was_running and current_cause == rlf_cause::max_mac_kos_reached) {
        // If the RLF was not due to MAC KOs, a C-RNTI CE is not enough to cancel the RLF.
        release_timer.stop();
      }
    }
    if (timer_was_running) {
      logger.info("ue={}: RLF timer reset. Cause: C-RNTI CE was received for the UE", ue_ctx.ue_index);
    }
  }

  void disconnect()
  {
    std::lock_guard<std::mutex> lock(mutex);
    disconnect_nolock();
  }

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
    logger.info("ue={}: RLF timer expired. Requesting a UE release...", ue_ctx.ue_index);

    std::lock_guard<std::mutex> lock(mutex);

    // Request UE release to the CU.
    cfg.f1ap.ue_mng.handle_ue_context_release_request(
        srs_du::f1ap_ue_context_release_request{ue_ctx.ue_index, current_cause});

    // Release timer so no new RLF is triggered for the same UE, after is scheduled for release.
    disconnect_nolock();
  }

  // Called by DU manager to disconnect the adapter during UE removal.
  void disconnect_nolock() { release_timer.reset(); }
  bool is_connected_nolock() const { return release_timer.is_valid(); }

  du_ue&                   ue_ctx;
  const du_manager_params& cfg;
  srslog::basic_logger&    logger = srslog::fetch_basic_logger("DU-MNG");

  rlf_cause    current_cause;
  unique_timer release_timer;

  // This class is accessed directly from the MAC/RLC, so potential race conditions apply when accessing the
  // \c release_timer.
  std::mutex mutex;
};

// -------------

du_ue_controller_impl::du_ue_controller_impl(du_ue_manager_repository& ue_db_,
                                             std::unique_ptr<du_ue>    context_,
                                             const du_manager_params&  cfg_) :
  context(std::move(context_)),
  ue_db(ue_db_),
  cfg(cfg_),
  rlf_handler(std::make_unique<rlf_state_machine>(*context, cfg)),
  mac_rlf_notifier(std::make_unique<mac_rlf_du_adapter>(*this)),
  rlc_rlf_notifier(std::make_unique<rlc_rlf_du_adapter>(*this))
{
}

du_ue_controller_impl::~du_ue_controller_impl() {}

async_task<void> du_ue_controller_impl::disconnect_notifiers()
{
  // > Disconnect RLF notifiers.
  rlf_handler->disconnect();

  // > Disconnect bearers from within the UE execution context.
  return dispatch_and_resume_on(
      cfg.services.ue_execs.ctrl_executor(context->ue_index), cfg.services.du_mng_exec, [this]() {
        // > Disconnect DRBs.
        for (auto& drb_pair : context->bearers.drbs()) {
          du_ue_drb& drb = *drb_pair.second;
          drb.disconnect();
        }

        // > Disconnect SRBs.
        for (du_ue_srb& srb : context->bearers.srbs()) {
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
