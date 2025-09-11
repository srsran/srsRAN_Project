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

#include "cu_cp_impl.h"
#include "du_processor/du_processor_repository.h"
#include "metrics_handler/metrics_handler_impl.h"
#include "routines/amf_connection_loss_routine.h"
#include "routines/cell_activation_routine.h"
#include "routines/initial_context_setup_routine.h"
#include "routines/mobility/inter_cu_handover_source_routine.h"
#include "routines/mobility/inter_cu_handover_target_routine.h"
#include "routines/mobility/intra_cu_handover_routine.h"
#include "routines/mobility/intra_cu_handover_target_routine.h"
#include "routines/pdu_session_resource_modification_routine.h"
#include "routines/pdu_session_resource_release_routine.h"
#include "routines/pdu_session_resource_setup_routine.h"
#include "routines/reestablishment_context_modification_routine.h"
#include "routines/ue_amf_context_release_request_routine.h"
#include "routines/ue_context_release_routine.h"
#include "routines/ue_removal_routine.h"
#include "routines/ue_transaction_info_release_routine.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/nrppa/nrppa.h"
#include "srsran/nrppa/nrppa_factory.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/support/async/coroutine.h"
#include "srsran/support/compiler.h"
#include "srsran/support/synchronization/sync_event.h"
#include <chrono>
#include <dlfcn.h>
#include <future>
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

static void assert_cu_cp_configuration_valid(const cu_cp_configuration& cfg)
{
  srsran_assert(cfg.services.cu_cp_executor != nullptr, "Invalid CU-CP executor");
  srsran_assert(!cfg.ngap.ngaps.empty(), "No NGAPs configured");
  for (const auto& ngap : cfg.ngap.ngaps) {
    srsran_assert(ngap.n2_gw != nullptr, "Invalid N2 GW client handler");
  }
  srsran_assert(cfg.services.timers != nullptr, "Invalid timers");

  report_error_if_not(cfg.admission.max_nof_dus <= MAX_NOF_DUS, "Invalid max number of DUs");
  report_error_if_not(cfg.admission.max_nof_cu_ups <= MAX_NOF_CU_UPS, "Invalid max number of CU-UPs");
}

