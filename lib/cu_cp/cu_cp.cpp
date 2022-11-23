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
#include "procedures/initial_cu_cp_setup_procedure.h"
#include "srsgnb/cu_cp/cu_up_processor_factory.h"
#include "srsgnb/cu_cp/du_processor_factory.h"
#include "srsgnb/f1c/cu_cp/f1c_cu_factory.h"
#include "srsgnb/ngap/ngc_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

void assert_cu_cp_configuration_valid(const cu_cp_configuration& cfg)
{
  srsgnb_assert(cfg.cu_executor != nullptr, "Invalid CU-CP executor");
  srsgnb_assert(cfg.f1c_notifier != nullptr, "Invalid F1C notifier");
  srsgnb_assert(cfg.ngc_notifier != nullptr, "Invalid NGC notifier");
}

cu_cp::cu_cp(const cu_cp_configuration& config_) :
  cfg(config_), main_ctrl_loop(128), ue_task_sched(timers), du_task_sched(timers), cu_up_task_sched(timers)
{
  assert_cu_cp_configuration_valid(cfg);

  // Create layers
  ngc_entity = create_ngc(timers, *cfg.ngc_notifier);

  // connect event notifiers to layers
  f1c_ev_notifier.connect_cu_cp(*this);
  cu_up_processor_ev_notifier.connect_cu_cp(*this);

  start();
}

cu_cp::~cu_cp()
{
  stop();
}

void cu_cp::start()
{
  // TODO: use real config
  ngc_configuration ngc_cfg = {};

  // start NG setup procedure.
  main_ctrl_loop.schedule<initial_cu_cp_setup_procedure>(cfg.ngc_config, *ngc_entity, *this);
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
    nof_ues += du->get_nof_ues();
  }
  return nof_ues;
}

f1c_message_handler& cu_cp::get_f1c_message_handler(du_index_t du_index)
{
  auto& du_it = find_du(du_index);
  return du_it.get_f1c_message_handler();
}

f1c_statistics_handler& cu_cp::get_f1c_statistics_handler(du_index_t du_index)
{
  auto& du_it = find_du(du_index);
  return du_it.get_f1c_statistics_handler();
}

size_t cu_cp::get_nof_cu_ups() const
{
  return cu_up_db.size();
}

e1_message_handler& cu_cp::get_e1_message_handler(cu_up_index_t cu_up_index)
{
  auto& cu_up_it = find_cu_up(cu_up_index);
  return cu_up_it.get_e1_message_handler();
}

ngc_message_handler& cu_cp::get_ngc_message_handler()
{
  return *ngc_entity;
};

ngc_event_handler& cu_cp::get_ngc_event_handler()
{
  return *ngc_entity;
}

void cu_cp::on_new_du_connection()
{
  du_index_t du_index = add_du();
  logger.info("Added DU {}", du_index);
  if (du_index != INVALID_DU_INDEX && amf_connected) {
    du_db[du_index]->get_rrc_amf_connection_handler().handle_amf_connection();
  }
}

void cu_cp::handle_du_remove_request(const du_index_t du_index)
{
  logger.info("removing DU {}", du_index);
  remove_du(du_index);
}

void cu_cp::on_new_cu_up_connection()
{
  cu_up_index_t cu_up_index = add_cu_up();
  logger.info("Added CU-UP {}", cu_up_index);
}

void cu_cp::handle_cu_up_remove_request(const cu_up_index_t cu_up_index)
{
  logger.info("removing CU-UP {}", cu_up_index);
  remove_cu_up(cu_up_index);
}

void cu_cp::on_amf_connection()
{
  amf_connected = true;

  // inform all connected DU objects about the new connection
  for (auto& du : du_db) {
    du->get_rrc_amf_connection_handler().handle_amf_connection();
  }
}

void cu_cp::on_amf_connection_drop()
{
  amf_connected = false;

  // inform all DU objects about the AMF connection drop
  for (auto& du : du_db) {
    du->get_rrc_amf_connection_handler().handle_amf_connection_drop();
  }
}

// private

/// Create DU object with valid index
du_index_t cu_cp::add_du()
{
  du_index_t du_index = get_next_du_index();
  if (du_index == INVALID_DU_INDEX) {
    logger.error("DU connection failed - maximum number of DUs connected ({})", MAX_NOF_DUS);
    return INVALID_DU_INDEX;
  }

  // TODO: use real config
  du_processor_config_t du_cfg = {};

  std::unique_ptr<du_processor_interface> du = create_du_processor(
      std::move(du_cfg), f1c_ev_notifier, *cfg.f1c_notifier, rrc_ue_ngc_ev_notifier, du_processor_task_sched);

  rrc_ue_ngc_ev_notifier.connect_ngc(*ngc_entity);
  du_processor_task_sched.connect_cu_cp(ue_task_sched);

  // Add DU index to adapter
  rrc_ue_ngc_ev_notifier.set_du_index(du_index);

  du->get_context().du_index = du_index;

  srsgnb_assert(du->get_context().du_index < MAX_NOF_DUS, "Invalid du_index={}", du->get_context().du_index);

  // Create DU object
  du_db.emplace(du_index, std::move(du));

  return du_index;
}

