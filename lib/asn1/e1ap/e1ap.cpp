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

#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// E1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E1AP-ELEMENTARY-PROCEDURE
uint16_t e1ap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {0,  3,  4,  5,  6,  7,  8,  9,  10, 11, 22, 24, 1,  12, 13,
                                   14, 18, 15, 16, 17, 19, 21, 20, 2,  25, 23, 26, 27, 28};
  return map_enum_number(names, 29, idx, "proc_code");
}
bool e1ap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {0,  3,  4,  5,  6,  7,  8,  9,  10, 11, 22, 24, 1,  12, 13,
                                   14, 18, 15, 16, 17, 19, 21, 20, 2,  25, 23, 26, 27, 28};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
e1ap_elem_procs_o::init_msg_c e1ap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(init_msg_c::types::reset);
      break;
    case 3:
      ret.set(init_msg_c::types::gnb_cu_up_e1_setup_request);
      break;
    case 4:
      ret.set(init_msg_c::types::gnb_cu_cp_e1_setup_request);
      break;
    case 5:
      ret.set(init_msg_c::types::gnb_cu_up_cfg_upd);
      break;
    case 6:
      ret.set(init_msg_c::types::gnb_cu_cp_cfg_upd);
      break;
    case 7:
      ret.set(init_msg_c::types::e1_release_request);
      break;
    case 8:
      ret.set(init_msg_c::types::bearer_context_setup_request);
      break;
    case 9:
      ret.set(init_msg_c::types::bearer_context_mod_request);
      break;
    case 10:
      ret.set(init_msg_c::types::bearer_context_mod_required);
      break;
    case 11:
      ret.set(init_msg_c::types::bearer_context_release_cmd);
      break;
    case 22:
      ret.set(init_msg_c::types::res_status_request);
      break;
    case 24:
      ret.set(init_msg_c::types::iab_up_tnl_address_upd);
      break;
    case 1:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 12:
      ret.set(init_msg_c::types::bearer_context_release_request);
      break;
    case 13:
      ret.set(init_msg_c::types::bearer_context_inactivity_notif);
      break;
    case 14:
      ret.set(init_msg_c::types::dl_data_notif);
      break;
    case 18:
      ret.set(init_msg_c::types::ul_data_notif);
      break;
    case 15:
      ret.set(init_msg_c::types::data_usage_report);
      break;
    case 16:
      ret.set(init_msg_c::types::gnb_cu_up_counter_check_request);
      break;
    case 17:
      ret.set(init_msg_c::types::gnb_cu_up_status_ind);
      break;
    case 19:
      ret.set(init_msg_c::types::mrdc_data_usage_report);
      break;
    case 21:
      ret.set(init_msg_c::types::deactiv_trace);
      break;
    case 20:
      ret.set(init_msg_c::types::trace_start);
      break;
    case 2:
      ret.set(init_msg_c::types::private_msg);
      break;
    case 25:
      ret.set(init_msg_c::types::cell_traffic_trace);
      break;
    case 23:
      ret.set(init_msg_c::types::res_status_upd);
      break;
    case 26:
      ret.set(init_msg_c::types::early_forwarding_sn_transfer);
      break;
    case 27:
      ret.set(init_msg_c::types::gnb_cu_cp_meas_results_info);
      break;
    case 28:
      ret.set(init_msg_c::types::iabpsk_notif);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e1ap_elem_procs_o::successful_outcome_c e1ap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(successful_outcome_c::types::reset_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::gnb_cu_up_e1_setup_resp);
      break;
    case 4:
      ret.set(successful_outcome_c::types::gnb_cu_cp_e1_setup_resp);
      break;
    case 5:
      ret.set(successful_outcome_c::types::gnb_cu_up_cfg_upd_ack);
      break;
    case 6:
      ret.set(successful_outcome_c::types::gnb_cu_cp_cfg_upd_ack);
      break;
    case 7:
      ret.set(successful_outcome_c::types::e1_release_resp);
      break;
    case 8:
      ret.set(successful_outcome_c::types::bearer_context_setup_resp);
      break;
    case 9:
      ret.set(successful_outcome_c::types::bearer_context_mod_resp);
      break;
    case 10:
      ret.set(successful_outcome_c::types::bearer_context_mod_confirm);
      break;
    case 11:
      ret.set(successful_outcome_c::types::bearer_context_release_complete);
      break;
    case 22:
      ret.set(successful_outcome_c::types::res_status_resp);
      break;
    case 24:
      ret.set(successful_outcome_c::types::iab_up_tnl_address_upd_ack);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