cu_cp_impl::cu_cp_impl(const cu_cp_configuration& config_) :
  cfg(config_),
  ue_mng(cfg),
  cell_meas_mng(cfg.mobility.meas_manager_config, cell_meas_mobility_notifier, ue_mng),
  du_db(du_repository_config{cfg,
                             *this,
                             get_cu_cp_ue_removal_handler(),
                             get_cu_cp_ue_context_handler(),
                             common_task_sched,
                             ue_mng,
                             rrc_du_cu_cp_notifier,
                             conn_notifier,
                             srslog::fetch_basic_logger("CU-CP")}),
  cu_up_db(cu_up_repository_config{cfg, e1ap_ev_notifier, common_task_sched, srslog::fetch_basic_logger("CU-CP")}),
  paging_handler(du_db),
  ngap_db(ngap_repository_config{cfg, get_cu_cp_ngap_handler(), paging_handler, srslog::fetch_basic_logger("CU-CP")}),
  mobility_mng(cfg.mobility.mobility_manager_config, mobility_manager_ev_notifier, ngap_db, du_db, ue_mng),
  controller(cfg,
             get_cu_cp_amf_reconnection_handler(),
             common_task_sched,
             ngap_db,
             cu_up_db,
             du_db,
             *cfg.services.cu_cp_executor),
  metrics_hdlr(std::make_unique<metrics_handler_impl>(*cfg.services.cu_cp_executor,
                                                      *cfg.services.timers,
                                                      ue_mng,
                                                      du_db,
                                                      ngap_db,
                                                      mobility_mng)),
  cu_cp_cfgtr(mobility_manager_ev_notifier, du_db, ngap_db, ue_mng)
{
  assert_cu_cp_configuration_valid(cfg);

  nrppa_entity = create_nrppa_entity(cfg, nrppa_cu_cp_ev_notifier, common_task_sched);

  // Connect event notifiers to layers.
  ngap_cu_cp_ev_notifier.connect_cu_cp(get_cu_cp_ngap_handler(), paging_handler);
  nrppa_cu_cp_ev_notifier.connect_cu_cp(get_cu_cp_nrppa_handler());
  mobility_manager_ev_notifier.connect_cu_cp(get_cu_cp_mobility_manager_handler());
  e1ap_ev_notifier.connect_cu_cp(get_cu_cp_e1ap_handler());
  rrc_du_cu_cp_notifier.connect_cu_cp(get_cu_cp_measurement_config_handler());
  cell_meas_mobility_notifier.connect_mobility_manager(mobility_mng);

  conn_notifier.connect_node_connection_handler(controller);

  // Start statistics report timer.
  statistics_report_timer = cfg.services.timers->create_unique_timer(*cfg.services.cu_cp_executor);
  statistics_report_timer.set(cfg.metrics.statistics_report_period,
                              [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
  statistics_report_timer.run();
  if (cfg.metrics_notifier != nullptr and cfg.metrics.metrics_report_period.count() != 0) {
    periodic_metric_report_request metric_cfg{cfg.metrics.metrics_report_period, cfg.metrics_notifier};
    metrics_session = metrics_hdlr->create_periodic_report_session(metric_cfg);
  }
}

cu_cp_impl::~cu_cp_impl()
{
  stop();
}

bool cu_cp_impl::start()
{
  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  if (not cfg.services.cu_cp_executor->execute([this, &p]() {
        // Start AMF connection procedure.
        controller.amf_connection_handler().connect_to_amf(&p);
      })) {
    report_fatal_error("Failed to initiate CU-CP setup");
  }
  // Block waiting for CU-CP setup to complete.
  return fut.get();
}

void cu_cp_impl::stop()
{
  bool already_stopped = stopped.exchange(true);
  if (already_stopped) {
    return;
  }
  logger.info("Stopping CU-CP...");

  // Shut down components from within CU-CP executor.
  sync_event ev;
  while (not cfg.services.cu_cp_executor->execute([this, token = ev.get_token()]() {
    // Stop statistics gathering.
    statistics_report_timer.stop();
    if (metrics_session != nullptr) {
      metrics_session->stop();
    }
  })) {
    logger.debug("Failed to dispatch CU-CP stop task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  ev.wait();

  controller.stop();
  logger.info("CU-CP stopped successfully.");
}

ngap_message_handler* cu_cp_impl::get_ngap_message_handler(const plmn_identity& plmn)
{
  return ngap_db.find_ngap(plmn);
}

bool cu_cp_impl::amfs_are_connected()
{
  if (cfg.ngap.no_core) {
    return true;
  }

  for (const auto& [amf_index, ngap] : ngap_db.get_ngaps()) {
    if (not controller.amf_connection_handler().is_amf_connected(amf_index)) {
      return false;
    }
  }

  return true;
}

#ifndef SRSRAN_HAS_ENTERPRISE

std::unique_ptr<srsran::srs_cu_cp::nrppa_interface>
cu_cp_impl::create_nrppa_entity(const cu_cp_configuration& cu_cp_cfg,
                                nrppa_cu_cp_notifier&      cu_cp_notif,
                                common_task_scheduler&     common_task_sched_)
{
  return create_nrppa(cu_cp_cfg, cu_cp_notif, common_task_sched_);
}

#endif // SRSRAN_HAS_ENTERPRISE

void cu_cp_impl::handle_bearer_context_release_request(const cu_cp_bearer_context_release_request& msg)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", msg.ue_index);

  if (ue->get_handover_ue_release_timer().is_running()) {
    logger.debug("ue={}: Ignoring Bearer Context Release Request. Cause: Ongoing handover for this UE", msg.ue_index);
    return;
  }

  cu_cp_ue_context_release_request req;
  req.ue_index = msg.ue_index;
  req.cause    = msg.cause;

  // Add PDU Session IDs.
  auto& up_resource_manager            = ue->get_up_resource_manager();
  req.pdu_session_res_list_cxt_rel_req = up_resource_manager.get_pdu_sessions();

  logger.debug("ue={}: Requesting UE context release with cause={}", req.ue_index, req.cause);

  // Schedule on UE task scheduler.
  ue->get_task_sched().schedule_async_task(launch_async([this, req](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    // Notify NGAP to request a release from the AMF.
    CORO_AWAIT(handle_ue_context_release(req));
    CORO_RETURN();
  }));
}

void cu_cp_impl::handle_bearer_context_inactivity_notification(const cu_cp_inactivity_notification& msg)
{
  if (msg.ue_inactive) {
    cu_cp_ue* ue = ue_mng.find_du_ue(msg.ue_index);
    srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", msg.ue_index);

    if (ue->get_handover_ue_release_timer().is_running()) {
      logger.debug("ue={}: Ignoring UE inactivity. Cause: Ongoing handover for this UE", msg.ue_index);
      return;
    }

    cu_cp_ue_context_release_request req;
    req.ue_index = msg.ue_index;
    req.cause    = ngap_cause_radio_network_t::user_inactivity;

    // Add PDU Session IDs.
    auto& up_resource_manager            = ue->get_up_resource_manager();
    req.pdu_session_res_list_cxt_rel_req = up_resource_manager.get_pdu_sessions();

    logger.debug("ue={}: Requesting UE context release with cause={}", req.ue_index, req.cause);

    // Schedule on UE task scheduler.
    ue->get_task_sched().schedule_async_task(launch_async([this, req](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      // Notify NGAP to request a release from the AMF.
      CORO_AWAIT(handle_ue_context_release(req));
      CORO_RETURN();
    }));
  } else {
    logger.debug("Inactivity notification level not supported");
  }
}

void cu_cp_impl::handle_e1_release_request(cu_up_index_t cu_up_index, const std::vector<ue_index_t>& ue_list)
{
  for (const auto& ue_index : ue_list) {
    cu_cp_ue* ue = ue_mng.find_du_ue(ue_index);
    srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", ue_index);

    cu_cp_ue_context_release_request req;
    req.ue_index = ue_index;
    req.cause    = ngap_cause_radio_network_t::release_due_to_ngran_generated_reason;

    // Add PDU Session IDs.
    auto& up_resource_manager            = ue->get_up_resource_manager();
    req.pdu_session_res_list_cxt_rel_req = up_resource_manager.get_pdu_sessions();

    logger.debug("ue={}: Requesting UE context release with cause={}", req.ue_index, req.cause);

    // Schedule on UE task scheduler.
    ue->get_task_sched().schedule_async_task(launch_async([this, req](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      // Notify NGAP to request a release from the AMF.
      CORO_AWAIT(handle_ue_context_release(req));
      CORO_RETURN();
    }));
  }

  // Schedule removal of CU-UP processor.
  common_task_sched.schedule_async_task(launch_async([this, cu_up_index](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(cu_up_db.remove_cu_up(cu_up_index));
    CORO_RETURN();
  }));
}

rrc_ue_reestablishment_context_response
cu_cp_impl::handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index)
{
  rrc_ue_reestablishment_context_response reest_context{};

  ue_index_t old_ue_index = ue_mng.get_ue_index(old_pci, old_c_rnti);
  if (old_ue_index == ue_index_t::invalid || old_ue_index == ue_index) {
    return reest_context;
  }

  auto* const old_ue = ue_mng.find_du_ue(old_ue_index);
  if (old_ue == nullptr) {
    logger.debug("ue={}: Could not find UE", old_ue_index);
    return reest_context;
  }

  // Stop the UE release timer if it is running.
  if (old_ue->get_handover_ue_release_timer().is_running()) {
    logger.debug("ue={}: Stopping handover UE release timer", old_ue_index);
    old_ue->get_handover_ue_release_timer().stop();
  }

  // Cancel any ongoing handover transaction for the UE.
  if (old_ue->get_ho_context().has_value()) {
    logger.debug("ue={}: Cancelling handover transaction", old_ue_index);

    auto* const target_ue = ue_mng.find_du_ue(old_ue->get_ho_context()->target_ue_index);
    if (target_ue == nullptr) {
      logger.debug("ue={}: Could not find UE", old_ue->get_ho_context()->target_ue_index);
    } else {
      target_ue->get_rrc_ue()->cancel_handover_reconfiguration_transaction(
          old_ue->get_ho_context().value().rrc_reconfig_transaction_id);
    }
  }

  // Check if a DRB and SRB2 were setup.
  if (old_ue->get_up_resource_manager().get_drbs().empty()) {
    logger.debug("ue={}: No DRB setup for this UE - rejecting RRC reestablishment", old_ue_index);
    reest_context.ue_index = old_ue_index;
    return reest_context;
  }

  auto srbs = old_ue->get_rrc_ue()->get_srbs();
  if (std::find(srbs.begin(), srbs.end(), srb_id_t::srb2) == srbs.end()) {
    logger.debug("ue={}: SRB2 not setup for this UE - rejecting RRC reestablishment", old_ue_index);
    reest_context.ue_index = old_ue_index;
    return reest_context;
  }

  auto* rrc_ue = old_ue->get_rrc_ue();
  if (rrc_ue == nullptr) {
    logger.debug("ue={}: RRC UE not found for this UE - rejecting RRC reestablishment", old_ue_index);
    reest_context.ue_index = old_ue_index;
    return reest_context;
  }

  // Get RRC Reestablishment UE Context from old UE.
  reest_context                       = rrc_ue->get_context();
  reest_context.old_ue_fully_attached = true;
  reest_context.ue_index              = old_ue_index;

  return reest_context;
}

async_task<bool> cu_cp_impl::handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", ue_index);
  srsran_assert(cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr,
                "cu_up_index={}: could not find CU-UP",
                uint_to_cu_up_index(0));

  return launch_async<reestablishment_context_modification_routine>(
      ue_index,
      ue->get_security_manager().get_up_as_config(),
      cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->get_e1ap_bearer_context_manager(),
      du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
      ue->get_rrc_ue(),
      get_cu_cp_rrc_ue_interface(),
      ue->get_task_sched(),
      ue->get_up_resource_manager(),
      logger);
}

void cu_cp_impl::handle_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request)
{
  auto* ue = ue_mng.find_ue(request.ue_index);
  if (ue != nullptr) {
    ue->get_task_sched().schedule_async_task(handle_ue_context_release(request));
  };
}