void cu_cp::remove_du(du_index_t du_index)
{
  // Note: The caller of this function can be a DU procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the DU. This is achieved via a scheduled async task

  srsgnb_assert(du_index < MAX_NOF_DUS, "Invalid du_index={}", du_index);
  logger.debug("Scheduling du_index={} deletion", du_index);

  // Schedule DU removal task
  du_task_sched.handle_du_async_task(
      du_index, launch_async([this, du_index](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        srsgnb_assert(du_db.contains(du_index), "Remove DU called for inexistent du_index={}", du_index);
        du_db.erase(du_index);
        logger.info("Removed du_index={}", du_index);
        CORO_RETURN();
      }));
}

du_processor_interface& cu_cp::find_du(du_index_t du_index)
{
  srsgnb_assert(du_index < MAX_NOF_DUS, "Invalid du_index={}", du_index);
  srsgnb_assert(du_db.contains(du_index), "DU not found du_index={}", du_index);
  return *du_db[du_index];
}

du_index_t cu_cp::get_next_du_index()
{
  for (int du_idx_int = MIN_DU_INDEX; du_idx_int < MAX_NOF_DUS; du_idx_int++) {
    du_index_t du_idx = int_to_du_index(du_idx_int);
    if (!du_db.contains(du_idx)) {
      return du_idx;
    }
  }
  logger.error("No DU index available");
  return INVALID_DU_INDEX;
}

/// Create CU-UP object with valid index
cu_up_index_t cu_cp::add_cu_up()
{
  cu_up_index_t cu_up_index = get_next_cu_up_index();
  if (cu_up_index == INVALID_CU_UP_INDEX) {
    logger.error("CU-UP connection failed - maximum number of CU-UPs connected ({})", MAX_NOF_CU_UPS);
    return INVALID_CU_UP_INDEX;
  }

  // TODO: use real config
  cu_up_processor_config_t cu_up_cfg = {};

  std::unique_ptr<cu_up_processor_interface> cu_up = create_cu_up_processor(
      std::move(cu_up_cfg), cu_up_processor_ev_notifier, *cfg.e1_notifier, cu_up_processor_task_sched);

  cu_up_processor_task_sched.connect_cu_cp(cu_up_task_sched);
  cu_up->get_context().cu_up_index = cu_up_index;

  cu_up->start();

  srsgnb_assert(
      cu_up->get_context().cu_up_index < MAX_NOF_CU_UPS, "Invalid cu_up_index={}", cu_up->get_context().cu_up_index);

  // Create CU-UP object
  cu_up_db.emplace(cu_up_index, std::move(cu_up));

  return cu_up_index;
}

void cu_cp::remove_cu_up(cu_up_index_t cu_up_index)
{
  // Note: The caller of this function can be a CU-UP procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the CU-UP. This is achieved via a scheduled async task

  srsgnb_assert(cu_up_index < MAX_NOF_CU_UPS, "Invalid cu_up_index={}", cu_up_index);
  logger.debug("Scheduling cu_up_index={} deletion", cu_up_index);

  // Schedule CU-UP removal task
  cu_up_task_sched.handle_cu_up_async_task(
      cu_up_index, launch_async([this, cu_up_index](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        srsgnb_assert(cu_up_db.contains(cu_up_index), "Remove CU-UP called for inexistent cu_up_index={}", cu_up_index);
        cu_up_db.erase(cu_up_index);
        logger.info("Removed cu_up_index={}", cu_up_index);
        CORO_RETURN();
      }));
}

cu_up_processor_interface& cu_cp::find_cu_up(cu_up_index_t cu_up_index)
{
  srsgnb_assert(cu_up_index < MAX_NOF_CU_UPS, "Invalid cu_up_index={}", cu_up_index);
  srsgnb_assert(cu_up_db.contains(cu_up_index), "DU not found cu_up_index={}", cu_up_index);
  return *cu_up_db[cu_up_index];
}

cu_up_index_t cu_cp::get_next_cu_up_index()
{
  for (int cu_up_idx_int = MIN_CU_UP_INDEX; cu_up_idx_int < MAX_NOF_CU_UPS; cu_up_idx_int++) {
    cu_up_index_t cu_up_idx = int_to_cu_up_index(cu_up_idx_int);
    if (!cu_up_db.contains(cu_up_idx)) {
      return cu_up_idx;
    }
  }
  logger.error("No CU-UP index available");
  return INVALID_CU_UP_INDEX;
}