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

#include "e2sm_ccc_asn1_packer.h"
#include "srsran/asn1/e2sm/e2sm_ccc.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_ccc;
using namespace srsran;

const std::string e2sm_ccc_asn1_packer::short_name       = "ORAN-E2SM-CCC";
const std::string e2sm_ccc_asn1_packer::oid              = "1.3.6.1.4.1.53148.1.6.2.4";
const std::string e2sm_ccc_asn1_packer::func_description = "Cell Configuration and Control";
const uint32_t    e2sm_ccc_asn1_packer::ran_func_id      = 4;
const uint32_t    e2sm_ccc_asn1_packer::revision         = 0;

static asn1::unbounded_octstring<true> json_to_octstring(const nlohmann::ordered_json& j)
{
  std::string          json_str = j.dump();
  std::vector<uint8_t> json_bytes(json_str.begin(), json_str.end());

  asn1::unbounded_octstring<true> octstr;
  if (not octstr.resize(json_bytes.size())) {
    printf("Failed to pack E2SM-CCC JSON message.\n");
    asn1::unbounded_octstring<true> empty;
    return empty;
  }
  std::copy(json_bytes.begin(), json_bytes.end(), octstr.begin());
  return octstr;
}

static expected<nlohmann::ordered_json, std::string> octstring_to_json(const asn1::unbounded_octstring<true>& octstr)
{
  std::string json_str(octstr.begin(), octstr.end());
  try {
    nlohmann::ordered_json j = nlohmann::ordered_json::parse(json_str);
    return j;
  } catch (const nlohmann::json::parse_error& e) {
    return make_unexpected(std::string("E2SM-CCC JSON parse error: ") + e.what());
  }
}

e2sm_ccc_asn1_packer::e2sm_ccc_asn1_packer() = default;

bool e2sm_ccc_asn1_packer::add_e2sm_control_service(e2sm_control_service* control_service)
{
  control_services.emplace(control_service->get_style_type(), control_service);
  return true;
}

e2sm_event_trigger_definition
e2sm_ccc_asn1_packer::handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition)
{
  // TODO: add support for RIC subscriptions.
  printf("Failure - Trigger definition handling not supported in E2SM-CCC.\n");
  return {};
}

e2sm_action_definition
e2sm_ccc_asn1_packer::handle_packed_e2sm_action_definition(const srsran::byte_buffer& action_definition)
{
  // TODO: add support for RIC subscriptions.
  printf("Failure - Action definition handling not supported in E2SM-CCC.\n");
  return {};
}

e2sm_ric_control_request
e2sm_ccc_asn1_packer::handle_packed_ric_control_request(const asn1::e2ap::ric_ctrl_request_s& req)
{
  e2sm_ric_control_request ric_control_request     = {};
  ric_control_request.service_model                = e2sm_service_model_t::CCC;
  ric_control_request.ric_call_process_id_present  = req->ric_call_process_id_present;
  ric_control_request.ric_ctrl_ack_request_present = req->ric_ctrl_ack_request_present;

  if (ric_control_request.ric_call_process_id_present) {
    ric_control_request.ric_call_process_id = req->ric_call_process_id.to_number();
  }
  auto ctrl_hdr_json_exp = octstring_to_json(req->ric_ctrl_hdr);
  if (ctrl_hdr_json_exp) {
    ric_ctrl_hdr_s ric_ctrl_hdr          = ctrl_hdr_json_exp.value();
    ric_control_request.request_ctrl_hdr = ric_ctrl_hdr;
  } else {
    printf("Failed to unpack E2SM-CCC Control Request Header\n");
  }

  auto ctrl_msg_json_exp = octstring_to_json(req->ric_ctrl_msg);
  if (ctrl_msg_json_exp) {
    ric_ctrl_msg_s ric_ctrl_msg          = ctrl_msg_json_exp.value();
    ric_control_request.request_ctrl_msg = ric_ctrl_msg;
  } else {
    printf("Failed to unpack E2SM-CCC Control Request Message\n");
  }

  if (ric_control_request.ric_ctrl_ack_request_present) {
    ric_control_request.ric_ctrl_ack_request =
        req->ric_ctrl_ack_request.value == asn1::e2ap::ric_ctrl_ack_request_e::ack;
  }
  return ric_control_request;
}

