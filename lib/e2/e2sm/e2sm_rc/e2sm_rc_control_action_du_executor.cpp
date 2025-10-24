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

#include "e2sm_rc_control_action_du_executor.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include <future>

using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace srsran;

e2sm_rc_control_action_du_executor_base::e2sm_rc_control_action_du_executor_base(
    srs_du::du_configurator&       du_configurator_,
    srs_du::f1ap_ue_id_translator& f1ap_ue_id_translator_,
    uint32_t                       action_id_) :
  logger(srslog::fetch_basic_logger("E2SM-RC")),
  action_id(action_id_),
  du_param_configurator(du_configurator_),
  f1ap_ue_id_provider(f1ap_ue_id_translator_)
{
}

uint32_t e2sm_rc_control_action_du_executor_base::get_action_id()
{
  return action_id;
}

std::string e2sm_rc_control_action_du_executor_base::get_action_name()
{
  return action_name;
}

ran_function_definition_ctrl_action_item_s e2sm_rc_control_action_du_executor_base::get_control_action_definition()
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

e2sm_rc_control_action_2_6_du_executor::e2sm_rc_control_action_2_6_du_executor(
    srs_du::du_configurator&       du_configurator_,
    srs_du::f1ap_ue_id_translator& f1ap_ue_id_translator_) :
  e2sm_rc_control_action_du_executor_base(du_configurator_, f1ap_ue_id_translator_, 6)
{
  // Control Action description:
  // To control the radio resource management policy for slice-specific PRB quota allocation
  // Note: Parameter id 4 missing/skipped in TS.
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
}