void cu_cp_impl::handle_rrc_reestablishment_complete(ue_index_t old_ue_index)
{
  auto* ue = ue_mng.find_ue(old_ue_index);
  if (ue != nullptr) {
    ue->get_task_sched().schedule_async_task(handle_ue_removal_request(old_ue_index));
  };
}

void cu_cp_impl::handle_rrc_reconf_complete_indicator(ue_index_t ue_index)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", ue_index);

  if (ue != nullptr) {
    ue->get_task_sched().schedule_async_task(
        launch_async([this, ue_index, ue_context_mod_request = f1ap_ue_context_modification_request{}](
                         coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);

          if (ue_mng.find_du_ue(ue_index) == nullptr) {
            CORO_EARLY_RETURN();
          }

          ue_context_mod_request.ue_index               = ue_index;
          ue_context_mod_request.rrc_recfg_complete_ind = f1ap_rrc_recfg_complete_ind::true_value;

          CORO_AWAIT(du_db.get_du_processor(ue_mng.find_du_ue(ue_index)->get_du_index())
                         .get_f1ap_handler()
                         .handle_ue_context_modification_request(ue_context_mod_request));

          CORO_RETURN();
        }));
  }
}

async_task<bool> cu_cp_impl::handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index)
{
  srsran_assert(cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr,
                "cu_up_index={}: could not find CU-UP",
                uint_to_cu_up_index(0));
  srsran_assert(ue_mng.find_ue(ue_index) != nullptr, "ue={} not found", ue_index);

  // Task to run in old UE task scheduler.
  auto handle_ue_context_transfer_impl = [this, ue_index, old_ue_index]() {
    if (ue_mng.find_du_ue(old_ue_index) == nullptr) {
      logger.warning("Old UE index={} got removed", old_ue_index);
      return false;
    }

    auto* old_ue = ue_mng.find_du_ue(old_ue_index);

    if (ue_mng.find_du_ue(ue_index) == nullptr) {
      logger.warning("UE index={} got removed", ue_index);
      return false;
    }

    auto* ue = ue_mng.find_du_ue(ue_index);

    // Notify old F1AP UE context to F1AP.
    if (old_ue->get_du_index() == ue->get_du_index()) {
      const bool result =
          du_db.get_du_processor(old_ue->get_du_index()).get_f1ap_handler().handle_ue_id_update(ue_index, old_ue_index);
      if (not result) {
        logger.warning("The F1AP UE context of the old UE index {} does not exist", old_ue_index);
        return false;
      }
    }

    auto* ngap = ngap_db.find_ngap(ue->get_ue_context().plmn);
    if (ngap == nullptr) {
      logger.warning("NGAP not found for PLMN={}", ue->get_ue_context().plmn);
      return false;
    }

    // Transfer NGAP UE Context to new UE and remove the old context.
    if (not ngap->update_ue_index(ue_index, old_ue_index, ue_mng.find_ue(ue_index)->get_ngap_cu_cp_ue_notifier())) {
      return false;
    }

    // Transfer E1AP UE Context to new UE and remove old context.
    cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->update_ue_index(ue_index, old_ue_index);

    return true;
  };

  // Task that the caller will use to sync with the old UE task scheduler.
  struct transfer_context_task {
    transfer_context_task(cu_cp_impl& parent_, ue_index_t old_ue_index_, unique_function<bool()> callable) :
      parent(parent_),
      old_ue_index(old_ue_index_),
      task([this, callable = std::move(callable)]() { transfer_successful = callable(); })
    {
    }

    void operator()(coro_context<async_task<bool>>& ctx)
    {
      CORO_BEGIN(ctx);

      CORO_AWAIT_VALUE(
          const bool task_run,
          parent.ue_mng.get_task_sched().dispatch_and_await_task_completion(old_ue_index, std::move(task)));

      CORO_RETURN(task_run and transfer_successful);
    }

    cu_cp_impl& parent;
    ue_index_t  old_ue_index;
    unique_task task;

    bool transfer_successful = false;
  };

  return launch_async<transfer_context_task>(*this, old_ue_index, handle_ue_context_transfer_impl);
}

