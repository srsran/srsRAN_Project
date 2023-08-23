/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2sm_rc_asn1_packer.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;
using namespace srsran;

const std::string e2sm_rc_asn1_packer::short_name       = "ORAN-E2SM-RC";
const std::string e2sm_rc_asn1_packer::oid              = "1.3.6.1.4.1.53148.1.1.2.3";
const std::string e2sm_rc_asn1_packer::func_description = "RAN Control";
const uint32_t    e2sm_rc_asn1_packer::revision         = 0;

e2sm_rc_asn1_packer::e2sm_rc_asn1_packer()
{
  // DRB QoS Configuration(1) -> Radio Bearer control(1)
  std::vector<action_parameter_t> action_params1_1;
  action_params1_1.push_back({1, "DRB ID"});
  action_params1_1.push_back({2, "5QI"});
  action_params1_1.push_back({3, "Packet Delay Budget"});
  action_params1_1.push_back({4, "Packet Error Rate"});

  e2sm_rc_action_provider action_provider1("DRB QoS Configuration");
  for (auto& param : action_params1_1) {
    action_provider1.add_parameter_support(param.id, param.name);
  }
  e2sm_rc_style_provider style_provider1("Radio Bearer control");
  style_provider1.add_action_provider(1, action_provider1);

  // Slice-level PRB quota(6) -> Radio Resource Allocation Control(2)
  std::vector<action_parameter_t> action_params2_6;
  action_params2_6.push_back({1, "RRM Policy Ratio List"});
  action_params2_6.push_back({2, "RRM Policy Ratio Group"});
  action_params2_6.push_back({3, "RRM Policy"});
  action_params2_6.push_back({4, "RRM Policy Member List"});
  action_params2_6.push_back({5, "RRM Policy Member"});
  action_params2_6.push_back({6, "PLMN Identity"});
  action_params2_6.push_back({7, "S-NSSAI"});
  action_params2_6.push_back({8, "SST"});
  action_params2_6.push_back({9, "SD"});
  action_params2_6.push_back({10, "Min PRB Policy Ratio"});
  action_params2_6.push_back({11, "Max PRB Policy Ratio"});
  action_params2_6.push_back({12, "Dedicated PRB Policy Ratio"});

  e2sm_rc_action_provider action_provider6("Slice-level PRB quota");
  for (auto& param : action_params2_6) {
    action_provider6.add_parameter_support(param.id, param.name);
  }
  e2sm_rc_style_provider style_provider2("Radio Resource Allocation Control");
  style_provider2.add_action_provider(6, action_provider6);

  e2sm_rc_provider provider1("Control Service");
  provider1.add_style_provider(1, style_provider1);
  provider1.add_style_provider(2, style_provider2);
  param_providers.emplace(1, provider1);
}

e2_sm_action_definition_s
e2sm_rc_asn1_packer::handle_packed_e2sm_action_definition(const srsran::byte_buffer& action_definition)
{
  e2_sm_action_definition_s action_def;
  asn1::cbit_ref            bref(action_definition);
  if (action_def.action_definition_kpm.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM RC Action Definition\n");
  }
  return action_def;
}

e2_sm_event_trigger_definition_s
e2sm_rc_asn1_packer::handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition)
{
  // TODO
  return e2_sm_event_trigger_definition_s();
}

void e2sm_rc_asn1_packer::populate_control_ran_function_description(
    e2sm_rc_provider                    provider,
    e2_sm_rc_ran_function_definition_s& ran_function_description)
{
  ran_function_description.ran_function_definition_ctrl_present = true;
  for (auto& style_provider : provider.style_providers) {
    ran_function_definition_ctrl_item_s ran_function_definition_ctrl_item;
    ran_function_definition_ctrl_item.ric_ctrl_style_type = style_provider.first;
    ran_function_definition_ctrl_item.ric_ctrl_style_name.from_string(style_provider.second.name);
    for (auto& action_provider : style_provider.second.action_providers) {
      ran_function_definition_ctrl_action_item_s ran_function_definition_ctrl_action_item;
      ran_function_definition_ctrl_action_item.ric_ctrl_action_id = action_provider.first;
      ran_function_definition_ctrl_action_item.ric_ctrl_action_name.from_string(action_provider.second.name);
      for (auto& param : action_provider.second.action_params) {
        ctrl_action_ran_param_item_s ran_param_item;
        ran_param_item.ran_param_id = param.second.id;
        ran_param_item.ran_param_name.from_string(param.second.name);
        ran_function_definition_ctrl_action_item.ran_ctrl_action_params_list.push_back(ran_param_item);
      }
      ran_function_definition_ctrl_item.ric_ctrl_action_list.push_back(ran_function_definition_ctrl_action_item);
    }
    ran_function_description.ran_function_definition_ctrl.ric_ctrl_style_list.push_back(
        ran_function_definition_ctrl_item);
  }
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
  for (auto& provider : param_providers) {
    if (provider.second.name == "Control Service") {
      ran_function_desc.ran_function_definition_ctrl_present = true;
      populate_control_ran_function_description(provider.second, ran_function_desc);
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
