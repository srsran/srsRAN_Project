/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "du_pucch_resource_manager.h"
#include "du_ran_resource_manager.h"
#include "srsran/ran/five_qi.h"

namespace srsran {
namespace srs_du {

class du_ran_resource_manager_impl;

class du_ue_ran_resource_updater_impl final : public ue_ran_resource_configurator::resource_updater
{
public:
  du_ue_ran_resource_updater_impl(cell_group_config*            cell_cfg_,
                                  du_ran_resource_manager_impl& parent_,
                                  du_ue_index_t                 ue_index_);
  du_ue_ran_resource_updater_impl(const du_ue_ran_resource_updater_impl&)            = delete;
  du_ue_ran_resource_updater_impl(const du_ue_ran_resource_updater_impl&&)           = delete;
  du_ue_ran_resource_updater_impl& operator=(const du_ue_ran_resource_updater_impl&) = delete;
  du_ue_ran_resource_updater_impl& operator=(du_ue_ran_resource_updater_impl&&)      = delete;
  ~du_ue_ran_resource_updater_impl() override;

  du_ue_resource_update_response update(du_cell_index_t                       pcell_index,
                                        const f1ap_ue_context_update_request& upd_req) override;

  const cell_group_config& get() override { return *cell_grp; }

private:
  cell_group_config*            cell_grp;
  du_ran_resource_manager_impl* parent;
  du_ue_index_t                 ue_index;
};

class du_ran_resource_manager_impl : public du_ran_resource_manager
{
public:
  du_ran_resource_manager_impl(span<const du_cell_config>                cell_cfg_list_,
                               const scheduler_expert_config&            scheduler_cfg,
                               const std::map<srb_id_t, du_srb_config>&  srbs,
                               const std::map<five_qi_t, du_qos_config>& qos);
  du_ran_resource_manager_impl(du_ran_resource_manager_impl&&)                 = delete;
  du_ran_resource_manager_impl(const du_ran_resource_manager_impl&)            = delete;
  du_ran_resource_manager_impl& operator=(du_ran_resource_manager_impl&&)      = delete;
  du_ran_resource_manager_impl& operator=(const du_ran_resource_manager_impl&) = delete;

  ue_ran_resource_configurator create_ue_resource_configurator(du_ue_index_t   ue_index,
                                                               du_cell_index_t pcell_index) override;

  /// \brief Updates a UE's cell configuration context based on the F1 UE Context Update request.
  ///
  /// UE Context updates may include the addition of new bearers, reservation of PUCCH Resources for Scheduling
  /// Requests or CSI, etc.
  /// \param ue_index Id of the UE whose context is being updated.
  /// \param pcell_idx DU Cell Id of the UE's PCell.
  /// \param upd_req UE Context Update Request received by the F1AP-DU from the CU.
  /// \return Result of the context update.
  du_ue_resource_update_response
  update_context(du_ue_index_t ue_index, du_cell_index_t pcell_idx, const f1ap_ue_context_update_request& upd_req);

  /// \brief Deallocates the RAN resources taken by the UE, so that they can be used by future UEs.
  ///
  /// \param ue_index Id of the UE whose context is being deallocated.
  void deallocate_context(du_ue_index_t ue_index);

private:
  error_type<std::string>
       allocate_cell_resources(du_ue_index_t ue_index, du_cell_index_t cell_index, serv_cell_index_t serv_cell_index);
  void deallocate_cell_resources(du_ue_index_t ue_index, serv_cell_index_t serv_cell_index);
  void modify_rlc_for_ntn(cell_group_config& ue_mcg);

  span<const du_cell_config>                cell_cfg_list;
  const scheduler_expert_config&            sched_cfg;
  const std::map<srb_id_t, du_srb_config>&  srb_config;
  const std::map<five_qi_t, du_qos_config>& qos_config;
  srslog::basic_logger&                     logger;

  struct ue_res_item {
    cell_group_config cg_cfg;
  };

  /// Current UE Resource Allocations.
  slotted_array<ue_res_item, MAX_NOF_DU_UES, false> ue_res_pool;

  /// Allocator of UE PUCCH resources.
  du_pucch_resource_manager pucch_res_mng;
};

} // namespace srs_du
} // namespace srsran