void cu_cp_impl::handle_handover_reconfiguration_sent(const cu_cp_intra_cu_handover_target_request& request)
{
  if (ue_mng.find_du_ue(request.target_ue_index) == nullptr) {
    logger.warning("UE index={} got removed", request.target_ue_index);
    return;
  }

  cu_cp_ue* ue = ue_mng.find_du_ue(request.target_ue_index);

  ue->get_task_sched().schedule_async_task(launch_async<intra_cu_handover_target_routine>(
      request,
      cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->get_e1ap_bearer_context_manager(),
      du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
      *this,
      get_cu_cp_ue_removal_handler(),
      *this,
      ue_mng,
      mobility_mng,
      logger));
}

void cu_cp_impl::handle_handover_ue_context_push(ue_index_t source_ue_index, ue_index_t target_ue_index)
{
  srsran_assert(cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr,
                "cu_up_index={}: could not find CU-UP",
                uint_to_cu_up_index(0));
  srsran_assert(ue_mng.find_ue(target_ue_index) != nullptr, "ue={} not found", target_ue_index);

  auto* ue = ue_mng.find_ue(target_ue_index);

  auto* ngap = ngap_db.find_ngap(ue->get_ue_context().plmn);
  if (ngap == nullptr) {
    logger.warning("NGAP not found for PLMN={}", ue->get_ue_context().plmn);
    return;
  }

  // Transfer NGAP UE Context to new UE and remove the old context.
  if (!ngap->update_ue_index(target_ue_index, source_ue_index, ue->get_ngap_cu_cp_ue_notifier())) {
    return;
  }
  // Transfer E1AP UE Context to new UE and remove old context.
  cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->update_ue_index(target_ue_index, source_ue_index);
}

