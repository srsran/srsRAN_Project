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

#include "e2sm_rc_control_action_du_executor.h"
#include <future>
using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace srsran;

e2sm_rc_control_action_du_executor_base::e2sm_rc_control_action_du_executor_base(du_configurator& du_configurator_,
                                                                                 uint32_t         action_id_) :
  logger(srslog::fetch_basic_logger("E2SM-RC")), action_id(action_id_), du_param_configurator(du_configurator_)
{
}

uint32_t e2sm_rc_control_action_du_executor_base::get_action_id()
{
  return action_id;
}

bool e2sm_rc_control_action_du_executor_base::fill_ran_function_description(
    asn1::e2sm::ran_function_definition_ctrl_action_item_s& action_item)
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
async_task<e2sm_ric_control_response>
e2sm_rc_control_action_du_executor_base::return_ctrl_failure(const e2sm_ric_control_request& req)
{
  return launch_async([req](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    e2sm_ric_control_response e2sm_response;
    e2sm_response.success                = false;
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
    CORO_RETURN(e2sm_response);
  });
}

void e2sm_rc_control_action_du_executor_base::parse_ran_parameter_value(const ran_param_value_type_c& ran_param,
                                                                        uint64_t                      ran_param_id,
                                                                        uint64_t                      ue_id,
                                                                        du_mac_sched_control_config&  ctrl_cfg)
{
  if (ran_param.type() == ran_param_value_type_c::types_opts::ran_p_choice_list) {
    for (auto& ran_p_list : ran_param.ran_p_choice_list().ran_param_list.list_of_ran_param) {
      for (auto& ran_p : ran_p_list.seq_of_ran_params) {
        if (action_params.find(ran_p.ran_param_id) != action_params.end()) {
          parse_ran_parameter_value(ran_p.ran_param_value_type, ran_p.ran_param_id, ue_id, ctrl_cfg);
        }
      }
    }
  } else if (ran_param.type() == ran_param_value_type_c::types_opts::ran_p_choice_structure) {
    for (auto& ran_seq : ran_param.ran_p_choice_structure().ran_param_structure.seq_of_ran_params) {
      if (action_params.find(ran_seq.ran_param_id) != action_params.end()) {
        parse_ran_parameter_value(ran_seq.ran_param_value_type, ran_seq.ran_param_id, ue_id, ctrl_cfg);
      }
    }
  } else if (ran_param.type() == ran_param_value_type_c::types_opts::ran_p_choice_elem_false) {
    parse_action_ran_parameter_value(ran_param, ran_param_id, ue_id, ctrl_cfg);
  }
}

