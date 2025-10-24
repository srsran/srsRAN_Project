/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "../procedures/du_ue_reset_procedure.h"
#include "../procedures/du_ue_ric_configuration_procedure.h"
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
  for (size_t i = 0; i != MAX_NOF_DU_UES; ++i) {
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
  if (stop_accepting_ues) {
    logger.info("rnti={}: UL-CCCH indication dropped. Caused: The DU is being shut down.", msg.tc_rnti);
    return;
  }

  // Enqueue UE creation procedure
  ue_ctrl_loop[ue_idx_candidate].schedule<ue_creation_procedure>(
      du_ue_creation_request{ue_idx_candidate, msg.cell_index, msg.tc_rnti, msg.subpdu.copy(), msg.slot_rx},
      *this,
      cfg,
      cell_res_alloc);
}

async_task<f1ap_ue_context_creation_response>
du_ue_manager::handle_ue_create_request(const f1ap_ue_context_creation_request& msg)
{
  srsran_assert(msg.ue_index != INVALID_DU_UE_INDEX, "Invalid DU UE index");
  srsran_assert(
      not ue_db.contains(msg.ue_index), "Creating a ue={} but it already exists", fmt::underlying(msg.ue_index));
  if (stop_accepting_ues) {
    logger.info("ue={}: UE creation request ignored. Caused: The DU is being shut down.",
                fmt::underlying(msg.ue_index));
    return launch_no_op_task(f1ap_ue_context_creation_response{false, rnti_t::INVALID_RNTI});
  }

  // Initiate UE creation procedure and respond back to F1AP with allocated C-RNTI.
  return launch_async([this, msg](coro_context<async_task<f1ap_ue_context_creation_response>>& ctx) {
    CORO_BEGIN(ctx);

    CORO_AWAIT(launch_async<ue_creation_procedure>(
        du_ue_creation_request{msg.ue_index, msg.pcell_index, rnti_t::INVALID_RNTI, {}}, *this, cfg, cell_res_alloc));

    bool result = ue_db.contains(msg.ue_index);
    CORO_RETURN(f1ap_ue_context_creation_response{result, result ? find_ue(msg.ue_index)->rnti : rnti_t::INVALID_RNTI});
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
  return launch_async<ue_deletion_procedure>(msg.ue_index, *this, cfg);
}

async_task<void> du_ue_manager::handle_ue_deactivation_request(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    logger.warning("ue={}: UE deactivation request for inexistent UE index", fmt::underlying(ue_index));
    return launch_no_op_task();
  }
  return ue_db[ue_index].handle_activity_stop_request(false);
}

void du_ue_manager::handle_reestablishment_request(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index)
{
  srsran_assert(ue_db.contains(new_ue_index), "Invalid UE index={}", fmt::underlying(new_ue_index));
  auto old_ue_it = find_ue(old_ue_index);
  srsran_assert(old_ue_it != nullptr, "Invalid UE index={}", fmt::underlying(old_ue_index));
  auto& new_ue = ue_db[new_ue_index];

  // Retrieve the old UE context for the RRC connection reestablishment procedure, as defined in TS 48.473, 8.4.2.2 and
  // TS 38.401.
  new_ue.reestablished_cfg_pending = std::make_unique<du_ue_resource_config>(old_ue_it->resources.value());
  if (old_ue_it->resources.get_ue_capabilities().has_value()) {
    new_ue.reestablished_ue_caps_summary =
        std::make_unique<ue_capability_summary>(*old_ue_it->resources.get_ue_capabilities());
  }

  // Delete the old UE context.
  schedule_async_task(old_ue_index, handle_ue_delete_request(f1ap_ue_delete_request{old_ue_index}));
}

void du_ue_manager::handle_ue_config_applied(du_ue_index_t ue_index)
{
  srsran_assert(ue_db.contains(ue_index), "Invalid UE index={}", fmt::underlying(ue_index));

  // Notify UE resource configurator of config completion.
  ue_db[ue_index].resources.handle_ue_config_applied();

  // Forward configuration to MAC.
  cfg.mac.mgr.get_ue_configurator().handle_ue_config_applied(ue_index);
}

async_task<du_mac_sched_control_config_response>
du_ue_manager::handle_ue_config_request(const du_mac_sched_control_config& msg)
{
  return launch_async<du_ue_ric_configuration_procedure>(msg, *this, cfg);
}

async_task<void> du_ue_manager::stop()
{
  // Note: We cannot rely on the clean removal of UEs, as at the time the stop() is called, the lower layers may have
  // already stopped, and no lower-layer indications are arriving.
  if (stop_accepting_ues) {
    return launch_no_op_task();
  }
  stop_accepting_ues = true;

  auto ue_it = ue_db.begin();
  return launch_async([this, ue_it, proc_logger = du_procedure_logger{logger, "DU UE Manager stop"}](
                          coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    proc_logger.log_proc_started();

    // Disconnect notifiers of all UEs bearers from within the ue_executors context.
    for (ue_it = ue_db.begin(); ue_it != ue_db.end(); ++ue_it) {
      CORO_AWAIT(ue_it->handle_activity_stop_request(true));
    }

    proc_logger.log_progress("All UEs are disconnected");

    // Cancel all pending procedures.
    for (ue_it = ue_db.begin(); ue_it != ue_db.end(); ++ue_it) {
      eager_async_task<void> ctrl_loop = ue_ctrl_loop[ue_it->ue_index].request_stop();

      // destroy ctrl_loop by letting it go out of scope.
    }

    proc_logger.log_progress("All UE procedures are interrupted");

    proc_logger.log_proc_completed();

    CORO_RETURN();
  });
}

du_ue* du_ue_manager::find_ue(du_ue_index_t ue_index)
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue index={}", fmt::underlying(ue_index));
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}
const du_ue* du_ue_manager::find_ue(du_ue_index_t ue_index) const
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue index={}", fmt::underlying(ue_index));
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

