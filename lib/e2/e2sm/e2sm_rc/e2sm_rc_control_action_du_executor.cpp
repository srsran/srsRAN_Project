/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2sm_rc_control_action_du_executor.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;
using namespace srsran;

e2sm_rc_control_action_du_executor_base::e2sm_rc_control_action_du_executor_base(
    e2sm_param_configurator& param_configurator_,
    uint32_t                 action_id_) :
  logger(srslog::fetch_basic_logger("E2SM-RC")), action_id(action_id_), param_configurator(param_configurator_)
{
}

uint32_t e2sm_rc_control_action_du_executor_base::get_action_id()
{
  return action_id;
}

bool e2sm_rc_control_action_du_executor_base::fill_ran_function_description(
    asn1::e2sm_rc::ran_function_definition_ctrl_action_item_s& action_item)
{
  action_item.ric_ctrl_action_id = action_id;
  action_item.ric_ctrl_action_name.from_string(action_name);

  for (auto& ran_p : action_params) {
    ctrl_action_ran_param_item_s ctrl_action_ran_param_item;
    ctrl_action_ran_param_item.ran_param_id = ran_p.first;
    ctrl_action_ran_param_item.ran_param_name.from_string(ran_p.second);
    action_item.ran_ctrl_action_params_list.push_back(ctrl_action_ran_param_item);
  }

  return true;
}

e2sm_rc_control_action_2_6_du_executor::e2sm_rc_control_action_2_6_du_executor(
    e2sm_param_configurator& param_configurator_) :
  e2sm_rc_control_action_du_executor_base(param_configurator_, 6)
{
  // Control Action description:
  // To control the radio resource management policy for slice-specific PRB quota allocation
  action_name = "Slice-level PRB quota";
  action_params.insert({1, "RRM Policy Ratio List"});
  action_params.insert({2, "RRM Policy Ratio Group"});
  action_params.insert({3, "RRM Policy"});
  // Note: 4 missing in TS
  action_params.insert({5, "RRM Policy Member List"});
  action_params.insert({6, "RRM Policy Member"});
  action_params.insert({7, "PLMN Identity"});
  action_params.insert({8, "S-NSSAI"});
  action_params.insert({9, "SST"});
  action_params.insert({10, "SD"});
  action_params.insert({11, "Min PRB Policy Ratio"});
  action_params.insert({12, "Max PRB Policy Ratio"});
  action_params.insert({13, "Dedicated PRB Policy Ratio"});
};

bool e2sm_rc_control_action_2_6_du_executor::ric_control_action_supported(const e2_sm_ric_control_request_s& req)
{
  const e2_sm_rc_ctrl_msg_format1_s& ctrl_msg =
      variant_get<e2_sm_rc_ctrl_msg_s>(req.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) == action_params.end()) {
      return false;
    }
  };
  return true;
};

async_task<e2_ric_control_response>
e2sm_rc_control_action_2_6_du_executor::execute_ric_control_action(const e2_sm_ric_control_request_s& req)
{
  const e2_sm_rc_ctrl_hdr_format1_s& ctrl_hdr =
      variant_get<e2_sm_rc_ctrl_hdr_s>(req.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format1();
  const e2_sm_rc_ctrl_msg_format1_s& ctrl_msg =
      variant_get<e2_sm_rc_ctrl_msg_s>(req.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  ctrl_config.ue_id = ctrl_hdr.ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id;

  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) != action_params.end()) {
      if (ran_p.ran_param_id == 11) {
        ctrl_config.min_prb_alloc = ran_p.ran_param_value_type.ran_p_choice_elem_true().ran_param_value.value_int();
      } else if (ran_p.ran_param_id == 12) {
        ctrl_config.max_prb_alloc = ran_p.ran_param_value_type.ran_p_choice_elem_true().ran_param_value.value_int();
      }
    } else {
      logger.error("Parameter not supported");
    }
  }

  return launch_async([this](coro_context<async_task<e2_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    ric_control_config_response ctrl_response;
    CORO_AWAIT_VALUE(ctrl_response, param_configurator.configure_ue_mac_scheduler(ctrl_config));
    e2_ric_control_response e2_resp;
    e2_resp.success = ctrl_response.harq_processes_result and ctrl_response.max_prb_alloc_result and
                      ctrl_response.min_prb_alloc_result;
    CORO_RETURN(e2_resp);
  });
};