e2sm_rc_control_action_2_6_du_executor::e2sm_rc_control_action_2_6_du_executor(du_configurator& du_configurator_) :
  e2sm_rc_control_action_du_executor_base(du_configurator_, 6)
{
  // Control Action description:
  // To control the radio resource management policy for slice-specific PRB quota allocation
  action_name = "Slice-level PRB quota";
  action_params.insert({1, "RRM Policy Ratio List"});
  action_params.insert({2, "RRM Policy Ratio Group"});
  action_params.insert({3, "RRM Policy"});
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

void e2sm_rc_control_action_2_6_du_executor::parse_action_ran_parameter_value(const ran_param_value_type_c& ran_param,
                                                                              uint64_t                     ran_param_id,
                                                                              uint64_t                     ue_id,
                                                                              du_mac_sched_control_config& ctrl_cfg)
{
  if (action_params[ran_param_id] == "PLMN Identity") {
    control_config_params cur_control_params = {};
    cur_control_params.rrm_policy_group.emplace();
    cur_control_params.rrm_policy_group.value().pol_member.plmn_id.append(
        ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_string());
    ctrl_cfg.param_list.push_back(cur_control_params);
  } else if (action_params[ran_param_id] == "SST") {
    if (ctrl_cfg.param_list.size()) {
      if (ctrl_cfg.param_list.back().rrm_policy_group.value().pol_member.s_nssai.sst) {
        control_config_params cur_control_params = {};
        cur_control_params.rrm_policy_group.emplace();
        cur_control_params = ctrl_cfg.param_list.back();
        cur_control_params.rrm_policy_group.value().pol_member.s_nssai.sst =
            ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_number();
        ctrl_cfg.param_list.push_back(cur_control_params);
      } else {
        ctrl_cfg.param_list.back().rrm_policy_group.value().pol_member.s_nssai.sst =
            ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_number();
      }
    }
  } else if (action_params[ran_param_id] == "SD") {
    if (ctrl_cfg.param_list.size()) {
      if (ctrl_cfg.param_list.back().rrm_policy_group.value().pol_member.s_nssai.sd.has_value()) {
        control_config_params cur_control_params = {};
        cur_control_params.rrm_policy_group.emplace();
        cur_control_params = ctrl_cfg.param_list.back();
        cur_control_params.rrm_policy_group.value().pol_member.s_nssai.sd.emplace();
        cur_control_params.rrm_policy_group.value().pol_member.s_nssai.sd =
            ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_number();
        ctrl_cfg.param_list.push_back(cur_control_params);
      } else {
        ctrl_cfg.param_list.back().rrm_policy_group.value().pol_member.s_nssai.sd.emplace();
        ctrl_cfg.param_list.back().rrm_policy_group.value().pol_member.s_nssai.sd =
            ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_number();
      }
    }
  } else if (action_params[ran_param_id] == "Min PRB Policy Ratio") {
    if (ctrl_cfg.param_list.size()) {
      if (!ctrl_cfg.param_list.back().rrm_policy_group.has_value()) {
        ctrl_cfg.param_list.back().rrm_policy_group.emplace();
      }
      ctrl_cfg.param_list.back().rrm_policy_group.value().min_prb_policy_ratio.emplace();
      ctrl_cfg.param_list.back().rrm_policy_group.value().min_prb_policy_ratio =
          ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
    } else {
      control_config_params cur_control_params = {};
      cur_control_params.rrm_policy_group.emplace();
      cur_control_params.rrm_policy_group.value().min_prb_policy_ratio.emplace();
      cur_control_params.rrm_policy_group.value().min_prb_policy_ratio =
          ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
      ctrl_cfg.param_list.push_back(cur_control_params);
    }
  } else if (action_params[ran_param_id] == "Max PRB Policy Ratio") {
    if (ctrl_cfg.param_list.size()) {
      if (!ctrl_cfg.param_list.back().rrm_policy_group.has_value()) {
        ctrl_cfg.param_list.back().rrm_policy_group.emplace();
      }
      ctrl_cfg.param_list.back().rrm_policy_group.value().max_prb_policy_ratio.emplace();
      ctrl_cfg.param_list.back().rrm_policy_group.value().max_prb_policy_ratio =
          ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
    } else {
      control_config_params cur_control_params = {};
      cur_control_params.rrm_policy_group.emplace();
      cur_control_params.rrm_policy_group.value().max_prb_policy_ratio.emplace();
      cur_control_params.rrm_policy_group.value().max_prb_policy_ratio =
          ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
      ctrl_cfg.param_list.push_back(cur_control_params);
    }
  } else {
    logger.error("Unknown RAN parameter ID {}", ran_param_id);
    return;
  }
}

bool e2sm_rc_control_action_2_6_du_executor::ric_control_action_supported(const e2sm_ric_control_request& req)
{
  const e2sm_rc_ctrl_msg_format1_s& ctrl_msg =
      std::get<e2sm_rc_ctrl_msg_s>(req.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) == action_params.end()) {
      return false;
    }
  };
  return true;
};

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_2_6_du_executor::execute_ric_control_action(const e2sm_ric_control_request& req)
{
  du_mac_sched_control_config ctrl_config = convert_to_du_config_request(req);
  if (ctrl_config.param_list.empty()) {
    return return_ctrl_failure(req);
  }
  return launch_async(
      [this, ctrl_config = std::move(ctrl_config)](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
        CORO_BEGIN(ctx);
        du_mac_sched_control_config_response ctrl_response;
        CORO_AWAIT_VALUE(ctrl_response, du_param_configurator.configure_ue_mac_scheduler(ctrl_config));
        e2sm_ric_control_response e2_resp = convert_to_e2sm_response(ctrl_config, ctrl_response);
        CORO_RETURN(e2_resp);
      });
};

