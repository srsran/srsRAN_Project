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

#include "e2sm_ccc_control_action_du_executor.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace asn1::e2sm_ccc;
using namespace srsran;

e2sm_ccc_control_action_du_executor_base::e2sm_ccc_control_action_du_executor_base(
    srs_du::du_configurator& du_configurator_,
    uint32_t                 action_id_) :
  logger(srslog::fetch_basic_logger("E2SM-CCC")), action_id(action_id_), du_param_configurator(du_configurator_)
{
}

uint32_t e2sm_ccc_control_action_du_executor_base::get_action_id()
{
  return action_id;
}

std::string e2sm_ccc_control_action_du_executor_base::get_action_name()
{
  return ran_cfg_structure_name;
}

ran_function_definition_ctrl_action_item_s e2sm_ccc_control_action_du_executor_base::get_control_action_definition()
{
  // TODO: ran_function_definition_ctrl_action_item_s is from RC, remove from the e2sm_control_action_executor interface
  return {};
}

static async_task<e2sm_ric_control_response> return_ctrl_failure(const e2sm_ric_control_request& req)
{
  return launch_async([req](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    e2sm_ric_control_response e2sm_response;
    e2sm_response.success                = false;
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
    CORO_RETURN(e2sm_response);
  });
}

static srs_du::du_param_config_request convert_to_du_config_request(const e2sm_ric_control_request& e2sm_ccc_req)
{
  srs_du::du_param_config_request du_request;
  const auto&                     e2_ctrl_msg       = std::get<ric_ctrl_msg_s>(e2sm_ccc_req.request_ctrl_msg);
  const auto&                     e2_cell_ctrl_list = e2_ctrl_msg.ctrl_msg_format.ctrl_msg_format2().list_of_cells_ctrl;
  for (auto const& e2_cell_ctrl : e2_cell_ctrl_list) {
    auto&       cell_cfg    = du_request.cells.emplace_back();
    auto&       e2_plmn_id  = e2_cell_ctrl.cell_global_id.nr_cgi().plmn_id;
    std::string plmn_str    = e2_plmn_id.mcc.to_string() + e2_plmn_id.mnc.to_string();
    auto        plmn_id_exp = plmn_identity::parse(plmn_str);
    if (plmn_id_exp.has_value()) {
      cell_cfg.nr_cgi.plmn_id = plmn_id_exp.value();
    }
    auto nr_cell_identity_exp = nr_cell_identity::create(e2_cell_ctrl.cell_global_id.nr_cgi().nr_cell_id.to_number());
    if (nr_cell_identity_exp.has_value()) {
      cell_cfg.nr_cgi.nci = nr_cell_identity_exp.value();
    }

    for (auto const& e2_cfg_struct : e2_cell_ctrl.list_of_cfg_structures) {
      rrm_policy_ratio_group rrm_policy;
      // Note: Here we use only new values.
      auto const& ran_cfg_structure = e2_cfg_struct.new_values_of_attributes.ran_cfg_structure;
      if (ran_cfg_structure.type() == e2_sm_ccc_ran_cfg_structure_c::types::o_rrm_policy_ratio) {
        // Translate resource type.
        if (ran_cfg_structure.o_rrm_policy_ratio().res_type_present) {
          if (ran_cfg_structure.o_rrm_policy_ratio().res_type.value == res_type_e::prb_dl) {
            rrm_policy.resource_type = rrm_policy_ratio_group::resource_type_t::prb_dl;
          } else if (ran_cfg_structure.o_rrm_policy_ratio().res_type.value == res_type_e::prb_ul) {
            rrm_policy.resource_type = rrm_policy_ratio_group::resource_type_t::prb_ul;
          }
        } else {
          rrm_policy.resource_type = rrm_policy_ratio_group::resource_type_t::prb;
        }
        // Convert RRM Policy Member List.
        if (ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_member_list.size()) {
          for (const auto& e2_member : ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_member_list) {
            auto&       member          = rrm_policy.policy_members_list.emplace_back();
            std::string mem_plmn_str    = e2_member.plmn_id.mcc.to_string() + e2_member.plmn_id.mnc.to_string();
            auto        mem_plmn_id_exp = plmn_identity::parse(mem_plmn_str);
            if (mem_plmn_id_exp.has_value()) {
              member.plmn_id = mem_plmn_id_exp.value();
            }
            if (e2_member.snssai_present) {
              if (e2_member.snssai.sd_present) {
                member.s_nssai.sd = slice_differentiator::create(e2_member.snssai.sd.to_number()).value();
              }
              if (e2_member.snssai.sst_present) {
                member.s_nssai.sst = slice_service_type{static_cast<uint8_t>(e2_member.snssai.sst)};
              }
            }
          }
        }
        // Convert Min Ratio.
        if (ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_min_ratio_present) {
          rrm_policy.min_prb_policy_ratio = ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_min_ratio;
        }
        // Convert Max Ratio.
        if (ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_max_ratio_present) {
          rrm_policy.max_prb_policy_ratio = ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_max_ratio;
        }
        // Convert Dedicated Ratio.
        if (ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_ded_ratio) {
          rrm_policy.ded_prb_policy_ratio = ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_ded_ratio;
        }
      }
      cell_cfg.rrm_policy_ratio_list.emplace_back(rrm_policy);
    }
  }
  return du_request;
}

