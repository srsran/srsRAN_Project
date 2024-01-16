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

#include "e2sm_rc_asn1_packer.h"
#include "e2sm_rc_control_service_impl.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;
using namespace srsran;

const std::string e2sm_rc_asn1_packer::short_name       = "ORAN-E2SM-RC";
const std::string e2sm_rc_asn1_packer::oid              = "1.3.6.1.4.1.53148.1.1.2.3";
const std::string e2sm_rc_asn1_packer::func_description = "RAN Control";
const uint32_t    e2sm_rc_asn1_packer::ran_func_id      = 3;
const uint32_t    e2sm_rc_asn1_packer::revision         = 0;

e2sm_rc_asn1_packer::e2sm_rc_asn1_packer() {}

bool e2sm_rc_asn1_packer::add_e2sm_control_service(e2sm_control_service* control_service)
{
  control_services.emplace(control_service->get_style_type(), control_service);
  return true;
}

e2sm_action_definition
e2sm_rc_asn1_packer::handle_packed_e2sm_action_definition(const srsran::byte_buffer& action_definition)
{
  e2sm_action_definition action_def;
  asn1::cbit_ref         bref(action_definition);
  if (variant_get<asn1::e2sm_rc::e2_sm_rc_action_definition_s>(action_def.action_definition).unpack(bref) !=
      asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM RC Action Definition\n");
  }
  return action_def;
}

e2sm_ric_control_request
e2sm_rc_asn1_packer::handle_packed_ric_control_request(const asn1::e2ap::ri_cctrl_request_s& req)
{
  e2sm_ric_control_request ric_control_request     = {};
  ric_control_request.service_model                = e2sm_service_model_t::RC;
  ric_control_request.ric_call_process_id_present  = req->ri_ccall_process_id_present;
  ric_control_request.ric_ctrl_ack_request_present = req->ri_cctrl_ack_request_present;

  if (ric_control_request.ric_call_process_id_present) {
    ric_control_request.ric_call_process_id = req->ri_ccall_process_id->to_number();
  }

  asn1::cbit_ref bref_hdr(req->ri_cctrl_hdr.value);
  asn1::cbit_ref bref_msg(req->ri_cctrl_msg.value);
  if (variant_get<asn1::e2sm_rc::e2_sm_rc_ctrl_hdr_s>(ric_control_request.request_ctrl_hdr).unpack(bref_hdr) !=
      asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM RC Control Request Header\n");
  }

  if (variant_get<asn1::e2sm_rc::e2_sm_rc_ctrl_msg_s>(ric_control_request.request_ctrl_msg).unpack(bref_msg) !=
      asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM RC Control Request Message\n");
  }

  if (ric_control_request.ric_ctrl_ack_request_present) {
    ric_control_request.ric_ctrl_ack_request =
        req->ri_cctrl_ack_request->value == asn1::e2ap::ri_cctrl_ack_request_e::ack;
  }
  return ric_control_request;
};

e2_ric_control_response e2sm_rc_asn1_packer::pack_ric_control_response(const e2sm_ric_control_response& e2sm_response)
{
  e2_ric_control_response e2_control_response = {};

  e2_control_response.success = e2sm_response.success;

  if (e2_control_response.success) {
    if (e2sm_response.ric_ctrl_outcome_present) {
      e2_control_response.ack->ri_cctrl_outcome_present = true;
      srsran::byte_buffer buf;
      asn1::bit_ref       bref(buf);
      if (variant_get<e2_sm_rc_ctrl_outcome_s>(e2sm_response.ric_ctrl_outcome).pack(bref) != asn1::SRSASN_SUCCESS) {
        printf("Failed to pack E2SM RC RIC Control Outcome (Ack)\n");
      }
      e2_control_response.ack->ri_cctrl_outcome->resize(buf.length());
      std::copy(buf.begin(), buf.end(), e2_control_response.ack->ri_cctrl_outcome->begin());
    }
  } else {
    if (e2sm_response.ric_ctrl_outcome_present) {
      e2_control_response.failure->ri_cctrl_outcome_present = true;
      srsran::byte_buffer buf;
      asn1::bit_ref       bref(buf);
      if (variant_get<e2_sm_rc_ctrl_outcome_s>(e2sm_response.ric_ctrl_outcome).pack(bref) != asn1::SRSASN_SUCCESS) {
        printf("Failed to pack E2SM RC RIC Control Outcome (Failure)\n");
      }
      e2_control_response.failure->ri_cctrl_outcome->resize(buf.length());
      std::copy(buf.begin(), buf.end(), e2_control_response.failure->ri_cctrl_outcome->begin());
    }
    e2_control_response.failure->cause.value = e2sm_response.cause;
  }

  return e2_control_response;
}

e2sm_event_trigger_definition
e2sm_rc_asn1_packer::handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition)
{
  // TODO
  return e2sm_event_trigger_definition();
}

asn1::unbounded_octstring<true> e2sm_rc_asn1_packer::pack_ran_function_description()
{
  e2_sm_rc_ran_function_definition_s ran_function_desc;
  ran_function_desc.ran_function_definition_report_present        = false;
  ran_function_desc.ran_function_definition_policy_present        = false;
  ran_function_desc.ran_function_definition_insert_present        = false;
  ran_function_desc.ran_function_definition_event_trigger_present = false;
  ran_function_desc.ran_function_definition_ctrl_present          = false;

  ran_function_desc.ran_function_name.ran_function_short_name.resize(short_name.size());
  ran_function_desc.ran_function_name.ran_function_e2_sm_o_id.resize(oid.size());
  ran_function_desc.ran_function_name.ran_function_description.resize(func_description.size());

  ran_function_desc.ran_function_name.ran_function_short_name.from_string(short_name.c_str());
  ran_function_desc.ran_function_name.ran_function_e2_sm_o_id.from_string(oid.c_str());
  ran_function_desc.ran_function_name.ran_function_description.from_string(func_description.c_str());
  ran_function_desc.ext = false;

  for (auto const& x : control_services) {
    ran_function_desc.ran_function_definition_ctrl_present = true;
    e2sm_rc_control_service_base* control_service          = dynamic_cast<e2sm_rc_control_service_base*>(x.second);

    if (!control_service) {
      continue;
    }

    ran_function_definition_ctrl_item_s ran_function_definition_ctrl_item;
    if (control_service->fill_ran_function_description(ran_function_definition_ctrl_item)) {
      ran_function_desc.ran_function_definition_ctrl.ric_ctrl_style_list.push_back(ran_function_definition_ctrl_item);
    }
  }
  asn1::unbounded_octstring<true> ran_function_description;

  srsran::byte_buffer buf;
  asn1::bit_ref       bref(buf);
  if (ran_function_desc.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to pack E2SM RC RAN Function Description\n");
    return ran_function_description;
  }

  ran_function_description.resize(buf.length());
  std::copy(buf.begin(), buf.end(), ran_function_description.begin());
  return ran_function_description;
}