async_task<void> cu_cp_impl::handle_ue_context_release(const cu_cp_ue_context_release_request& request)
{
  auto* ue = ue_mng.find_ue(request.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Could not find UE", request.ue_index);
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  auto* ngap = ngap_db.find_ngap(ue->get_ue_context().plmn);
  if (ngap == nullptr) {
    logger.warning("NGAP not found for PLMN={}", ue->get_ue_context().plmn);
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  return launch_async<ue_amf_context_release_request_routine>(
      request, ngap->get_ngap_control_message_handler(), *this, logger);
}

bool cu_cp_impl::handle_handover_request(ue_index_t ue_index, const security::security_context& sec_ctxt)
{
  cu_cp_ue* ue = ue_mng.find_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Could not find UE", ue_index);
    return false;
  }
  return ue->get_security_manager().init_security_context(sec_ctxt);
}

async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
cu_cp_impl::handle_new_initial_context_setup_request(const ngap_init_context_setup_request& request)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(request.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find UE", request.ue_index);
  rrc_ue_interface* rrc_ue = ue->get_rrc_ue();
  srsran_assert(rrc_ue != nullptr, "ue={}: Could not find RRC UE", request.ue_index);

  auto* ngap = ngap_db.find_ngap(ue->get_ue_context().plmn);
  if (ngap == nullptr) {
    logger.warning("NGAP not found for PLMN={}", ue->get_ue_context().plmn);
    return launch_async(
        [](coro_context<async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>>& ctx) {
          CORO_BEGIN(ctx);
          CORO_RETURN(make_unexpected(ngap_init_context_setup_failure{}));
        });
  }

  return launch_async<initial_context_setup_routine>(request,
                                                     *rrc_ue,
                                                     ngap->get_ngap_ue_radio_cap_management_handler(),
                                                     ue->get_security_manager(),
                                                     du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
                                                     get_cu_cp_ngap_handler(),
                                                     logger);
}

async_task<cu_cp_pdu_session_resource_setup_response>
cu_cp_impl::handle_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(request.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", request.ue_index);
  srsran_assert(cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr,
                "cu_up_index={}: could not find CU-UP",
                uint_to_cu_up_index(0));

  return launch_async<pdu_session_resource_setup_routine>(
      request,
      ue_mng.get_ue_config(),
      ue->get_security_manager().get_up_as_config(),
      cfg.security.default_security_indication,
      cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->get_e1ap_bearer_context_manager(),
      du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
      ue->get_rrc_ue(),
      get_cu_cp_rrc_ue_interface(),
      ue->get_task_sched(),
      ue->get_up_resource_manager(),
      logger);
}

async_task<cu_cp_pdu_session_resource_modify_response>
cu_cp_impl::handle_new_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request& request)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(request.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", request.ue_index);
  srsran_assert(cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr,
                "cu_up_index={}: could not find CU-UP",
                uint_to_cu_up_index(0));

  return launch_async<pdu_session_resource_modification_routine>(
      request,
      cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->get_e1ap_bearer_context_manager(),
      du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
      ue->get_rrc_ue(),
      get_cu_cp_rrc_ue_interface(),
      ue->get_task_sched(),
      ue->get_up_resource_manager(),
      logger);
}

