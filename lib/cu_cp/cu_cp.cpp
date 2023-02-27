/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp.h"
#include "routines/initial_cu_cp_setup_routine.h"
#include "srsran/cu_cp/cu_up_processor_factory.h"
#include "srsran/cu_cp/du_processor_factory.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsran/ngap/ngap_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

void assert_cu_cp_configuration_valid(const cu_cp_configuration& cfg)
{
  srsran_assert(cfg.cu_cp_executor != nullptr, "Invalid CU-CP executor");
  srsran_assert(cfg.f1ap_notifier != nullptr, "Invalid F1AP notifier");
  srsran_assert(cfg.ngap_notifier != nullptr, "Invalid NGAP notifier");
}

cu_cp::cu_cp(const cu_cp_configuration& config_) :
  cfg(config_), ue_task_sched(timers), du_task_sched(timers), cu_up_task_sched(timers)
{
  assert_cu_cp_configuration_valid(cfg);

  // connect event notifiers to layers
  f1ap_ev_notifier.connect_cu_cp(get_cu_cp_du_handler());
  cu_up_processor_ev_notifier.connect_cu_cp(get_cu_cp_cu_up_handler());
  ngap_cu_cp_ev_notifier.connect_cu_cp(get_cu_cp_ngap_connection_handler(), get_cu_cp_ngap_paging_handler());

  // connect task schedulers
  ngap_task_sched.connect_cu_cp(ue_task_sched);
  du_processor_task_sched.connect_cu_cp(ue_task_sched);
  cu_up_processor_task_sched.connect_cu_cp(cu_up_task_sched);

  // Create layers
  ngap_entity = create_ngap(
      cfg.ngap_config, ngap_cu_cp_ev_notifier, ngap_task_sched, ue_mng, *cfg.ngap_notifier, *cfg.cu_cp_executor);
  ngap_adapter.connect_ngap(*ngap_entity);

  routine_mng = std::make_unique<cu_cp_routine_manager>(ngap_adapter, ngap_cu_cp_ev_notifier, cu_up_db);
}

cu_cp::~cu_cp()
{
  stop();
}

void cu_cp::start()
{
  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  cfg.cu_cp_executor->execute([this, &p]() {
    // start NG setup procedure.
    routine_mng->start_initial_cu_cp_setup_routine(cfg.ngap_config);
    p.set_value();
  });

  // Block waiting for CU-CP setup to complete.
  fut.wait();
}

void cu_cp::stop() {}

size_t cu_cp::get_nof_dus() const
{
  return du_db.size();
}

size_t cu_cp::get_nof_ues() const
{
  size_t nof_ues = 0;
  for (auto& du : du_db) {
    nof_ues += du.second->get_nof_ues();
  }
  return nof_ues;
}

f1ap_message_handler& cu_cp::get_f1ap_message_handler(du_index_t du_index)
{
  auto& du_it = find_du(du_index);
  return du_it.get_f1ap_message_handler();
}

f1ap_statistics_handler& cu_cp::get_f1ap_statistics_handler(du_index_t du_index)
{
  auto& du_it = find_du(du_index);
  return du_it.get_f1ap_statistics_handler();
}

size_t cu_cp::get_nof_cu_ups() const
{
  return cu_up_db.size();
}

e1ap_message_handler& cu_cp::get_e1ap_message_handler(cu_up_index_t cu_up_index)
{
  auto& cu_up_it = find_cu_up(cu_up_index);
  return cu_up_it.get_e1ap_message_handler();
}

ngap_message_handler& cu_cp::get_ngap_message_handler()
{
  return *ngap_entity;
};

ngap_event_handler& cu_cp::get_ngap_event_handler()
{
  return *ngap_entity;
}

void cu_cp::handle_new_du_connection()
{
  du_index_t du_index = add_du();
  if (du_index == du_index_t::invalid) {
    logger.error("Failed to add new DU");
    return;
  }

  logger.info("Added DU {}", du_index);
  if (amf_connected) {
    du_db.at(du_index)->get_rrc_amf_connection_handler().handle_amf_connection();
  }
}

void cu_cp::handle_du_remove_request(const du_index_t du_index)
{
  logger.info("removing DU {}", du_index);
  remove_du(du_index);
}

void cu_cp::handle_rrc_ue_creation(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface* rrc_ue)
{
  ngap_rrc_ue_ev_notifiers.emplace(ue_index_to_uint(ue_index));

  ngap_rrc_ue_adapter&       rrc_ue_adapter       = ngap_rrc_ue_ev_notifiers[ue_index_to_uint(ue_index)];
  ngap_du_processor_adapter& du_processor_adapter = ngap_du_processor_ev_notifiers.at(du_index);
  ngap_entity->create_ngap_ue(ue_index, rrc_ue_adapter, rrc_ue_adapter, du_processor_adapter);
  rrc_ue_adapter.connect_rrc_ue(&rrc_ue->get_rrc_ue_dl_nas_message_handler(),
                                &rrc_ue->get_rrc_ue_control_message_handler(),
                                &rrc_ue->get_rrc_ue_init_security_context_handler());
}