void e2sm_rc_control_action_2_6_du_executor::parse_action_ran_parameter_value(
    const ran_param_value_type_c&        ran_param,
    uint64_t                             ran_param_id,
    uint64_t                             ue_id,
    srs_du::du_mac_sched_control_config& ctrl_cfg)
{
  // TODO: validate types.
  if (ran_param_id == 1 or ran_param_id == 3 or ran_param_id == 5 or ran_param_id == 8) {
    // No need to parse.
  } else if (ran_param_id == 2) {
    // RRM Policy Ratio Group.
    ctrl_cfg.rrm_policy_ratio_list.emplace_back();
  } else if (ran_param_id == 6) {
    // RRM Policy Member List.
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.emplace_back();
  } else if (ran_param_id == 7) {
    // PLMN Identity.
    if (ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().size() != 3) {
      logger.info("E2SM-RC Slice-level PRB quota Control Request: PLMN (param_id={}) encoded not correctly.",
                  ran_param_id);
      return;
    }
    std::array<uint8_t, 3> plmn_bytes;
    std::copy(ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().begin(),
              ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().end(),
              plmn_bytes.begin());
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.back().plmn_id =
        plmn_identity::from_bytes(plmn_bytes).value();
  } else if (ran_param_id == 9) {
    // SST.
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.back().s_nssai.sst =
        slice_service_type{(uint8_t)ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_number()};
  } else if (ran_param_id == 10) {
    // SD.
    uint64_t sd = ran_param.ran_p_choice_elem_false().ran_param_value.value_oct_s().to_number();
    ctrl_cfg.rrm_policy_ratio_list.back().policy_members_list.back().s_nssai.sd =
        slice_differentiator::create(sd).value();
  } else if (ran_param_id == 11) {
    // Min PRB Policy Ratio.
    ctrl_cfg.rrm_policy_ratio_list.back().minimum_ratio =
        ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
  } else if (ran_param_id == 12) {
    // Max PRB Policy Ratio.
    ctrl_cfg.rrm_policy_ratio_list.back().maximum_ratio =
        ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
  } else if (ran_param_id == 13) {
    // Dedicated PRB Policy Ratio.
    ctrl_cfg.rrm_policy_ratio_list.back().dedicated_ratio =
        ran_param.ran_p_choice_elem_false().ran_param_value.value_int();
  } else {
    logger.error("Unknown RAN parameter ID {}", ran_param_id);
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
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_action_2_6_du_executor::execute_ric_control_action(const e2sm_ric_control_request& req)
{
  srs_du::du_mac_sched_control_config ctrl_config = convert_to_du_config_request(req);
  // If empty request, return failure.
  if (ctrl_config.rrm_policy_ratio_list.empty()) {
    return return_ctrl_failure(req);
  }
  // If any policy is missing members, return failure.
  for (const auto& policy : ctrl_config.rrm_policy_ratio_list) {
    if (policy.policy_members_list.empty()) {
      return return_ctrl_failure(req);
    }
  }
  // If PRB quota not provided, return failure.
  for (const auto& policy : ctrl_config.rrm_policy_ratio_list) {
    if (!policy.minimum_ratio.has_value()) {
      return return_ctrl_failure(req);
    }
    if (!policy.maximum_ratio.has_value()) {
      return return_ctrl_failure(req);
    }
    if (!policy.dedicated_ratio.has_value()) {
      return return_ctrl_failure(req);
    }
  }

  // Log received control request.
  fmt::memory_buffer log_buffer;
  fmt::format_to(std::back_inserter(log_buffer),
                 "Slice-level PRB quota Control Request for UE gNB-DU-UE-F1AP-ID={}:\n",
                 gnb_du_ue_f1ap_id_to_uint(ctrl_config.ue_id));
  fmt::format_to(std::back_inserter(log_buffer), "RRM Policy Ratio Group:\n");
  for (const auto& rrm_policy_ratio : ctrl_config.rrm_policy_ratio_list) {
    fmt::format_to(std::back_inserter(log_buffer), " RRM Policy:\n");
    fmt::format_to(std::back_inserter(log_buffer), "  Min PRB Policy Ratio: {}\n", *rrm_policy_ratio.minimum_ratio);
    fmt::format_to(std::back_inserter(log_buffer), "  Max PRB Policy Ratio: {}\n", *rrm_policy_ratio.maximum_ratio);
    fmt::format_to(
        std::back_inserter(log_buffer), "  Dedicated PRB Policy Ratio: {}\n", *rrm_policy_ratio.dedicated_ratio);
    fmt::format_to(std::back_inserter(log_buffer), "  RRM Policy Member List:\n");
    for (const auto& policy_member : rrm_policy_ratio.policy_members_list) {
      fmt::format_to(std::back_inserter(log_buffer),
                     "  - PLMN:{}, SST:{}, SD:{}\n",
                     policy_member.plmn_id.to_string(),
                     policy_member.s_nssai.sst.value(),
                     policy_member.s_nssai.sd.value());
    }
  }
  logger.info("{}", to_c_str(log_buffer));
  log_buffer.clear();

  return launch_async(
      [this, ctrl_config = std::move(ctrl_config)](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
        CORO_BEGIN(ctx);
        srs_du::du_mac_sched_control_config_response ctrl_response;
        CORO_AWAIT_VALUE(ctrl_response, du_param_configurator.configure_ue_mac_scheduler(ctrl_config));
        e2sm_ric_control_response e2_resp = convert_to_e2sm_response(ctrl_config, ctrl_response);
        CORO_RETURN(e2_resp);
      });
}

srs_du::du_mac_sched_control_config
e2sm_rc_control_action_2_6_du_executor::convert_to_du_config_request(const e2sm_ric_control_request& e2sm_req_)
{
  srs_du::du_mac_sched_control_config ctrl_config = {};
  const e2sm_rc_ctrl_hdr_format1_s&   ctrl_hdr =
      std::get<e2sm_rc_ctrl_hdr_s>(e2sm_req_.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format1();
  const e2sm_rc_ctrl_msg_format1_s& ctrl_msg =
      std::get<e2sm_rc_ctrl_msg_s>(e2sm_req_.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format1();

  if (ctrl_hdr.ue_id.type() != ue_id_c::types_opts::gnb_du_ue_id) {
    return {};
  }
  uint64_t gnb_cu_ue_f1ap_id = ctrl_hdr.ue_id.gnb_du_ue_id().gnb_cu_ue_f1ap_id;
  ctrl_config.ue_id          = f1ap_ue_id_provider.get_gnb_du_ue_f1ap_id(int_to_gnb_cu_ue_f1ap_id(gnb_cu_ue_f1ap_id));

  if (ctrl_config.ue_id == gnb_du_ue_f1ap_id_t::invalid) {
    logger.info("Slice-level PRB quota Control Request received but UE not found, gNB-CU-UE-F1AP-ID={}",
                gnb_cu_ue_f1ap_id);
    return {};
  }

  // Use a lambda to define the template function in parse_ran_parameter_value
  auto parse_action_ran_parameter_value_lambda = [this](const ran_param_value_type_c&        ran_param,
                                                        uint64_t                             ran_param_id,
                                                        uint64_t                             ue_id,
                                                        srs_du::du_mac_sched_control_config& ctrl_cfg) {
    this->parse_action_ran_parameter_value(ran_param, ran_param_id, ue_id, ctrl_cfg);
  };
  for (auto& ran_p : ctrl_msg.ran_p_list) {
    if (action_params.find(ran_p.ran_param_id) != action_params.end()) {
      parse_ran_parameter_value(ran_p.ran_param_value_type,
                                ran_p.ran_param_id,
                                gnb_cu_ue_f1ap_id,
                                ctrl_config,
                                parse_action_ran_parameter_value_lambda);
    }
  }
  if (ctrl_config.rrm_policy_ratio_list.empty()) {
    return {};
  }
  return ctrl_config;
}

e2sm_ric_control_response e2sm_rc_control_action_2_6_du_executor::convert_to_e2sm_response(
    const srs_du::du_mac_sched_control_config&          du_config_req_,
    const srs_du::du_mac_sched_control_config_response& du_response_)
{
  e2sm_ric_control_response e2sm_response;
  e2sm_response.success =
      du_response_.harq_processes_result and du_response_.max_prb_alloc_result and du_response_.min_prb_alloc_result;

  // Always fill outcome here, it will be decided later whether it should be included in the e2 response.
  e2sm_response.ric_ctrl_outcome_present = false;
  if (!e2sm_response.success) {
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
  }

  return e2sm_response;
}
