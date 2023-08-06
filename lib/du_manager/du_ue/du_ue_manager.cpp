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

#include "du_ue_manager.h"
#include "../procedures/procedure_logger.h"
#include "../procedures/ue_configuration_procedure.h"
#include "../procedures/ue_creation_procedure.h"
#include "../procedures/ue_deletion_procedure.h"
#include "srsran/gtpu/gtpu_teid_pool_factory.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_du;

du_ue_manager::du_ue_manager(du_manager_params& cfg_, du_ran_resource_manager& cell_res_alloc_) :
  cfg(cfg_),
  cell_res_alloc(cell_res_alloc_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  f1u_teid_pool(create_gtpu_allocator({MAX_NOF_DU_UES * MAX_NOF_DRBS}))
{
  // Initialize a control loop for all UE indexes.
  const size_t max_number_of_pending_procedures = 16U;
  for (size_t i = 0; i < MAX_NOF_DU_UES; ++i) {
    ue_ctrl_loop.emplace(i, max_number_of_pending_procedures);
  }
}

du_ue_index_t du_ue_manager::find_unused_du_ue_index()
{
  // Search unallocated UE index with no pending events.
  for (size_t i = 0; i < ue_ctrl_loop.size(); ++i) {
    du_ue_index_t ue_index = to_du_ue_index(i);
    if (not ue_db.contains(ue_index) and ue_ctrl_loop[ue_index].empty()) {
      return ue_index;
    }
  }
  return INVALID_DU_UE_INDEX;
}

void du_ue_manager::handle_ue_create_request(const ul_ccch_indication_message& msg)
{
  du_ue_index_t ue_idx_candidate = find_unused_du_ue_index();
  if (ue_idx_candidate == INVALID_DU_UE_INDEX) {
    logger.warning("No available UE index for UE creation");
    return;
  }

  // Enqueue UE creation procedure
  ue_ctrl_loop[ue_idx_candidate].schedule<ue_creation_procedure>(
      du_ue_creation_request{ue_idx_candidate, msg.cell_index, msg.tc_rnti, msg.subpdu.copy()},
      *this,
      cfg,
      cell_res_alloc);
}

async_task<f1ap_ue_context_creation_response>
du_ue_manager::handle_ue_create_request(const f1ap_ue_context_creation_request& msg)
{
  srsran_assert(msg.ue_index != INVALID_DU_UE_INDEX, "Invalid DU UE index");
  srsran_assert(not ue_db.contains(msg.ue_index), "Creating a ue={} but it already exists", msg.ue_index);

  // Initiate UE creation procedure and respond back to F1AP with allocated C-RNTI.
  return launch_async([this, msg](coro_context<async_task<f1ap_ue_context_creation_response>>& ctx) {
    CORO_BEGIN(ctx);

    CORO_AWAIT(launch_async<ue_creation_procedure>(
        du_ue_creation_request{msg.ue_index, msg.pcell_index, INVALID_RNTI, {}}, *this, cfg, cell_res_alloc));

    bool result = ue_db.contains(msg.ue_index);
    CORO_RETURN(f1ap_ue_context_creation_response{result, result ? find_ue(msg.ue_index)->rnti : INVALID_RNTI});
  });
}

async_task<f1ap_ue_context_update_response>
du_ue_manager::handle_ue_config_request(const f1ap_ue_context_update_request& msg)
{
  return launch_async<ue_configuration_procedure>(msg, *this, cfg);
}

async_task<void> du_ue_manager::handle_ue_delete_request(const f1ap_ue_delete_request& msg)
{
  // Enqueue UE deletion procedure
  return launch_async<ue_deletion_procedure>(msg, *this, cfg);
}

void du_ue_manager::handle_reestablishment_request(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index)
{
  srsran_assert(get_ues().contains(new_ue_index), "Invalid UE index={}", new_ue_index);
  srsran_assert(get_ues().contains(old_ue_index), "Invalid UE index={}", old_ue_index);

  // Reset the cellGroupConfig of the new UE context, so that previous changes are included in the next UEContextUpdate.
  du_ue& new_ue                  = *get_ues()[new_ue_index];
  new_ue.reestablishment_pending = true;

  // Delete the old UE context.
  schedule_async_task(old_ue_index, handle_ue_delete_request(f1ap_ue_delete_request{old_ue_index}));
}

async_task<void> du_ue_manager::stop()
{
  auto ue_it = ue_db.begin();
  return launch_async([this, ue_it, proc_logger = du_procedure_logger{logger, "DU UE Manager stop"}](
                          coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    proc_logger.log_proc_started();

    // Disconnect all UEs RLC->MAC buffer state adapters.
    for (ue_it = ue_db.begin(); ue_it != ue_db.end(); ++ue_it) {
      for (auto& srb : (*ue_it)->bearers.srbs()) {
        srb.connector.rlc_tx_buffer_state_notif.disconnect();
      }
      for (auto& drb_pair : (*ue_it)->bearers.drbs()) {
        du_ue_drb& drb = *drb_pair.second;
        drb.connector.rlc_tx_buffer_state_notif.disconnect();
      }
    }

    // Disconnect notifiers of all UEs bearers from within the ue_executors context.
    for (ue_it = ue_db.begin(); ue_it != ue_db.end(); ++ue_it) {
      CORO_AWAIT_VALUE(bool res, execute_on(cfg.services.ue_execs.executor((*ue_it)->ue_index)));
      if (not res) {
        CORO_EARLY_RETURN();
      }

      for (auto& srb : (*ue_it)->bearers.srbs()) {
        srb.disconnect();
      }

      for (auto& drb_pair : (*ue_it)->bearers.drbs()) {
        du_ue_drb& drb = *drb_pair.second;

        drb.disconnect();
      }
    }

    proc_logger.log_progress("All UEs are disconnected");

    CORO_AWAIT_VALUE(bool res, execute_on(cfg.services.du_mng_exec));
    if (not res) {
      CORO_EARLY_RETURN();
    }

    // Cancel all pending procedures.
    for (ue_it = ue_db.begin(); ue_it != ue_db.end(); ++ue_it) {
      eager_async_task<void> ctrl_loop = ue_ctrl_loop[(*ue_it)->ue_index].request_stop();

      // destroy ctrl_loop by letting it go out of scope.
    }

    proc_logger.log_progress("All UE procedures are interrupted");

    proc_logger.log_proc_completed();

    CORO_RETURN();
  });
}

du_ue* du_ue_manager::find_ue(du_ue_index_t ue_index)
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);
  return ue_db.contains(ue_index) ? ue_db[ue_index].get() : nullptr;
}

