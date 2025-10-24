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

#pragma once

#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/mac/mac_ue_configurator.h"

namespace srsran {
namespace srs_du {

class ue_configuration_procedure
{
public:
  ue_configuration_procedure(const f1ap_ue_context_update_request& request_,
                             du_ue_manager_repository&             ue_mng_,
                             const du_manager_params&              du_params_);

  void operator()(coro_context<async_task<f1ap_ue_context_update_response>>& ctx);

  const char* name() const { return "UE Configuration"; }

private:
  bool changed_detected() const;
  // Stop activity in DRBs that need to be replaced.
  async_task<void> stop_drbs_to_rem();
  // Update DU UE bearers. This stage includes the creation/modification/removal of SRBs/DRBs, creation of RLC
  // and F1-U bearers.
  void update_ue_context();
  void clear_old_ue_context();

  /// \brief Update MAC MUX and DEMUX tables of the respective UE, given the newly added/modified/removed bearers and
  /// the scheduler with the new UE config.
  async_task<mac_ue_reconfiguration_response> update_mac_and_sched();

  f1ap_ue_context_update_response        make_ue_config_response();
  static f1ap_ue_context_update_response make_empty_ue_config_response();
  static f1ap_ue_context_update_response make_ue_config_failure();

  void handle_rrc_reconfiguration_complete_ind();

  const f1ap_ue_context_update_request request;
  du_ue_manager_repository&            ue_mng;
  const du_manager_params&             du_params;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("DU-MNG");
  du_ue*                ue     = nullptr;
  ue_procedure_logger   proc_logger;

  // Snapshot of the UE resources at the start of the UE configuration procedure.
  du_ue_resource_config          prev_ue_res_cfg;
  du_ue_resource_update_response ue_res_cfg_resp;

  // MAC response to a config.
  mac_ue_reconfiguration_response mac_res;

  // SRBs that were actually added during the configuration.
  static_vector<srb_id_t, MAX_NOF_SRBS> srbs_added;

  std::vector<std::unique_ptr<du_ue_drb>> drbs_to_rem;
};

} // namespace srs_du
} // namespace srsran