static const char* resource_type_to_string(rrm_policy_ratio_group::resource_type_t type)
{
  switch (type) {
    case rrm_policy_ratio_group::resource_type_t::prb:
      return "PRB";
    case rrm_policy_ratio_group::resource_type_t::prb_ul:
      return "PRB-UL";
    case rrm_policy_ratio_group::resource_type_t::prb_dl:
      return "PRB-DL";
    default:
      return "UNKNOWN";
  }
}

static void log_du_config_request(srslog::basic_logger& logger, const srs_du::du_param_config_request& req)
{
  fmt::memory_buffer log_buffer;
  for (const auto& cell_cfg : req.cells) {
    fmt::format_to(std::back_inserter(log_buffer),
                   "E2SM-CCC: O-RRMPolicyRatio Control Request for NR-CGI=[plmn: {}, nci: {}]\n",
                   cell_cfg.nr_cgi.plmn_id.to_string(),
                   cell_cfg.nr_cgi.nci.value());
    fmt::format_to(std::back_inserter(log_buffer), "RRM Policy Ratio Group:\n");
    for (const auto& rrm_policy_ratio : cell_cfg.rrm_policy_ratio_list) {
      fmt::format_to(std::back_inserter(log_buffer), " RRM Policy:\n");
      fmt::format_to(std::back_inserter(log_buffer),
                     "  Resource Type: {}\n",
                     resource_type_to_string(rrm_policy_ratio.resource_type));
      fmt::format_to(
          std::back_inserter(log_buffer), "  Min PRB Policy Ratio: {}\n", *rrm_policy_ratio.min_prb_policy_ratio);
      fmt::format_to(
          std::back_inserter(log_buffer), "  Max PRB Policy Ratio: {}\n", *rrm_policy_ratio.max_prb_policy_ratio);
      fmt::format_to(
          std::back_inserter(log_buffer), "  Dedicated PRB Policy Ratio: {}\n", *rrm_policy_ratio.ded_prb_policy_ratio);
      fmt::format_to(std::back_inserter(log_buffer), "  RRM Policy Member List:\n");
      for (const auto& policy_member : rrm_policy_ratio.policy_members_list) {
        fmt::format_to(std::back_inserter(log_buffer),
                       "  - PLMN:{}, SST:{}, SD:{}\n",
                       policy_member.plmn_id.to_string(),
                       policy_member.s_nssai.sst.value(),
                       policy_member.s_nssai.sd.value());
      }
    }
  }

  logger.info("{}", to_c_str(log_buffer));
  log_buffer.clear();
}

static e2sm_ric_control_response convert_to_e2sm_response(const e2sm_ric_control_request&         e2sm_ccc_req,
                                                          const srs_du::du_param_config_request&  du_config_req_,
                                                          const srs_du::du_param_config_response& du_response_)
{
  const auto& e2_ctrl_req       = std::get<ric_ctrl_msg_s>(e2sm_ccc_req.request_ctrl_msg);
  const auto& e2_cell_ctrl_list = e2_ctrl_req.ctrl_msg_format.ctrl_msg_format2().list_of_cells_ctrl;

  e2sm_ric_control_response e2sm_response   = {};
  e2sm_response.service_model               = e2sm_service_model_t::CCC;
  e2sm_response.success                     = du_response_.success;
  e2sm_response.ric_call_process_id_present = false;
  e2sm_response.ric_ctrl_outcome_present    = true;

  if (not du_response_.success) {
    e2sm_response.cause.set_misc().value = cause_misc_e::options::unspecified;
  }

  ctrl_outcome_format_c ctrl_outcome;
  auto&                 ctrl_outcome_f2 = ctrl_outcome.set_ctrl_outcome_format2();
  ctrl_outcome_f2.rx_timestamp_present  = false;
  ctrl_outcome_f2.list_of_cells_for_ctrl_outcome.resize(e2_cell_ctrl_list.size());
  auto& outcome_cell_ctrl_list = ctrl_outcome_f2.list_of_cells_for_ctrl_outcome;
  for (unsigned i = 0, e = e2_cell_ctrl_list.size(); i < e; i++) {
    outcome_cell_ctrl_list[i].cell_global_id = e2_cell_ctrl_list[i].cell_global_id;

    if (du_response_.success) {
      // If success then all to accepted.
      for (const auto& req_struct : e2_cell_ctrl_list[i].list_of_cfg_structures) {
        cfg_structure_accepted_s cfg_structure_accepted;
        cfg_structure_accepted.ran_cfg_structure_name       = req_struct.ran_cfg_structure_name;
        cfg_structure_accepted.old_values_of_attributes     = req_struct.old_values_of_attributes;
        cfg_structure_accepted.current_values_of_attributes = req_struct.new_values_of_attributes;
        cfg_structure_accepted.applied_timestamp_present    = false;
        outcome_cell_ctrl_list[i].ran_cfg_structures_accepted_list.push_back(cfg_structure_accepted);
      }
    } else {
      for (const auto& req_struct : e2_cell_ctrl_list[i].list_of_cfg_structures) {
        cfg_structure_failed_s cfg_structure_failed;
        cfg_structure_failed.ran_cfg_structure_name         = req_struct.ran_cfg_structure_name;
        cfg_structure_failed.old_values_of_attributes       = req_struct.old_values_of_attributes;
        cfg_structure_failed.requested_values_of_attributes = req_struct.new_values_of_attributes;
        cfg_structure_failed.cause                          = cause_opts::unspecified;
        outcome_cell_ctrl_list[i].ran_cfg_structures_failed_list.push_back(cfg_structure_failed);
      }
    }
  }
  e2sm_response.ric_ctrl_outcome = ctrl_outcome;

  return e2sm_response;
}

