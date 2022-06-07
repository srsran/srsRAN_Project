/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ue_manager.h"
#include "procedures/ue_creation_procedure.h"
#include "procedures/ue_deletion_procedure.h"

using namespace srsgnb;
using namespace srs_du;

du_ue_manager::du_ue_manager(du_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  std::fill(rnti_to_ue_index.begin(), rnti_to_ue_index.end(), -1);

  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_DU_UES; ++i) {
    ue_ctrl_loop.emplace(i, number_of_pending_procedures);
  }
}

void du_ue_manager::handle_ue_create_request(const ul_ccch_indication_message& msg)
{
  // Search unallocated UE index with no pending events.
  du_ue_index_t ue_idx_candidate = MAX_NOF_DU_UES;
  for (size_t i = 0; i < ue_ctrl_loop.size(); ++i) {
    if (not ue_db.contains(i) and ue_ctrl_loop[i].empty()) {
      ue_idx_candidate = to_du_ue_index(i);
      break;
    }
  }

  // Enqueue UE creation procedure
  ue_ctrl_loop[ue_idx_candidate].schedule<ue_creation_procedure>(ue_idx_candidate, msg, cfg, *this);
}

void du_ue_manager::handle_ue_delete_request(const du_ue_delete_message& msg)
{
  // Enqueue UE deletion procedure
  ue_ctrl_loop[msg.ue_index].schedule<ue_deletion_procedure>(msg, cfg, *this);
}

du_ue_context* du_ue_manager::find_ue(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

du_ue_context* du_ue_manager::find_rnti(rnti_t rnti)
{
  if (rnti_to_ue_index[rnti % MAX_NOF_DU_UES] < 0) {
    return nullptr;
  }
  return &ue_db[rnti_to_ue_index[rnti % MAX_NOF_DU_UES]];
}

du_ue_context* du_ue_manager::add_ue(du_ue_context ue_ctx)
{
  srsran_assert(ue_ctx.rnti != INVALID_RNTI, "Invalid RNTI");
  srsran_assert(ue_ctx.ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_ctx.ue_index);

  if (ue_db.contains(ue_ctx.ue_index) or rnti_to_ue_index[ue_ctx.rnti % MAX_NOF_DU_UES] >= 0) {
    // UE already existed with same ue_index
    return nullptr;
  }

  // Create UE object
  du_ue_index_t ue_index = ue_ctx.ue_index;
  ue_db.emplace(ue_index, std::move(ue_ctx));
  auto& u = ue_db[ue_index];

  // Update RNTI -> UE index map
  srsran_sanity_check(rnti_to_ue_index[u.rnti % MAX_NOF_DU_UES] < 0, "Invalid RNTI=0x{:x}", u.rnti);
  rnti_to_ue_index[u.rnti % MAX_NOF_DU_UES] = ue_index;

  return &u;
}

void du_ue_manager::remove_ue(du_ue_index_t ue_index)
{
  // Note: The caller of this function can be a UE procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the UE. This achieved via a scheduled async task

  srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ueId={}", ue_index);
  logger.debug("Scheduling ueId={} deletion", ue_index);

  // Schedule UE removal task
  ue_ctrl_loop[ue_index].schedule([this, ue_index](coro_context<async_task<void> >& ctx) {
    CORO_BEGIN(ctx);
    srsran_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ueId={}", ue_index);
    rnti_to_ue_index[ue_db[ue_index].rnti % MAX_NOF_DU_UES] = -1;
    ue_db.erase(ue_index);
    logger.info("Removed ueId={}", ue_index);
    CORO_RETURN();
  });
}