async_task<cu_cp_pdu_session_resource_release_response>
cu_cp_impl::handle_new_pdu_session_resource_release_command(const cu_cp_pdu_session_resource_release_command& command)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(command.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", command.ue_index);
  srsran_assert(cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr,
                "cu_up_index={}: could not find CU-UP",
                uint_to_cu_up_index(0));

  return launch_async<pdu_session_resource_release_routine>(
      command,
      cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->get_e1ap_bearer_context_manager(),
      du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
      ue->get_rrc_ue(),
      get_cu_cp_rrc_ue_interface(),
      ue->get_task_sched(),
      ue->get_up_resource_manager(),
      logger);
}

async_task<cu_cp_ue_context_release_complete>
cu_cp_impl::handle_ue_context_release_command(const cu_cp_ue_context_release_command& command)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(command.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", command.ue_index);

  e1ap_bearer_context_manager* e1ap_bearer_ctxt_mng = nullptr;
  if (cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr) {
    e1ap_bearer_ctxt_mng = &cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->get_e1ap_bearer_context_manager();
  }

  return launch_async<ue_context_release_routine>(command,
                                                  e1ap_bearer_ctxt_mng,
                                                  du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
                                                  get_cu_cp_ue_removal_handler(),
                                                  ue_mng,
                                                  logger);
}

async_task<ngap_handover_resource_allocation_response>
cu_cp_impl::handle_ngap_handover_request(const ngap_handover_request& request)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(request.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", request.ue_index);
  srsran_assert(cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0)) != nullptr,
                "cu_up_index={}: could not find CU-UP",
                uint_to_cu_up_index(0));

  return start_inter_cu_handover_target_routine(
      request,
      cu_up_db.find_cu_up_processor(uint_to_cu_up_index(0))->get_e1ap_bearer_context_manager(),
      du_db.get_du_processor(ue->get_du_index()).get_f1ap_handler(),
      get_cu_cp_ue_removal_handler(),
      ue_mng,
      cell_meas_mng,
      cfg.security.default_security_indication,
      logger);
}

void cu_cp_impl::handle_transmission_of_handover_required()
{
  // Notify mobility manager metrics handler about the requested handover preparation.
  mobility_mng.get_metrics_handler().aggregate_requested_handover_preparation();
}

async_task<bool> cu_cp_impl::handle_new_handover_command(ue_index_t ue_index, byte_buffer command)
{
  // Notify mobility manager metrics handler about the successful handover preparation.
  mobility_mng.get_metrics_handler().aggregate_successful_handover_preparation();

  cu_cp_ue* ue = ue_mng.find_du_ue(ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: UE not found for handover command handling", ue_index);
    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(false);
    });
  }
  ngap_interface* ngap = ngap_db.find_ngap(ue->get_ue_context().plmn);
  if (ngap == nullptr) {
    logger.warning("ue={}: NGAP not found for PLMN={}", ue_index, ue->get_ue_context().plmn);
    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(false);
    });
  }
  return start_inter_cu_handover_source_routine(
      ue_index, std::move(command), ue_mng, du_db, cu_up_db, ngap->get_ngap_control_message_handler(), logger);
}

ue_index_t cu_cp_impl::handle_ue_index_allocation_request(const nr_cell_global_id_t& cgi)
{
  du_index_t du_index = du_db.find_du(cgi);
  if (du_index == du_index_t::invalid) {
    logger.warning("Could not find DU for CGI={}", cgi.nci);
    return ue_index_t::invalid;
  }
  return ue_mng.add_ue(du_index, cgi.plmn_id);
}

#ifndef SRSRAN_HAS_ENTERPRISE

void cu_cp_impl::handle_dl_ue_associated_nrppa_transport_pdu(ue_index_t ue_index, const byte_buffer& nrppa_pdu)
{
  logger.info("DL UE associated NRPPa messages are not supported");
}

void cu_cp_impl::handle_dl_non_ue_associated_nrppa_transport_pdu(amf_index_t amf_index, const byte_buffer& nrppa_pdu)
{
  logger.info("DL non UE associated NRPPa messages are not supported");
}

nrppa_cu_cp_ue_notifier* cu_cp_impl::handle_new_nrppa_ue(ue_index_t ue_index)
{
  return nullptr;
}

void cu_cp_impl::handle_ul_nrppa_pdu(const byte_buffer&                    nrppa_pdu,
                                     std::variant<ue_index_t, amf_index_t> ue_or_amf_index)
{
  logger.info("UL NRPPa messages are not supported");
}

async_task<trp_information_cu_cp_response_t>
cu_cp_impl::handle_trp_information_request(const trp_information_request_t& request)
{
  logger.info("TRP information requests are not supported");
  return launch_async([](coro_context<async_task<trp_information_cu_cp_response_t>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(trp_information_cu_cp_response_t{});
  });
}

