/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/cu_up_processor_factory.h"
#include "srsran/ngap/ngap_factory.h"
#include <future>

using namespace srsran;
using namespace srs_cu_cp;

void assert_cu_cp_configuration_valid(const cu_cp_configuration& cfg)
{
  srsran_assert(cfg.cu_cp_executor != nullptr, "Invalid CU-CP executor");
  srsran_assert(cfg.ngap_notifier != nullptr, "Invalid NGAP notifier");
}

cu_cp_impl::cu_cp_impl(const cu_cp_configuration& config_) :
  cfg(config_),
  ue_mng(config_.ue_config, up_resource_manager_cfg{config_.rrc_config.drb_config}),
  mobility_mng(create_mobility_manager(config_.mobility_config.mobility_manager_config, du_db, ue_mng)),
  cell_meas_mng(create_cell_meas_manager(config_.mobility_config.meas_manager_config, cell_meas_ev_notifier)),
  du_db(du_repository_config{cfg,
                             timers,
                             *this,
                             du_processor_e1ap_notifier,
                             du_processor_ngap_notifier,
                             rrc_ue_ngap_notifier,
                             rrc_ue_ngap_notifier,
                             rrc_ue_cu_cp_notifier,
                             du_processor_task_sched,
                             ue_mng,
                             *cell_meas_mng,
                             amf_connected,
                             srslog::fetch_basic_logger("CU-CP")}),
  ue_task_sched(timers, *config_.cu_cp_executor),
  cu_up_task_sched(timers, *config_.cu_cp_executor)
{
  assert_cu_cp_configuration_valid(cfg);

  // connect event notifiers to layers
  cu_up_processor_ev_notifier.connect_cu_cp(get_cu_cp_cu_up_handler());
  ngap_cu_cp_ev_notifier.connect_cu_cp(get_cu_cp_ngap_handler(), du_db);
  e1ap_ev_notifier.connect_cu_cp(get_cu_cp_e1ap_handler());
  cell_meas_ev_notifier.connect_mobility_manager(*mobility_mng.get());
  rrc_ue_cu_cp_notifier.connect_cu_cp(this->get_cu_cp_rrc_ue_interface());

  // connect task schedulers
  ngap_task_sched.connect_cu_cp(ue_task_sched);
  du_processor_task_sched.connect_cu_cp(ue_task_sched);
  cu_up_processor_task_sched.connect_cu_cp(cu_up_task_sched);

  // Create NGAP.
  ngap_entity = create_ngap(
      cfg.ngap_config, ngap_cu_cp_ev_notifier, ngap_task_sched, ue_mng, *cfg.ngap_notifier, *cfg.cu_cp_executor);
  ngap_adapter.connect_ngap(ngap_entity->get_ngap_connection_manager(),
                            ngap_entity->get_ngap_control_message_handler());
  du_processor_ngap_notifier.connect_ngap(ngap_entity->get_ngap_control_message_handler());
  rrc_ue_ngap_notifier.connect_ngap(ngap_entity->get_ngap_nas_message_handler(),
                                    ngap_entity->get_ngap_control_message_handler());

  routine_mng = std::make_unique<cu_cp_routine_manager>(ngap_adapter, ngap_cu_cp_ev_notifier, cu_up_db);
}

cu_cp_impl::~cu_cp_impl()
{
  stop();
}

void cu_cp_impl::start()
{
  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (not cfg.cu_cp_executor->execute([this, &p]() {
        // start NG setup procedure.
        routine_mng->start_initial_cu_cp_setup_routine(cfg.ngap_config);
        p.set_value();
      })) {
    report_fatal_error("Failed to initiate CU-CP setup.");
  }

  // Block waiting for CU-CP setup to complete.
  fut.wait();
}

void cu_cp_impl::stop() {}

size_t cu_cp_impl::get_nof_cu_ups() const
{
  return cu_up_db.size();
}

e1ap_message_handler& cu_cp_impl::get_e1ap_message_handler(cu_up_index_t cu_up_index)
{
  auto& cu_up_it = find_cu_up(cu_up_index);
  return cu_up_it.get_e1ap_message_handler();
}

ngap_message_handler& cu_cp_impl::get_ngap_message_handler()
{
  return *ngap_entity;
};

ngap_event_handler& cu_cp_impl::get_ngap_event_handler()
{
  return *ngap_entity;
}

void cu_cp_impl::handle_rrc_ue_creation(du_index_t                          du_index,
                                        ue_index_t                          ue_index,
                                        rrc_ue_interface&                   rrc_ue,
                                        ngap_du_processor_control_notifier& ngap_du_notifier)
{
  ngap_rrc_ue_ev_notifiers.emplace(ue_index_to_uint(ue_index));

  ngap_rrc_ue_adapter& rrc_ue_adapter = ngap_rrc_ue_ev_notifiers[ue_index_to_uint(ue_index)];
  ngap_entity->create_ngap_ue(ue_index, rrc_ue_adapter, rrc_ue_adapter, ngap_du_notifier);
  rrc_ue_adapter.connect_rrc_ue(&rrc_ue.get_rrc_dl_nas_message_handler(),
                                &rrc_ue.get_rrc_ue_init_security_context_handler(),
                                &rrc_ue.get_rrc_ue_handover_preparation_handler(),
                                &ue_mng.find_du_ue(ue_index)->get_up_resource_manager());

  // Create and connect cu-cp to rrc ue adapter
  cu_cp_rrc_ue_ev_notifiers[ue_index] = {};
  cu_cp_rrc_ue_ev_notifiers.at(ue_index).connect_rrc_ue(rrc_ue.get_rrc_ue_context_handler());
}