e1ap_elem_procs_o::unsuccessful_outcome_c e1ap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 3:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_up_e1_setup_fail);
      break;
    case 4:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_cp_e1_setup_fail);
      break;
    case 5:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_up_cfg_upd_fail);
      break;
    case 6:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_cp_cfg_upd_fail);
      break;
    case 8:
      ret.set(unsuccessful_outcome_c::types::bearer_context_setup_fail);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::bearer_context_mod_fail);
      break;
    case 22:
      ret.set(unsuccessful_outcome_c::types::res_status_fail);
      break;
    case 24:
      ret.set(unsuccessful_outcome_c::types::iab_up_tnl_address_upd_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e e1ap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 0:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 6:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 8:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 22:
      return crit_e::reject;
    case 24:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 13:
      return crit_e::ignore;
    case 14:
      return crit_e::ignore;
    case 18:
      return crit_e::ignore;
    case 15:
      return crit_e::ignore;
    case 16:
      return crit_e::ignore;
    case 17:
      return crit_e::ignore;
    case 19:
      return crit_e::ignore;
    case 21:
      return crit_e::ignore;
    case 20:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 25:
      return crit_e::ignore;
    case 23:
      return crit_e::ignore;
    case 26:
      return crit_e::ignore;
    case 27:
      return crit_e::ignore;
    case 28:
      return crit_e::reject;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void e1ap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::reset:
      c = reset_s{};
      break;
    case types::gnb_cu_up_e1_setup_request:
      c = gnb_cu_up_e1_setup_request_s{};
      break;
    case types::gnb_cu_cp_e1_setup_request:
      c = gnb_cu_cp_e1_setup_request_s{};
      break;
    case types::gnb_cu_up_cfg_upd:
      c = gnb_cu_up_cfg_upd_s{};
      break;
    case types::gnb_cu_cp_cfg_upd:
      c = gnb_cu_cp_cfg_upd_s{};
      break;
    case types::e1_release_request:
      c = e1_release_request_s{};
      break;
    case types::bearer_context_setup_request:
      c = bearer_context_setup_request_s{};
      break;
    case types::bearer_context_mod_request:
      c = bearer_context_mod_request_s{};
      break;
    case types::bearer_context_mod_required:
      c = bearer_context_mod_required_s{};
      break;
    case types::bearer_context_release_cmd:
      c = bearer_context_release_cmd_s{};
      break;
    case types::res_status_request:
      c = res_status_request_s{};
      break;
    case types::iab_up_tnl_address_upd:
      c = iab_up_tnl_address_upd_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::bearer_context_release_request:
      c = bearer_context_release_request_s{};
      break;
    case types::bearer_context_inactivity_notif:
      c = bearer_context_inactivity_notif_s{};
      break;
    case types::dl_data_notif:
      c = dl_data_notif_s{};
      break;
    case types::ul_data_notif:
      c = ul_data_notif_s{};
      break;
    case types::data_usage_report:
      c = data_usage_report_s{};
      break;
    case types::gnb_cu_up_counter_check_request:
      c = gnb_cu_up_counter_check_request_s{};
      break;
    case types::gnb_cu_up_status_ind:
      c = gnb_cu_up_status_ind_s{};
      break;
    case types::mrdc_data_usage_report:
      c = mrdc_data_usage_report_s{};
      break;
    case types::deactiv_trace:
      c = deactiv_trace_s{};
      break;
    case types::trace_start:
      c = trace_start_s{};
      break;
    case types::private_msg:
      c = private_msg_s{};
      break;
    case types::cell_traffic_trace:
      c = cell_traffic_trace_s{};
      break;
    case types::res_status_upd:
      c = res_status_upd_s{};
      break;
    case types::early_forwarding_sn_transfer:
      c = early_forwarding_sn_transfer_s{};
      break;
    case types::gnb_cu_cp_meas_results_info:
      c = gnb_cu_cp_meas_results_info_s{};
      break;
    case types::iabpsk_notif:
      c = iabpsk_notif_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::init_msg_c");
  }
}
reset_s& e1ap_elem_procs_o::init_msg_c::reset()
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
gnb_cu_up_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_e1_setup_request()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_e1_setup_request_s>();
}
gnb_cu_cp_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_e1_setup_request()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_e1_setup_request_s>();
}
gnb_cu_up_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_cfg_upd()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_cfg_upd_s>();
}
gnb_cu_cp_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_cfg_upd()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_cfg_upd_s>();
}
e1_release_request_s& e1ap_elem_procs_o::init_msg_c::e1_release_request()
{
  assert_choice_type(types::e1_release_request, type_, "InitiatingMessage");
  return c.get<e1_release_request_s>();
}
bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_setup_request()
{
  assert_choice_type(types::bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bearer_context_setup_request_s>();
}
bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_request()
{
  assert_choice_type(types::bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_request_s>();
}
bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_required()
{
  assert_choice_type(types::bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_required_s>();
}
bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_cmd()
{
  assert_choice_type(types::bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bearer_context_release_cmd_s>();
}
res_status_request_s& e1ap_elem_procs_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
iab_up_tnl_address_upd_s& e1ap_elem_procs_o::init_msg_c::iab_up_tnl_address_upd()
{
  assert_choice_type(types::iab_up_tnl_address_upd, type_, "InitiatingMessage");
  return c.get<iab_up_tnl_address_upd_s>();
}
error_ind_s& e1ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_request()
{
  assert_choice_type(types::bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bearer_context_release_request_s>();
}
bearer_context_inactivity_notif_s& e1ap_elem_procs_o::init_msg_c::bearer_context_inactivity_notif()
{
  assert_choice_type(types::bearer_context_inactivity_notif, type_, "InitiatingMessage");
  return c.get<bearer_context_inactivity_notif_s>();
}
dl_data_notif_s& e1ap_elem_procs_o::init_msg_c::dl_data_notif()
{
  assert_choice_type(types::dl_data_notif, type_, "InitiatingMessage");
  return c.get<dl_data_notif_s>();
}
ul_data_notif_s& e1ap_elem_procs_o::init_msg_c::ul_data_notif()
{
  assert_choice_type(types::ul_data_notif, type_, "InitiatingMessage");
  return c.get<ul_data_notif_s>();
}
data_usage_report_s& e1ap_elem_procs_o::init_msg_c::data_usage_report()
{
  assert_choice_type(types::data_usage_report, type_, "InitiatingMessage");
  return c.get<data_usage_report_s>();
}
gnb_cu_up_counter_check_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_counter_check_request()
{
  assert_choice_type(types::gnb_cu_up_counter_check_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_counter_check_request_s>();
}
gnb_cu_up_status_ind_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_status_ind()
{
  assert_choice_type(types::gnb_cu_up_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_status_ind_s>();
}
mrdc_data_usage_report_s& e1ap_elem_procs_o::init_msg_c::mrdc_data_usage_report()
{
  assert_choice_type(types::mrdc_data_usage_report, type_, "InitiatingMessage");
  return c.get<mrdc_data_usage_report_s>();
}
deactiv_trace_s& e1ap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
trace_start_s& e1ap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
private_msg_s& e1ap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
cell_traffic_trace_s& e1ap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
res_status_upd_s& e1ap_elem_procs_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
early_forwarding_sn_transfer_s& e1ap_elem_procs_o::init_msg_c::early_forwarding_sn_transfer()
{
  assert_choice_type(types::early_forwarding_sn_transfer, type_, "InitiatingMessage");
  return c.get<early_forwarding_sn_transfer_s>();
}
gnb_cu_cp_meas_results_info_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_meas_results_info()
{
  assert_choice_type(types::gnb_cu_cp_meas_results_info, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_meas_results_info_s>();
}
iabpsk_notif_s& e1ap_elem_procs_o::init_msg_c::iabpsk_notif()
{
  assert_choice_type(types::iabpsk_notif, type_, "InitiatingMessage");
  return c.get<iabpsk_notif_s>();
}
const reset_s& e1ap_elem_procs_o::init_msg_c::reset() const
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
const gnb_cu_up_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_e1_setup_request() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_e1_setup_request_s>();
}
const gnb_cu_cp_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_e1_setup_request() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_e1_setup_request_s>();
}
const gnb_cu_up_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_cfg_upd_s>();
}
const gnb_cu_cp_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_cfg_upd_s>();
}
const e1_release_request_s& e1ap_elem_procs_o::init_msg_c::e1_release_request() const
{
  assert_choice_type(types::e1_release_request, type_, "InitiatingMessage");
  return c.get<e1_release_request_s>();
}
const bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_setup_request() const
{
  assert_choice_type(types::bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bearer_context_setup_request_s>();
}
const bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_request() const
{
  assert_choice_type(types::bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_request_s>();
}
const bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_required() const
{
  assert_choice_type(types::bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_required_s>();
}
const bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_cmd() const
{
  assert_choice_type(types::bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bearer_context_release_cmd_s>();
}
const res_status_request_s& e1ap_elem_procs_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const iab_up_tnl_address_upd_s& e1ap_elem_procs_o::init_msg_c::iab_up_tnl_address_upd() const
{
  assert_choice_type(types::iab_up_tnl_address_upd, type_, "InitiatingMessage");
  return c.get<iab_up_tnl_address_upd_s>();
}
const error_ind_s& e1ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_request() const
{
  assert_choice_type(types::bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bearer_context_release_request_s>();
}
const bearer_context_inactivity_notif_s& e1ap_elem_procs_o::init_msg_c::bearer_context_inactivity_notif() const
{
  assert_choice_type(types::bearer_context_inactivity_notif, type_, "InitiatingMessage");
  return c.get<bearer_context_inactivity_notif_s>();
}
const dl_data_notif_s& e1ap_elem_procs_o::init_msg_c::dl_data_notif() const
{
  assert_choice_type(types::dl_data_notif, type_, "InitiatingMessage");
  return c.get<dl_data_notif_s>();
}
const ul_data_notif_s& e1ap_elem_procs_o::init_msg_c::ul_data_notif() const
{
  assert_choice_type(types::ul_data_notif, type_, "InitiatingMessage");
  return c.get<ul_data_notif_s>();
}
const data_usage_report_s& e1ap_elem_procs_o::init_msg_c::data_usage_report() const
{
  assert_choice_type(types::data_usage_report, type_, "InitiatingMessage");
  return c.get<data_usage_report_s>();
}
const gnb_cu_up_counter_check_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_counter_check_request() const
{
  assert_choice_type(types::gnb_cu_up_counter_check_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_counter_check_request_s>();
}
const gnb_cu_up_status_ind_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_status_ind() const
{
  assert_choice_type(types::gnb_cu_up_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_status_ind_s>();
}
const mrdc_data_usage_report_s& e1ap_elem_procs_o::init_msg_c::mrdc_data_usage_report() const
{
  assert_choice_type(types::mrdc_data_usage_report, type_, "InitiatingMessage");
  return c.get<mrdc_data_usage_report_s>();
}
const deactiv_trace_s& e1ap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const trace_start_s& e1ap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const private_msg_s& e1ap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const cell_traffic_trace_s& e1ap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const res_status_upd_s& e1ap_elem_procs_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const early_forwarding_sn_transfer_s& e1ap_elem_procs_o::init_msg_c::early_forwarding_sn_transfer() const
{
  assert_choice_type(types::early_forwarding_sn_transfer, type_, "InitiatingMessage");
  return c.get<early_forwarding_sn_transfer_s>();
}
const gnb_cu_cp_meas_results_info_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_meas_results_info() const
{
  assert_choice_type(types::gnb_cu_cp_meas_results_info, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_meas_results_info_s>();
}
const iabpsk_notif_s& e1ap_elem_procs_o::init_msg_c::iabpsk_notif() const
{
  assert_choice_type(types::iabpsk_notif, type_, "InitiatingMessage");
  return c.get<iabpsk_notif_s>();
}
void e1ap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::reset:
      j.write_fieldname("Reset");
      c.get<reset_s>().to_json(j);
      break;
    case types::gnb_cu_up_e1_setup_request:
      j.write_fieldname("GNB-CU-UP-E1SetupRequest");
      c.get<gnb_cu_up_e1_setup_request_s>().to_json(j);
      break;
    case types::gnb_cu_cp_e1_setup_request:
      j.write_fieldname("GNB-CU-CP-E1SetupRequest");
      c.get<gnb_cu_cp_e1_setup_request_s>().to_json(j);
      break;
    case types::gnb_cu_up_cfg_upd:
      j.write_fieldname("GNB-CU-UP-ConfigurationUpdate");
      c.get<gnb_cu_up_cfg_upd_s>().to_json(j);
      break;
    case types::gnb_cu_cp_cfg_upd:
      j.write_fieldname("GNB-CU-CP-ConfigurationUpdate");
      c.get<gnb_cu_cp_cfg_upd_s>().to_json(j);
      break;
    case types::e1_release_request:
      j.write_fieldname("E1ReleaseRequest");
      c.get<e1_release_request_s>().to_json(j);
      break;
    case types::bearer_context_setup_request:
      j.write_fieldname("BearerContextSetupRequest");
      c.get<bearer_context_setup_request_s>().to_json(j);
      break;
    case types::bearer_context_mod_request:
      j.write_fieldname("BearerContextModificationRequest");
      c.get<bearer_context_mod_request_s>().to_json(j);
      break;
    case types::bearer_context_mod_required:
      j.write_fieldname("BearerContextModificationRequired");
      c.get<bearer_context_mod_required_s>().to_json(j);
      break;
    case types::bearer_context_release_cmd:
      j.write_fieldname("BearerContextReleaseCommand");
      c.get<bearer_context_release_cmd_s>().to_json(j);
      break;
    case types::res_status_request:
      j.write_fieldname("ResourceStatusRequest");
      c.get<res_status_request_s>().to_json(j);
      break;
    case types::iab_up_tnl_address_upd:
      j.write_fieldname("IAB-UPTNLAddressUpdate");
      c.get<iab_up_tnl_address_upd_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::bearer_context_release_request:
      j.write_fieldname("BearerContextReleaseRequest");
      c.get<bearer_context_release_request_s>().to_json(j);
      break;
    case types::bearer_context_inactivity_notif:
      j.write_fieldname("BearerContextInactivityNotification");
      c.get<bearer_context_inactivity_notif_s>().to_json(j);
      break;
    case types::dl_data_notif:
      j.write_fieldname("DLDataNotification");
      c.get<dl_data_notif_s>().to_json(j);
      break;
    case types::ul_data_notif:
      j.write_fieldname("ULDataNotification");
      c.get<ul_data_notif_s>().to_json(j);
      break;
    case types::data_usage_report:
      j.write_fieldname("DataUsageReport");
      c.get<data_usage_report_s>().to_json(j);
      break;
    case types::gnb_cu_up_counter_check_request:
      j.write_fieldname("GNB-CU-UP-CounterCheckRequest");
      c.get<gnb_cu_up_counter_check_request_s>().to_json(j);
      break;
    case types::gnb_cu_up_status_ind:
      j.write_fieldname("GNB-CU-UP-StatusIndication");
      c.get<gnb_cu_up_status_ind_s>().to_json(j);
      break;
    case types::mrdc_data_usage_report:
      j.write_fieldname("MRDC-DataUsageReport");
      c.get<mrdc_data_usage_report_s>().to_json(j);
      break;
    case types::deactiv_trace:
      j.write_fieldname("DeactivateTrace");
      c.get<deactiv_trace_s>().to_json(j);
      break;
    case types::trace_start:
      j.write_fieldname("TraceStart");
      c.get<trace_start_s>().to_json(j);
      break;
    case types::private_msg:
      j.write_fieldname("PrivateMessage");
      c.get<private_msg_s>().to_json(j);
      break;
    case types::cell_traffic_trace:
      j.write_fieldname("CellTrafficTrace");
      c.get<cell_traffic_trace_s>().to_json(j);
      break;
    case types::res_status_upd:
      j.write_fieldname("ResourceStatusUpdate");
      c.get<res_status_upd_s>().to_json(j);
      break;
    case types::early_forwarding_sn_transfer:
      j.write_fieldname("EarlyForwardingSNTransfer");
      c.get<early_forwarding_sn_transfer_s>().to_json(j);
      break;
    case types::gnb_cu_cp_meas_results_info:
      j.write_fieldname("GNB-CU-CPMeasurementResultsInformation");
      c.get<gnb_cu_cp_meas_results_info_s>().to_json(j);
      break;
    case types::iabpsk_notif:
      j.write_fieldname("IABPSKNotification");
      c.get<iabpsk_notif_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE e1ap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset:
      HANDLE_CODE(c.get<reset_s>().pack(bref));
      break;
    case types::gnb_cu_up_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_request_s>().pack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_request_s>().pack(bref));
      break;
    case types::gnb_cu_up_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_s>().pack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_s>().pack(bref));
      break;
    case types::e1_release_request:
      HANDLE_CODE(c.get<e1_release_request_s>().pack(bref));
      break;
    case types::bearer_context_setup_request:
      HANDLE_CODE(c.get<bearer_context_setup_request_s>().pack(bref));
      break;
    case types::bearer_context_mod_request:
      HANDLE_CODE(c.get<bearer_context_mod_request_s>().pack(bref));
      break;
    case types::bearer_context_mod_required:
      HANDLE_CODE(c.get<bearer_context_mod_required_s>().pack(bref));
      break;
    case types::bearer_context_release_cmd:
      HANDLE_CODE(c.get<bearer_context_release_cmd_s>().pack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().pack(bref));
      break;
    case types::iab_up_tnl_address_upd:
      HANDLE_CODE(c.get<iab_up_tnl_address_upd_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::bearer_context_release_request:
      HANDLE_CODE(c.get<bearer_context_release_request_s>().pack(bref));
      break;
    case types::bearer_context_inactivity_notif:
      HANDLE_CODE(c.get<bearer_context_inactivity_notif_s>().pack(bref));
      break;
    case types::dl_data_notif:
      HANDLE_CODE(c.get<dl_data_notif_s>().pack(bref));
      break;
    case types::ul_data_notif:
      HANDLE_CODE(c.get<ul_data_notif_s>().pack(bref));
      break;
    case types::data_usage_report:
      HANDLE_CODE(c.get<data_usage_report_s>().pack(bref));
      break;
    case types::gnb_cu_up_counter_check_request:
      HANDLE_CODE(c.get<gnb_cu_up_counter_check_request_s>().pack(bref));
      break;
    case types::gnb_cu_up_status_ind:
      HANDLE_CODE(c.get<gnb_cu_up_status_ind_s>().pack(bref));
      break;
    case types::mrdc_data_usage_report:
      HANDLE_CODE(c.get<mrdc_data_usage_report_s>().pack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().pack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().pack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().pack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().pack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().pack(bref));
      break;
    case types::early_forwarding_sn_transfer:
      HANDLE_CODE(c.get<early_forwarding_sn_transfer_s>().pack(bref));
      break;
    case types::gnb_cu_cp_meas_results_info:
      HANDLE_CODE(c.get<gnb_cu_cp_meas_results_info_s>().pack(bref));
      break;
    case types::iabpsk_notif:
      HANDLE_CODE(c.get<iabpsk_notif_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1ap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset:
      HANDLE_CODE(c.get<reset_s>().unpack(bref));
      break;
    case types::gnb_cu_up_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_request_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_request:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_request_s>().unpack(bref));
      break;
    case types::gnb_cu_up_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_s>().unpack(bref));
      break;
    case types::e1_release_request:
      HANDLE_CODE(c.get<e1_release_request_s>().unpack(bref));
      break;
    case types::bearer_context_setup_request:
      HANDLE_CODE(c.get<bearer_context_setup_request_s>().unpack(bref));
      break;
    case types::bearer_context_mod_request:
      HANDLE_CODE(c.get<bearer_context_mod_request_s>().unpack(bref));
      break;
    case types::bearer_context_mod_required:
      HANDLE_CODE(c.get<bearer_context_mod_required_s>().unpack(bref));
      break;
    case types::bearer_context_release_cmd:
      HANDLE_CODE(c.get<bearer_context_release_cmd_s>().unpack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().unpack(bref));
      break;
    case types::iab_up_tnl_address_upd:
      HANDLE_CODE(c.get<iab_up_tnl_address_upd_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::bearer_context_release_request:
      HANDLE_CODE(c.get<bearer_context_release_request_s>().unpack(bref));
      break;
    case types::bearer_context_inactivity_notif:
      HANDLE_CODE(c.get<bearer_context_inactivity_notif_s>().unpack(bref));
      break;
    case types::dl_data_notif:
      HANDLE_CODE(c.get<dl_data_notif_s>().unpack(bref));
      break;
    case types::ul_data_notif:
      HANDLE_CODE(c.get<ul_data_notif_s>().unpack(bref));
      break;
    case types::data_usage_report:
      HANDLE_CODE(c.get<data_usage_report_s>().unpack(bref));
      break;
    case types::gnb_cu_up_counter_check_request:
      HANDLE_CODE(c.get<gnb_cu_up_counter_check_request_s>().unpack(bref));
      break;
    case types::gnb_cu_up_status_ind:
      HANDLE_CODE(c.get<gnb_cu_up_status_ind_s>().unpack(bref));
      break;
    case types::mrdc_data_usage_report:
      HANDLE_CODE(c.get<mrdc_data_usage_report_s>().unpack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().unpack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().unpack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().unpack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().unpack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().unpack(bref));
      break;
    case types::early_forwarding_sn_transfer:
      HANDLE_CODE(c.get<early_forwarding_sn_transfer_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_meas_results_info:
      HANDLE_CODE(c.get<gnb_cu_cp_meas_results_info_s>().unpack(bref));
      break;
    case types::iabpsk_notif:
      HANDLE_CODE(c.get<iabpsk_notif_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1ap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"Reset",
                                "GNB-CU-UP-E1SetupRequest",
                                "GNB-CU-CP-E1SetupRequest",
                                "GNB-CU-UP-ConfigurationUpdate",
                                "GNB-CU-CP-ConfigurationUpdate",
                                "E1ReleaseRequest",
                                "BearerContextSetupRequest",
                                "BearerContextModificationRequest",
                                "BearerContextModificationRequired",
                                "BearerContextReleaseCommand",
                                "ResourceStatusRequest",
                                "IAB-UPTNLAddressUpdate",
                                "ErrorIndication",
                                "BearerContextReleaseRequest",
                                "BearerContextInactivityNotification",
                                "DLDataNotification",
                                "ULDataNotification",
                                "DataUsageReport",
                                "GNB-CU-UP-CounterCheckRequest",
                                "GNB-CU-UP-StatusIndication",
                                "MRDC-DataUsageReport",
                                "DeactivateTrace",
                                "TraceStart",
                                "PrivateMessage",
                                "CellTrafficTrace",
                                "ResourceStatusUpdate",
                                "EarlyForwardingSNTransfer",
                                "GNB-CU-CPMeasurementResultsInformation",
                                "IABPSKNotification"};
  return convert_enum_idx(names, 29, value, "e1ap_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void e1ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::reset_ack:
      c = reset_ack_s{};
      break;
    case types::gnb_cu_up_e1_setup_resp:
      c = gnb_cu_up_e1_setup_resp_s{};
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      c = gnb_cu_cp_e1_setup_resp_s{};
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      c = gnb_cu_up_cfg_upd_ack_s{};
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      c = gnb_cu_cp_cfg_upd_ack_s{};
      break;
    case types::e1_release_resp:
      c = e1_release_resp_s{};
      break;
    case types::bearer_context_setup_resp:
      c = bearer_context_setup_resp_s{};
      break;
    case types::bearer_context_mod_resp:
      c = bearer_context_mod_resp_s{};
      break;
    case types::bearer_context_mod_confirm:
      c = bearer_context_mod_confirm_s{};
      break;
    case types::bearer_context_release_complete:
      c = bearer_context_release_complete_s{};
      break;
    case types::res_status_resp:
      c = res_status_resp_s{};
      break;
    case types::iab_up_tnl_address_upd_ack:
      c = iab_up_tnl_address_upd_ack_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::successful_outcome_c");
  }
}
reset_ack_s& e1ap_elem_procs_o::successful_outcome_c::reset_ack()
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
gnb_cu_up_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_e1_setup_resp()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_resp_s>();
}
gnb_cu_cp_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_e1_setup_resp()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_resp_s>();
}
gnb_cu_up_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_ack_s>();
}
gnb_cu_cp_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_ack_s>();
}
e1_release_resp_s& e1ap_elem_procs_o::successful_outcome_c::e1_release_resp()
{
  assert_choice_type(types::e1_release_resp, type_, "SuccessfulOutcome");
  return c.get<e1_release_resp_s>();
}
bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_setup_resp()
{
  assert_choice_type(types::bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_setup_resp_s>();
}
bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_resp()
{
  assert_choice_type(types::bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_resp_s>();
}
bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_confirm()
{
  assert_choice_type(types::bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_confirm_s>();
}
bearer_context_release_complete_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_release_complete()
{
  assert_choice_type(types::bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bearer_context_release_complete_s>();
}
res_status_resp_s& e1ap_elem_procs_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
iab_up_tnl_address_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::iab_up_tnl_address_upd_ack()
{
  assert_choice_type(types::iab_up_tnl_address_upd_ack, type_, "SuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_ack_s>();
}
const reset_ack_s& e1ap_elem_procs_o::successful_outcome_c::reset_ack() const
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
const gnb_cu_up_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_e1_setup_resp() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_resp_s>();
}
const gnb_cu_cp_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_e1_setup_resp() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_resp_s>();
}
const gnb_cu_up_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_ack_s>();
}
const gnb_cu_cp_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_ack_s>();
}
const e1_release_resp_s& e1ap_elem_procs_o::successful_outcome_c::e1_release_resp() const
{
  assert_choice_type(types::e1_release_resp, type_, "SuccessfulOutcome");
  return c.get<e1_release_resp_s>();
}
const bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_setup_resp() const
{
  assert_choice_type(types::bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_setup_resp_s>();
}
const bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_resp() const
{
  assert_choice_type(types::bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_resp_s>();
}
const bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_confirm() const
{
  assert_choice_type(types::bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_confirm_s>();
}
const bearer_context_release_complete_s&
e1ap_elem_procs_o::successful_outcome_c::bearer_context_release_complete() const
{
  assert_choice_type(types::bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bearer_context_release_complete_s>();
}
const res_status_resp_s& e1ap_elem_procs_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const iab_up_tnl_address_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::iab_up_tnl_address_upd_ack() const
{
  assert_choice_type(types::iab_up_tnl_address_upd_ack, type_, "SuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_ack_s>();
}
void e1ap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::reset_ack:
      j.write_fieldname("ResetAcknowledge");
      c.get<reset_ack_s>().to_json(j);
      break;
    case types::gnb_cu_up_e1_setup_resp:
      j.write_fieldname("GNB-CU-UP-E1SetupResponse");
      c.get<gnb_cu_up_e1_setup_resp_s>().to_json(j);
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      j.write_fieldname("GNB-CU-CP-E1SetupResponse");
      c.get<gnb_cu_cp_e1_setup_resp_s>().to_json(j);
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      j.write_fieldname("GNB-CU-UP-ConfigurationUpdateAcknowledge");
      c.get<gnb_cu_up_cfg_upd_ack_s>().to_json(j);
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      j.write_fieldname("GNB-CU-CP-ConfigurationUpdateAcknowledge");
      c.get<gnb_cu_cp_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e1_release_resp:
      j.write_fieldname("E1ReleaseResponse");
      c.get<e1_release_resp_s>().to_json(j);
      break;
    case types::bearer_context_setup_resp:
      j.write_fieldname("BearerContextSetupResponse");
      c.get<bearer_context_setup_resp_s>().to_json(j);
      break;
    case types::bearer_context_mod_resp:
      j.write_fieldname("BearerContextModificationResponse");
      c.get<bearer_context_mod_resp_s>().to_json(j);
      break;
    case types::bearer_context_mod_confirm:
      j.write_fieldname("BearerContextModificationConfirm");
      c.get<bearer_context_mod_confirm_s>().to_json(j);
      break;
    case types::bearer_context_release_complete:
      j.write_fieldname("BearerContextReleaseComplete");
      c.get<bearer_context_release_complete_s>().to_json(j);
      break;
    case types::res_status_resp:
      j.write_fieldname("ResourceStatusResponse");
      c.get<res_status_resp_s>().to_json(j);
      break;
    case types::iab_up_tnl_address_upd_ack:
      j.write_fieldname("IAB-UPTNLAddressUpdateAcknowledge");
      c.get<iab_up_tnl_address_upd_ack_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e1ap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset_ack:
      HANDLE_CODE(c.get<reset_ack_s>().pack(bref));
      break;
    case types::gnb_cu_up_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_resp_s>().pack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_resp_s>().pack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_ack_s>().pack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e1_release_resp:
      HANDLE_CODE(c.get<e1_release_resp_s>().pack(bref));
      break;
    case types::bearer_context_setup_resp:
      HANDLE_CODE(c.get<bearer_context_setup_resp_s>().pack(bref));
      break;
    case types::bearer_context_mod_resp:
      HANDLE_CODE(c.get<bearer_context_mod_resp_s>().pack(bref));
      break;
    case types::bearer_context_mod_confirm:
      HANDLE_CODE(c.get<bearer_context_mod_confirm_s>().pack(bref));
      break;
    case types::bearer_context_release_complete:
      HANDLE_CODE(c.get<bearer_context_release_complete_s>().pack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().pack(bref));
      break;
    case types::iab_up_tnl_address_upd_ack:
      HANDLE_CODE(c.get<iab_up_tnl_address_upd_ack_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1ap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset_ack:
      HANDLE_CODE(c.get<reset_ack_s>().unpack(bref));
      break;
    case types::gnb_cu_up_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_resp_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_resp:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_resp_s>().unpack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e1_release_resp:
      HANDLE_CODE(c.get<e1_release_resp_s>().unpack(bref));
      break;
    case types::bearer_context_setup_resp:
      HANDLE_CODE(c.get<bearer_context_setup_resp_s>().unpack(bref));
      break;
    case types::bearer_context_mod_resp:
      HANDLE_CODE(c.get<bearer_context_mod_resp_s>().unpack(bref));
      break;
    case types::bearer_context_mod_confirm:
      HANDLE_CODE(c.get<bearer_context_mod_confirm_s>().unpack(bref));
      break;
    case types::bearer_context_release_complete:
      HANDLE_CODE(c.get<bearer_context_release_complete_s>().unpack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().unpack(bref));
      break;
    case types::iab_up_tnl_address_upd_ack:
      HANDLE_CODE(c.get<iab_up_tnl_address_upd_ack_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1ap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"ResetAcknowledge",
                                "GNB-CU-UP-E1SetupResponse",
                                "GNB-CU-CP-E1SetupResponse",
                                "GNB-CU-UP-ConfigurationUpdateAcknowledge",
                                "GNB-CU-CP-ConfigurationUpdateAcknowledge",
                                "E1ReleaseResponse",
                                "BearerContextSetupResponse",
                                "BearerContextModificationResponse",
                                "BearerContextModificationConfirm",
                                "BearerContextReleaseComplete",
                                "ResourceStatusResponse",
                                "IAB-UPTNLAddressUpdateAcknowledge"};
  return convert_enum_idx(names, 12, value, "e1ap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void e1ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      c = gnb_cu_up_e1_setup_fail_s{};
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      c = gnb_cu_cp_e1_setup_fail_s{};
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      c = gnb_cu_up_cfg_upd_fail_s{};
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      c = gnb_cu_cp_cfg_upd_fail_s{};
      break;
    case types::bearer_context_setup_fail:
      c = bearer_context_setup_fail_s{};
      break;
    case types::bearer_context_mod_fail:
      c = bearer_context_mod_fail_s{};
      break;
    case types::res_status_fail:
      c = res_status_fail_s{};
      break;
    case types::iab_up_tnl_address_upd_fail:
      c = iab_up_tnl_address_upd_fail_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
gnb_cu_up_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_e1_setup_fail()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_fail_s>();
}
gnb_cu_cp_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_e1_setup_fail()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_fail_s>();
}
gnb_cu_up_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_fail_s>();
}
gnb_cu_cp_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_fail_s>();
}
bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_setup_fail()
{
  assert_choice_type(types::bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_setup_fail_s>();
}
bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_mod_fail()
{
  assert_choice_type(types::bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_mod_fail_s>();
}
res_status_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
iab_up_tnl_address_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_tnl_address_upd_fail()
{
  assert_choice_type(types::iab_up_tnl_address_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_fail_s>();
}
const gnb_cu_up_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_e1_setup_fail() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_fail_s>();
}
const gnb_cu_cp_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_e1_setup_fail() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_fail_s>();
}
const gnb_cu_up_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_fail_s>();
}
const gnb_cu_cp_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_fail_s>();
}
const bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_setup_fail() const
{
  assert_choice_type(types::bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_setup_fail_s>();
}
const bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_mod_fail() const
{
  assert_choice_type(types::bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_mod_fail_s>();
}
const res_status_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const iab_up_tnl_address_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_tnl_address_upd_fail() const
{
  assert_choice_type(types::iab_up_tnl_address_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_fail_s>();
}
void e1ap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      j.write_fieldname("GNB-CU-UP-E1SetupFailure");
      c.get<gnb_cu_up_e1_setup_fail_s>().to_json(j);
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      j.write_fieldname("GNB-CU-CP-E1SetupFailure");
      c.get<gnb_cu_cp_e1_setup_fail_s>().to_json(j);
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      j.write_fieldname("GNB-CU-UP-ConfigurationUpdateFailure");
      c.get<gnb_cu_up_cfg_upd_fail_s>().to_json(j);
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      j.write_fieldname("GNB-CU-CP-ConfigurationUpdateFailure");
      c.get<gnb_cu_cp_cfg_upd_fail_s>().to_json(j);
      break;
    case types::bearer_context_setup_fail:
      j.write_fieldname("BearerContextSetupFailure");
      c.get<bearer_context_setup_fail_s>().to_json(j);
      break;
    case types::bearer_context_mod_fail:
      j.write_fieldname("BearerContextModificationFailure");
      c.get<bearer_context_mod_fail_s>().to_json(j);
      break;
    case types::res_status_fail:
      j.write_fieldname("ResourceStatusFailure");
      c.get<res_status_fail_s>().to_json(j);
      break;
    case types::iab_up_tnl_address_upd_fail:
      j.write_fieldname("IAB-UPTNLAddressUpdateFailure");
      c.get<iab_up_tnl_address_upd_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e1ap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_fail_s>().pack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_fail_s>().pack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_fail_s>().pack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_fail_s>().pack(bref));
      break;
    case types::bearer_context_setup_fail:
      HANDLE_CODE(c.get<bearer_context_setup_fail_s>().pack(bref));
      break;
    case types::bearer_context_mod_fail:
      HANDLE_CODE(c.get<bearer_context_mod_fail_s>().pack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().pack(bref));
      break;
    case types::iab_up_tnl_address_upd_fail:
      HANDLE_CODE(c.get<iab_up_tnl_address_upd_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1ap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::gnb_cu_up_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_up_e1_setup_fail_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_e1_setup_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_e1_setup_fail_s>().unpack(bref));
      break;
    case types::gnb_cu_up_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_up_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::gnb_cu_cp_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_cp_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::bearer_context_setup_fail:
      HANDLE_CODE(c.get<bearer_context_setup_fail_s>().unpack(bref));
      break;
    case types::bearer_context_mod_fail:
      HANDLE_CODE(c.get<bearer_context_mod_fail_s>().unpack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().unpack(bref));
      break;
    case types::iab_up_tnl_address_upd_fail:
      HANDLE_CODE(c.get<iab_up_tnl_address_upd_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-CU-UP-E1SetupFailure",
                                "GNB-CU-CP-E1SetupFailure",
                                "GNB-CU-UP-ConfigurationUpdateFailure",
                                "GNB-CU-CP-ConfigurationUpdateFailure",
                                "BearerContextSetupFailure",
                                "BearerContextModificationFailure",
                                "ResourceStatusFailure",
                                "IAB-UPTNLAddressUpdateFailure"};
  return convert_enum_idx(names, 8, value, "e1ap_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e1ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e1ap_elem_procs_o::get_init_msg(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void init_msg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e1ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e1ap_elem_procs_o::get_crit(proc_code);
  value     = e1ap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != e1ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e1ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e1ap_elem_procs_o::get_successful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void successful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e1ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e1ap_elem_procs_o::get_crit(proc_code);
  value     = e1ap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != e1ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e1ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e1ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void unsuccessful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e1ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e1ap_elem_procs_o::get_crit(proc_code);
  value     = e1ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// E1AP-PDU ::= CHOICE
void e1ap_pdu_c::destroy_()
{
  switch (type_) {
    case types::init_msg:
      c.destroy<init_msg_s>();
      break;
    case types::successful_outcome:
      c.destroy<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.destroy<unsuccessful_outcome_s>();
      break;
    default:
      break;
  }
}
void e1ap_pdu_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::init_msg:
      c.init<init_msg_s>();
      break;
    case types::successful_outcome:
      c.init<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.init<unsuccessful_outcome_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_pdu_c");
  }
}
e1ap_pdu_c::e1ap_pdu_c(const e1ap_pdu_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::init_msg:
      c.init(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.init(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.init(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_pdu_c");
  }
}
e1ap_pdu_c& e1ap_pdu_c::operator=(const e1ap_pdu_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::init_msg:
      c.set(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.set(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.set(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_pdu_c");
  }

  return *this;
}
init_msg_s& e1ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& e1ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& e1ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void e1ap_pdu_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::init_msg:
      j.write_fieldname("initiatingMessage");
      c.get<init_msg_s>().to_json(j);
      break;
    case types::successful_outcome:
      j.write_fieldname("successfulOutcome");
      c.get<successful_outcome_s>().to_json(j);
      break;
    case types::unsuccessful_outcome:
      j.write_fieldname("unsuccessfulOutcome");
      c.get<unsuccessful_outcome_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_pdu_c");
  }
  j.end_obj();
}
SRSASN_CODE e1ap_pdu_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().pack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().pack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_pdu_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e1ap_pdu_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().unpack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().unpack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e1ap_pdu_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e1ap_pdu_c::types_opts::to_string() const
{
  static const char* names[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(names, 3, value, "e1ap_pdu_c::types");
}
