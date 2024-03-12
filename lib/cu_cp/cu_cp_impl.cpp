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

#include "cu_cp_impl.h"
#include "metrics_handler/metrics_handler_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap_factory.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/support/executors/sync_task_executor.h"
#include <chrono>
#include <future>
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

void assert_cu_cp_configuration_valid(const cu_cp_configuration& cfg)
{
  srsran_assert(cfg.cu_cp_executor != nullptr, "Invalid CU-CP executor");
  srsran_assert(cfg.ngap_notifier != nullptr, "Invalid NGAP notifier");
  srsran_assert(cfg.timers != nullptr, "Invalid timers");

  report_error_if_not(cfg.max_nof_dus <= MAX_NOF_DUS, "Invalid max number of DUs");
  report_error_if_not(cfg.max_nof_cu_ups <= MAX_NOF_CU_UPS, "Invalid max number of CU-UPs");
}

cu_cp_impl::cu_cp_impl(const cu_cp_configuration& config_) :
  cfg(config_),
  ue_mng(config_.ue_config, up_resource_manager_cfg{config_.rrc_config.drb_config}, *cfg.timers, *cfg.cu_cp_executor),
  mobility_mng(config_.mobility_config.mobility_manager_config, du_db, ue_mng),
  cell_meas_mng(config_.mobility_config.meas_manager_config, cell_meas_ev_notifier),
  du_db(du_repository_config{cfg,
                             *this,
                             get_cu_cp_ue_removal_handler(),
                             get_cu_cp_ue_context_handler(),
                             du_processor_e1ap_notifier,
                             du_processor_ngap_notifier,
                             f1ap_cu_cp_notifier,
                             rrc_ue_ngap_notifier,
                             rrc_ue_ngap_notifier,
                             du_processor_task_sched,
                             ue_mng,
                             rrc_du_cu_cp_notifier,
                             conn_notifier,
                             srslog::fetch_basic_logger("CU-CP")}),
  cu_up_db(cu_up_repository_config{cfg, e1ap_ev_notifier, srslog::fetch_basic_logger("CU-CP")}),
  routine_mng(ue_mng.get_task_sched()),
  controller(routine_mng, cfg.ngap_config, ngap_adapter, cu_up_db),
  metrics_hdlr(std::make_unique<metrics_handler_impl>(*cfg.cu_cp_executor, *cfg.timers, ue_mng, du_db))
{
  assert_cu_cp_configuration_valid(cfg);

  // connect event notifiers to layers
  ngap_cu_cp_ev_notifier.connect_cu_cp(du_db, *this);
  e1ap_ev_notifier.connect_cu_cp(get_cu_cp_e1ap_handler());
  f1ap_cu_cp_notifier.connect_cu_cp(get_cu_cp_ue_removal_handler());
  cell_meas_ev_notifier.connect_mobility_manager(mobility_mng);
  rrc_du_cu_cp_notifier.connect_cu_cp(get_cu_cp_measurement_config_handler());
  conn_notifier.connect_node_connection_handler(controller);

  // connect task schedulers
  ngap_task_sched.connect_cu_cp(ue_mng.get_task_sched());
  du_processor_task_sched.connect_cu_cp(ue_mng.get_task_sched());

  // Create NGAP.
  ngap_entity = create_ngap(cfg.ngap_config,
                            ngap_cu_cp_ev_notifier,
                            ngap_cu_cp_ev_notifier,
                            ngap_task_sched,
                            ue_mng,
                            *cfg.ngap_notifier,
                            *cfg.cu_cp_executor);
  ngap_adapter.connect_ngap(ngap_entity->get_ngap_connection_manager(),
                            ngap_entity->get_ngap_ue_context_removal_handler(),
                            ngap_entity->get_ngap_statistics_handler());
  du_processor_ngap_notifier.connect_ngap(ngap_entity->get_ngap_control_message_handler());
  rrc_ue_ngap_notifier.connect_ngap(ngap_entity->get_ngap_nas_message_handler(),
                                    ngap_entity->get_ngap_control_message_handler());

  // Start statistics report timer
  statistics_report_timer = cfg.timers->create_unique_timer(*cfg.cu_cp_executor);
  statistics_report_timer.set(cfg.statistics_report_period,
                              [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
  statistics_report_timer.run();
}

cu_cp_impl::~cu_cp_impl()
{
  stop();
}

bool cu_cp_impl::start()
{
  std::promise<bool> p;
  std::future<bool>  fut = p.get_future();

  if (not cfg.cu_cp_executor->execute([this, &p]() {
        // start AMF connection procedure.
        controller.amf_connection_handler().connect_to_amf(&p);
      })) {
    report_fatal_error("Failed to initiate CU-CP setup.");
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
  force_blocking_execute(
      *cfg.cu_cp_executor,
      [this]() {
        // Stop the activity of UEs that are currently attached.
        ue_mng.stop();

        // Stop DU repository.
        du_db.stop();

        // Stop statistics gathering.
        statistics_report_timer.stop();
      },
      [this]() {
        logger.debug("Failed to dispatch CU-CP stop task. Retrying...");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      });

  logger.info("CU-CP stopped successfully.");
}

ngap_message_handler& cu_cp_impl::get_ngap_message_handler()
{
  return *ngap_entity;
};

ngap_event_handler& cu_cp_impl::get_ngap_event_handler()
{
  return *ngap_entity;
}

void cu_cp_impl::handle_e1ap_created(e1ap_bearer_context_manager&         bearer_context_manager,
                                     e1ap_bearer_context_removal_handler& bearer_removal_handler,
                                     e1ap_statistics_handler&             e1ap_statistic_handler)
{
  // Connect e1ap to DU processor
  du_processor_e1ap_notifier.connect_e1ap(bearer_context_manager);
  // Connect e1ap to CU-CP
  e1ap_adapters[uint_to_cu_up_index(0)] = {};
  e1ap_adapters.at(uint_to_cu_up_index(0)).connect_e1ap(bearer_removal_handler, e1ap_statistic_handler);
}

void cu_cp_impl::handle_bearer_context_inactivity_notification(const cu_cp_inactivity_notification& msg)
{
  du_db.handle_inactivity_notification(get_du_index_from_ue_index(msg.ue_index), msg);
}

rrc_reestablishment_ue_context_t
cu_cp_impl::handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index)
{
  rrc_reestablishment_ue_context_t reest_context;

  ue_index_t old_ue_index = ue_mng.get_ue_index(old_pci, old_c_rnti);
  if (old_ue_index == ue_index_t::invalid || old_ue_index == ue_index) {
    return reest_context;
  }

  // check if a DRB and SRB2 were setup
  auto srbs = ue_mng.find_du_ue(old_ue_index)->get_rrc_ue_srb_notifier().get_srbs();
  if (ue_mng.find_du_ue(old_ue_index)->get_up_resource_manager().get_drbs().empty()) {
    logger.debug("ue={} No DRB setup for this UE - rejecting RRC reestablishment.", old_ue_index);
    reest_context.ue_index = old_ue_index;
    return reest_context;
  } else if (std::find(srbs.begin(), srbs.end(), srb_id_t::srb2) == srbs.end()) {
    logger.debug("ue={} SRB2 not setup for this UE - rejecting RRC reestablishment.", old_ue_index);
    reest_context.ue_index = old_ue_index;
    return reest_context;
  } else {
    reest_context.old_ue_fully_attached = true;
  }

  // Get RRC Reestablishment UE Context from old UE
  reest_context = ue_mng.get_cu_cp_rrc_ue_adapter(old_ue_index).on_rrc_ue_context_transfer();

  reest_context.ue_index = old_ue_index;

  return reest_context;
}

async_task<bool> cu_cp_impl::handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index)
{
  // Task to run in old UE task scheduler.
  auto handle_ue_context_transfer_impl = [this, ue_index, old_ue_index]() {
    if (ue_mng.find_du_ue(old_ue_index) == nullptr) {
      logger.warning("Old UE index={} got removed", old_ue_index);
      return false;
    }

    // Notify old F1AP UE context to F1AP.
    if (get_du_index_from_ue_index(old_ue_index) == get_du_index_from_ue_index(ue_index)) {
      const bool result = du_db.get_du(get_du_index_from_ue_index(old_ue_index))
                              .get_f1ap_ue_context_notifier()
                              .on_intra_du_reestablishment(ue_index, old_ue_index);
      if (not result) {
        logger.warning("The F1AP UE context of the old UE index {} does not exist", old_ue_index);
        return false;
      }
    }

    // Transfer NGAP UE Context to new UE and remove the old context
    ngap_entity->update_ue_index(ue_index, old_ue_index);

    // Transfer E1AP UE Context to new UE and remove old context
    cu_up_db.get_cu_up(uint_to_cu_up_index(0)).update_ue_index(ue_index, old_ue_index);

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

void cu_cp_impl::handle_handover_ue_context_push(ue_index_t source_ue_index, ue_index_t target_ue_index)
{
  // Transfer NGAP UE Context to new UE and remove the old context
  ngap_entity->update_ue_index(target_ue_index, source_ue_index);
  // Transfer E1AP UE Context to new UE and remove old context
  cu_up_db.get_cu_up(uint_to_cu_up_index(0)).update_ue_index(target_ue_index, source_ue_index);
}

optional<rrc_meas_cfg> cu_cp_impl::handle_measurement_config_request(ue_index_t             ue_index,
                                                                     nr_cell_id_t           nci,
                                                                     optional<rrc_meas_cfg> current_meas_config)
{
  return cell_meas_mng.get_measurement_config(ue_index, nci, current_meas_config);
}

void cu_cp_impl::handle_measurement_report(const ue_index_t ue_index, const rrc_meas_results& meas_results)
{
  cell_meas_mng.report_measurement(ue_index, meas_results);
}

void cu_cp_impl::handle_cell_config_update_request(nr_cell_id_t                           nci,
                                                   const serving_cell_meas_config&        serv_cell_cfg,
                                                   std::vector<neighbor_cell_meas_config> ncells)
{
  cell_meas_mng.update_cell_config(nci, serv_cell_cfg, ncells);
}

void cu_cp_impl::handle_ue_removal_request(ue_index_t ue_index)
{
  du_index_t    du_index    = get_du_index_from_ue_index(ue_index);
  cu_up_index_t cu_up_index = uint_to_cu_up_index(0); // TODO: Update when mapping from UE index to CU-UP exists
  auto          e1_adapter  = e1ap_adapters.find(cu_up_index);
  routine_mng.start_ue_removal_routine(ue_index,
                                       rrc_du_adapters.at(du_index),
                                       e1_adapter != e1ap_adapters.end() ? &e1_adapter->second : nullptr,
                                       f1ap_adapters.at(du_index),
                                       ngap_adapter,
                                       cell_meas_mng,
                                       ue_mng,
                                       logger);
}

// private

void cu_cp_impl::handle_du_processor_creation(du_index_t                       du_index,
                                              f1ap_ue_context_removal_handler& f1ap_handler,
                                              f1ap_statistics_handler&         f1ap_statistic_handler,
                                              rrc_ue_removal_handler&          rrc_handler,
                                              rrc_du_statistics_handler&       rrc_statistic_handler)
{
  f1ap_adapters[du_index] = {};
  f1ap_adapters.at(du_index).connect_f1ap(f1ap_handler, f1ap_statistic_handler);
  rrc_du_adapters[du_index] = {};
  rrc_du_adapters.at(du_index).connect_rrc_du(rrc_handler, rrc_statistic_handler);
}

void cu_cp_impl::handle_rrc_ue_creation(ue_index_t                          ue_index,
                                        rrc_ue_interface&                   rrc_ue,
                                        ngap_du_processor_control_notifier& ngap_du_notifier)
{
  // Store NGAP to DU processor notifier
  ngap_du_processor_ctrl_notifiers.emplace(get_du_index_from_ue_index(ue_index), ngap_du_notifier);

  // Connect RRC UE to NGAP to RRC UE adapter
  ue_mng.get_ngap_rrc_ue_adapter(ue_index).connect_rrc_ue(&rrc_ue.get_rrc_dl_nas_message_handler(),
                                                          &rrc_ue.get_rrc_ue_init_security_context_handler(),
                                                          &rrc_ue.get_rrc_ue_handover_preparation_handler());

  // Connect cu-cp to rrc ue adapters
  ue_mng.get_cu_cp_rrc_ue_adapter(ue_index).connect_rrc_ue(rrc_ue.get_rrc_ue_context_handler());
  ue_mng.get_rrc_ue_cu_cp_adapter(ue_index).connect_cu_cp(
      get_cu_cp_rrc_ue_interface(), get_cu_cp_ue_removal_handler(), controller, get_cu_cp_measurement_handler());
}

bool cu_cp_impl::handle_new_ngap_ue(ue_index_t ue_index)
{
  if (ngap_du_processor_ctrl_notifiers.find(get_du_index_from_ue_index(ue_index)) ==
      ngap_du_processor_ctrl_notifiers.end()) {
    logger.warning(
        "ue={}, du={}: ngap_du_processor_ctrl_notifier was not found", ue_index, get_du_index_from_ue_index(ue_index));
    return false;
  }

  return ue_mng.set_ue_ng_context(ue_index,
                                  ue_mng.get_ngap_rrc_ue_adapter(ue_index),
                                  ue_mng.get_ngap_rrc_ue_adapter(ue_index),
                                  ngap_du_processor_ctrl_notifiers.at(get_du_index_from_ue_index(ue_index)));
}

void cu_cp_impl::on_statistics_report_timer_expired()
{
  // Get number of F1AP UEs
  unsigned nof_f1ap_ues = 0;
  for (auto& f1ap_adapter_pair : f1ap_adapters) {
    nof_f1ap_ues += f1ap_adapter_pair.second.get_nof_ues();
  }

  // Get number of RRC UEs
  unsigned nof_rrc_ues = 0;
  for (auto& rrc_du_adapter_pair : rrc_du_adapters) {
    nof_rrc_ues += rrc_du_adapter_pair.second.get_nof_ues();
  }

  // Get number of NGAP UEs
  unsigned nof_ngap_ues = 0;
  nof_ngap_ues          = ngap_adapter.get_nof_ues();

  // Get number of E1AP UEs
  unsigned nof_e1ap_ues = 0;
  for (auto& e1ap_adapter_pair : e1ap_adapters) {
    nof_e1ap_ues += e1ap_adapter_pair.second.get_nof_ues();
  }

  // Get number of CU-CP UEs
  unsigned nof_cu_cp_ues = 0;
  nof_cu_cp_ues          = ue_mng.get_nof_ues();

  // Log statistics
  logger.debug("num_f1ap_ues={} num_rrc_ues={} num_ngap_ues={} num_e1ap_ues={} num_cu_cp_ues={}",
               nof_f1ap_ues,
               nof_rrc_ues,
               nof_ngap_ues,
               nof_e1ap_ues,
               nof_cu_cp_ues);

  // Restart timer
  statistics_report_timer.set(cfg.statistics_report_period,
                              [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
  statistics_report_timer.run();
}
