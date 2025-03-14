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

#include "e2sm_rc_control_action_cu_executor.h"
#include <future>
using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace srsran;

e2sm_rc_control_action_cu_executor_base::e2sm_rc_control_action_cu_executor_base(cu_configurator& cu_configurator_,
                                                                                 uint32_t         action_id_) :
  logger(srslog::fetch_basic_logger("E2SM-RC")), action_id(action_id_), cu_param_configurator(cu_configurator_)
{
}

uint32_t e2sm_rc_control_action_cu_executor_base::get_action_id()
{
  return action_id;
}

ran_function_definition_ctrl_action_item_s e2sm_rc_control_action_cu_executor_base::get_control_action_definition()
{
  ran_function_definition_ctrl_action_item_s action_item;
  action_item.ric_ctrl_action_id = action_id;
  action_item.ric_ctrl_action_name.resize(action_name.size());
  action_item.ric_ctrl_action_name.from_string(action_name);

  for (auto& ran_p : action_params) {
    ctrl_action_ran_param_item_s ctrl_action_ran_param_item;
    ctrl_action_ran_param_item.ran_param_id = ran_p.first;
    ctrl_action_ran_param_item.ran_param_name.resize(ran_p.second.size());
    ctrl_action_ran_param_item.ran_param_name.from_string(ran_p.second);
    action_item.ran_ctrl_action_params_list.push_back(ctrl_action_ran_param_item);
  }

  return action_item;
}
async_task<e2sm_ric_control_response>
e2sm_rc_control_action_cu_executor_base::return_ctrl_failure(const e2sm_ric_control_request& req)
{
  return launch_async([req](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    e2sm_ric_control_response e2sm_response;
    e2sm_response.success                = false;
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
    CORO_RETURN(e2sm_response);
  });
}

e2sm_rc_control_action_3_1_cu_executor::e2sm_rc_control_action_3_1_cu_executor(cu_configurator& cu_configurator_) :
  e2sm_rc_control_action_cu_executor_base(cu_configurator_, 1)
{
  action_name = "Handover Control";
  action_params.insert({1, "Target Primary Cell ID"});
  action_params.insert({2, ">CHOICE Target Cell"});
  action_params.insert({3, ">>NR Cell"});
  action_params.insert({4, ">>>NR CGI"});
  action_params.insert({5, ">>E-UTRA Cell"});
  action_params.insert({6, ">>>E-UTRA CGI"});
  // TODO: add parameters 7-21 (related to PDU session, DRBs, Secondary cell) when supported.
}

bool e2sm_rc_control_action_3_1_cu_executor::ric_control_action_supported(const e2sm_ric_control_request& req)
{
  return true;
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_3_1_cu_executor::execute_ric_control_action(const e2sm_ric_control_request& req)
{
  return launch_async([this, req](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    srs_cu_cp::cu_cp_intra_cu_handover_response cu_cp_response;
    srs_cu_cp::cu_cp_intra_cu_handover_request  handover_req;
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(cu_cp_response,
                     cu_param_configurator.get_mobility_notifier().on_intra_cu_handover_required(
                         handover_req, srs_cu_cp::du_index_t(0), srs_cu_cp::du_index_t(1)));
    e2sm_ric_control_response e2sm_response;
    e2sm_response.success = cu_cp_response.success;
    CORO_RETURN(e2sm_response);
  });
}

void e2sm_rc_control_action_3_1_cu_executor::parse_action_ran_parameter_value(
    const asn1::e2sm::ran_param_value_type_c& ran_p,
    uint64_t                                  ran_param_id,
    uint64_t                                  ue_id,
    cu_handover_control_config&               ctrl_cfg)
{
}