#endif // SRSRAN_HAS_ENTERPRISE

void cu_cp_impl::handle_n2_disconnection(amf_index_t amf_index)
{
  std::vector<plmn_identity> plmns = ngap_db.find_ngap(amf_index)->get_ngap_context().get_supported_plmns();

  logger.warning("Handling N2 disconnection. Lost PLMNs: {}", fmt::format("{}", fmt::join(plmns, " ")));

  common_task_sched.schedule_async_task(
      launch_async<amf_connection_loss_routine>(amf_index, cfg, plmns, du_db, *this, ue_mng, controller, logger));
}

std::optional<rrc_meas_cfg>
cu_cp_impl::handle_measurement_config_request(ue_index_t                         ue_index,
                                              nr_cell_identity                   nci,
                                              const std::optional<rrc_meas_cfg>& current_meas_config)
{
  return cell_meas_mng.get_measurement_config(ue_index, nci, current_meas_config);
}

void cu_cp_impl::handle_measurement_report(const ue_index_t ue_index, const rrc_meas_results& meas_results)
{
  cell_meas_mng.report_measurement(ue_index, meas_results);
}

bool cu_cp_impl::handle_cell_config_update_request(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg)
{
  return cell_meas_mng.update_cell_config(nci, serv_cell_cfg);
}

