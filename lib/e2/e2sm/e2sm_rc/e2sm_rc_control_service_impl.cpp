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

#include "e2sm_rc_control_service_impl.h"
#include "e2sm_rc_control_action_du_executor.h"
#include <algorithm>

using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;
using namespace srsran;

e2sm_rc_control_service_base::e2sm_rc_control_service_base(uint32_t style_id_) :
  logger(srslog::fetch_basic_logger("E2SM-RC")), style_id(style_id_)
{
  if (!get_e2sm_rc_control_style_def(style_id_, control_service_def)) {
    logger.error("Control Service Style %i does not exist\n", style_id_);
  }
}

bool e2sm_rc_control_service_base::fill_ran_function_description(
    asn1::e2sm_rc::ran_function_definition_ctrl_item_s& ctrl_item)
{
  ctrl_item.ric_call_process_id_format_type_present = false;
  ctrl_item.ric_ctrl_style_type                     = style_id;
  ctrl_item.ric_ctrl_style_name.from_string(control_service_def.style_name);

  ctrl_item.ric_ctrl_hdr_format_type = control_service_def.ctrl_hdr_format;
  ctrl_item.ric_ctrl_msg_format_type = control_service_def.ctrl_msg_format;
  if (control_service_def.call_process_id_format) {
    ctrl_item.ric_call_process_id_format_type_present = true;
    ctrl_item.ric_call_process_id_format_type         = control_service_def.call_process_id_format;
  }
  ctrl_item.ric_ctrl_outcome_format_type = control_service_def.ctrl_outcome_format;

  for (auto const& x : config_req_executors) {
    e2sm_rc_control_action_du_executor_base* action_executor =
        dynamic_cast<e2sm_rc_control_action_du_executor_base*>(x.second.get());

    if (!action_executor) {
      continue;
    }

    ran_function_definition_ctrl_action_item_s ran_function_definition_ctrl_action_item;
    if (action_executor->fill_ran_function_description(ran_function_definition_ctrl_action_item)) {
      ctrl_item.ric_ctrl_action_list.push_back(ran_function_definition_ctrl_action_item);
    };

    // TODO: fill outcome properly
    ctrl_outcome_ran_param_item_s ctrl_outcome_ran_param_item;
    ctrl_outcome_ran_param_item.ran_param_id = 1;
    ctrl_outcome_ran_param_item.ran_param_name.from_string("test_outcome_ran_param");
    ctrl_item.ran_ctrl_outcome_params_list.push_back(ctrl_outcome_ran_param_item);
  }

  return true;
}

uint32_t e2sm_rc_control_service_base::get_style_type()
{
  return style_id;
}

bool e2sm_rc_control_service_base::add_e2sm_rc_control_action_executor(
    std::unique_ptr<e2sm_control_action_executor> config_req_executor)
{
  config_req_executors.emplace(config_req_executor->get_action_id(), std::move(config_req_executor));
  return true;
}

e2sm_rc_control_service::e2sm_rc_control_service(uint32_t style_id_) : e2sm_rc_control_service_base(style_id_) {}