e2sm_ccc_control_o_rrm_policy_ratio_executor::e2sm_ccc_control_o_rrm_policy_ratio_executor(
    srs_du::du_configurator& du_configurator_) :
  e2sm_ccc_control_action_du_executor_base(du_configurator_, 6)
{
  // RAN Configuration Structure description:
  ran_cfg_structure_name = "O-RRMPolicyRatio";
  attributes.emplace_back("resourceType");
  attributes.emplace_back("rRMPolicyMemberList");
  attributes.emplace_back("rRMPolicyMaxRatio");
  attributes.emplace_back("rRMPolicyMinRatio");
  attributes.emplace_back("rRMPolicyDedicatedRatio");
}

bool e2sm_ccc_control_o_rrm_policy_ratio_executor::ric_control_action_supported(const e2sm_ric_control_request& req)
{
  const auto& ctrl_hdr = std::get<ric_ctrl_hdr_s>(req.request_ctrl_hdr);
  const auto& ctrl_msg = std::get<ric_ctrl_msg_s>(req.request_ctrl_msg);

  int64_t ric_style_type = ctrl_hdr.ctrl_hdr_format.ctrl_hdr_format1().ric_style_type;
  if (ric_style_type != 2) {
    return false;
  }

  if (ctrl_msg.ctrl_msg_format.type() != ctrl_msg_format_c::types::ctrl_msg_format2) {
    return false;
  }

  const auto& cell_ctrl      = ctrl_msg.ctrl_msg_format.ctrl_msg_format2().list_of_cells_ctrl[0];
  const auto& ran_cfg_struct = cell_ctrl.list_of_cfg_structures[0];

  if (ran_cfg_struct.ran_cfg_structure_name.to_string() != ran_cfg_structure_name) {
    return false;
  }
  const auto& rrm_cfg = ran_cfg_struct.new_values_of_attributes.ran_cfg_structure.o_rrm_policy_ratio();
  if (rrm_cfg.res_type.value != res_type_opts::prb_dl and rrm_cfg.res_type.value != res_type_opts::prb_ul) {
    return false;
  }

  return true;
}

async_task<e2sm_ric_control_response>
e2sm_ccc_control_o_rrm_policy_ratio_executor::execute_ric_control_action(const e2sm_ric_control_request& req)
{
  srs_du::du_param_config_request du_ctrl_config_req = convert_to_du_config_request(req);

  for (const auto& cell_cfg : du_ctrl_config_req.cells) {
    // If empty request, return failure.
    if (cell_cfg.rrm_policy_ratio_list.empty()) {
      return return_ctrl_failure(req);
    }
    // If any policy is missing members, return failure.
    for (const auto& policy : cell_cfg.rrm_policy_ratio_list) {
      if (policy.policy_members_list.empty()) {
        return return_ctrl_failure(req);
      }
    }
    // If PRB quota not provided, return failure.
    for (const auto& policy : cell_cfg.rrm_policy_ratio_list) {
      if (!policy.min_prb_policy_ratio.has_value()) {
        return return_ctrl_failure(req);
      }
      if (!policy.max_prb_policy_ratio.has_value()) {
        return return_ctrl_failure(req);
      }
      if (!policy.ded_prb_policy_ratio.has_value()) {
        return return_ctrl_failure(req);
      }
    }
  }
  // Log received control request.
  log_du_config_request(logger, du_ctrl_config_req);

  return launch_async([this, &req, ctrl_config = std::move(du_ctrl_config_req)](
                          coro_context<async_task<e2sm_ric_control_response>>& ctx) {
    CORO_BEGIN(ctx);
    srs_du::du_param_config_response ctrl_response = du_param_configurator.handle_operator_config_request(ctrl_config);
    e2sm_ric_control_response        e2_resp       = convert_to_e2sm_response(req, ctrl_config, ctrl_response);
    CORO_RETURN(e2_resp);
  });
}
