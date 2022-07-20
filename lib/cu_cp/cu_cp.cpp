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
#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"
#include "srsgnb/ngap/ngap_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

void assert_cu_cp_configuration_valid(const cu_cp_configuration& cfg)
{
  srsran_assert(cfg.cu_executor != nullptr, "Invalid CU-CP executor");
  srsran_assert(cfg.f1c_notifier != nullptr, "Invalid F1C notifier");
}

cu_cp::cu_cp(const cu_cp_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_cp_configuration_valid(cfg);

  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_DUS; ++i) {
    du_ctrl_loop.emplace(i, number_of_pending_procedures);
  }

  // Create layers
  ngap_entity = create_ngap();

  // connect event notifier to layers
  f1ap_ev_notifier.connect(*this);
}

cu_cp::~cu_cp()
{
  stop();
}

void cu_cp::start() {}

void cu_cp::stop() {}

f1c_message_handler& cu_cp::get_f1c_message_handler(du_index_t du_index)
{
  auto& du_it = find_du(du_index);

  return du_it.get_f1c_message_handler();
}

void cu_cp::on_new_connection()
{
  logger.info("New DU connection - adding DU");
  add_du();
}

void cu_cp::handle_du_remove_request(const du_index_t du_index)
{
  logger.info("removing DU {}", du_index);
  remove_du(du_index);
}

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

void cu_cp::on_amf_connection()
{
  // inform all connected DU objects about the new connection
  for (auto& du : du_db) {
    du->get_amf_connection_handler().handle_amf_connection();
  }
}

void cu_cp::on_amf_connection_drop()
{
  // inform all DU objects about the AMF connection drop
  for (auto& du : du_db) {
    du->get_amf_connection_handler().handle_amf_connection_drop();
  }
}

// private

/// Create DU object with valid index
void cu_cp::add_du()
{
  du_processor_config_t du_cfg = {};
  du_cfg.f1c_du_mgmt_notifier  = &f1ap_ev_notifier;
  du_cfg.f1c_notifier          = cfg.f1c_notifier;

  std::unique_ptr<du_processor> du = std::make_unique<du_processor>(std::move(du_cfg));

  du_index_t du_index = get_next_du_index();
  if (du_index == INVALID_DU_INDEX) {
    logger.error("DU connection failed - maximum number of DUs connected ({})", MAX_NOF_DUS);
    return;
  }

  du->get_context().du_index = du_index;

  srsran_assert(du->get_context().du_index < MAX_NOF_DUS, "Invalid du_index={}", du->get_context().du_index);

  // Create DU object
  du_db.emplace(du_index, std::move(du));

  return;
}

void cu_cp::remove_du(du_index_t du_index)
{
  // Note: The caller of this function can be a DU procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the DU. This is achieved via a scheduled async task

  srsran_assert(du_index < MAX_NOF_DUS, "Invalid du_index={}", du_index);
  logger.debug("Scheduling du_index={} deletion", du_index);

  // Schedule DU removal task
  du_ctrl_loop[du_index].schedule([this, du_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    srsran_assert(du_db.contains(du_index), "Remove DU called for inexistent du_index={}", du_index);
    du_db.erase(du_index);
    logger.info("Removed du_index={}", du_index);
    CORO_RETURN();
  });
}

du_processor& cu_cp::find_du(du_index_t du_index)
{
  srsran_assert(du_index < MAX_NOF_DUS, "Invalid du_index={}", du_index);
  srsran_assert(du_db.contains(du_index), "DU not found du_index={}", du_index);
  return *du_db[du_index];
}

du_processor& cu_cp::find_du(uint64_t packed_nr_cell_id)
{
  auto du_it = du_dict.find(packed_nr_cell_id);
  srsran_assert(du_it != du_dict.end(), "DU not found packed_nr_cell_id={}", packed_nr_cell_id);

  return *du_it->second;
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