async_task<cu_cp_intra_cu_handover_response>
cu_cp_impl::handle_intra_cu_handover_request(const cu_cp_intra_cu_handover_request& request,
                                             du_index_t&                            source_du_index,
                                             du_index_t&                            target_du_index)
{
  cu_cp_ue* ue = ue_mng.find_du_ue(request.source_ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", request.source_ue_index);

  byte_buffer sib1 = du_db.get_du_processor(target_du_index).get_mobility_handler().get_packed_sib1(request.cgi);

  return launch_async<intra_cu_handover_routine>(request,
                                                 std::move(sib1),
                                                 du_db.get_du_processor(source_du_index).get_f1ap_handler(),
                                                 du_db.get_du_processor(target_du_index).get_f1ap_handler(),
                                                 *this,
                                                 get_cu_cp_ue_removal_handler(),
                                                 *this,
                                                 ue_mng,
                                                 mobility_mng,
                                                 logger);
}

async_task<void> cu_cp_impl::handle_ue_removal_request(ue_index_t ue_index)
{
  if (ue_mng.find_du_ue(ue_index) == nullptr) {
    logger.warning("ue={}: Could not find DU UE", ue_index);
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }
  auto* ue = ue_mng.find_ue(ue_index);

  du_index_t    du_index    = ue->get_du_index();
  cu_up_index_t cu_up_index = uint_to_cu_up_index(0); // TODO: Update when mapping from UE index to CU-UP exists

  e1ap_bearer_context_removal_handler* e1ap_removal_handler = nullptr;
  if (cu_up_db.find_cu_up_processor(cu_up_index) != nullptr) {
    e1ap_removal_handler = &cu_up_db.find_cu_up_processor(cu_up_index)->get_e1ap_bearer_context_removal_handler();
  }

  auto* ngap = ngap_db.find_ngap(ue->get_ue_context().plmn);
  if (ngap == nullptr) {
    logger.warning("NGAP not found for PLMN={}", ue->get_ue_context().plmn);
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  nrppa_ue_context_removal_handler* nrppa_removal_handler = nullptr;
  nrppa_removal_handler                                   = &nrppa_entity->get_nrppa_ue_context_removal_handler();

  return launch_async<ue_removal_routine>(ue_index,
                                          du_db.get_du_processor(du_index).get_rrc_du_handler(),
                                          e1ap_removal_handler,
                                          du_db.get_du_processor(du_index).get_f1ap_handler(),
                                          ngap->get_ngap_ue_context_removal_handler(),
                                          nrppa_removal_handler,
                                          ue_mng,
                                          logger);
}

void cu_cp_impl::handle_pending_ue_task_cancellation(ue_index_t ue_index)
{
  srsran_assert(ue_mng.find_du_ue(ue_index) != nullptr, "ue={}: Could not find DU UE", ue_index);

  // Clear all enqueued tasks for this UE.
  ue_mng.get_task_sched().clear_pending_tasks(ue_index);

  // Cancel running transactions for the RRC UE.
  rrc_ue_interface* rrc_ue = ue_mng.find_du_ue(ue_index)->get_rrc_ue();
  if (rrc_ue != nullptr) {
    rrc_ue->get_controller().stop();
  }
}

void cu_cp_impl::handle_amf_reconnection(amf_index_t amf_index)
{
  if (ngap_db.find_ngap(amf_index) == nullptr) {
    logger.warning("AMF index={} not found", amf_index);
    return;
  }

  std::vector<plmn_identity> served_plmns = ngap_db.find_ngap(amf_index)->get_ngap_context().get_supported_plmns();

  common_task_sched.schedule_async_task(launch_async<cell_activation_routine>(cfg, served_plmns, du_db, logger));
}

void cu_cp_impl::initialize_handover_ue_release_timer(
    ue_index_t                              ue_index,
    std::chrono::milliseconds               handover_ue_release_timeout,
    const cu_cp_ue_context_release_request& ue_context_release_request)
{
  if (ue_mng.find_du_ue(ue_index) == nullptr) {
    logger.warning("ue={}: Could not find UE", ue_index);
    return;
  }

  cu_cp_ue* ue = ue_mng.find_ue(ue_index);

  if (ue->get_handover_ue_release_timer().is_running()) {
    logger.warning("ue={}: handover UE release timer already running", ue_index);
    return;
  }

  // Start timer.
  logger.debug("ue={}: Setting release timer to {}ms", ue_index, handover_ue_release_timeout.count());
  ue->get_handover_ue_release_timer().set(
      handover_ue_release_timeout, [this, ue, ue_context_release_request](timer_id_t /*tid*/) {
        ue->get_task_sched().schedule_async_task(handle_ue_context_release(ue_context_release_request));
      });
  ue->get_handover_ue_release_timer().run();
}

// private

void cu_cp_impl::handle_rrc_ue_creation(ue_index_t ue_index, rrc_ue_interface& rrc_ue)
{
  // Store the RRC UE in the UE manager.
  auto* ue = ue_mng.find_ue(ue_index);
  ue->set_rrc_ue(rrc_ue);

  // Connect RRC UE to NGAP to RRC UE adapter.
  ue_mng.get_ngap_rrc_ue_adapter(ue_index).connect_rrc_ue(rrc_ue.get_rrc_ngap_message_handler());

  // Connect NGAP to RRC UE to NGAP adapter.
  ue_mng.get_rrc_ue_ngap_adapter(ue_index).connect_ngap(ngap_db.find_ngap(ue->get_ue_context().plmn));

  // Connect cu-cp to rrc ue adapters.
  ue_mng.get_rrc_ue_cu_cp_adapter(ue_index).connect_cu_cp(get_cu_cp_rrc_ue_interface(),
                                                          get_cu_cp_ue_removal_handler(),
                                                          controller,
                                                          ue->get_up_resource_manager(),
                                                          get_cu_cp_measurement_handler());
}

byte_buffer cu_cp_impl::handle_target_cell_sib1_required(du_index_t du_index, nr_cell_global_id_t cgi)
{
  return du_db.get_du_processor(du_index).get_mobility_handler().get_packed_sib1(cgi);
}

async_task<void> cu_cp_impl::handle_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev)
{
  return launch_async<ue_transaction_info_release_routine>(ev, ue_mng, ngap_db, *this, logger);
}

ngap_cu_cp_ue_notifier* cu_cp_impl::handle_new_ngap_ue(ue_index_t ue_index)
{
  auto* ue = ue_mng.find_ue(ue_index);
  if (ue == nullptr) {
    return nullptr;
  }
  return &ue->get_ngap_cu_cp_ue_notifier();
}

bool cu_cp_impl::schedule_ue_task(ue_index_t ue_index, async_task<void> task)
{
  if (ue_mng.find_ue_task_scheduler(ue_index) == nullptr) {
    logger.debug("UE task scheduler not found for UE index={}", ue_index);
    return false;
  }

  return ue_mng.find_ue_task_scheduler(ue_index)->schedule_async_task(std::move(task));
}

void cu_cp_impl::on_statistics_report_timer_expired()
{
  // Get number of F1AP UEs.
  unsigned nof_f1ap_ues = du_db.get_nof_f1ap_ues();

  // Get number of RRC UEs.
  unsigned nof_rrc_ues = du_db.get_nof_rrc_ues();

  // Get number of NGAP UEs.
  unsigned nof_ngap_ues = ngap_db.get_nof_ngap_ues();

  // Get number of E1AP UEs.
  unsigned nof_e1ap_ues = cu_up_db.get_nof_e1ap_ues();

  // Get number of CU-CP UEs.
  unsigned nof_cu_cp_ues = ue_mng.get_nof_ues();

  // Log statistics.
  logger.debug("num_f1ap_ues={} num_rrc_ues={} num_ngap_ues={} num_e1ap_ues={} num_cu_cp_ues={}",
               nof_f1ap_ues,
               nof_rrc_ues,
               nof_ngap_ues,
               nof_e1ap_ues,
               nof_cu_cp_ues);

  // Restart timer.
  statistics_report_timer.set(cfg.metrics.statistics_report_period,
                              [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
  statistics_report_timer.run();
}