du_ue* du_ue_manager::find_rnti(rnti_t rnti)
{
  auto it = rnti_to_ue_index.find(rnti);
  srsran_assert(ue_db.contains(it->second), "Detected invalid container state for rnti={}", rnti);
  return it != rnti_to_ue_index.end() ? &ue_db[it->second] : nullptr;
}

du_ue* du_ue_manager::find_f1ap_ue_id(gnb_du_ue_f1ap_id_t f1ap_ue_id)
{
  for (auto& ue : ue_db) {
    if (ue.f1ap_ue_id == f1ap_ue_id) {
      return &ue;
    }
  }
  return nullptr;
}

expected<du_ue*, std::string> du_ue_manager::add_ue(const du_ue_context&         ue_ctx,
                                                    ue_ran_resource_configurator ue_ran_res)
{
  if (not is_du_ue_index_valid(ue_ctx.ue_index) or
      (not is_crnti(ue_ctx.rnti) and ue_ctx.rnti != rnti_t::INVALID_RNTI)) {
    // UE identifiers are invalid.
    return make_unexpected(std::string("Invalid UE identifiers"));
  }

  if (ue_db.contains(ue_ctx.ue_index) or (is_crnti(ue_ctx.rnti) and rnti_to_ue_index.count(ue_ctx.rnti) > 0)) {
    // UE already existed with same ue_index or C-RNTI.
    return make_unexpected(std::string("UE already existed with same ue_index or C-RNTI"));
  }

  // Create UE context object
  du_ue_index_t ue_index = ue_ctx.ue_index;
  ue_db.emplace(ue_index, ue_ctx, *this, cfg, std::move(ue_ran_res));
  auto& u = ue_db[ue_index];

  // Update RNTI -> UE index map
  if (u.rnti != rnti_t::INVALID_RNTI) {
    rnti_to_ue_index.insert(std::make_pair(u.rnti, ue_index));
  }

  return &u;
}

void du_ue_manager::remove_ue(du_ue_index_t ue_index)
{
  // Note: The caller of this function can be a UE procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the UE. This achieved via a scheduled async task

  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue index={}", fmt::underlying(ue_index));
  logger.debug("ue={}: Scheduled deletion of UE context", fmt::underlying(ue_index));
  ue_ctrl_loop[ue_index].clear_pending_tasks();

  // Schedule UE removal task
  ue_ctrl_loop[ue_index].schedule([this, ue_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    srsran_assert(ue_db.contains(ue_index), "ue={}: Remove UE called for inexistent UE", fmt::underlying(ue_index));
    rnti_to_ue_index.erase(ue_db[ue_index].rnti);
    ue_db.erase(ue_index);
    ue_ctrl_loop[ue_index].clear_pending_tasks();
    logger.debug("ue={}: Freeing UE context", fmt::underlying(ue_index));
    CORO_RETURN();
  });
}

void du_ue_manager::update_crnti(du_ue_index_t ue_index, rnti_t crnti)
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue index={}", fmt::underlying(ue_index));
  srsran_assert(is_crnti(crnti), "Invalid c-rnti={}", crnti);
  srsran_assert(ue_db.contains(ue_index), "Update C-RNTI called for inexistent ueId={}", fmt::underlying(ue_index));
  du_ue& u = ue_db[ue_index];

  if (u.rnti == crnti) {
    // No change.
    return;
  }

  // Update RNTI -> UE index map
  rnti_to_ue_index.erase(u.rnti);
  rnti_to_ue_index.insert(std::make_pair(crnti, ue_index));

  // Update UE context with new C-RNTI.
  u.rnti = crnti;
}
