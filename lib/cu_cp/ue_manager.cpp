/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ue_manager::ue_manager(cu_cp_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_UES; ++i) {
    ue_ctrl_loop.emplace(i, number_of_pending_procedures);
  }
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

ue_context* ue_manager::add_ue(ue_context ue_ctx)
{
  srsran_assert(ue_ctx.ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_ctx.ue_index);

  if (ue_db.contains(ue_ctx.ue_index)) {
    // UE already existed with same ue_index
    return nullptr;
  }

  // Create UE object
  ue_index_t ue_index = ue_ctx.ue_index;
  ue_db.emplace(ue_index, std::move(ue_ctx));
  auto& u = ue_db[ue_index];

  return &u;
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  // Note: The caller of this function can be a UE procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the UE. This is achieved via a scheduled async task

  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
  logger.debug("Scheduling ueId={} deletion", ue_index);

  // Schedule UE removal task
  ue_ctrl_loop[ue_index].schedule([this, ue_index](coro_context<async_task<void> >& ctx) {
    CORO_BEGIN(ctx);
    srsran_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ueId={}", ue_index);
    ue_db.erase(ue_index);
    logger.info("Removed ueId={}", ue_index);
    CORO_RETURN();
  });
}

ue_index_t ue_manager::get_next_ue_index()
{
  ue_index_t new_index;
  do {
    new_index = int_to_ue_index(next_ue_index.fetch_add(1, std::memory_order_relaxed));
  } while (ue_db.contains(new_index));
  return new_index;
}
