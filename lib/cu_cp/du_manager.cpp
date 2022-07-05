/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_manager.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_manager::du_manager(cu_cp_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_DUS; ++i) {
    du_ctrl_loop.emplace(i, number_of_pending_procedures);
  }
}

du_processor* du_manager::find_du(du_index_t du_index)
{
  srsran_assert(du_index < MAX_NOF_DUS, "Invalid du_index={}", du_index);
  return du_db.contains(du_index) ? du_db[du_index].get() : nullptr;
}

du_processor* du_manager::find_du(uint64_t packed_nr_cell_id)
{
  auto du_it = du_dict.find(packed_nr_cell_id);
  if (du_it != du_dict.end()) {
    return du_it->second;
  }
  return nullptr;
}

/// Create DU object with valid index
du_processor* du_manager::add_du(std::unique_ptr<du_processor> du)
{
  srsran_assert(du->get_context().du_index < MAX_NOF_DUS, "Invalid du_index={}", du->get_context().du_index);

  if (du_db.contains(du->get_context().du_index)) {
    // DU already existed with same du_index
    return nullptr;
  }

  // Create DU object
  du_index_t du_index = du->get_context().du_index;
  du_db.emplace(du_index, std::move(du));
  auto& u = du_db[du_index];

  // Add each of the DU's cells to our cell database using packed NCI as lookup key
  for (auto& cell : u->get_cell_db()) {
    du_dict.insert(std::make_pair(cell.cgi.nci.packed, u.get()));
  }

  return u.get();
}

void du_manager::remove_du(du_index_t du_index)
{
  // Note: The caller of this function can be a DU procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the DU. This is achieved via a scheduled async task

  srsran_assert(du_index < MAX_NOF_DUS, "Invalid du_index={}", du_index);
  logger.debug("Scheduling du_index={} deletion", du_index);

  // Schedule DU removal task
  du_ctrl_loop[du_index].schedule([this, du_index](coro_context<async_task<void> >& ctx) {
    CORO_BEGIN(ctx);
    srsran_assert(du_db.contains(du_index), "Remove DU called for inexistent du_index={}", du_index);
    du_db.erase(du_index);
    logger.info("Removed du_index={}", du_index);
    CORO_RETURN();
  });
}

size_t du_manager::get_nof_dus() const
{
  return du_db.size();
}

du_index_t du_manager::get_next_du_index()
{
  du_index_t new_index;
  do {
    new_index = int_to_du_index(next_du_index.fetch_add(1, std::memory_order_relaxed));
  } while (du_db.contains(new_index));
  return new_index;
}

void du_manager::handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg)
{
  logger.debug("Handling initial UL RRC message transfer");
  auto du_it = find_du(msg.msg->nrcgi.value.nrcell_id.to_number());
  if (du_it == nullptr) {
    logger.error("Could not find cell with cell_id={}", msg.msg->nrcgi.value.nrcell_id.to_number());
    return;
  }
  du_it->handle_initial_ul_rrc_message_transfer(msg);
}