void cu_cp::handle_new_cu_up_connection()
{
  cu_up_index_t cu_up_index = add_cu_up();
  if (cu_up_index == cu_up_index_t::invalid) {
    logger.error("Failed to add new CU-UP");
    return;
  }

  logger.info("Added CU-UP {}", cu_up_index);
}

void cu_cp::handle_cu_up_remove_request(const cu_up_index_t cu_up_index)
{
  logger.info("removing CU-UP {}", cu_up_index);
  remove_cu_up(cu_up_index);
}

void cu_cp::handle_amf_connection()
{
  amf_connected = true;

  // inform all connected DU objects about the new connection
  for (auto& du : du_db) {
    du.second->get_rrc_amf_connection_handler().handle_amf_connection();
  }
}

void cu_cp::handle_amf_connection_drop()
{
  amf_connected = false;

  // inform all DU objects about the AMF connection drop
  for (auto& du : du_db) {
    du.second->get_rrc_amf_connection_handler().handle_amf_connection_drop();
  }
}

void cu_cp::handle_paging_message(cu_cp_paging_message& msg)
{
  // Forward paging message to all DU processors
  for (auto& du : du_db) {
    du.second->get_du_processor_paging_handler().handle_paging_message(msg);
  }
}

// private

/// Create DU object with valid index
du_index_t cu_cp::add_du()
{
  du_index_t du_index = get_next_du_index();
  if (du_index == du_index_t::invalid) {
    logger.error("DU connection failed - maximum number of DUs connected ({})", MAX_NOF_DUS);
    return du_index_t::invalid;
  }

  // TODO: use real config
  du_processor_config_t du_cfg = {};
  du_cfg.du_index              = du_index;
  du_cfg.rrc_cfg               = cfg.rrc_config;

  std::unique_ptr<du_processor_interface> du = create_du_processor(std::move(du_cfg),
                                                                   du_processor_ev_notifier,
                                                                   f1ap_ev_notifier,
                                                                   *cfg.f1ap_notifier,
                                                                   du_processor_e1ap_notifier,
                                                                   rrc_ue_ngap_notifier,
                                                                   rrc_ue_ngap_notifier,
                                                                   du_processor_task_sched,
                                                                   ue_mng,
                                                                   *cfg.cu_cp_executor);

  du_processor_ev_notifier.connect_cu_cp(*this);
  rrc_ue_ngap_notifier.connect_ngap(*ngap_entity);
  ngap_du_processor_ev_notifiers[du_index] = {};
  ngap_du_processor_ev_notifiers.at(du_index).connect_du_processor(du.get());

  du->get_context().du_index = du_index;

  srsran_assert(du->get_context().du_index != du_index_t::invalid, "Invalid du_index={}", du->get_context().du_index);

  // Create DU object
  du_db.emplace(du_index, std::move(du));

  return du_index;
}

void cu_cp::remove_du(du_index_t du_index)
{
  // Note: The caller of this function can be a DU procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the DU. This is achieved via a scheduled async task

  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  logger.debug("Scheduling du_index={} deletion", du_index);

  // Schedule DU removal task
  du_task_sched.handle_du_async_task(
      du_index, launch_async([this, du_index](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        srsran_assert(du_db.find(du_index) != du_db.end(), "Remove DU called for inexistent du_index={}", du_index);

        // Remove DU
        du_db.erase(du_index);
        logger.info("Removed du_index={}", du_index);
        CORO_RETURN();
      }));
}

du_processor_interface& cu_cp::find_du(du_index_t du_index)
{
  srsran_assert(du_index != du_index_t::invalid, "Invalid du_index={}", du_index);
  srsran_assert(du_db.find(du_index) != du_db.end(), "DU not found du_index={}", du_index);
  return *du_db.at(du_index);
}

du_index_t cu_cp::get_next_du_index()
{
  for (int du_idx_int = du_index_to_uint(du_index_t::min); du_idx_int < MAX_NOF_DUS; du_idx_int++) {
    du_index_t du_idx = uint_to_du_index(du_idx_int);
    if (du_db.find(du_idx) == du_db.end()) {
      return du_idx;
    }
  }
  logger.error("No DU index available");
  return du_index_t::invalid;
}

/// Create CU-UP object with valid index
cu_up_index_t cu_cp::add_cu_up()
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

void cu_cp::remove_cu_up(cu_up_index_t cu_up_index)
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

cu_up_processor_interface& cu_cp::find_cu_up(cu_up_index_t cu_up_index)
{
  srsran_assert(cu_up_index != cu_up_index_t::invalid, "Invalid cu_up_index={}", cu_up_index);
  srsran_assert(cu_up_db.find(cu_up_index) != cu_up_db.end(), "CU-UP not found cu_up_index={}", cu_up_index);
  return *cu_up_db.at(cu_up_index);
}

cu_up_index_t cu_cp::get_next_cu_up_index()
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
