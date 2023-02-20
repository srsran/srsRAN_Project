/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_ue/ue_manager_ctrl_configurator.h"
#include "srsgnb/du_manager/du_manager_params.h"

namespace srsgnb {
namespace srs_du {

class ue_configuration_procedure
{
public:
  ue_configuration_procedure(const f1ap_ue_context_update_request& request_,
                             ue_manager_ctrl_configurator&         ue_mng_,
                             const du_manager_params&              du_params_);

  void operator()(coro_context<async_task<f1ap_ue_context_update_response>>& ctx);

private:
  /// \brief Update DU UE bearers. This stage includes the creation/modification/removal of SRBs/DRBs, creation of RLC
  /// and F1-U bearers.
  void add_srbs_to_du_ue_context();
  void add_drbs_to_du_ue_context();
  void remove_drbs_from_du_ue_context();

  /// \brief Update MAC MUX and DEMUX tables of the respective UE, given the newly added/modified/removed bearers.
  async_task<mac_ue_reconfiguration_response_message> update_mac_lcid_mux();

  f1ap_ue_context_update_response make_ue_config_response();
  f1ap_ue_context_update_response make_ue_config_failure();

  const f1ap_ue_context_update_request request;
  ue_manager_ctrl_configurator&        ue_mng;
  const du_manager_params&             du_params;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("DU-MNG");
  du_ue*                ue     = nullptr;

  cell_group_config prev_cell_group;
};

} // namespace srs_du
} // namespace srsgnb
