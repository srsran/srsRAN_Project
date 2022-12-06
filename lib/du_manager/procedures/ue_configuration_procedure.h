/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_manager_config.h"
#include "../du_manager_interfaces.h"

namespace srsgnb {
namespace srs_du {

class ue_configuration_procedure
{
public:
  ue_configuration_procedure(const f1ap_ue_context_update_request&        request_,
                             ue_manager_ctrl_configurator&                ue_mng_,
                             const du_manager_params::service_params&     du_services_,
                             mac_ue_configurator&                         mac_ue_mng_,
                             const du_manager_params::f1ap_config_params& f1ap_mng_);

  void operator()(coro_context<async_task<f1ap_ue_context_update_response>>& ctx);

private:
  void calculate_next_ue_context();

  void add_bearers_to_ue_context();

  void                                                update_f1_bearers();
  void                                                update_rlc_bearers();
  async_task<mac_ue_reconfiguration_response_message> update_mac_bearers();

  f1ap_ue_context_update_response make_ue_config_response();

  const f1ap_ue_context_update_request         request;
  ue_manager_ctrl_configurator&                ue_mng;
  const du_manager_params::service_params&     services;
  mac_ue_configurator&                         mac_ue_mng;
  const du_manager_params::f1ap_config_params& f1ap_mng;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("DU-MNG");
  du_ue*                ue     = nullptr;

  cell_group_config next_cell_group;
};

} // namespace srs_du
} // namespace srsgnb