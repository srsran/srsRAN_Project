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

#pragma once

#include "du_ue.h"
#include "du_ue_manager_repository.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/du_manager/du_manager.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/support/async/async_task_loop.h"
#include <unordered_map>

namespace srsran {
namespace srs_du {

/// \brief This entity orchestrates the addition/reconfiguration/removal of UE contexts in the DU.
class du_ue_manager : public du_ue_manager_repository
{
public:
  explicit du_ue_manager(du_manager_params& cfg_, du_ran_resource_manager& cell_res_alloc);

  du_ue_index_t find_unused_du_ue_index();

  void handle_ue_create_request(const ul_ccch_indication_message& msg);

  /// \brief Handle the creation of a new UE context by F1AP request.
  async_task<f1ap_ue_context_creation_response> handle_ue_create_request(const f1ap_ue_context_creation_request& msg);

  /// \brief Handle the update of an existing UE context by F1AP request.
  async_task<f1ap_ue_context_update_response> handle_ue_config_request(const f1ap_ue_context_update_request& msg);

  /// \brief Handle the removal of an existing UE context by F1AP request.
  async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& msg);

  void handle_reestablishment_request(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index);

  /// \brief Force the interruption of all UE activity.
  async_task<void> stop();

  const slotted_id_table<du_ue_index_t, std::unique_ptr<du_ue>, MAX_NOF_DU_UES>& get_ues() const { return ue_db; }

  /// \brief Schedule an asynchronous task to be executed in the UE control loop.
  void schedule_async_task(du_ue_index_t ue_index, async_task<void>&& task)
  {
    ue_ctrl_loop[ue_index].schedule(std::move(task));
  }

  gtpu_teid_pool& get_f1u_teid_pool() override { return *f1u_teid_pool; }

private:
  du_ue* add_ue(std::unique_ptr<du_ue> ue_ctx) override;
  void   update_crnti(du_ue_index_t ue_index, rnti_t crnti) override;
  du_ue* find_ue(du_ue_index_t ue_index) override;
  du_ue* find_rnti(rnti_t rnti) override;
  void   remove_ue(du_ue_index_t ue_index) override;
  void   handle_radio_link_failure(du_ue_index_t ue_index, rlf_cause cause) override;

  du_manager_params&       cfg;
  du_ran_resource_manager& cell_res_alloc;
  srslog::basic_logger&    logger;

  // Pool of available TEIDs for F1-U.
  std::unique_ptr<gtpu_teid_pool> f1u_teid_pool;

  // Mapping of ue_index and rnti to UEs.
  slotted_id_table<du_ue_index_t, std::unique_ptr<du_ue>, MAX_NOF_DU_UES> ue_db;
  std::unordered_map<rnti_t, du_ue_index_t>                               rnti_to_ue_index;

  // task event loops indexed by ue_index
  slotted_array<async_task_sequencer, MAX_NOF_DU_UES> ue_ctrl_loop;
};

} // namespace srs_du
} // namespace srsran
