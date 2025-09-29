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

#include "e2sm_ccc_control_service_impl.h"
#include "e2sm_ccc_control_action_du_executor.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm;
using namespace asn1::e2sm_ccc;
using namespace srsran;

e2sm_ccc_control_service::e2sm_ccc_control_service(uint32_t style_id_) :
  logger(srslog::fetch_basic_logger("E2SM-CCC")), style_id(style_id_)
{
  if (!get_e2sm_ccc_control_style_def(style_id_, control_service_def)) {
    logger.error("Control Service Style %i does not exist\n", style_id_);
  }
}

ran_function_definition_ctrl_item_s e2sm_ccc_control_service::get_control_style_definition()
{
  // TODO: ran_function_definition_ctrl_item_s is from RC, need to remove it from the control_service interface.
  return {};
}

uint32_t e2sm_ccc_control_service::get_style_type()
{
  return style_id;
}

bool e2sm_ccc_control_service::add_e2sm_rc_control_action_executor(
    std::unique_ptr<e2sm_control_action_executor> config_req_executor)
{
  config_req_executors.emplace(config_req_executor->get_action_name(), std::move(config_req_executor));
  return true;
}

e2sm_ccc_control_service_style_2::e2sm_ccc_control_service_style_2() : e2sm_ccc_control_service(2) {}

bool e2sm_ccc_control_service_style_2::control_request_supported(const e2sm_ric_control_request& req)
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

  const auto& cell_ctrl_list = ctrl_msg.ctrl_msg_format.ctrl_msg_format2().list_of_cells_ctrl;
  for (auto const& cell_ctrl : cell_ctrl_list) {
    // Check if requested cells are present.
    if (cell_ctrl.cell_global_id.type() != cell_global_id_c::types::nr_cgi) {
      return false;
    }

    // TODO: we need to have cell_ids here.
    const auto& nr_cgi = cell_ctrl.cell_global_id.nr_cgi();
    if (nr_cgi.nr_cell_id.to_number() == 0) {
      return false;
    }

    // Check if requested RAN control structures are present.
    for (auto const& cfg_struct : cell_ctrl.list_of_cfg_structures) {
      std::string ran_cfg_struct_name = cfg_struct.ran_cfg_structure_name.to_string();
      auto        it                  = config_req_executors.find(ran_cfg_struct_name);
      if (it == config_req_executors.end()) {
        return false;
      }
      e2sm_control_action_executor* executor = it->second.get();
      if (not executor->ric_control_action_supported(req)) {
        return false;
      }
    }
  }

  return true;
}

async_task<e2sm_ric_control_response>
e2sm_ccc_control_service_style_2::execute_control_request(const e2sm_ric_control_request& req)
{
  const auto& ctrl_msg = std::get<ric_ctrl_msg_s>(req.request_ctrl_msg);

  if (not control_request_supported(req)) {
    return launch_async([](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
      CORO_BEGIN(ctx);
      e2sm_ric_control_response ctrl_response;
      ctrl_response.success                 = false;
      ctrl_response.cause.set_ric_request() = cause_ric_request_e::options::action_not_supported;
      CORO_RETURN(ctrl_response);
    });
  }

  // Currently, only O-RRMPolicyRatio is accepted, so all fields are the same here.
  const auto& ran_cfg_struct =
      ctrl_msg.ctrl_msg_format.ctrl_msg_format2().list_of_cells_ctrl[0].list_of_cfg_structures[0];
  std::string ran_cfg_struct_name = ran_cfg_struct.ran_cfg_structure_name.to_string();
  return config_req_executors[ran_cfg_struct_name]->execute_ric_control_action(req);
}
