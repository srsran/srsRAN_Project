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
#include <future>

using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;
using namespace srsran;

e2sm_rc_control_action_du_executor_base::e2sm_rc_control_action_du_executor_base(du_configurator& du_configurator_,
                                                                                 uint32_t         action_id_) :
  logger(srslog::fetch_basic_logger("E2SM-RC")),
  action_id(action_id_),
  du_param_configurator(du_configurator_),
  async_tasks(10)
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

e2sm_rc_control_action_2_6_du_executor::e2sm_rc_control_action_2_6_du_executor(du_configurator& du_configurator_) :
  e2sm_rc_control_action_du_executor_base(du_configurator_, 6)
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

e2_sm_ric_control_response_s
e2sm_rc_control_action_2_6_du_executor::execute_ric_control_action(const e2_sm_ric_control_request_s& req)
{
  du_mac_sched_control_config        ctrl_config;
  e2_sm_ric_control_response_s       e2_resp;
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

  std::promise<void>                   p;
  std::future<void>                    fut = p.get_future();
  du_mac_sched_control_config_response ctrl_response;

  async_tasks.schedule([this, &p, &ctrl_config, &ctrl_response](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(ctrl_response, du_param_configurator.configure_ue_mac_scheduler(ctrl_config));

    // Signal caller thread that the operation is complete.
    p.set_value();

    CORO_RETURN();
  });

  // Block waiting for reconfiguration to complete.
  fut.wait();

  e2_resp.success =
      ctrl_response.harq_processes_result and ctrl_response.max_prb_alloc_result and ctrl_response.min_prb_alloc_result;

  // Always fill outcome here, it will be decided later whether it should be included in the e2 response.
  e2_resp.ric_ctrl_outcome_present              = true;
  e2_sm_rc_ctrl_outcome_format1_s& ctrl_outcome = variant_get<e2_sm_rc_ctrl_outcome_s>(e2_resp.ric_ctrl_outcome)
                                                      .ric_ctrl_outcome_formats.set_ctrl_outcome_format1();

  // TODO: fill outcome properly
  e2_sm_rc_ctrl_outcome_format1_item_s test_outcome;
  test_outcome.ran_param_id                    = 1;
  test_outcome.ran_param_value.set_value_int() = 100;
  ctrl_outcome.ran_p_list.push_back(test_outcome);

  if (ctrl_config.min_prb_alloc.has_value()) {
    e2_sm_rc_ctrl_outcome_format1_item_s min_prb_outcome;
    min_prb_outcome.ran_param_id                    = 11;
    min_prb_outcome.ran_param_value.set_value_int() = ctrl_config.min_prb_alloc.value();
    ctrl_outcome.ran_p_list.push_back(min_prb_outcome);
  }

  if (ctrl_config.max_prb_alloc.has_value()) {
    e2_sm_rc_ctrl_outcome_format1_item_s max_prb_outcome;
    max_prb_outcome.ran_param_id                    = 12;
    max_prb_outcome.ran_param_value.set_value_int() = ctrl_config.max_prb_alloc.value();
    ctrl_outcome.ran_p_list.push_back(max_prb_outcome);
  }

  if (!e2_resp.success) {
    e2_resp.cause.set_misc().value = cause_misc_e::options::unspecified;
  }

  return e2_resp;
};
