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
#include "procedures/ue_creation_procedure.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ue_manager::ue_manager(cu_cp_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_UES; ++i) {
    ue_ctrl_loop.emplace(i, number_of_pending_procedures);
  }
  std::fill(rnti_to_ue_index.begin(), rnti_to_ue_index.end(), -1);
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

ue_context* ue_manager::find_rnti(rnti_t rnti)
{
  if (rnti_to_ue_index[rnti % MAX_NOF_UES] < 0) {
    return nullptr;
  }
  return &ue_db[rnti_to_ue_index[rnti % MAX_NOF_UES]];
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

  // Update RNTI -> UE index map
  srsran_sanity_check(rnti_to_ue_index[u.c_rnti % MAX_NOF_UES] < 0, "Invalid RNTI=0x{:x}", u.c_rnti);
  rnti_to_ue_index[u.c_rnti % MAX_NOF_UES] = ue_index;

  return &u;
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  // Note: The caller of this function can be a UE procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the UE. This is achieved via a scheduled async task

  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
  logger.debug("Scheduling ueId={} deletion", ue_index);

  // Schedule UE removal task
  ue_ctrl_loop[ue_index].schedule([this, ue_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    srsran_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ueId={}", ue_index);
    ue_db.erase(ue_index);
    logger.info("Removed ueId={}", ue_index);
    CORO_RETURN();
  });
}

size_t ue_manager::get_nof_ues()
{
  return ue_db.size();
}

void ue_manager::handle_initial_ul_rrc_message_transfer(du_cell_index_t pcell_idx, const f1ap_initial_ul_rrc_msg& msg)
{
  // Search unallocated UE index with no pending events
  ue_index_t ue_idx_candidate = MAX_NOF_UES;
  for (size_t i = 0; i < ue_ctrl_loop.size(); ++i) {
    if (not ue_db.contains(i) and ue_ctrl_loop[i].empty()) {
      ue_idx_candidate = int_to_ue_index(i);
      break;
    }
  }

  if (ue_idx_candidate == MAX_NOF_UES) {
    logger.error("No free UE index found");
    return;
  }

  // Enqueue UE creation procedure
  ue_ctrl_loop[ue_idx_candidate].schedule<ue_creation_procedure>(ue_idx_candidate, pcell_idx, msg, cfg, *this);
}

void ue_manager::handle_ul_rrc_message_transfer(const f1ap_ul_rrc_msg& msg)
{
  logger.info("Handling UL RRC Message transfer.");

  // Convert RRCContainer to byte_buffer
  byte_buffer pdcp_pdu = make_byte_buffer(msg.msg->rrc_container.value.to_string());
  // TODO: Send pdcp_pdu to PDCP
}