bool e2sm_rc_control_service::control_request_supported(const e2sm_ric_control_request& req)
{
  const e2_sm_rc_ctrl_hdr_s& ctrl_hdr = variant_get<e2_sm_rc_ctrl_hdr_s>(req.request_ctrl_hdr);
  const e2_sm_rc_ctrl_msg_s& ctrl_msg = variant_get<e2_sm_rc_ctrl_msg_s>(req.request_ctrl_msg);

  // All styles 1 - 10 use hdr and msg format 1
  if (ctrl_hdr.ric_ctrl_hdr_formats.type().value !=
      e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format1) {
    return false;
  }

  if (ctrl_msg.ric_ctrl_msg_formats.type().value !=
      e2_sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::ctrl_msg_format1) {
    return false;
  }

  const e2_sm_rc_ctrl_hdr_format1_s& ctrl_hdr_f1 = ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format1();

  if (ctrl_hdr_f1.ric_style_type != style_id) {
    return false;
  }
  // TODO: check if UE supported
  // ctrl_hdr_f1.ue_id

  if (ctrl_hdr_f1.ric_ctrl_decision_present) {
    // TODO: check if ric_ctrl_decision supported
  }

  // If call process id not supported, then it cannot be provided in the request.
  if (control_service_def.call_process_id_format == 0) {
    if (req.ric_call_process_id_present) {
      return false;
    }
  }

  if (config_req_executors.find(ctrl_hdr_f1.ric_ctrl_action_id) != config_req_executors.end()) {
    if (!config_req_executors.at(ctrl_hdr_f1.ric_ctrl_action_id)->ric_control_action_supported(req)) {
      return false;
    }
  } else {
    return false;
  }
  return true;
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_service::execute_control_request(const e2sm_ric_control_request& req)
{
  const e2_sm_rc_ctrl_hdr_format1_s& ctrl_hdr =
      variant_get<e2_sm_rc_ctrl_hdr_s>(req.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format1();

  if (config_req_executors.find(ctrl_hdr.ric_ctrl_action_id) == config_req_executors.end()) {
    return launch_async([](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
      CORO_BEGIN(ctx);
      e2sm_ric_control_response ctrl_response;
      ctrl_response.success                 = false;
      ctrl_response.cause.set_ric_request() = cause_ri_crequest_e::options::action_not_supported;
      CORO_RETURN(ctrl_response);
    });
  }

  return config_req_executors[ctrl_hdr.ric_ctrl_action_id]->execute_ric_control_action(req);
}

e2sm_rc_control_service_style_255::e2sm_rc_control_service_style_255() : e2sm_rc_control_service_base(255) {}

e2sm_ric_control_request e2sm_rc_control_service_style_255::create_req_f1_from_req_f2(
    const e2_sm_rc_ctrl_hdr_format2_s&                                ctrl_hdr_f2,
    const asn1::e2sm_rc::e2_sm_rc_ctrl_msg_format2_style_item_s       style,
    const asn1::e2sm_rc::e2_sm_rc_ctrl_msg_format2_ctrl_action_item_s action)
{
  e2sm_ric_control_request req_f1;
  req_f1.ric_call_process_id_present  = false;
  req_f1.ric_ctrl_ack_request_present = false;
  req_f1.ric_ctrl_ack_request         = false;

  e2_sm_rc_ctrl_hdr_format1_s& hdr_f1 = variant_get<asn1::e2sm_rc::e2_sm_rc_ctrl_hdr_s>(req_f1.request_ctrl_hdr)
                                            .ric_ctrl_hdr_formats.set_ctrl_hdr_format1();

  e2_sm_rc_ctrl_msg_format1_s& msg_f1 = variant_get<asn1::e2sm_rc::e2_sm_rc_ctrl_msg_s>(req_f1.request_ctrl_msg)
                                            .ric_ctrl_msg_formats.set_ctrl_msg_format1();

  hdr_f1.ric_style_type     = style.indicated_ctrl_style_type;
  hdr_f1.ric_ctrl_action_id = action.ric_ctrl_action_id;

  if (ctrl_hdr_f2.ue_id_present) {
    hdr_f1.ue_id = ctrl_hdr_f2.ue_id;
  } else {
    // TODO: what if there is no UE?
    hdr_f1.ue_id.set(); // set nulltype
  }

  if (ctrl_hdr_f2.ric_ctrl_decision_present) {
    hdr_f1.ric_ctrl_decision_present = true;
    if (ctrl_hdr_f2.ric_ctrl_decision == e2_sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision_e_::accept) {
      hdr_f1.ric_ctrl_decision = e2_sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision_e_::accept;
    } else {
      hdr_f1.ric_ctrl_decision = e2_sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision_e_::reject;
    }
  }
  msg_f1.ran_p_list = action.ran_p_list.ran_p_list;

  return req_f1;
}

bool e2sm_rc_control_service_style_255::control_request_supported(const e2sm_ric_control_request& req)
{
  const e2_sm_rc_ctrl_hdr_s& ctrl_hdr = variant_get<e2_sm_rc_ctrl_hdr_s>(req.request_ctrl_hdr);
  const e2_sm_rc_ctrl_msg_s& ctrl_msg = variant_get<e2_sm_rc_ctrl_msg_s>(req.request_ctrl_msg);

  if (ctrl_hdr.ric_ctrl_hdr_formats.type().value !=
      e2_sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::ctrl_hdr_format2) {
    return false;
  }

  if (ctrl_msg.ric_ctrl_msg_formats.type().value !=
      e2_sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::ctrl_msg_format2) {
    return false;
  }

  const e2_sm_rc_ctrl_hdr_format2_s& ctrl_hdr_f2 = ctrl_hdr.ric_ctrl_hdr_formats.ctrl_hdr_format2();
  const e2_sm_rc_ctrl_msg_format2_s& ctrl_msg_f2 = ctrl_msg.ric_ctrl_msg_formats.ctrl_msg_format2();

  if (ctrl_hdr_f2.ue_id_present) {
    // TODO: check if UE supported
    // ctrl_hdr_f2.ue_id
  }

  if (ctrl_hdr_f2.ric_ctrl_decision_present) {
    // TODO: check if ric_ctrl_decision supported
  }

  for (auto& style : ctrl_msg_f2.ric_ctrl_style_list) {
    if (style.indicated_ctrl_style_type > 10) {
      return false;
    }

    if (e2sm_rc_control_services.find(style.indicated_ctrl_style_type) == e2sm_rc_control_services.end()) {
      return false;
    }

    for (auto& action : style.ric_ctrl_action_list) {
      // Create a control request with format 1 to match the API of control service styles 1-10.
      e2sm_ric_control_request tmp_req_f1 = create_req_f1_from_req_f2(ctrl_hdr_f2, style, action);

      if (!e2sm_rc_control_services.at(style.indicated_ctrl_style_type)->control_request_supported(tmp_req_f1)) {
        return false;
      }
    }
  }
  return false;
}

async_task<e2sm_ric_control_response>
e2sm_rc_control_service_style_255::execute_control_request(const e2sm_ric_control_request& req)
{
  std::vector<async_task<e2sm_ric_control_response>> tasks;
  e2sm_ric_control_response                          aggregated_response;
  aggregated_response.success = false;

  const e2_sm_rc_ctrl_hdr_format2_s& ctrl_hdr_f2 =
      variant_get<e2_sm_rc_ctrl_hdr_s>(req.request_ctrl_hdr).ric_ctrl_hdr_formats.ctrl_hdr_format2();
  const e2_sm_rc_ctrl_msg_format2_s& ctrl_msg_f2 =
      variant_get<e2_sm_rc_ctrl_msg_s>(req.request_ctrl_msg).ric_ctrl_msg_formats.ctrl_msg_format2();

  for (auto& style : ctrl_msg_f2.ric_ctrl_style_list) {
    for (auto& action : style.ric_ctrl_action_list) {
      // Create a control request with format 1 to match the API of control service styles 1-10.
      e2sm_ric_control_request t_req              = create_req_f1_from_req_f2(ctrl_hdr_f2, style, action);
      uint32_t                 ric_ctrl_action_id = action.ric_ctrl_action_id;

      if (config_req_executors.find(ric_ctrl_action_id) != config_req_executors.end()) {
        async_task<e2sm_ric_control_response> task =
            config_req_executors[ric_ctrl_action_id]->execute_ric_control_action(t_req);
        tasks.push_back(std::move(task));
      }
    }
  }

  if (tasks.size() == 0) {
    return launch_async([](coro_context<async_task<e2sm_ric_control_response>>& ctx) {
      CORO_BEGIN(ctx);
      e2sm_ric_control_response ctrl_response;
      ctrl_response.success                 = false;
      ctrl_response.cause.set_ric_request() = cause_ri_crequest_e::options::action_not_supported;
      CORO_RETURN(ctrl_response);
    });
  }

  return launch_async([tasks = std::move(tasks), aggregated_response = std::move(aggregated_response), i = (unsigned)0](
                          coro_context<async_task<e2sm_ric_control_response>>& ctx) mutable {
    e2sm_ric_control_response task_response;
    CORO_BEGIN(ctx);
    for (; i < tasks.size(); i++) {
      CORO_AWAIT_VALUE(task_response, tasks[i]);
      // Aggregate RIC control action response.
      aggregated_response.success &= task_response.success;
    }
    CORO_RETURN(aggregated_response);
  });
}
