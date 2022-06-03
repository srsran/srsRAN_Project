/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_du_manager.h"

using namespace srsgnb;
using namespace cu_cp;

cu_cp_du_manager::cu_cp_du_manager(cu_cp_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_CU_CP_DUS; ++i) {
    du_ctrl_loop.emplace(i, number_of_pending_procedures);
  }
}

cu_cp_du_context* cu_cp_du_manager::find_du(cu_cp_du_index_t du_index)
{
  srsran_assert(du_index < MAX_NOF_CU_CP_DUS, "Invalid du_index={}", du_index);
  return du_db.contains(du_index) ? &du_db[du_index] : nullptr;
}

cu_cp_du_context* cu_cp_du_manager::add_du(cu_cp_du_context du_ctx)
{
  srsran_assert(du_ctx.du_index < MAX_NOF_CU_CP_DUS, "Invalid du_index={}", du_ctx.du_index);

  if (du_db.contains(du_ctx.du_index)) {
    // DU already existed with same du_index
    return nullptr;
  }

  // Create DU object
  cu_cp_du_index_t du_index = du_ctx.du_index;
  du_db.emplace(du_index, std::move(du_ctx));
  auto& u = du_db[du_index];

  return &u;
}

void cu_cp_du_manager::remove_du(cu_cp_du_index_t du_index)
{
  // Note: The caller of this function can be a DU procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the DU. This is achieved via a scheduled async task

  srsran_assert(du_index < MAX_NOF_CU_CP_DUS, "Invalid du_index={}", du_index);
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