e2_ric_control_response e2sm_ccc_asn1_packer::pack_ric_control_response(const e2sm_ric_control_response& e2sm_response)
{
  e2_ric_control_response e2_control_response = {};
  e2_control_response.success                 = e2sm_response.success;

  if (e2_control_response.success) {
    if (e2sm_response.ric_ctrl_outcome_present) {
      e2_control_response.ack->ric_ctrl_outcome_present = true;
      nlohmann::ordered_json outcome_json       = std::get<ctrl_outcome_format_c>(e2sm_response.ric_ctrl_outcome);
      e2_control_response.ack->ric_ctrl_outcome = json_to_octstring(outcome_json);
    }
  } else {
    if (e2sm_response.ric_ctrl_outcome_present) {
      e2_control_response.failure->ric_ctrl_outcome_present = true;
      nlohmann::ordered_json outcome_json       = std::get<ctrl_outcome_format_c>(e2sm_response.ric_ctrl_outcome);
      e2_control_response.ack->ric_ctrl_outcome = json_to_octstring(outcome_json);
    }
    e2_control_response.failure->cause = e2sm_response.cause;
  }

  return e2_control_response;
}

asn1::unbounded_octstring<true> e2sm_ccc_asn1_packer::pack_ran_function_description()
{
  ran_function_definition_s ran_function_desc;

  // RAN Function name.
  ran_function_desc.ran_function_name.ran_function_short_name.resize(short_name.size());
  ran_function_desc.ran_function_name.ran_function_short_name.from_string(short_name);
  ran_function_desc.ran_function_name.ran_function_service_model_o_id.resize(oid.size());
  ran_function_desc.ran_function_name.ran_function_service_model_o_id.from_string(oid);
  ran_function_desc.ran_function_name.ran_function_description.resize(func_description.size());
  ran_function_desc.ran_function_name.ran_function_description.from_string(func_description);
  ran_function_desc.ran_function_name.ran_function_instance_present = true;
  ran_function_desc.ran_function_name.ran_function_instance         = revision;

  // E2 Node level config not supported.
  ran_function_desc.list_of_supported_node_level_cfg_structures.clear();

  // Cell-level configs.
  ran_function_desc.list_of_cells_for_ran_function_definition.resize(1); /// NUmber of cells
  auto& cell_desc = ran_function_desc.list_of_cells_for_ran_function_definition.back();
  auto& nr_cgi    = cell_desc.cell_global_id.set_nr_cgi();
  // TODO: we need to have cell_ids here. Now will with dummy values.
  nr_cgi.nr_cell_id.from_number(6733824);
  nr_cgi.plmn_id.mcc_present = true;
  nr_cgi.plmn_id.mcc.from_string("001");
  nr_cgi.plmn_id.mnc_present = true;
  nr_cgi.plmn_id.mnc.from_string("01");

  // TODO: currently filled statically, it has to be taken from the loaded services.
  // Now only O-RRMPolicyRatio supported in Control service style 2 (cell-level).
  cell_desc.list_of_supported_cell_level_ran_cfg_structures.resize(1);
  auto& rrm_policy_ran_cfg_struct = cell_desc.list_of_supported_cell_level_ran_cfg_structures.back();
  rrm_policy_ran_cfg_struct.ran_cfg_structure_name.from_string("O-RRMPolicyRatio");
  rrm_policy_ran_cfg_struct.list_of_supported_attributes.resize(5);
  // O-RRMPolicyRatio::resourceType. Is-writable = False.
  attribute_s& resource_type_attribute = rrm_policy_ran_cfg_struct.list_of_supported_attributes[0];
  resource_type_attribute.attribute_name.from_string("resourceType");
  resource_type_attribute.supported_services.ctrl_service_present = true;
  resource_type_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.resize(1);
  auto& resource_type_ctrl_style =
      resource_type_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.back();
  resource_type_ctrl_style.ctrl_service_style_type = 2;
  resource_type_ctrl_style.ctrl_service_style_name.from_string("Cell Configuration and Control");
  resource_type_ctrl_style.ctrl_service_hdr_format_type            = 1;
  resource_type_ctrl_style.ctrl_service_msg_format_type            = 2;
  resource_type_ctrl_style.ric_call_process_id_format_type_present = false;
  resource_type_ctrl_style.ctrl_service_ctrl_outcome_format_type   = 2;
  // O-RRMPolicyRatio::rRMPolicyMemberList. Is-writable = True.
  attribute_s& member_list_attribute = rrm_policy_ran_cfg_struct.list_of_supported_attributes[1];
  member_list_attribute.attribute_name.from_string("rRMPolicyMemberList");
  member_list_attribute.supported_services.ctrl_service_present = true;
  member_list_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.resize(1);
  auto& member_list_ctrl_style =
      member_list_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.back();
  member_list_ctrl_style.ctrl_service_style_type = 2;
  member_list_ctrl_style.ctrl_service_style_name.from_string("Cell Configuration and Control");
  member_list_ctrl_style.ctrl_service_hdr_format_type            = 1;
  member_list_ctrl_style.ctrl_service_msg_format_type            = 2;
  member_list_ctrl_style.ric_call_process_id_format_type_present = false;
  member_list_ctrl_style.ctrl_service_ctrl_outcome_format_type   = 2;
  // O-RRMPolicyRatio::rRMPolicyMaxRatio. Is-writable = True.
  attribute_s& policy_max_attribute = rrm_policy_ran_cfg_struct.list_of_supported_attributes[2];
  policy_max_attribute.attribute_name.from_string("rRMPolicyMaxRatio");
  policy_max_attribute.supported_services.ctrl_service_present = true;
  policy_max_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.resize(1);
  auto& policy_max_ctrl_style =
      policy_max_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.back();
  policy_max_ctrl_style.ctrl_service_style_type = 2;
  policy_max_ctrl_style.ctrl_service_style_name.from_string("Cell Configuration and Control");
  policy_max_ctrl_style.ctrl_service_hdr_format_type            = 1;
  policy_max_ctrl_style.ctrl_service_msg_format_type            = 2;
  policy_max_ctrl_style.ric_call_process_id_format_type_present = false;
  policy_max_ctrl_style.ctrl_service_ctrl_outcome_format_type   = 2;
  // O-RRMPolicyRatio::rRMPolicyMinRatio. Is-writable = True.
  attribute_s& policy_min_attribute = rrm_policy_ran_cfg_struct.list_of_supported_attributes[3];
  policy_min_attribute.attribute_name.from_string("rRMPolicyMinRatio");
  policy_min_attribute.supported_services.ctrl_service_present = true;
  policy_min_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.resize(1);
  auto& policy_min_ctrl_style =
      policy_min_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.back();
  policy_min_ctrl_style.ctrl_service_style_type = 2;
  policy_min_ctrl_style.ctrl_service_style_name.from_string("Cell Configuration and Control");
  policy_min_ctrl_style.ctrl_service_hdr_format_type            = 1;
  policy_min_ctrl_style.ctrl_service_msg_format_type            = 2;
  policy_min_ctrl_style.ric_call_process_id_format_type_present = false;
  policy_min_ctrl_style.ctrl_service_ctrl_outcome_format_type   = 2;
  // O-RRMPolicyRatio::rRMPolicyDedicatedRatio. Is-writable = True.
  attribute_s& policy_ded_attribute = rrm_policy_ran_cfg_struct.list_of_supported_attributes[4];
  policy_ded_attribute.attribute_name.from_string("rRMPolicyDedicatedRatio");
  policy_ded_attribute.supported_services.ctrl_service_present = true;
  policy_ded_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.resize(1);
  auto& policy_ded_ctrl_style =
      policy_ded_attribute.supported_services.ctrl_service.list_of_supported_ctrl_styles.back();
  policy_ded_ctrl_style.ctrl_service_style_type = 2;
  policy_ded_ctrl_style.ctrl_service_style_name.from_string("Cell Configuration and Control");
  policy_ded_ctrl_style.ctrl_service_hdr_format_type            = 1;
  policy_ded_ctrl_style.ctrl_service_msg_format_type            = 2;
  policy_ded_ctrl_style.ric_call_process_id_format_type_present = false;
  policy_ded_ctrl_style.ctrl_service_ctrl_outcome_format_type   = 2;

  nlohmann::ordered_json json = ran_function_desc;
  return json_to_octstring(json);
}