void cu_cp_impl::handle_new_cu_up_connection()
{
  cu_up_index_t cu_up_index = add_cu_up();
  if (cu_up_index == cu_up_index_t::invalid) {
    logger.error("Failed to add new CU-UP");
    return;
  }

  logger.info("Added CU-UP {}", cu_up_index);
}

void cu_cp_impl::handle_cu_up_remove_request(const cu_up_index_t cu_up_index)
{
  logger.info("removing CU-UP {}", cu_up_index);
  remove_cu_up(cu_up_index);
}

void cu_cp_impl::handle_bearer_context_inactivity_notification(const cu_cp_inactivity_notification& msg)
{
  du_db.handle_inactivity_notification(get_du_index_from_ue_index(msg.ue_index), msg);
}

void cu_cp_impl::handle_amf_connection()
{
  amf_connected = true;

  du_db.handle_amf_connection();
}

void cu_cp_impl::handle_amf_connection_drop()
{
  amf_connected = false;

  du_db.handle_amf_connection_drop();
}

rrc_reestablishment_ue_context_t
cu_cp_impl::handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index)
{
  rrc_reestablishment_ue_context_t reest_context;

  ue_index_t old_ue_index = ue_mng.get_ue_index(old_pci, old_c_rnti);
  if (old_ue_index == ue_index_t::invalid || old_ue_index == ue_index) {
    return reest_context;
  }

  // Get RRC Reestablishment UE Context from old UE
  reest_context = cu_cp_rrc_ue_ev_notifiers.at(old_ue_index).on_rrc_ue_context_transfer();

  reest_context.ue_index = old_ue_index;

  return reest_context;
}

void cu_cp_impl::handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index)
{
  // Transfer NGAP UE Context to new UE and remove the old context
  ue_mng.transfer_ngap_ue_context(ue_index, old_ue_index);

  // Transfer E1AP UE Context to new UE and remove old context
  cu_up_db.begin()->second->update_ue_index(ue_index, old_ue_index);

  // Remove old RRC UE and DU UE
  du_db.request_ue_removal(get_du_index_from_ue_index(old_ue_index), old_ue_index);
}

// private

/// Create CU-UP object with valid index
cu_up_index_t cu_cp_impl::add_cu_up()
{
  cu_up_index_t cu_up_index = get_next_cu_up_index();
  if (cu_up_index == cu_up_index_t::invalid) {
    logger.error("CU-UP connection failed - maximum number of CU-UPs connected ({})", MAX_NOF_CU_UPS);
    return cu_up_index_t::invalid;
  }

  // TODO: use real config
  cu_up_processor_config_t cu_up_cfg = {};

  std::unique_ptr<cu_up_processor_interface> cu_up = create_cu_up_processor(std::move(cu_up_cfg),
                                                                            cu_up_processor_ev_notifier,
                                                                            *cfg.e1ap_notifier,
                                                                            e1ap_ev_notifier,
                                                                            cu_up_processor_task_sched,
                                                                            *cfg.cu_cp_executor);

  cu_up->get_context().cu_up_index = cu_up_index;

  // Connect e1ap to DU processor
  du_processor_e1ap_notifier.connect_e1ap(cu_up->get_e1ap_bearer_context_manager());

  srsran_assert(cu_up->get_context().cu_up_index != cu_up_index_t::invalid,
                "Invalid cu_up_index={}",
                cu_up->get_context().cu_up_index);

  // Create CU-UP object
  cu_up_db.emplace(cu_up_index, std::move(cu_up));

  return cu_up_index;
}

void cu_cp_impl::remove_cu_up(cu_up_index_t cu_up_index)
{
  // Note: The caller of this function can be a CU-UP procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the CU-UP. This is achieved via a scheduled async task

  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  logger.debug("Scheduling cu_up_index={} deletion", cu_up_index);

  // Schedule CU-UP removal task
  cu_up_task_sched.handle_cu_up_async_task(cu_up_index,
                                           launch_async([this, cu_up_index](coro_context<async_task<void>>& ctx) {
                                             CORO_BEGIN(ctx);
                                             srsran_assert(cu_up_db.find(cu_up_index) != cu_up_db.end(),
                                                           "Remove CU-UP called for inexistent cu_up_index={}",
                                                           cu_up_index);
                                             cu_up_db.erase(cu_up_index);
                                             logger.info("Removed cu_up_index={}", cu_up_index);
                                             CORO_RETURN();
                                           }));
}

cu_up_processor_interface& cu_cp_impl::find_cu_up(cu_up_index_t cu_up_index)
{
  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  srsran_assert(cu_up_db.find(cu_up_index) != cu_up_db.end(), "CU-UP not found cu_up_index={}", cu_up_index);
  return *cu_up_db.at(cu_up_index);
}

cu_up_index_t cu_cp_impl::get_next_cu_up_index()
{
  for (int cu_up_idx_int = cu_up_index_to_uint(cu_up_index_t::min); cu_up_idx_int < MAX_NOF_CU_UPS; cu_up_idx_int++) {
    cu_up_index_t cu_up_idx = uint_to_cu_up_index(cu_up_idx_int);
    if (cu_up_db.find(cu_up_idx) == cu_up_db.end()) {
      return cu_up_idx;
    }
  }
  logger.error("No CU-UP index available");
  return cu_up_index_t::invalid;
}