du_ue* du_ue_manager::find_rnti(rnti_t rnti)
{
  auto it = rnti_to_ue_index.find(rnti);
  srsran_assert(ue_db.contains(it->second), "Detected invalid container state for rnti={:#x}", rnti);
  return it != rnti_to_ue_index.end() ? ue_db[it->second].get() : nullptr;
}

du_ue* du_ue_manager::add_ue(std::unique_ptr<du_ue> ue_ctx)
{
  if (not is_du_ue_index_valid(ue_ctx->ue_index) or (not is_crnti(ue_ctx->rnti) and ue_ctx->rnti != INVALID_RNTI)) {
    // UE identifiers are invalid.
    return nullptr;
  }

  if (ue_db.contains(ue_ctx->ue_index) or (is_crnti(ue_ctx->rnti) and rnti_to_ue_index.count(ue_ctx->rnti) > 0)) {
    // UE already existed with same ue_index or C-RNTI.
    return nullptr;
  }

  // Create UE context object
  du_ue_index_t ue_index = ue_ctx->ue_index;
  ue_db.insert(ue_index, std::move(ue_ctx));
  auto& u = ue_db[ue_index];

  // Update RNTI -> UE index map
  if (u->rnti != INVALID_RNTI) {
    rnti_to_ue_index.insert(std::make_pair(u->rnti, ue_index));
  }

  return u.get();
}

void du_ue_manager::remove_ue(du_ue_index_t ue_index)
{
  // Note: The caller of this function can be a UE procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the UE. This achieved via a scheduled async task

  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);
  logger.debug("ue={}: Scheduled deletion of UE context", ue_index);

  // Schedule UE removal task
  ue_ctrl_loop[ue_index].schedule([this, ue_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    srsran_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ueId={}", ue_index);
    rnti_to_ue_index.erase(ue_db[ue_index]->rnti);
    ue_db.erase(ue_index);
    logger.debug("ue={}: Freeing UE context", ue_index);
    CORO_RETURN();
  });
}

void du_ue_manager::update_crnti(du_ue_index_t ue_index, rnti_t crnti)
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue index={}", ue_index);
  srsran_assert(is_crnti(crnti), "Invalid C-RNTI={:#x}", crnti);
  srsran_assert(ue_db.contains(ue_index), "Update C-RNTI called for inexistent ueId={}", ue_index);

  // Update RNTI -> UE index map
  rnti_to_ue_index.erase(ue_db[ue_index]->rnti);
  rnti_to_ue_index.insert(std::make_pair(crnti, ue_index));

  // Update UE context
  ue_db[ue_index]->rnti = crnti;
}

void du_ue_manager::handle_radio_link_failure(du_ue_index_t ue_index, rlf_cause cause)
{
  if (find_ue(ue_index) == nullptr) {
    logger.warning("Discarding RLF for unknown UE index={}", ue_index);
    return;
  }

  // Start F1AP UE Release Request (gNB-initiated) procedure with cause set to RLF.
  cfg.f1ap.ue_mng.handle_ue_context_release_request(srs_du::f1ap_ue_context_release_request{ue_index, cause});
}
