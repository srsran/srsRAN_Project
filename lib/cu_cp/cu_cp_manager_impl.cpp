/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_manager_impl.h"
#include "../ran/bcd_helpers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

cu_cp_manager_impl::cu_cp_manager_impl(const cu_cp_manager_config_t& cfg_) :
  cfg(cfg_), logger(cfg_.logger), main_ctrl_loop(128)
{
  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_DUS; ++i) {
    du_ctrl_loop.emplace(i, number_of_pending_procedures);
  }

  ctx = {}; // make it compile
}

du_processor* cu_cp_manager_impl::find_du(du_index_t du_index)
{
  srsran_assert(du_index < MAX_NOF_DUS, "Invalid du_index={}", du_index);
  return du_db.contains(du_index) ? du_db[du_index].get() : nullptr;
}

du_processor* cu_cp_manager_impl::find_du(uint64_t packed_nr_cell_id)
{
  auto du_it = du_dict.find(packed_nr_cell_id);
  if (du_it != du_dict.end()) {
    return du_it->second;
  }
  return nullptr;
}

/// Create DU object with valid index
void cu_cp_manager_impl::add_du()
{
  std::unique_ptr<du_processor> du       = std::unique_ptr<du_processor>(new du_processor(cfg));
  du_index_t                    du_index = get_next_du_index();
  du->get_context().du_index             = du_index;

  srsran_assert(du->get_context().du_index < MAX_NOF_DUS, "Invalid du_index={}", du->get_context().du_index);

  // Create DU object
  du_db.emplace(du_index, std::move(du));

  return;
}

void cu_cp_manager_impl::remove_du(du_index_t du_index)
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

size_t cu_cp_manager_impl::get_nof_dus() const
{
  return du_db.size();
}

du_index_t cu_cp_manager_impl::get_next_du_index()
{
  for (int du_idx_int = MIN_DU_INDEX; du_idx_int < MAX_NOF_DUS; du_idx_int++) {
    du_index_t du_idx = int_to_du_index(du_idx_int);
    if (!du_db.contains(du_idx)) {
      return du_idx;
    }
  }
  logger.error("No du index available");
  return INVALID_DU_INDEX;
}

size_t cu_cp_manager_impl::get_nof_ues() const
{
  size_t nof_ues = 0;
  for (auto& du : du_db) {
    nof_ues += du->get_nof_ues();
  }
  return nof_ues;
}

f1c_message_handler* cu_cp_manager_impl::get_f1c_message_handler(const du_index_t du_index)
{
  auto du_it = cu_cp_manager_impl::find_du(du_index);
  if (du_it == nullptr) {
    logger.error("Could not find DU with du_index={}", du_index);
    return nullptr;
  }

  return du_it->get_f1c_message_handler();
}

void cu_cp_manager_impl::on_new_connection()
{
  logger.info("New DU connection - adding DU");
  cu_cp_manager_impl::add_du();
}

void cu_cp_manager_impl::handle_du_remove_request(const du_index_t du_index)
{
  logger.info("removing DU {}", du_index);
  cu_cp_manager_impl::remove_du(du_index);
}