du_mac_sched_control_config
e2sm_rc_control_action_2_6_du_executor::convert_to_du_config_request(const e2sm_ric_control_request& e2sm_req_)
{
  du_mac_sched_control_config       ctrl_config = {};
  const e2sm_rc_ctrl_hdr_format1_s& ctrl_hdr =
      std::get<e2sm_rc_ctrl_hdr_s>(e2sm_req_.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format1();
  const e2sm_rc_ctrl_msg_format1_s& ctrl_msg =
      std::get<e2sm_rc_ctrl_msg_s>(e2sm_req_.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();
  switch (ctrl_hdr.ue_id.type()) {
    case ue_id_c::types_opts::gnb_ue_id:
      ctrl_config.ue_id = ctrl_hdr.ue_id.gnb_ue_id().amf_ue_ngap_id;
      break;
    case ue_id_c::types_opts::gnb_du_ue_id:
      ctrl_config.ue_id = ctrl_hdr.ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id;
      break;
    default:
      ctrl_config.ue_id = 0;
      break;
  }

  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) != action_params.end()) {
      parse_ran_parameter_value(ran_p.ran_param_value_type, ran_p.ran_param_id, ctrl_config.ue_id, ctrl_config);
    }
    if (ctrl_config.param_list.empty()) {
      return {};
    }
  }
  return ctrl_config;
}

e2sm_ric_control_response e2sm_rc_control_action_2_6_du_executor::convert_to_e2sm_response(
    const du_mac_sched_control_config&          du_config_req_,
    const du_mac_sched_control_config_response& du_response_)
{
  e2sm_ric_control_response e2sm_response;
  e2sm_response.success =
      du_response_.harq_processes_result and du_response_.max_prb_alloc_result and du_response_.min_prb_alloc_result;

  // Always fill outcome here, it will be decided later whether it should be included in the e2 response.
  e2sm_response.ric_ctrl_outcome_present       = true;
  e2sm_rc_ctrl_outcome_format1_s& ctrl_outcome = std::get<e2sm_rc_ctrl_outcome_s>(e2sm_response.ric_ctrl_outcome)
                                                     .ric_ctrl_outcome_formats.set_ctrl_outcome_format1();

  // TODO: fill outcome properly
  control_config_params req = du_config_req_.param_list[0];
  if (req.rrm_policy_group.has_value()) {
    e2sm_rc_ctrl_outcome_format1_item_s min_prb_outcome;
    min_prb_outcome.ran_param_id = 10;
    if (req.rrm_policy_group.value().min_prb_policy_ratio.has_value()) {
      min_prb_outcome.ran_param_value.set_value_int() = req.rrm_policy_group.value().min_prb_policy_ratio.value();
      ctrl_outcome.ran_p_list.push_back(min_prb_outcome);
    }
  }

  if (req.rrm_policy_group.has_value()) {
    e2sm_rc_ctrl_outcome_format1_item_s max_prb_outcome;
    max_prb_outcome.ran_param_id = 11;
    if (req.rrm_policy_group.value().max_prb_policy_ratio.has_value()) {
      max_prb_outcome.ran_param_value.set_value_int() = req.rrm_policy_group.value().max_prb_policy_ratio.value();
      ctrl_outcome.ran_p_list.push_back(max_prb_outcome);
    }
  }

  if (!e2sm_response.success) {
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
  }

  return e2sm_response;
}
