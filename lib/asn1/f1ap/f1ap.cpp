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

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
using namespace asn1;
using namespace asn1::f1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// F1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF F1AP-ELEMENTARY-PROCEDURE
uint16_t f1ap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {0,  1,  3,  4,  5,  6,  7,  8,  20, 21, 16, 26, 32, 33, 34, 35, 36, 41, 48, 49,
                                   50, 52, 59, 60, 62, 64, 65, 67, 68, 69, 70, 75, 76, 2,  10, 12, 13, 15, 14, 11,
                                   17, 18, 19, 22, 23, 24, 25, 27, 28, 29, 30, 31, 37, 38, 58, 57, 39, 40, 42, 43,
                                   44, 45, 46, 47, 51, 53, 54, 55, 56, 63, 61, 66, 71, 79, 80, 77, 78, 81};
  return map_enum_number(names, 78, idx, "proc_code");
}
bool f1ap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {0,  1,  3,  4,  5,  6,  7,  8,  20, 21, 16, 26, 32, 33, 34, 35, 36, 41, 48, 49,
                                   50, 52, 59, 60, 62, 64, 65, 67, 68, 69, 70, 75, 76, 2,  10, 12, 13, 15, 14, 11,
                                   17, 18, 19, 22, 23, 24, 25, 27, 28, 29, 30, 31, 37, 38, 58, 57, 39, 40, 42, 43,
                                   44, 45, 46, 47, 51, 53, 54, 55, 56, 63, 61, 66, 71, 79, 80, 77, 78, 81};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
f1ap_elem_procs_o::init_msg_c f1ap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(init_msg_c::types::reset);
      break;
    case 1:
      ret.set(init_msg_c::types::f1_setup_request);
      break;
    case 3:
      ret.set(init_msg_c::types::gnb_du_cfg_upd);
      break;
    case 4:
      ret.set(init_msg_c::types::gnb_cu_cfg_upd);
      break;
    case 5:
      ret.set(init_msg_c::types::ue_context_setup_request);
      break;
    case 6:
      ret.set(init_msg_c::types::ue_context_release_cmd);
      break;
    case 7:
      ret.set(init_msg_c::types::ue_context_mod_request);
      break;
    case 8:
      ret.set(init_msg_c::types::ue_context_mod_required);
      break;
    case 20:
      ret.set(init_msg_c::types::write_replace_warning_request);
      break;
    case 21:
      ret.set(init_msg_c::types::pws_cancel_request);
      break;
    case 16:
      ret.set(init_msg_c::types::gnb_du_res_coordination_request);
      break;
    case 26:
      ret.set(init_msg_c::types::f1_removal_request);
      break;
    case 32:
      ret.set(init_msg_c::types::bap_map_cfg);
      break;
    case 33:
      ret.set(init_msg_c::types::gnb_du_res_cfg);
      break;
    case 34:
      ret.set(init_msg_c::types::iab_tnl_address_request);
      break;
    case 35:
      ret.set(init_msg_c::types::iab_up_cfg_upd_request);
      break;
    case 36:
      ret.set(init_msg_c::types::res_status_request);
      break;
    case 41:
      ret.set(init_msg_c::types::positioning_meas_request);
      break;
    case 48:
      ret.set(init_msg_c::types::trp_info_request);
      break;
    case 49:
      ret.set(init_msg_c::types::positioning_info_request);
      break;
    case 50:
      ret.set(init_msg_c::types::positioning_activation_request);
      break;
    case 52:
      ret.set(init_msg_c::types::e_c_id_meas_initiation_request);
      break;
    case 59:
      ret.set(init_msg_c::types::broadcast_context_setup_request);
      break;
    case 60:
      ret.set(init_msg_c::types::broadcast_context_release_cmd);
      break;
    case 62:
      ret.set(init_msg_c::types::broadcast_context_mod_request);
      break;
    case 64:
      ret.set(init_msg_c::types::multicast_context_setup_request);
      break;
    case 65:
      ret.set(init_msg_c::types::multicast_context_release_cmd);
      break;
    case 67:
      ret.set(init_msg_c::types::multicast_context_mod_request);
      break;
    case 68:
      ret.set(init_msg_c::types::multicast_distribution_setup_request);
      break;
    case 69:
      ret.set(init_msg_c::types::multicast_distribution_release_cmd);
      break;
    case 70:
      ret.set(init_msg_c::types::pdc_meas_initiation_request);
      break;
    case 75:
      ret.set(init_msg_c::types::prs_cfg_request);
      break;
    case 76:
      ret.set(init_msg_c::types::meas_precfg_required);
      break;
    case 2:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 10:
      ret.set(init_msg_c::types::ue_context_release_request);
      break;
    case 12:
      ret.set(init_msg_c::types::dl_rrc_msg_transfer);
      break;
    case 13:
      ret.set(init_msg_c::types::ul_rrc_msg_transfer);
      break;
    case 15:
      ret.set(init_msg_c::types::ue_inactivity_notif);
      break;
    case 14:
      ret.set(init_msg_c::types::private_msg);
      break;
    case 11:
      ret.set(init_msg_c::types::init_ul_rrc_msg_transfer);
      break;
    case 17:
      ret.set(init_msg_c::types::sys_info_delivery_cmd);
      break;
    case 18:
      ret.set(init_msg_c::types::paging);
      break;
    case 19:
      ret.set(init_msg_c::types::notify);
      break;
    case 22:
      ret.set(init_msg_c::types::pws_restart_ind);
      break;
    case 23:
      ret.set(init_msg_c::types::pws_fail_ind);
      break;
    case 24:
      ret.set(init_msg_c::types::gnb_du_status_ind);
      break;
    case 25:
      ret.set(init_msg_c::types::rrc_delivery_report);
      break;
    case 27:
      ret.set(init_msg_c::types::network_access_rate_reduction);
      break;
    case 28:
      ret.set(init_msg_c::types::trace_start);
      break;
    case 29:
      ret.set(init_msg_c::types::deactiv_trace);
      break;
    case 30:
      ret.set(init_msg_c::types::du_cu_radio_info_transfer);
      break;
    case 31:
      ret.set(init_msg_c::types::cu_du_radio_info_transfer);
      break;
    case 37:
      ret.set(init_msg_c::types::res_status_upd);
      break;
    case 38:
      ret.set(init_msg_c::types::access_and_mob_ind);
      break;
    case 58:
      ret.set(init_msg_c::types::ref_time_info_report_ctrl);
      break;
    case 57:
      ret.set(init_msg_c::types::ref_time_info_report);
      break;
    case 39:
      ret.set(init_msg_c::types::access_success);
      break;
    case 40:
      ret.set(init_msg_c::types::cell_traffic_trace);
      break;
    case 42:
      ret.set(init_msg_c::types::positioning_assist_info_ctrl);
      break;
    case 43:
      ret.set(init_msg_c::types::positioning_assist_info_feedback);
      break;
    case 44:
      ret.set(init_msg_c::types::positioning_meas_report);
      break;
    case 45:
      ret.set(init_msg_c::types::positioning_meas_abort);
      break;
    case 46:
      ret.set(init_msg_c::types::positioning_meas_fail_ind);
      break;
    case 47:
      ret.set(init_msg_c::types::positioning_meas_upd);
      break;
    case 51:
      ret.set(init_msg_c::types::positioning_deactivation);
      break;
    case 53:
      ret.set(init_msg_c::types::e_c_id_meas_fail_ind);
      break;
    case 54:
      ret.set(init_msg_c::types::e_c_id_meas_report);
      break;
    case 55:
      ret.set(init_msg_c::types::e_c_id_meas_termination_cmd);
      break;
    case 56:
      ret.set(init_msg_c::types::positioning_info_upd);
      break;
    case 63:
      ret.set(init_msg_c::types::multicast_group_paging);
      break;
    case 61:
      ret.set(init_msg_c::types::broadcast_context_release_request);
      break;
    case 66:
      ret.set(init_msg_c::types::multicast_context_release_request);
      break;
    case 71:
      ret.set(init_msg_c::types::pdc_meas_report);
      break;
    case 79:
      ret.set(init_msg_c::types::pdc_meas_termination_cmd);
      break;
    case 80:
      ret.set(init_msg_c::types::pdc_meas_fail_ind);
      break;
    case 77:
      ret.set(init_msg_c::types::meas_activation);
      break;
    case 78:
      ret.set(init_msg_c::types::qo_e_info_transfer);
      break;
    case 81:
      ret.set(init_msg_c::types::pos_sys_info_delivery_cmd);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
f1ap_elem_procs_o::successful_outcome_c f1ap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(successful_outcome_c::types::reset_ack);
      break;
    case 1:
      ret.set(successful_outcome_c::types::f1_setup_resp);
      break;
    case 3:
      ret.set(successful_outcome_c::types::gnb_du_cfg_upd_ack);
      break;
    case 4:
      ret.set(successful_outcome_c::types::gnb_cu_cfg_upd_ack);
      break;
    case 5:
      ret.set(successful_outcome_c::types::ue_context_setup_resp);
      break;
    case 6:
      ret.set(successful_outcome_c::types::ue_context_release_complete);
      break;
    case 7:
      ret.set(successful_outcome_c::types::ue_context_mod_resp);
      break;
    case 8:
      ret.set(successful_outcome_c::types::ue_context_mod_confirm);
      break;
    case 20:
      ret.set(successful_outcome_c::types::write_replace_warning_resp);
      break;
    case 21:
      ret.set(successful_outcome_c::types::pws_cancel_resp);
      break;
    case 16:
      ret.set(successful_outcome_c::types::gnb_du_res_coordination_resp);
      break;
    case 26:
      ret.set(successful_outcome_c::types::f1_removal_resp);
      break;
    case 32:
      ret.set(successful_outcome_c::types::bap_map_cfg_ack);
      break;
    case 33:
      ret.set(successful_outcome_c::types::gnb_du_res_cfg_ack);
      break;
    case 34:
      ret.set(successful_outcome_c::types::iab_tnl_address_resp);
      break;
    case 35:
      ret.set(successful_outcome_c::types::iab_up_cfg_upd_resp);
      break;
    case 36:
      ret.set(successful_outcome_c::types::res_status_resp);
      break;
    case 41:
      ret.set(successful_outcome_c::types::positioning_meas_resp);
      break;
    case 48:
      ret.set(successful_outcome_c::types::trp_info_resp);
      break;
    case 49:
      ret.set(successful_outcome_c::types::positioning_info_resp);
      break;
    case 50:
      ret.set(successful_outcome_c::types::positioning_activation_resp);
      break;
    case 52:
      ret.set(successful_outcome_c::types::e_c_id_meas_initiation_resp);
      break;
    case 59:
      ret.set(successful_outcome_c::types::broadcast_context_setup_resp);
      break;
    case 60:
      ret.set(successful_outcome_c::types::broadcast_context_release_complete);
      break;
    case 62:
      ret.set(successful_outcome_c::types::broadcast_context_mod_resp);
      break;
    case 64:
      ret.set(successful_outcome_c::types::multicast_context_setup_resp);
      break;
    case 65:
      ret.set(successful_outcome_c::types::multicast_context_release_complete);
      break;
    case 67:
      ret.set(successful_outcome_c::types::multicast_context_mod_resp);
      break;
    case 68:
      ret.set(successful_outcome_c::types::multicast_distribution_setup_resp);
      break;
    case 69:
      ret.set(successful_outcome_c::types::multicast_distribution_release_complete);
      break;
    case 70:
      ret.set(successful_outcome_c::types::pdc_meas_initiation_resp);
      break;
    case 75:
      ret.set(successful_outcome_c::types::prs_cfg_resp);
      break;
    case 76:
      ret.set(successful_outcome_c::types::meas_precfg_confirm);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
f1ap_elem_procs_o::unsuccessful_outcome_c f1ap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 1:
      ret.set(unsuccessful_outcome_c::types::f1_setup_fail);
      break;
    case 3:
      ret.set(unsuccessful_outcome_c::types::gnb_du_cfg_upd_fail);
      break;
    case 4:
      ret.set(unsuccessful_outcome_c::types::gnb_cu_cfg_upd_fail);
      break;
    case 5:
      ret.set(unsuccessful_outcome_c::types::ue_context_setup_fail);
      break;
    case 7:
      ret.set(unsuccessful_outcome_c::types::ue_context_mod_fail);
      break;
    case 8:
      ret.set(unsuccessful_outcome_c::types::ue_context_mod_refuse);
      break;
    case 26:
      ret.set(unsuccessful_outcome_c::types::f1_removal_fail);
      break;
    case 32:
      ret.set(unsuccessful_outcome_c::types::bap_map_cfg_fail);
      break;
    case 33:
      ret.set(unsuccessful_outcome_c::types::gnb_du_res_cfg_fail);
      break;
    case 34:
      ret.set(unsuccessful_outcome_c::types::iab_tnl_address_fail);
      break;
    case 35:
      ret.set(unsuccessful_outcome_c::types::iab_up_cfg_upd_fail);
      break;
    case 36:
      ret.set(unsuccessful_outcome_c::types::res_status_fail);
      break;
    case 41:
      ret.set(unsuccessful_outcome_c::types::positioning_meas_fail);
      break;
    case 48:
      ret.set(unsuccessful_outcome_c::types::trp_info_fail);
      break;
    case 49:
      ret.set(unsuccessful_outcome_c::types::positioning_info_fail);
      break;
    case 50:
      ret.set(unsuccessful_outcome_c::types::positioning_activation_fail);
      break;
    case 52:
      ret.set(unsuccessful_outcome_c::types::e_c_id_meas_initiation_fail);
      break;
    case 59:
      ret.set(unsuccessful_outcome_c::types::broadcast_context_setup_fail);
      break;
    case 62:
      ret.set(unsuccessful_outcome_c::types::broadcast_context_mod_fail);
      break;
    case 64:
      ret.set(unsuccessful_outcome_c::types::multicast_context_setup_fail);
      break;
    case 67:
      ret.set(unsuccessful_outcome_c::types::multicast_context_mod_fail);
      break;
    case 68:
      ret.set(unsuccessful_outcome_c::types::multicast_distribution_setup_fail);
      break;
    case 70:
      ret.set(unsuccessful_outcome_c::types::pdc_meas_initiation_fail);
      break;
    case 75:
      ret.set(unsuccessful_outcome_c::types::prs_cfg_fail);
      break;
    case 76:
      ret.set(unsuccessful_outcome_c::types::meas_precfg_refuse);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e f1ap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 0:
      return crit_e::reject;
    case 1:
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
    case 20:
      return crit_e::reject;
    case 21:
      return crit_e::reject;
    case 16:
      return crit_e::reject;
    case 26:
      return crit_e::reject;
    case 32:
      return crit_e::reject;
    case 33:
      return crit_e::reject;
    case 34:
      return crit_e::reject;
    case 35:
      return crit_e::reject;
    case 36:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 48:
      return crit_e::reject;
    case 49:
      return crit_e::reject;
    case 50:
      return crit_e::reject;
    case 52:
      return crit_e::reject;
    case 59:
      return crit_e::reject;
    case 60:
      return crit_e::reject;
    case 62:
      return crit_e::reject;
    case 64:
      return crit_e::reject;
    case 65:
      return crit_e::reject;
    case 67:
      return crit_e::reject;
    case 68:
      return crit_e::reject;
    case 69:
      return crit_e::reject;
    case 70:
      return crit_e::reject;
    case 75:
      return crit_e::reject;
    case 76:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    case 10:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    case 13:
      return crit_e::ignore;
    case 15:
      return crit_e::ignore;
    case 14:
      return crit_e::ignore;
    case 11:
      return crit_e::ignore;
    case 17:
      return crit_e::ignore;
    case 18:
      return crit_e::ignore;
    case 19:
      return crit_e::ignore;
    case 22:
      return crit_e::ignore;
    case 23:
      return crit_e::ignore;
    case 24:
      return crit_e::ignore;
    case 25:
      return crit_e::ignore;
    case 27:
      return crit_e::ignore;
    case 28:
      return crit_e::ignore;
    case 29:
      return crit_e::ignore;
    case 30:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 37:
      return crit_e::ignore;
    case 38:
      return crit_e::ignore;
    case 58:
      return crit_e::ignore;
    case 57:
      return crit_e::ignore;
    case 39:
      return crit_e::ignore;
    case 40:
      return crit_e::ignore;
    case 42:
      return crit_e::ignore;
    case 43:
      return crit_e::ignore;
    case 44:
      return crit_e::ignore;
    case 45:
      return crit_e::ignore;
    case 46:
      return crit_e::ignore;
    case 47:
      return crit_e::ignore;
    case 51:
      return crit_e::ignore;
    case 53:
      return crit_e::ignore;
    case 54:
      return crit_e::ignore;
    case 55:
      return crit_e::ignore;
    case 56:
      return crit_e::ignore;
    case 63:
      return crit_e::ignore;
    case 61:
      return crit_e::reject;
    case 66:
      return crit_e::reject;
    case 71:
      return crit_e::ignore;
    case 79:
      return crit_e::ignore;
    case 80:
      return crit_e::ignore;
    case 77:
      return crit_e::ignore;
    case 78:
      return crit_e::ignore;
    case 81:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void f1ap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::reset:
      c = reset_s{};
      break;
    case types::f1_setup_request:
      c = f1_setup_request_s{};
      break;
    case types::gnb_du_cfg_upd:
      c = gnb_du_cfg_upd_s{};
      break;
    case types::gnb_cu_cfg_upd:
      c = gnb_cu_cfg_upd_s{};
      break;
    case types::ue_context_setup_request:
      c = ue_context_setup_request_s{};
      break;
    case types::ue_context_release_cmd:
      c = ue_context_release_cmd_s{};
      break;
    case types::ue_context_mod_request:
      c = ue_context_mod_request_s{};
      break;
    case types::ue_context_mod_required:
      c = ue_context_mod_required_s{};
      break;
    case types::write_replace_warning_request:
      c = write_replace_warning_request_s{};
      break;
    case types::pws_cancel_request:
      c = pws_cancel_request_s{};
      break;
    case types::gnb_du_res_coordination_request:
      c = gnb_du_res_coordination_request_s{};
      break;
    case types::f1_removal_request:
      c = f1_removal_request_s{};
      break;
    case types::bap_map_cfg:
      c = bap_map_cfg_s{};
      break;
    case types::gnb_du_res_cfg:
      c = gnb_du_res_cfg_s{};
      break;
    case types::iab_tnl_address_request:
      c = iab_tnl_address_request_s{};
      break;
    case types::iab_up_cfg_upd_request:
      c = iab_up_cfg_upd_request_s{};
      break;
    case types::res_status_request:
      c = res_status_request_s{};
      break;
    case types::positioning_meas_request:
      c = positioning_meas_request_s{};
      break;
    case types::trp_info_request:
      c = trp_info_request_s{};
      break;
    case types::positioning_info_request:
      c = positioning_info_request_s{};
      break;
    case types::positioning_activation_request:
      c = positioning_activation_request_s{};
      break;
    case types::e_c_id_meas_initiation_request:
      c = e_c_id_meas_initiation_request_s{};
      break;
    case types::broadcast_context_setup_request:
      c = broadcast_context_setup_request_s{};
      break;
    case types::broadcast_context_release_cmd:
      c = broadcast_context_release_cmd_s{};
      break;
    case types::broadcast_context_mod_request:
      c = broadcast_context_mod_request_s{};
      break;
    case types::multicast_context_setup_request:
      c = multicast_context_setup_request_s{};
      break;
    case types::multicast_context_release_cmd:
      c = multicast_context_release_cmd_s{};
      break;
    case types::multicast_context_mod_request:
      c = multicast_context_mod_request_s{};
      break;
    case types::multicast_distribution_setup_request:
      c = multicast_distribution_setup_request_s{};
      break;
    case types::multicast_distribution_release_cmd:
      c = multicast_distribution_release_cmd_s{};
      break;
    case types::pdc_meas_initiation_request:
      c = pdc_meas_initiation_request_s{};
      break;
    case types::prs_cfg_request:
      c = prs_cfg_request_s{};
      break;
    case types::meas_precfg_required:
      c = meas_precfg_required_s{};
      break;
    case types::error_ind:
      c = error_ind_s{};
      break;
    case types::ue_context_release_request:
      c = ue_context_release_request_s{};
      break;
    case types::dl_rrc_msg_transfer:
      c = dl_rrc_msg_transfer_s{};
      break;
    case types::ul_rrc_msg_transfer:
      c = ul_rrc_msg_transfer_s{};
      break;
    case types::ue_inactivity_notif:
      c = ue_inactivity_notif_s{};
      break;
    case types::private_msg:
      c = private_msg_s{};
      break;
    case types::init_ul_rrc_msg_transfer:
      c = init_ul_rrc_msg_transfer_s{};
      break;
    case types::sys_info_delivery_cmd:
      c = sys_info_delivery_cmd_s{};
      break;
    case types::paging:
      c = paging_s{};
      break;
    case types::notify:
      c = notify_s{};
      break;
    case types::pws_restart_ind:
      c = pws_restart_ind_s{};
      break;
    case types::pws_fail_ind:
      c = pws_fail_ind_s{};
      break;
    case types::gnb_du_status_ind:
      c = gnb_du_status_ind_s{};
      break;
    case types::rrc_delivery_report:
      c = rrc_delivery_report_s{};
      break;
    case types::network_access_rate_reduction:
      c = network_access_rate_reduction_s{};
      break;
    case types::trace_start:
      c = trace_start_s{};
      break;
    case types::deactiv_trace:
      c = deactiv_trace_s{};
      break;
    case types::du_cu_radio_info_transfer:
      c = du_cu_radio_info_transfer_s{};
      break;
    case types::cu_du_radio_info_transfer:
      c = cu_du_radio_info_transfer_s{};
      break;
    case types::res_status_upd:
      c = res_status_upd_s{};
      break;
    case types::access_and_mob_ind:
      c = access_and_mob_ind_s{};
      break;
    case types::ref_time_info_report_ctrl:
      c = ref_time_info_report_ctrl_s{};
      break;
    case types::ref_time_info_report:
      c = ref_time_info_report_s{};
      break;
    case types::access_success:
      c = access_success_s{};
      break;
    case types::cell_traffic_trace:
      c = cell_traffic_trace_s{};
      break;
    case types::positioning_assist_info_ctrl:
      c = positioning_assist_info_ctrl_s{};
      break;
    case types::positioning_assist_info_feedback:
      c = positioning_assist_info_feedback_s{};
      break;
    case types::positioning_meas_report:
      c = positioning_meas_report_s{};
      break;
    case types::positioning_meas_abort:
      c = positioning_meas_abort_s{};
      break;
    case types::positioning_meas_fail_ind:
      c = positioning_meas_fail_ind_s{};
      break;
    case types::positioning_meas_upd:
      c = positioning_meas_upd_s{};
      break;
    case types::positioning_deactivation:
      c = positioning_deactivation_s{};
      break;
    case types::e_c_id_meas_fail_ind:
      c = e_c_id_meas_fail_ind_s{};
      break;
    case types::e_c_id_meas_report:
      c = e_c_id_meas_report_s{};
      break;
    case types::e_c_id_meas_termination_cmd:
      c = e_c_id_meas_termination_cmd_s{};
      break;
    case types::positioning_info_upd:
      c = positioning_info_upd_s{};
      break;
    case types::multicast_group_paging:
      c = multicast_group_paging_s{};
      break;
    case types::broadcast_context_release_request:
      c = broadcast_context_release_request_s{};
      break;
    case types::multicast_context_release_request:
      c = multicast_context_release_request_s{};
      break;
    case types::pdc_meas_report:
      c = pdc_meas_report_s{};
      break;
    case types::pdc_meas_termination_cmd:
      c = pdc_meas_termination_cmd_s{};
      break;
    case types::pdc_meas_fail_ind:
      c = pdc_meas_fail_ind_s{};
      break;
    case types::meas_activation:
      c = meas_activation_s{};
      break;
    case types::qo_e_info_transfer:
      c = qo_e_info_transfer_s{};
      break;
    case types::pos_sys_info_delivery_cmd:
      c = pos_sys_info_delivery_cmd_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::init_msg_c");
  }
}
reset_s& f1ap_elem_procs_o::init_msg_c::reset()
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
f1_setup_request_s& f1ap_elem_procs_o::init_msg_c::f1_setup_request()
{
  assert_choice_type(types::f1_setup_request, type_, "InitiatingMessage");
  return c.get<f1_setup_request_s>();
}
gnb_du_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_du_cfg_upd()
{
  assert_choice_type(types::gnb_du_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_du_cfg_upd_s>();
}
gnb_cu_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_cu_cfg_upd()
{
  assert_choice_type(types::gnb_cu_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cfg_upd_s>();
}
ue_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_setup_request()
{
  assert_choice_type(types::ue_context_setup_request, type_, "InitiatingMessage");
  return c.get<ue_context_setup_request_s>();
}
ue_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_cmd()
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
ue_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_request()
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
ue_context_mod_required_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_required()
{
  assert_choice_type(types::ue_context_mod_required, type_, "InitiatingMessage");
  return c.get<ue_context_mod_required_s>();
}
write_replace_warning_request_s& f1ap_elem_procs_o::init_msg_c::write_replace_warning_request()
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
pws_cancel_request_s& f1ap_elem_procs_o::init_msg_c::pws_cancel_request()
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
gnb_du_res_coordination_request_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_coordination_request()
{
  assert_choice_type(types::gnb_du_res_coordination_request, type_, "InitiatingMessage");
  return c.get<gnb_du_res_coordination_request_s>();
}
f1_removal_request_s& f1ap_elem_procs_o::init_msg_c::f1_removal_request()
{
  assert_choice_type(types::f1_removal_request, type_, "InitiatingMessage");
  return c.get<f1_removal_request_s>();
}
bap_map_cfg_s& f1ap_elem_procs_o::init_msg_c::bap_map_cfg()
{
  assert_choice_type(types::bap_map_cfg, type_, "InitiatingMessage");
  return c.get<bap_map_cfg_s>();
}
gnb_du_res_cfg_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_cfg()
{
  assert_choice_type(types::gnb_du_res_cfg, type_, "InitiatingMessage");
  return c.get<gnb_du_res_cfg_s>();
}
iab_tnl_address_request_s& f1ap_elem_procs_o::init_msg_c::iab_tnl_address_request()
{
  assert_choice_type(types::iab_tnl_address_request, type_, "InitiatingMessage");
  return c.get<iab_tnl_address_request_s>();
}
iab_up_cfg_upd_request_s& f1ap_elem_procs_o::init_msg_c::iab_up_cfg_upd_request()
{
  assert_choice_type(types::iab_up_cfg_upd_request, type_, "InitiatingMessage");
  return c.get<iab_up_cfg_upd_request_s>();
}
res_status_request_s& f1ap_elem_procs_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
positioning_meas_request_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_request()
{
  assert_choice_type(types::positioning_meas_request, type_, "InitiatingMessage");
  return c.get<positioning_meas_request_s>();
}
trp_info_request_s& f1ap_elem_procs_o::init_msg_c::trp_info_request()
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
positioning_info_request_s& f1ap_elem_procs_o::init_msg_c::positioning_info_request()
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
positioning_activation_request_s& f1ap_elem_procs_o::init_msg_c::positioning_activation_request()
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
e_c_id_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request()
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
broadcast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_setup_request()
{
  assert_choice_type(types::broadcast_context_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_setup_request_s>();
}
broadcast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_cmd()
{
  assert_choice_type(types::broadcast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_cmd_s>();
}
broadcast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_mod_request()
{
  assert_choice_type(types::broadcast_context_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_mod_request_s>();
}
multicast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_setup_request()
{
  assert_choice_type(types::multicast_context_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_context_setup_request_s>();
}
multicast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_cmd()
{
  assert_choice_type(types::multicast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_context_release_cmd_s>();
}
multicast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_mod_request()
{
  assert_choice_type(types::multicast_context_mod_request, type_, "InitiatingMessage");
  return c.get<multicast_context_mod_request_s>();
}
multicast_distribution_setup_request_s& f1ap_elem_procs_o::init_msg_c::multicast_distribution_setup_request()
{
  assert_choice_type(types::multicast_distribution_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_distribution_setup_request_s>();
}
multicast_distribution_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_distribution_release_cmd()
{
  assert_choice_type(types::multicast_distribution_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_distribution_release_cmd_s>();
}
pdc_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_initiation_request()
{
  assert_choice_type(types::pdc_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<pdc_meas_initiation_request_s>();
}
prs_cfg_request_s& f1ap_elem_procs_o::init_msg_c::prs_cfg_request()
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
meas_precfg_required_s& f1ap_elem_procs_o::init_msg_c::meas_precfg_required()
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
error_ind_s& f1ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ue_context_release_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_request()
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
dl_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::dl_rrc_msg_transfer()
{
  assert_choice_type(types::dl_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<dl_rrc_msg_transfer_s>();
}
ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::ul_rrc_msg_transfer()
{
  assert_choice_type(types::ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<ul_rrc_msg_transfer_s>();
}
ue_inactivity_notif_s& f1ap_elem_procs_o::init_msg_c::ue_inactivity_notif()
{
  assert_choice_type(types::ue_inactivity_notif, type_, "InitiatingMessage");
  return c.get<ue_inactivity_notif_s>();
}
private_msg_s& f1ap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
init_ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::init_ul_rrc_msg_transfer()
{
  assert_choice_type(types::init_ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<init_ul_rrc_msg_transfer_s>();
}
sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::sys_info_delivery_cmd()
{
  assert_choice_type(types::sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<sys_info_delivery_cmd_s>();
}
paging_s& f1ap_elem_procs_o::init_msg_c::paging()
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
notify_s& f1ap_elem_procs_o::init_msg_c::notify()
{
  assert_choice_type(types::notify, type_, "InitiatingMessage");
  return c.get<notify_s>();
}
pws_restart_ind_s& f1ap_elem_procs_o::init_msg_c::pws_restart_ind()
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
pws_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pws_fail_ind()
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
gnb_du_status_ind_s& f1ap_elem_procs_o::init_msg_c::gnb_du_status_ind()
{
  assert_choice_type(types::gnb_du_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_du_status_ind_s>();
}
rrc_delivery_report_s& f1ap_elem_procs_o::init_msg_c::rrc_delivery_report()
{
  assert_choice_type(types::rrc_delivery_report, type_, "InitiatingMessage");
  return c.get<rrc_delivery_report_s>();
}
network_access_rate_reduction_s& f1ap_elem_procs_o::init_msg_c::network_access_rate_reduction()
{
  assert_choice_type(types::network_access_rate_reduction, type_, "InitiatingMessage");
  return c.get<network_access_rate_reduction_s>();
}
trace_start_s& f1ap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
deactiv_trace_s& f1ap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
du_cu_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::du_cu_radio_info_transfer()
{
  assert_choice_type(types::du_cu_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<du_cu_radio_info_transfer_s>();
}
cu_du_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::cu_du_radio_info_transfer()
{
  assert_choice_type(types::cu_du_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<cu_du_radio_info_transfer_s>();
}
res_status_upd_s& f1ap_elem_procs_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
access_and_mob_ind_s& f1ap_elem_procs_o::init_msg_c::access_and_mob_ind()
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
ref_time_info_report_ctrl_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report_ctrl()
{
  assert_choice_type(types::ref_time_info_report_ctrl, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_ctrl_s>();
}
ref_time_info_report_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report()
{
  assert_choice_type(types::ref_time_info_report, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_s>();
}
access_success_s& f1ap_elem_procs_o::init_msg_c::access_success()
{
  assert_choice_type(types::access_success, type_, "InitiatingMessage");
  return c.get<access_success_s>();
}
cell_traffic_trace_s& f1ap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
positioning_assist_info_ctrl_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_ctrl()
{
  assert_choice_type(types::positioning_assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_ctrl_s>();
}
positioning_assist_info_feedback_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_feedback()
{
  assert_choice_type(types::positioning_assist_info_feedback, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_feedback_s>();
}
positioning_meas_report_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_report()
{
  assert_choice_type(types::positioning_meas_report, type_, "InitiatingMessage");
  return c.get<positioning_meas_report_s>();
}
positioning_meas_abort_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_abort()
{
  assert_choice_type(types::positioning_meas_abort, type_, "InitiatingMessage");
  return c.get<positioning_meas_abort_s>();
}
positioning_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_fail_ind()
{
  assert_choice_type(types::positioning_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<positioning_meas_fail_ind_s>();
}
positioning_meas_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_upd()
{
  assert_choice_type(types::positioning_meas_upd, type_, "InitiatingMessage");
  return c.get<positioning_meas_upd_s>();
}
positioning_deactivation_s& f1ap_elem_procs_o::init_msg_c::positioning_deactivation()
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
e_c_id_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind()
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
e_c_id_meas_report_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_report()
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
e_c_id_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd()
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
positioning_info_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_info_upd()
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
multicast_group_paging_s& f1ap_elem_procs_o::init_msg_c::multicast_group_paging()
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
broadcast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_request()
{
  assert_choice_type(types::broadcast_context_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_request_s>();
}
multicast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_request()
{
  assert_choice_type(types::multicast_context_release_request, type_, "InitiatingMessage");
  return c.get<multicast_context_release_request_s>();
}
pdc_meas_report_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_report()
{
  assert_choice_type(types::pdc_meas_report, type_, "InitiatingMessage");
  return c.get<pdc_meas_report_s>();
}
pdc_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_termination_cmd()
{
  assert_choice_type(types::pdc_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<pdc_meas_termination_cmd_s>();
}
pdc_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_fail_ind()
{
  assert_choice_type(types::pdc_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<pdc_meas_fail_ind_s>();
}
meas_activation_s& f1ap_elem_procs_o::init_msg_c::meas_activation()
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
qo_e_info_transfer_s& f1ap_elem_procs_o::init_msg_c::qo_e_info_transfer()
{
  assert_choice_type(types::qo_e_info_transfer, type_, "InitiatingMessage");
  return c.get<qo_e_info_transfer_s>();
}
pos_sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::pos_sys_info_delivery_cmd()
{
  assert_choice_type(types::pos_sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<pos_sys_info_delivery_cmd_s>();
}
const reset_s& f1ap_elem_procs_o::init_msg_c::reset() const
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
const f1_setup_request_s& f1ap_elem_procs_o::init_msg_c::f1_setup_request() const
{
  assert_choice_type(types::f1_setup_request, type_, "InitiatingMessage");
  return c.get<f1_setup_request_s>();
}
const gnb_du_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_du_cfg_upd() const
{
  assert_choice_type(types::gnb_du_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_du_cfg_upd_s>();
}
const gnb_cu_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_cu_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cfg_upd_s>();
}
const ue_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_setup_request() const
{
  assert_choice_type(types::ue_context_setup_request, type_, "InitiatingMessage");
  return c.get<ue_context_setup_request_s>();
}
const ue_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_cmd() const
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
const ue_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_request() const
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
const ue_context_mod_required_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_required() const
{
  assert_choice_type(types::ue_context_mod_required, type_, "InitiatingMessage");
  return c.get<ue_context_mod_required_s>();
}
const write_replace_warning_request_s& f1ap_elem_procs_o::init_msg_c::write_replace_warning_request() const
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
const pws_cancel_request_s& f1ap_elem_procs_o::init_msg_c::pws_cancel_request() const
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
const gnb_du_res_coordination_request_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_coordination_request() const
{
  assert_choice_type(types::gnb_du_res_coordination_request, type_, "InitiatingMessage");
  return c.get<gnb_du_res_coordination_request_s>();
}
const f1_removal_request_s& f1ap_elem_procs_o::init_msg_c::f1_removal_request() const
{
  assert_choice_type(types::f1_removal_request, type_, "InitiatingMessage");
  return c.get<f1_removal_request_s>();
}
const bap_map_cfg_s& f1ap_elem_procs_o::init_msg_c::bap_map_cfg() const
{
  assert_choice_type(types::bap_map_cfg, type_, "InitiatingMessage");
  return c.get<bap_map_cfg_s>();
}
const gnb_du_res_cfg_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_cfg() const
{
  assert_choice_type(types::gnb_du_res_cfg, type_, "InitiatingMessage");
  return c.get<gnb_du_res_cfg_s>();
}
const iab_tnl_address_request_s& f1ap_elem_procs_o::init_msg_c::iab_tnl_address_request() const
{
  assert_choice_type(types::iab_tnl_address_request, type_, "InitiatingMessage");
  return c.get<iab_tnl_address_request_s>();
}
const iab_up_cfg_upd_request_s& f1ap_elem_procs_o::init_msg_c::iab_up_cfg_upd_request() const
{
  assert_choice_type(types::iab_up_cfg_upd_request, type_, "InitiatingMessage");
  return c.get<iab_up_cfg_upd_request_s>();
}
const res_status_request_s& f1ap_elem_procs_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const positioning_meas_request_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_request() const
{
  assert_choice_type(types::positioning_meas_request, type_, "InitiatingMessage");
  return c.get<positioning_meas_request_s>();
}
const trp_info_request_s& f1ap_elem_procs_o::init_msg_c::trp_info_request() const
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
const positioning_info_request_s& f1ap_elem_procs_o::init_msg_c::positioning_info_request() const
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
const positioning_activation_request_s& f1ap_elem_procs_o::init_msg_c::positioning_activation_request() const
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
const e_c_id_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request() const
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
const broadcast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_setup_request() const
{
  assert_choice_type(types::broadcast_context_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_setup_request_s>();
}
const broadcast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_cmd() const
{
  assert_choice_type(types::broadcast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_cmd_s>();
}
const broadcast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_mod_request() const
{
  assert_choice_type(types::broadcast_context_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_mod_request_s>();
}
const multicast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_setup_request() const
{
  assert_choice_type(types::multicast_context_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_context_setup_request_s>();
}
const multicast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_cmd() const
{
  assert_choice_type(types::multicast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_context_release_cmd_s>();
}
const multicast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_mod_request() const
{
  assert_choice_type(types::multicast_context_mod_request, type_, "InitiatingMessage");
  return c.get<multicast_context_mod_request_s>();
}
const multicast_distribution_setup_request_s&
f1ap_elem_procs_o::init_msg_c::multicast_distribution_setup_request() const
{
  assert_choice_type(types::multicast_distribution_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_distribution_setup_request_s>();
}
const multicast_distribution_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_distribution_release_cmd() const
{
  assert_choice_type(types::multicast_distribution_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_distribution_release_cmd_s>();
}
const pdc_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_initiation_request() const
{
  assert_choice_type(types::pdc_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<pdc_meas_initiation_request_s>();
}
const prs_cfg_request_s& f1ap_elem_procs_o::init_msg_c::prs_cfg_request() const
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
const meas_precfg_required_s& f1ap_elem_procs_o::init_msg_c::meas_precfg_required() const
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
const error_ind_s& f1ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ue_context_release_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_request() const
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
const dl_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::dl_rrc_msg_transfer() const
{
  assert_choice_type(types::dl_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<dl_rrc_msg_transfer_s>();
}
const ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::ul_rrc_msg_transfer() const
{
  assert_choice_type(types::ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<ul_rrc_msg_transfer_s>();
}
const ue_inactivity_notif_s& f1ap_elem_procs_o::init_msg_c::ue_inactivity_notif() const
{
  assert_choice_type(types::ue_inactivity_notif, type_, "InitiatingMessage");
  return c.get<ue_inactivity_notif_s>();
}
const private_msg_s& f1ap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const init_ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::init_ul_rrc_msg_transfer() const
{
  assert_choice_type(types::init_ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<init_ul_rrc_msg_transfer_s>();
}
const sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::sys_info_delivery_cmd() const
{
  assert_choice_type(types::sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<sys_info_delivery_cmd_s>();
}
const paging_s& f1ap_elem_procs_o::init_msg_c::paging() const
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
const notify_s& f1ap_elem_procs_o::init_msg_c::notify() const
{
  assert_choice_type(types::notify, type_, "InitiatingMessage");
  return c.get<notify_s>();
}
const pws_restart_ind_s& f1ap_elem_procs_o::init_msg_c::pws_restart_ind() const
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
const pws_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pws_fail_ind() const
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
const gnb_du_status_ind_s& f1ap_elem_procs_o::init_msg_c::gnb_du_status_ind() const
{
  assert_choice_type(types::gnb_du_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_du_status_ind_s>();
}
const rrc_delivery_report_s& f1ap_elem_procs_o::init_msg_c::rrc_delivery_report() const
{
  assert_choice_type(types::rrc_delivery_report, type_, "InitiatingMessage");
  return c.get<rrc_delivery_report_s>();
}
const network_access_rate_reduction_s& f1ap_elem_procs_o::init_msg_c::network_access_rate_reduction() const
{
  assert_choice_type(types::network_access_rate_reduction, type_, "InitiatingMessage");
  return c.get<network_access_rate_reduction_s>();
}
const trace_start_s& f1ap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const deactiv_trace_s& f1ap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const du_cu_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::du_cu_radio_info_transfer() const
{
  assert_choice_type(types::du_cu_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<du_cu_radio_info_transfer_s>();
}
const cu_du_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::cu_du_radio_info_transfer() const
{
  assert_choice_type(types::cu_du_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<cu_du_radio_info_transfer_s>();
}
const res_status_upd_s& f1ap_elem_procs_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const access_and_mob_ind_s& f1ap_elem_procs_o::init_msg_c::access_and_mob_ind() const
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
const ref_time_info_report_ctrl_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report_ctrl() const
{
  assert_choice_type(types::ref_time_info_report_ctrl, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_ctrl_s>();
}
const ref_time_info_report_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report() const
{
  assert_choice_type(types::ref_time_info_report, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_s>();
}
const access_success_s& f1ap_elem_procs_o::init_msg_c::access_success() const
{
  assert_choice_type(types::access_success, type_, "InitiatingMessage");
  return c.get<access_success_s>();
}
const cell_traffic_trace_s& f1ap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const positioning_assist_info_ctrl_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_ctrl() const
{
  assert_choice_type(types::positioning_assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_ctrl_s>();
}
const positioning_assist_info_feedback_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_feedback() const
{
  assert_choice_type(types::positioning_assist_info_feedback, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_feedback_s>();
}
const positioning_meas_report_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_report() const
{
  assert_choice_type(types::positioning_meas_report, type_, "InitiatingMessage");
  return c.get<positioning_meas_report_s>();
}
const positioning_meas_abort_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_abort() const
{
  assert_choice_type(types::positioning_meas_abort, type_, "InitiatingMessage");
  return c.get<positioning_meas_abort_s>();
}
const positioning_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_fail_ind() const
{
  assert_choice_type(types::positioning_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<positioning_meas_fail_ind_s>();
}
const positioning_meas_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_upd() const
{
  assert_choice_type(types::positioning_meas_upd, type_, "InitiatingMessage");
  return c.get<positioning_meas_upd_s>();
}
const positioning_deactivation_s& f1ap_elem_procs_o::init_msg_c::positioning_deactivation() const
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
const e_c_id_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind() const
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
const e_c_id_meas_report_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_report() const
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
const e_c_id_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd() const
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
const positioning_info_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_info_upd() const
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
const multicast_group_paging_s& f1ap_elem_procs_o::init_msg_c::multicast_group_paging() const
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
const broadcast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_request() const
{
  assert_choice_type(types::broadcast_context_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_request_s>();
}
const multicast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_request() const
{
  assert_choice_type(types::multicast_context_release_request, type_, "InitiatingMessage");
  return c.get<multicast_context_release_request_s>();
}
const pdc_meas_report_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_report() const
{
  assert_choice_type(types::pdc_meas_report, type_, "InitiatingMessage");
  return c.get<pdc_meas_report_s>();
}
const pdc_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_termination_cmd() const
{
  assert_choice_type(types::pdc_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<pdc_meas_termination_cmd_s>();
}
const pdc_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_fail_ind() const
{
  assert_choice_type(types::pdc_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<pdc_meas_fail_ind_s>();
}
const meas_activation_s& f1ap_elem_procs_o::init_msg_c::meas_activation() const
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
const qo_e_info_transfer_s& f1ap_elem_procs_o::init_msg_c::qo_e_info_transfer() const
{
  assert_choice_type(types::qo_e_info_transfer, type_, "InitiatingMessage");
  return c.get<qo_e_info_transfer_s>();
}
const pos_sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::pos_sys_info_delivery_cmd() const
{
  assert_choice_type(types::pos_sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<pos_sys_info_delivery_cmd_s>();
}
void f1ap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::reset:
      j.write_fieldname("Reset");
      c.get<reset_s>().to_json(j);
      break;
    case types::f1_setup_request:
      j.write_fieldname("F1SetupRequest");
      c.get<f1_setup_request_s>().to_json(j);
      break;
    case types::gnb_du_cfg_upd:
      j.write_fieldname("GNBDUConfigurationUpdate");
      c.get<gnb_du_cfg_upd_s>().to_json(j);
      break;
    case types::gnb_cu_cfg_upd:
      j.write_fieldname("GNBCUConfigurationUpdate");
      c.get<gnb_cu_cfg_upd_s>().to_json(j);
      break;
    case types::ue_context_setup_request:
      j.write_fieldname("UEContextSetupRequest");
      c.get<ue_context_setup_request_s>().to_json(j);
      break;
    case types::ue_context_release_cmd:
      j.write_fieldname("UEContextReleaseCommand");
      c.get<ue_context_release_cmd_s>().to_json(j);
      break;
    case types::ue_context_mod_request:
      j.write_fieldname("UEContextModificationRequest");
      c.get<ue_context_mod_request_s>().to_json(j);
      break;
    case types::ue_context_mod_required:
      j.write_fieldname("UEContextModificationRequired");
      c.get<ue_context_mod_required_s>().to_json(j);
      break;
    case types::write_replace_warning_request:
      j.write_fieldname("WriteReplaceWarningRequest");
      c.get<write_replace_warning_request_s>().to_json(j);
      break;
    case types::pws_cancel_request:
      j.write_fieldname("PWSCancelRequest");
      c.get<pws_cancel_request_s>().to_json(j);
      break;
    case types::gnb_du_res_coordination_request:
      j.write_fieldname("GNBDUResourceCoordinationRequest");
      c.get<gnb_du_res_coordination_request_s>().to_json(j);
      break;
    case types::f1_removal_request:
      j.write_fieldname("F1RemovalRequest");
      c.get<f1_removal_request_s>().to_json(j);
      break;
    case types::bap_map_cfg:
      j.write_fieldname("BAPMappingConfiguration");
      c.get<bap_map_cfg_s>().to_json(j);
      break;
    case types::gnb_du_res_cfg:
      j.write_fieldname("GNBDUResourceConfiguration");
      c.get<gnb_du_res_cfg_s>().to_json(j);
      break;
    case types::iab_tnl_address_request:
      j.write_fieldname("IABTNLAddressRequest");
      c.get<iab_tnl_address_request_s>().to_json(j);
      break;
    case types::iab_up_cfg_upd_request:
      j.write_fieldname("IABUPConfigurationUpdateRequest");
      c.get<iab_up_cfg_upd_request_s>().to_json(j);
      break;
    case types::res_status_request:
      j.write_fieldname("ResourceStatusRequest");
      c.get<res_status_request_s>().to_json(j);
      break;
    case types::positioning_meas_request:
      j.write_fieldname("PositioningMeasurementRequest");
      c.get<positioning_meas_request_s>().to_json(j);
      break;
    case types::trp_info_request:
      j.write_fieldname("TRPInformationRequest");
      c.get<trp_info_request_s>().to_json(j);
      break;
    case types::positioning_info_request:
      j.write_fieldname("PositioningInformationRequest");
      c.get<positioning_info_request_s>().to_json(j);
      break;
    case types::positioning_activation_request:
      j.write_fieldname("PositioningActivationRequest");
      c.get<positioning_activation_request_s>().to_json(j);
      break;
    case types::e_c_id_meas_initiation_request:
      j.write_fieldname("E-CIDMeasurementInitiationRequest");
      c.get<e_c_id_meas_initiation_request_s>().to_json(j);
      break;
    case types::broadcast_context_setup_request:
      j.write_fieldname("BroadcastContextSetupRequest");
      c.get<broadcast_context_setup_request_s>().to_json(j);
      break;
    case types::broadcast_context_release_cmd:
      j.write_fieldname("BroadcastContextReleaseCommand");
      c.get<broadcast_context_release_cmd_s>().to_json(j);
      break;
    case types::broadcast_context_mod_request:
      j.write_fieldname("BroadcastContextModificationRequest");
      c.get<broadcast_context_mod_request_s>().to_json(j);
      break;
    case types::multicast_context_setup_request:
      j.write_fieldname("MulticastContextSetupRequest");
      c.get<multicast_context_setup_request_s>().to_json(j);
      break;
    case types::multicast_context_release_cmd:
      j.write_fieldname("MulticastContextReleaseCommand");
      c.get<multicast_context_release_cmd_s>().to_json(j);
      break;
    case types::multicast_context_mod_request:
      j.write_fieldname("MulticastContextModificationRequest");
      c.get<multicast_context_mod_request_s>().to_json(j);
      break;
    case types::multicast_distribution_setup_request:
      j.write_fieldname("MulticastDistributionSetupRequest");
      c.get<multicast_distribution_setup_request_s>().to_json(j);
      break;
    case types::multicast_distribution_release_cmd:
      j.write_fieldname("MulticastDistributionReleaseCommand");
      c.get<multicast_distribution_release_cmd_s>().to_json(j);
      break;
    case types::pdc_meas_initiation_request:
      j.write_fieldname("PDCMeasurementInitiationRequest");
      c.get<pdc_meas_initiation_request_s>().to_json(j);
      break;
    case types::prs_cfg_request:
      j.write_fieldname("PRSConfigurationRequest");
      c.get<prs_cfg_request_s>().to_json(j);
      break;
    case types::meas_precfg_required:
      j.write_fieldname("MeasurementPreconfigurationRequired");
      c.get<meas_precfg_required_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::ue_context_release_request:
      j.write_fieldname("UEContextReleaseRequest");
      c.get<ue_context_release_request_s>().to_json(j);
      break;
    case types::dl_rrc_msg_transfer:
      j.write_fieldname("DLRRCMessageTransfer");
      c.get<dl_rrc_msg_transfer_s>().to_json(j);
      break;
    case types::ul_rrc_msg_transfer:
      j.write_fieldname("ULRRCMessageTransfer");
      c.get<ul_rrc_msg_transfer_s>().to_json(j);
      break;
    case types::ue_inactivity_notif:
      j.write_fieldname("UEInactivityNotification");
      c.get<ue_inactivity_notif_s>().to_json(j);
      break;
    case types::private_msg:
      j.write_fieldname("PrivateMessage");
      c.get<private_msg_s>().to_json(j);
      break;
    case types::init_ul_rrc_msg_transfer:
      j.write_fieldname("InitialULRRCMessageTransfer");
      c.get<init_ul_rrc_msg_transfer_s>().to_json(j);
      break;
    case types::sys_info_delivery_cmd:
      j.write_fieldname("SystemInformationDeliveryCommand");
      c.get<sys_info_delivery_cmd_s>().to_json(j);
      break;
    case types::paging:
      j.write_fieldname("Paging");
      c.get<paging_s>().to_json(j);
      break;
    case types::notify:
      j.write_fieldname("Notify");
      c.get<notify_s>().to_json(j);
      break;
    case types::pws_restart_ind:
      j.write_fieldname("PWSRestartIndication");
      c.get<pws_restart_ind_s>().to_json(j);
      break;
    case types::pws_fail_ind:
      j.write_fieldname("PWSFailureIndication");
      c.get<pws_fail_ind_s>().to_json(j);
      break;
    case types::gnb_du_status_ind:
      j.write_fieldname("GNBDUStatusIndication");
      c.get<gnb_du_status_ind_s>().to_json(j);
      break;
    case types::rrc_delivery_report:
      j.write_fieldname("RRCDeliveryReport");
      c.get<rrc_delivery_report_s>().to_json(j);
      break;
    case types::network_access_rate_reduction:
      j.write_fieldname("NetworkAccessRateReduction");
      c.get<network_access_rate_reduction_s>().to_json(j);
      break;
    case types::trace_start:
      j.write_fieldname("TraceStart");
      c.get<trace_start_s>().to_json(j);
      break;
    case types::deactiv_trace:
      j.write_fieldname("DeactivateTrace");
      c.get<deactiv_trace_s>().to_json(j);
      break;
    case types::du_cu_radio_info_transfer:
      j.write_fieldname("DUCURadioInformationTransfer");
      c.get<du_cu_radio_info_transfer_s>().to_json(j);
      break;
    case types::cu_du_radio_info_transfer:
      j.write_fieldname("CUDURadioInformationTransfer");
      c.get<cu_du_radio_info_transfer_s>().to_json(j);
      break;
    case types::res_status_upd:
      j.write_fieldname("ResourceStatusUpdate");
      c.get<res_status_upd_s>().to_json(j);
      break;
    case types::access_and_mob_ind:
      j.write_fieldname("AccessAndMobilityIndication");
      c.get<access_and_mob_ind_s>().to_json(j);
      break;
    case types::ref_time_info_report_ctrl:
      j.write_fieldname("ReferenceTimeInformationReportingControl");
      c.get<ref_time_info_report_ctrl_s>().to_json(j);
      break;
    case types::ref_time_info_report:
      j.write_fieldname("ReferenceTimeInformationReport");
      c.get<ref_time_info_report_s>().to_json(j);
      break;
    case types::access_success:
      j.write_fieldname("AccessSuccess");
      c.get<access_success_s>().to_json(j);
      break;
    case types::cell_traffic_trace:
      j.write_fieldname("CellTrafficTrace");
      c.get<cell_traffic_trace_s>().to_json(j);
      break;
    case types::positioning_assist_info_ctrl:
      j.write_fieldname("PositioningAssistanceInformationControl");
      c.get<positioning_assist_info_ctrl_s>().to_json(j);
      break;
    case types::positioning_assist_info_feedback:
      j.write_fieldname("PositioningAssistanceInformationFeedback");
      c.get<positioning_assist_info_feedback_s>().to_json(j);
      break;
    case types::positioning_meas_report:
      j.write_fieldname("PositioningMeasurementReport");
      c.get<positioning_meas_report_s>().to_json(j);
      break;
    case types::positioning_meas_abort:
      j.write_fieldname("PositioningMeasurementAbort");
      c.get<positioning_meas_abort_s>().to_json(j);
      break;
    case types::positioning_meas_fail_ind:
      j.write_fieldname("PositioningMeasurementFailureIndication");
      c.get<positioning_meas_fail_ind_s>().to_json(j);
      break;
    case types::positioning_meas_upd:
      j.write_fieldname("PositioningMeasurementUpdate");
      c.get<positioning_meas_upd_s>().to_json(j);
      break;
    case types::positioning_deactivation:
      j.write_fieldname("PositioningDeactivation");
      c.get<positioning_deactivation_s>().to_json(j);
      break;
    case types::e_c_id_meas_fail_ind:
      j.write_fieldname("E-CIDMeasurementFailureIndication");
      c.get<e_c_id_meas_fail_ind_s>().to_json(j);
      break;
    case types::e_c_id_meas_report:
      j.write_fieldname("E-CIDMeasurementReport");
      c.get<e_c_id_meas_report_s>().to_json(j);
      break;
    case types::e_c_id_meas_termination_cmd:
      j.write_fieldname("E-CIDMeasurementTerminationCommand");
      c.get<e_c_id_meas_termination_cmd_s>().to_json(j);
      break;
    case types::positioning_info_upd:
      j.write_fieldname("PositioningInformationUpdate");
      c.get<positioning_info_upd_s>().to_json(j);
      break;
    case types::multicast_group_paging:
      j.write_fieldname("MulticastGroupPaging");
      c.get<multicast_group_paging_s>().to_json(j);
      break;
    case types::broadcast_context_release_request:
      j.write_fieldname("BroadcastContextReleaseRequest");
      c.get<broadcast_context_release_request_s>().to_json(j);
      break;
    case types::multicast_context_release_request:
      j.write_fieldname("MulticastContextReleaseRequest");
      c.get<multicast_context_release_request_s>().to_json(j);
      break;
    case types::pdc_meas_report:
      j.write_fieldname("PDCMeasurementReport");
      c.get<pdc_meas_report_s>().to_json(j);
      break;
    case types::pdc_meas_termination_cmd:
      j.write_fieldname("PDCMeasurementTerminationCommand");
      c.get<pdc_meas_termination_cmd_s>().to_json(j);
      break;
    case types::pdc_meas_fail_ind:
      j.write_fieldname("PDCMeasurementFailureIndication");
      c.get<pdc_meas_fail_ind_s>().to_json(j);
      break;
    case types::meas_activation:
      j.write_fieldname("MeasurementActivation");
      c.get<meas_activation_s>().to_json(j);
      break;
    case types::qo_e_info_transfer:
      j.write_fieldname("QoEInformationTransfer");
      c.get<qo_e_info_transfer_s>().to_json(j);
      break;
    case types::pos_sys_info_delivery_cmd:
      j.write_fieldname("PosSystemInformationDeliveryCommand");
      c.get<pos_sys_info_delivery_cmd_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE f1ap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset:
      HANDLE_CODE(c.get<reset_s>().pack(bref));
      break;
    case types::f1_setup_request:
      HANDLE_CODE(c.get<f1_setup_request_s>().pack(bref));
      break;
    case types::gnb_du_cfg_upd:
      HANDLE_CODE(c.get<gnb_du_cfg_upd_s>().pack(bref));
      break;
    case types::gnb_cu_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_cfg_upd_s>().pack(bref));
      break;
    case types::ue_context_setup_request:
      HANDLE_CODE(c.get<ue_context_setup_request_s>().pack(bref));
      break;
    case types::ue_context_release_cmd:
      HANDLE_CODE(c.get<ue_context_release_cmd_s>().pack(bref));
      break;
    case types::ue_context_mod_request:
      HANDLE_CODE(c.get<ue_context_mod_request_s>().pack(bref));
      break;
    case types::ue_context_mod_required:
      HANDLE_CODE(c.get<ue_context_mod_required_s>().pack(bref));
      break;
    case types::write_replace_warning_request:
      HANDLE_CODE(c.get<write_replace_warning_request_s>().pack(bref));
      break;
    case types::pws_cancel_request:
      HANDLE_CODE(c.get<pws_cancel_request_s>().pack(bref));
      break;
    case types::gnb_du_res_coordination_request:
      HANDLE_CODE(c.get<gnb_du_res_coordination_request_s>().pack(bref));
      break;
    case types::f1_removal_request:
      HANDLE_CODE(c.get<f1_removal_request_s>().pack(bref));
      break;
    case types::bap_map_cfg:
      HANDLE_CODE(c.get<bap_map_cfg_s>().pack(bref));
      break;
    case types::gnb_du_res_cfg:
      HANDLE_CODE(c.get<gnb_du_res_cfg_s>().pack(bref));
      break;
    case types::iab_tnl_address_request:
      HANDLE_CODE(c.get<iab_tnl_address_request_s>().pack(bref));
      break;
    case types::iab_up_cfg_upd_request:
      HANDLE_CODE(c.get<iab_up_cfg_upd_request_s>().pack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().pack(bref));
      break;
    case types::positioning_meas_request:
      HANDLE_CODE(c.get<positioning_meas_request_s>().pack(bref));
      break;
    case types::trp_info_request:
      HANDLE_CODE(c.get<trp_info_request_s>().pack(bref));
      break;
    case types::positioning_info_request:
      HANDLE_CODE(c.get<positioning_info_request_s>().pack(bref));
      break;
    case types::positioning_activation_request:
      HANDLE_CODE(c.get<positioning_activation_request_s>().pack(bref));
      break;
    case types::e_c_id_meas_initiation_request:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_request_s>().pack(bref));
      break;
    case types::broadcast_context_setup_request:
      HANDLE_CODE(c.get<broadcast_context_setup_request_s>().pack(bref));
      break;
    case types::broadcast_context_release_cmd:
      HANDLE_CODE(c.get<broadcast_context_release_cmd_s>().pack(bref));
      break;
    case types::broadcast_context_mod_request:
      HANDLE_CODE(c.get<broadcast_context_mod_request_s>().pack(bref));
      break;
    case types::multicast_context_setup_request:
      HANDLE_CODE(c.get<multicast_context_setup_request_s>().pack(bref));
      break;
    case types::multicast_context_release_cmd:
      HANDLE_CODE(c.get<multicast_context_release_cmd_s>().pack(bref));
      break;
    case types::multicast_context_mod_request:
      HANDLE_CODE(c.get<multicast_context_mod_request_s>().pack(bref));
      break;
    case types::multicast_distribution_setup_request:
      HANDLE_CODE(c.get<multicast_distribution_setup_request_s>().pack(bref));
      break;
    case types::multicast_distribution_release_cmd:
      HANDLE_CODE(c.get<multicast_distribution_release_cmd_s>().pack(bref));
      break;
    case types::pdc_meas_initiation_request:
      HANDLE_CODE(c.get<pdc_meas_initiation_request_s>().pack(bref));
      break;
    case types::prs_cfg_request:
      HANDLE_CODE(c.get<prs_cfg_request_s>().pack(bref));
      break;
    case types::meas_precfg_required:
      HANDLE_CODE(c.get<meas_precfg_required_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::ue_context_release_request:
      HANDLE_CODE(c.get<ue_context_release_request_s>().pack(bref));
      break;
    case types::dl_rrc_msg_transfer:
      HANDLE_CODE(c.get<dl_rrc_msg_transfer_s>().pack(bref));
      break;
    case types::ul_rrc_msg_transfer:
      HANDLE_CODE(c.get<ul_rrc_msg_transfer_s>().pack(bref));
      break;
    case types::ue_inactivity_notif:
      HANDLE_CODE(c.get<ue_inactivity_notif_s>().pack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().pack(bref));
      break;
    case types::init_ul_rrc_msg_transfer:
      HANDLE_CODE(c.get<init_ul_rrc_msg_transfer_s>().pack(bref));
      break;
    case types::sys_info_delivery_cmd:
      HANDLE_CODE(c.get<sys_info_delivery_cmd_s>().pack(bref));
      break;
    case types::paging:
      HANDLE_CODE(c.get<paging_s>().pack(bref));
      break;
    case types::notify:
      HANDLE_CODE(c.get<notify_s>().pack(bref));
      break;
    case types::pws_restart_ind:
      HANDLE_CODE(c.get<pws_restart_ind_s>().pack(bref));
      break;
    case types::pws_fail_ind:
      HANDLE_CODE(c.get<pws_fail_ind_s>().pack(bref));
      break;
    case types::gnb_du_status_ind:
      HANDLE_CODE(c.get<gnb_du_status_ind_s>().pack(bref));
      break;
    case types::rrc_delivery_report:
      HANDLE_CODE(c.get<rrc_delivery_report_s>().pack(bref));
      break;
    case types::network_access_rate_reduction:
      HANDLE_CODE(c.get<network_access_rate_reduction_s>().pack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().pack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().pack(bref));
      break;
    case types::du_cu_radio_info_transfer:
      HANDLE_CODE(c.get<du_cu_radio_info_transfer_s>().pack(bref));
      break;
    case types::cu_du_radio_info_transfer:
      HANDLE_CODE(c.get<cu_du_radio_info_transfer_s>().pack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().pack(bref));
      break;
    case types::access_and_mob_ind:
      HANDLE_CODE(c.get<access_and_mob_ind_s>().pack(bref));
      break;
    case types::ref_time_info_report_ctrl:
      HANDLE_CODE(c.get<ref_time_info_report_ctrl_s>().pack(bref));
      break;
    case types::ref_time_info_report:
      HANDLE_CODE(c.get<ref_time_info_report_s>().pack(bref));
      break;
    case types::access_success:
      HANDLE_CODE(c.get<access_success_s>().pack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().pack(bref));
      break;
    case types::positioning_assist_info_ctrl:
      HANDLE_CODE(c.get<positioning_assist_info_ctrl_s>().pack(bref));
      break;
    case types::positioning_assist_info_feedback:
      HANDLE_CODE(c.get<positioning_assist_info_feedback_s>().pack(bref));
      break;
    case types::positioning_meas_report:
      HANDLE_CODE(c.get<positioning_meas_report_s>().pack(bref));
      break;
    case types::positioning_meas_abort:
      HANDLE_CODE(c.get<positioning_meas_abort_s>().pack(bref));
      break;
    case types::positioning_meas_fail_ind:
      HANDLE_CODE(c.get<positioning_meas_fail_ind_s>().pack(bref));
      break;
    case types::positioning_meas_upd:
      HANDLE_CODE(c.get<positioning_meas_upd_s>().pack(bref));
      break;
    case types::positioning_deactivation:
      HANDLE_CODE(c.get<positioning_deactivation_s>().pack(bref));
      break;
    case types::e_c_id_meas_fail_ind:
      HANDLE_CODE(c.get<e_c_id_meas_fail_ind_s>().pack(bref));
      break;
    case types::e_c_id_meas_report:
      HANDLE_CODE(c.get<e_c_id_meas_report_s>().pack(bref));
      break;
    case types::e_c_id_meas_termination_cmd:
      HANDLE_CODE(c.get<e_c_id_meas_termination_cmd_s>().pack(bref));
      break;
    case types::positioning_info_upd:
      HANDLE_CODE(c.get<positioning_info_upd_s>().pack(bref));
      break;
    case types::multicast_group_paging:
      HANDLE_CODE(c.get<multicast_group_paging_s>().pack(bref));
      break;
    case types::broadcast_context_release_request:
      HANDLE_CODE(c.get<broadcast_context_release_request_s>().pack(bref));
      break;
    case types::multicast_context_release_request:
      HANDLE_CODE(c.get<multicast_context_release_request_s>().pack(bref));
      break;
    case types::pdc_meas_report:
      HANDLE_CODE(c.get<pdc_meas_report_s>().pack(bref));
      break;
    case types::pdc_meas_termination_cmd:
      HANDLE_CODE(c.get<pdc_meas_termination_cmd_s>().pack(bref));
      break;
    case types::pdc_meas_fail_ind:
      HANDLE_CODE(c.get<pdc_meas_fail_ind_s>().pack(bref));
      break;
    case types::meas_activation:
      HANDLE_CODE(c.get<meas_activation_s>().pack(bref));
      break;
    case types::qo_e_info_transfer:
      HANDLE_CODE(c.get<qo_e_info_transfer_s>().pack(bref));
      break;
    case types::pos_sys_info_delivery_cmd:
      HANDLE_CODE(c.get<pos_sys_info_delivery_cmd_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE f1ap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset:
      HANDLE_CODE(c.get<reset_s>().unpack(bref));
      break;
    case types::f1_setup_request:
      HANDLE_CODE(c.get<f1_setup_request_s>().unpack(bref));
      break;
    case types::gnb_du_cfg_upd:
      HANDLE_CODE(c.get<gnb_du_cfg_upd_s>().unpack(bref));
      break;
    case types::gnb_cu_cfg_upd:
      HANDLE_CODE(c.get<gnb_cu_cfg_upd_s>().unpack(bref));
      break;
    case types::ue_context_setup_request:
      HANDLE_CODE(c.get<ue_context_setup_request_s>().unpack(bref));
      break;
    case types::ue_context_release_cmd:
      HANDLE_CODE(c.get<ue_context_release_cmd_s>().unpack(bref));
      break;
    case types::ue_context_mod_request:
      HANDLE_CODE(c.get<ue_context_mod_request_s>().unpack(bref));
      break;
    case types::ue_context_mod_required:
      HANDLE_CODE(c.get<ue_context_mod_required_s>().unpack(bref));
      break;
    case types::write_replace_warning_request:
      HANDLE_CODE(c.get<write_replace_warning_request_s>().unpack(bref));
      break;
    case types::pws_cancel_request:
      HANDLE_CODE(c.get<pws_cancel_request_s>().unpack(bref));
      break;
    case types::gnb_du_res_coordination_request:
      HANDLE_CODE(c.get<gnb_du_res_coordination_request_s>().unpack(bref));
      break;
    case types::f1_removal_request:
      HANDLE_CODE(c.get<f1_removal_request_s>().unpack(bref));
      break;
    case types::bap_map_cfg:
      HANDLE_CODE(c.get<bap_map_cfg_s>().unpack(bref));
      break;
    case types::gnb_du_res_cfg:
      HANDLE_CODE(c.get<gnb_du_res_cfg_s>().unpack(bref));
      break;
    case types::iab_tnl_address_request:
      HANDLE_CODE(c.get<iab_tnl_address_request_s>().unpack(bref));
      break;
    case types::iab_up_cfg_upd_request:
      HANDLE_CODE(c.get<iab_up_cfg_upd_request_s>().unpack(bref));
      break;
    case types::res_status_request:
      HANDLE_CODE(c.get<res_status_request_s>().unpack(bref));
      break;
    case types::positioning_meas_request:
      HANDLE_CODE(c.get<positioning_meas_request_s>().unpack(bref));
      break;
    case types::trp_info_request:
      HANDLE_CODE(c.get<trp_info_request_s>().unpack(bref));
      break;
    case types::positioning_info_request:
      HANDLE_CODE(c.get<positioning_info_request_s>().unpack(bref));
      break;
    case types::positioning_activation_request:
      HANDLE_CODE(c.get<positioning_activation_request_s>().unpack(bref));
      break;
    case types::e_c_id_meas_initiation_request:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_request_s>().unpack(bref));
      break;
    case types::broadcast_context_setup_request:
      HANDLE_CODE(c.get<broadcast_context_setup_request_s>().unpack(bref));
      break;
    case types::broadcast_context_release_cmd:
      HANDLE_CODE(c.get<broadcast_context_release_cmd_s>().unpack(bref));
      break;
    case types::broadcast_context_mod_request:
      HANDLE_CODE(c.get<broadcast_context_mod_request_s>().unpack(bref));
      break;
    case types::multicast_context_setup_request:
      HANDLE_CODE(c.get<multicast_context_setup_request_s>().unpack(bref));
      break;
    case types::multicast_context_release_cmd:
      HANDLE_CODE(c.get<multicast_context_release_cmd_s>().unpack(bref));
      break;
    case types::multicast_context_mod_request:
      HANDLE_CODE(c.get<multicast_context_mod_request_s>().unpack(bref));
      break;
    case types::multicast_distribution_setup_request:
      HANDLE_CODE(c.get<multicast_distribution_setup_request_s>().unpack(bref));
      break;
    case types::multicast_distribution_release_cmd:
      HANDLE_CODE(c.get<multicast_distribution_release_cmd_s>().unpack(bref));
      break;
    case types::pdc_meas_initiation_request:
      HANDLE_CODE(c.get<pdc_meas_initiation_request_s>().unpack(bref));
      break;
    case types::prs_cfg_request:
      HANDLE_CODE(c.get<prs_cfg_request_s>().unpack(bref));
      break;
    case types::meas_precfg_required:
      HANDLE_CODE(c.get<meas_precfg_required_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::ue_context_release_request:
      HANDLE_CODE(c.get<ue_context_release_request_s>().unpack(bref));
      break;
    case types::dl_rrc_msg_transfer:
      HANDLE_CODE(c.get<dl_rrc_msg_transfer_s>().unpack(bref));
      break;
    case types::ul_rrc_msg_transfer:
      HANDLE_CODE(c.get<ul_rrc_msg_transfer_s>().unpack(bref));
      break;
    case types::ue_inactivity_notif:
      HANDLE_CODE(c.get<ue_inactivity_notif_s>().unpack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().unpack(bref));
      break;
    case types::init_ul_rrc_msg_transfer:
      HANDLE_CODE(c.get<init_ul_rrc_msg_transfer_s>().unpack(bref));
      break;
    case types::sys_info_delivery_cmd:
      HANDLE_CODE(c.get<sys_info_delivery_cmd_s>().unpack(bref));
      break;
    case types::paging:
      HANDLE_CODE(c.get<paging_s>().unpack(bref));
      break;
    case types::notify:
      HANDLE_CODE(c.get<notify_s>().unpack(bref));
      break;
    case types::pws_restart_ind:
      HANDLE_CODE(c.get<pws_restart_ind_s>().unpack(bref));
      break;
    case types::pws_fail_ind:
      HANDLE_CODE(c.get<pws_fail_ind_s>().unpack(bref));
      break;
    case types::gnb_du_status_ind:
      HANDLE_CODE(c.get<gnb_du_status_ind_s>().unpack(bref));
      break;
    case types::rrc_delivery_report:
      HANDLE_CODE(c.get<rrc_delivery_report_s>().unpack(bref));
      break;
    case types::network_access_rate_reduction:
      HANDLE_CODE(c.get<network_access_rate_reduction_s>().unpack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().unpack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().unpack(bref));
      break;
    case types::du_cu_radio_info_transfer:
      HANDLE_CODE(c.get<du_cu_radio_info_transfer_s>().unpack(bref));
      break;
    case types::cu_du_radio_info_transfer:
      HANDLE_CODE(c.get<cu_du_radio_info_transfer_s>().unpack(bref));
      break;
    case types::res_status_upd:
      HANDLE_CODE(c.get<res_status_upd_s>().unpack(bref));
      break;
    case types::access_and_mob_ind:
      HANDLE_CODE(c.get<access_and_mob_ind_s>().unpack(bref));
      break;
    case types::ref_time_info_report_ctrl:
      HANDLE_CODE(c.get<ref_time_info_report_ctrl_s>().unpack(bref));
      break;
    case types::ref_time_info_report:
      HANDLE_CODE(c.get<ref_time_info_report_s>().unpack(bref));
      break;
    case types::access_success:
      HANDLE_CODE(c.get<access_success_s>().unpack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().unpack(bref));
      break;
    case types::positioning_assist_info_ctrl:
      HANDLE_CODE(c.get<positioning_assist_info_ctrl_s>().unpack(bref));
      break;
    case types::positioning_assist_info_feedback:
      HANDLE_CODE(c.get<positioning_assist_info_feedback_s>().unpack(bref));
      break;
    case types::positioning_meas_report:
      HANDLE_CODE(c.get<positioning_meas_report_s>().unpack(bref));
      break;
    case types::positioning_meas_abort:
      HANDLE_CODE(c.get<positioning_meas_abort_s>().unpack(bref));
      break;
    case types::positioning_meas_fail_ind:
      HANDLE_CODE(c.get<positioning_meas_fail_ind_s>().unpack(bref));
      break;
    case types::positioning_meas_upd:
      HANDLE_CODE(c.get<positioning_meas_upd_s>().unpack(bref));
      break;
    case types::positioning_deactivation:
      HANDLE_CODE(c.get<positioning_deactivation_s>().unpack(bref));
      break;
    case types::e_c_id_meas_fail_ind:
      HANDLE_CODE(c.get<e_c_id_meas_fail_ind_s>().unpack(bref));
      break;
    case types::e_c_id_meas_report:
      HANDLE_CODE(c.get<e_c_id_meas_report_s>().unpack(bref));
      break;
    case types::e_c_id_meas_termination_cmd:
      HANDLE_CODE(c.get<e_c_id_meas_termination_cmd_s>().unpack(bref));
      break;
    case types::positioning_info_upd:
      HANDLE_CODE(c.get<positioning_info_upd_s>().unpack(bref));
      break;
    case types::multicast_group_paging:
      HANDLE_CODE(c.get<multicast_group_paging_s>().unpack(bref));
      break;
    case types::broadcast_context_release_request:
      HANDLE_CODE(c.get<broadcast_context_release_request_s>().unpack(bref));
      break;
    case types::multicast_context_release_request:
      HANDLE_CODE(c.get<multicast_context_release_request_s>().unpack(bref));
      break;
    case types::pdc_meas_report:
      HANDLE_CODE(c.get<pdc_meas_report_s>().unpack(bref));
      break;
    case types::pdc_meas_termination_cmd:
      HANDLE_CODE(c.get<pdc_meas_termination_cmd_s>().unpack(bref));
      break;
    case types::pdc_meas_fail_ind:
      HANDLE_CODE(c.get<pdc_meas_fail_ind_s>().unpack(bref));
      break;
    case types::meas_activation:
      HANDLE_CODE(c.get<meas_activation_s>().unpack(bref));
      break;
    case types::qo_e_info_transfer:
      HANDLE_CODE(c.get<qo_e_info_transfer_s>().unpack(bref));
      break;
    case types::pos_sys_info_delivery_cmd:
      HANDLE_CODE(c.get<pos_sys_info_delivery_cmd_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* f1ap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"Reset",
                                "F1SetupRequest",
                                "GNBDUConfigurationUpdate",
                                "GNBCUConfigurationUpdate",
                                "UEContextSetupRequest",
                                "UEContextReleaseCommand",
                                "UEContextModificationRequest",
                                "UEContextModificationRequired",
                                "WriteReplaceWarningRequest",
                                "PWSCancelRequest",
                                "GNBDUResourceCoordinationRequest",
                                "F1RemovalRequest",
                                "BAPMappingConfiguration",
                                "GNBDUResourceConfiguration",
                                "IABTNLAddressRequest",
                                "IABUPConfigurationUpdateRequest",
                                "ResourceStatusRequest",
                                "PositioningMeasurementRequest",
                                "TRPInformationRequest",
                                "PositioningInformationRequest",
                                "PositioningActivationRequest",
                                "E-CIDMeasurementInitiationRequest",
                                "BroadcastContextSetupRequest",
                                "BroadcastContextReleaseCommand",
                                "BroadcastContextModificationRequest",
                                "MulticastContextSetupRequest",
                                "MulticastContextReleaseCommand",
                                "MulticastContextModificationRequest",
                                "MulticastDistributionSetupRequest",
                                "MulticastDistributionReleaseCommand",
                                "PDCMeasurementInitiationRequest",
                                "PRSConfigurationRequest",
                                "MeasurementPreconfigurationRequired",
                                "ErrorIndication",
                                "UEContextReleaseRequest",
                                "DLRRCMessageTransfer",
                                "ULRRCMessageTransfer",
                                "UEInactivityNotification",
                                "PrivateMessage",
                                "InitialULRRCMessageTransfer",
                                "SystemInformationDeliveryCommand",
                                "Paging",
                                "Notify",
                                "PWSRestartIndication",
                                "PWSFailureIndication",
                                "GNBDUStatusIndication",
                                "RRCDeliveryReport",
                                "NetworkAccessRateReduction",
                                "TraceStart",
                                "DeactivateTrace",
                                "DUCURadioInformationTransfer",
                                "CUDURadioInformationTransfer",
                                "ResourceStatusUpdate",
                                "AccessAndMobilityIndication",
                                "ReferenceTimeInformationReportingControl",
                                "ReferenceTimeInformationReport",
                                "AccessSuccess",
                                "CellTrafficTrace",
                                "PositioningAssistanceInformationControl",
                                "PositioningAssistanceInformationFeedback",
                                "PositioningMeasurementReport",
                                "PositioningMeasurementAbort",
                                "PositioningMeasurementFailureIndication",
                                "PositioningMeasurementUpdate",
                                "PositioningDeactivation",
                                "E-CIDMeasurementFailureIndication",
                                "E-CIDMeasurementReport",
                                "E-CIDMeasurementTerminationCommand",
                                "PositioningInformationUpdate",
                                "MulticastGroupPaging",
                                "BroadcastContextReleaseRequest",
                                "MulticastContextReleaseRequest",
                                "PDCMeasurementReport",
                                "PDCMeasurementTerminationCommand",
                                "PDCMeasurementFailureIndication",
                                "MeasurementActivation",
                                "QoEInformationTransfer",
                                "PosSystemInformationDeliveryCommand"};
  return convert_enum_idx(names, 78, value, "f1ap_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void f1ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::reset_ack:
      c = reset_ack_s{};
      break;
    case types::f1_setup_resp:
      c = f1_setup_resp_s{};
      break;
    case types::gnb_du_cfg_upd_ack:
      c = gnb_du_cfg_upd_ack_s{};
      break;
    case types::gnb_cu_cfg_upd_ack:
      c = gnb_cu_cfg_upd_ack_s{};
      break;
    case types::ue_context_setup_resp:
      c = ue_context_setup_resp_s{};
      break;
    case types::ue_context_release_complete:
      c = ue_context_release_complete_s{};
      break;
    case types::ue_context_mod_resp:
      c = ue_context_mod_resp_s{};
      break;
    case types::ue_context_mod_confirm:
      c = ue_context_mod_confirm_s{};
      break;
    case types::write_replace_warning_resp:
      c = write_replace_warning_resp_s{};
      break;
    case types::pws_cancel_resp:
      c = pws_cancel_resp_s{};
      break;
    case types::gnb_du_res_coordination_resp:
      c = gnb_du_res_coordination_resp_s{};
      break;
    case types::f1_removal_resp:
      c = f1_removal_resp_s{};
      break;
    case types::bap_map_cfg_ack:
      c = bap_map_cfg_ack_s{};
      break;
    case types::gnb_du_res_cfg_ack:
      c = gnb_du_res_cfg_ack_s{};
      break;
    case types::iab_tnl_address_resp:
      c = iab_tnl_address_resp_s{};
      break;
    case types::iab_up_cfg_upd_resp:
      c = iab_up_cfg_upd_resp_s{};
      break;
    case types::res_status_resp:
      c = res_status_resp_s{};
      break;
    case types::positioning_meas_resp:
      c = positioning_meas_resp_s{};
      break;
    case types::trp_info_resp:
      c = trp_info_resp_s{};
      break;
    case types::positioning_info_resp:
      c = positioning_info_resp_s{};
      break;
    case types::positioning_activation_resp:
      c = positioning_activation_resp_s{};
      break;
    case types::e_c_id_meas_initiation_resp:
      c = e_c_id_meas_initiation_resp_s{};
      break;
    case types::broadcast_context_setup_resp:
      c = broadcast_context_setup_resp_s{};
      break;
    case types::broadcast_context_release_complete:
      c = broadcast_context_release_complete_s{};
      break;
    case types::broadcast_context_mod_resp:
      c = broadcast_context_mod_resp_s{};
      break;
    case types::multicast_context_setup_resp:
      c = multicast_context_setup_resp_s{};
      break;
    case types::multicast_context_release_complete:
      c = multicast_context_release_complete_s{};
      break;
    case types::multicast_context_mod_resp:
      c = multicast_context_mod_resp_s{};
      break;
    case types::multicast_distribution_setup_resp:
      c = multicast_distribution_setup_resp_s{};
      break;
    case types::multicast_distribution_release_complete:
      c = multicast_distribution_release_complete_s{};
      break;
    case types::pdc_meas_initiation_resp:
      c = pdc_meas_initiation_resp_s{};
      break;
    case types::prs_cfg_resp:
      c = prs_cfg_resp_s{};
      break;
    case types::meas_precfg_confirm:
      c = meas_precfg_confirm_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::successful_outcome_c");
  }
}
reset_ack_s& f1ap_elem_procs_o::successful_outcome_c::reset_ack()
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
f1_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_setup_resp()
{
  assert_choice_type(types::f1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<f1_setup_resp_s>();
}
gnb_du_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_cfg_upd_ack()
{
  assert_choice_type(types::gnb_du_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_ack_s>();
}
gnb_cu_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_cu_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_ack_s>();
}
ue_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_setup_resp()
{
  assert_choice_type(types::ue_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_setup_resp_s>();
}
ue_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_release_complete()
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
ue_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_resp()
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
ue_context_mod_confirm_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_confirm()
{
  assert_choice_type(types::ue_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_confirm_s>();
}
write_replace_warning_resp_s& f1ap_elem_procs_o::successful_outcome_c::write_replace_warning_resp()
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}
pws_cancel_resp_s& f1ap_elem_procs_o::successful_outcome_c::pws_cancel_resp()
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
gnb_du_res_coordination_resp_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_coordination_resp()
{
  assert_choice_type(types::gnb_du_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_coordination_resp_s>();
}
f1_removal_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_removal_resp()
{
  assert_choice_type(types::f1_removal_resp, type_, "SuccessfulOutcome");
  return c.get<f1_removal_resp_s>();
}
bap_map_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::bap_map_cfg_ack()
{
  assert_choice_type(types::bap_map_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<bap_map_cfg_ack_s>();
}
gnb_du_res_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_cfg_ack()
{
  assert_choice_type(types::gnb_du_res_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_cfg_ack_s>();
}
iab_tnl_address_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_tnl_address_resp()
{
  assert_choice_type(types::iab_tnl_address_resp, type_, "SuccessfulOutcome");
  return c.get<iab_tnl_address_resp_s>();
}
iab_up_cfg_upd_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_up_cfg_upd_resp()
{
  assert_choice_type(types::iab_up_cfg_upd_resp, type_, "SuccessfulOutcome");
  return c.get<iab_up_cfg_upd_resp_s>();
}
res_status_resp_s& f1ap_elem_procs_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
positioning_meas_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_meas_resp()
{
  assert_choice_type(types::positioning_meas_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_meas_resp_s>();
}
trp_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::trp_info_resp()
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
positioning_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_info_resp()
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
positioning_activation_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_activation_resp()
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
e_c_id_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp()
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
broadcast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_setup_resp()
{
  assert_choice_type(types::broadcast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_setup_resp_s>();
}
broadcast_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_release_complete()
{
  assert_choice_type(types::broadcast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_release_complete_s>();
}
broadcast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_mod_resp()
{
  assert_choice_type(types::broadcast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_mod_resp_s>();
}
multicast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_setup_resp()
{
  assert_choice_type(types::multicast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_setup_resp_s>();
}
multicast_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_release_complete()
{
  assert_choice_type(types::multicast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_context_release_complete_s>();
}
multicast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_mod_resp()
{
  assert_choice_type(types::multicast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_mod_resp_s>();
}
multicast_distribution_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_setup_resp()
{
  assert_choice_type(types::multicast_distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_setup_resp_s>();
}
multicast_distribution_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_release_complete()
{
  assert_choice_type(types::multicast_distribution_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_release_complete_s>();
}
pdc_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::pdc_meas_initiation_resp()
{
  assert_choice_type(types::pdc_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<pdc_meas_initiation_resp_s>();
}
prs_cfg_resp_s& f1ap_elem_procs_o::successful_outcome_c::prs_cfg_resp()
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
meas_precfg_confirm_s& f1ap_elem_procs_o::successful_outcome_c::meas_precfg_confirm()
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}
const reset_ack_s& f1ap_elem_procs_o::successful_outcome_c::reset_ack() const
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
const f1_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_setup_resp() const
{
  assert_choice_type(types::f1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<f1_setup_resp_s>();
}
const gnb_du_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_du_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_ack_s>();
}
const gnb_cu_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_cu_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_ack_s>();
}
const ue_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_setup_resp() const
{
  assert_choice_type(types::ue_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_setup_resp_s>();
}
const ue_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_release_complete() const
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
const ue_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_resp() const
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
const ue_context_mod_confirm_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_confirm() const
{
  assert_choice_type(types::ue_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_confirm_s>();
}
const write_replace_warning_resp_s& f1ap_elem_procs_o::successful_outcome_c::write_replace_warning_resp() const
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}
const pws_cancel_resp_s& f1ap_elem_procs_o::successful_outcome_c::pws_cancel_resp() const
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
const gnb_du_res_coordination_resp_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_coordination_resp() const
{
  assert_choice_type(types::gnb_du_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_coordination_resp_s>();
}
const f1_removal_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_removal_resp() const
{
  assert_choice_type(types::f1_removal_resp, type_, "SuccessfulOutcome");
  return c.get<f1_removal_resp_s>();
}
const bap_map_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::bap_map_cfg_ack() const
{
  assert_choice_type(types::bap_map_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<bap_map_cfg_ack_s>();
}
const gnb_du_res_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_cfg_ack() const
{
  assert_choice_type(types::gnb_du_res_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_cfg_ack_s>();
}
const iab_tnl_address_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_tnl_address_resp() const
{
  assert_choice_type(types::iab_tnl_address_resp, type_, "SuccessfulOutcome");
  return c.get<iab_tnl_address_resp_s>();
}
const iab_up_cfg_upd_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_up_cfg_upd_resp() const
{
  assert_choice_type(types::iab_up_cfg_upd_resp, type_, "SuccessfulOutcome");
  return c.get<iab_up_cfg_upd_resp_s>();
}
const res_status_resp_s& f1ap_elem_procs_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const positioning_meas_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_meas_resp() const
{
  assert_choice_type(types::positioning_meas_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_meas_resp_s>();
}
const trp_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::trp_info_resp() const
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
const positioning_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_info_resp() const
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
const positioning_activation_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_activation_resp() const
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
const e_c_id_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp() const
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
const broadcast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_setup_resp() const
{
  assert_choice_type(types::broadcast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_setup_resp_s>();
}
const broadcast_context_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::broadcast_context_release_complete() const
{
  assert_choice_type(types::broadcast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_release_complete_s>();
}
const broadcast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_mod_resp() const
{
  assert_choice_type(types::broadcast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_mod_resp_s>();
}
const multicast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_setup_resp() const
{
  assert_choice_type(types::multicast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_setup_resp_s>();
}
const multicast_context_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_context_release_complete() const
{
  assert_choice_type(types::multicast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_context_release_complete_s>();
}
const multicast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_mod_resp() const
{
  assert_choice_type(types::multicast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_mod_resp_s>();
}
const multicast_distribution_setup_resp_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_setup_resp() const
{
  assert_choice_type(types::multicast_distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_setup_resp_s>();
}
const multicast_distribution_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_release_complete() const
{
  assert_choice_type(types::multicast_distribution_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_release_complete_s>();
}
const pdc_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::pdc_meas_initiation_resp() const
{
  assert_choice_type(types::pdc_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<pdc_meas_initiation_resp_s>();
}
const prs_cfg_resp_s& f1ap_elem_procs_o::successful_outcome_c::prs_cfg_resp() const
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
const meas_precfg_confirm_s& f1ap_elem_procs_o::successful_outcome_c::meas_precfg_confirm() const
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}
void f1ap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::reset_ack:
      j.write_fieldname("ResetAcknowledge");
      c.get<reset_ack_s>().to_json(j);
      break;
    case types::f1_setup_resp:
      j.write_fieldname("F1SetupResponse");
      c.get<f1_setup_resp_s>().to_json(j);
      break;
    case types::gnb_du_cfg_upd_ack:
      j.write_fieldname("GNBDUConfigurationUpdateAcknowledge");
      c.get<gnb_du_cfg_upd_ack_s>().to_json(j);
      break;
    case types::gnb_cu_cfg_upd_ack:
      j.write_fieldname("GNBCUConfigurationUpdateAcknowledge");
      c.get<gnb_cu_cfg_upd_ack_s>().to_json(j);
      break;
    case types::ue_context_setup_resp:
      j.write_fieldname("UEContextSetupResponse");
      c.get<ue_context_setup_resp_s>().to_json(j);
      break;
    case types::ue_context_release_complete:
      j.write_fieldname("UEContextReleaseComplete");
      c.get<ue_context_release_complete_s>().to_json(j);
      break;
    case types::ue_context_mod_resp:
      j.write_fieldname("UEContextModificationResponse");
      c.get<ue_context_mod_resp_s>().to_json(j);
      break;
    case types::ue_context_mod_confirm:
      j.write_fieldname("UEContextModificationConfirm");
      c.get<ue_context_mod_confirm_s>().to_json(j);
      break;
    case types::write_replace_warning_resp:
      j.write_fieldname("WriteReplaceWarningResponse");
      c.get<write_replace_warning_resp_s>().to_json(j);
      break;
    case types::pws_cancel_resp:
      j.write_fieldname("PWSCancelResponse");
      c.get<pws_cancel_resp_s>().to_json(j);
      break;
    case types::gnb_du_res_coordination_resp:
      j.write_fieldname("GNBDUResourceCoordinationResponse");
      c.get<gnb_du_res_coordination_resp_s>().to_json(j);
      break;
    case types::f1_removal_resp:
      j.write_fieldname("F1RemovalResponse");
      c.get<f1_removal_resp_s>().to_json(j);
      break;
    case types::bap_map_cfg_ack:
      j.write_fieldname("BAPMappingConfigurationAcknowledge");
      c.get<bap_map_cfg_ack_s>().to_json(j);
      break;
    case types::gnb_du_res_cfg_ack:
      j.write_fieldname("GNBDUResourceConfigurationAcknowledge");
      c.get<gnb_du_res_cfg_ack_s>().to_json(j);
      break;
    case types::iab_tnl_address_resp:
      j.write_fieldname("IABTNLAddressResponse");
      c.get<iab_tnl_address_resp_s>().to_json(j);
      break;
    case types::iab_up_cfg_upd_resp:
      j.write_fieldname("IABUPConfigurationUpdateResponse");
      c.get<iab_up_cfg_upd_resp_s>().to_json(j);
      break;
    case types::res_status_resp:
      j.write_fieldname("ResourceStatusResponse");
      c.get<res_status_resp_s>().to_json(j);
      break;
    case types::positioning_meas_resp:
      j.write_fieldname("PositioningMeasurementResponse");
      c.get<positioning_meas_resp_s>().to_json(j);
      break;
    case types::trp_info_resp:
      j.write_fieldname("TRPInformationResponse");
      c.get<trp_info_resp_s>().to_json(j);
      break;
    case types::positioning_info_resp:
      j.write_fieldname("PositioningInformationResponse");
      c.get<positioning_info_resp_s>().to_json(j);
      break;
    case types::positioning_activation_resp:
      j.write_fieldname("PositioningActivationResponse");
      c.get<positioning_activation_resp_s>().to_json(j);
      break;
    case types::e_c_id_meas_initiation_resp:
      j.write_fieldname("E-CIDMeasurementInitiationResponse");
      c.get<e_c_id_meas_initiation_resp_s>().to_json(j);
      break;
    case types::broadcast_context_setup_resp:
      j.write_fieldname("BroadcastContextSetupResponse");
      c.get<broadcast_context_setup_resp_s>().to_json(j);
      break;
    case types::broadcast_context_release_complete:
      j.write_fieldname("BroadcastContextReleaseComplete");
      c.get<broadcast_context_release_complete_s>().to_json(j);
      break;
    case types::broadcast_context_mod_resp:
      j.write_fieldname("BroadcastContextModificationResponse");
      c.get<broadcast_context_mod_resp_s>().to_json(j);
      break;
    case types::multicast_context_setup_resp:
      j.write_fieldname("MulticastContextSetupResponse");
      c.get<multicast_context_setup_resp_s>().to_json(j);
      break;
    case types::multicast_context_release_complete:
      j.write_fieldname("MulticastContextReleaseComplete");
      c.get<multicast_context_release_complete_s>().to_json(j);
      break;
    case types::multicast_context_mod_resp:
      j.write_fieldname("MulticastContextModificationResponse");
      c.get<multicast_context_mod_resp_s>().to_json(j);
      break;
    case types::multicast_distribution_setup_resp:
      j.write_fieldname("MulticastDistributionSetupResponse");
      c.get<multicast_distribution_setup_resp_s>().to_json(j);
      break;
    case types::multicast_distribution_release_complete:
      j.write_fieldname("MulticastDistributionReleaseComplete");
      c.get<multicast_distribution_release_complete_s>().to_json(j);
      break;
    case types::pdc_meas_initiation_resp:
      j.write_fieldname("PDCMeasurementInitiationResponse");
      c.get<pdc_meas_initiation_resp_s>().to_json(j);
      break;
    case types::prs_cfg_resp:
      j.write_fieldname("PRSConfigurationResponse");
      c.get<prs_cfg_resp_s>().to_json(j);
      break;
    case types::meas_precfg_confirm:
      j.write_fieldname("MeasurementPreconfigurationConfirm");
      c.get<meas_precfg_confirm_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE f1ap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset_ack:
      HANDLE_CODE(c.get<reset_ack_s>().pack(bref));
      break;
    case types::f1_setup_resp:
      HANDLE_CODE(c.get<f1_setup_resp_s>().pack(bref));
      break;
    case types::gnb_du_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_du_cfg_upd_ack_s>().pack(bref));
      break;
    case types::gnb_cu_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_cfg_upd_ack_s>().pack(bref));
      break;
    case types::ue_context_setup_resp:
      HANDLE_CODE(c.get<ue_context_setup_resp_s>().pack(bref));
      break;
    case types::ue_context_release_complete:
      HANDLE_CODE(c.get<ue_context_release_complete_s>().pack(bref));
      break;
    case types::ue_context_mod_resp:
      HANDLE_CODE(c.get<ue_context_mod_resp_s>().pack(bref));
      break;
    case types::ue_context_mod_confirm:
      HANDLE_CODE(c.get<ue_context_mod_confirm_s>().pack(bref));
      break;
    case types::write_replace_warning_resp:
      HANDLE_CODE(c.get<write_replace_warning_resp_s>().pack(bref));
      break;
    case types::pws_cancel_resp:
      HANDLE_CODE(c.get<pws_cancel_resp_s>().pack(bref));
      break;
    case types::gnb_du_res_coordination_resp:
      HANDLE_CODE(c.get<gnb_du_res_coordination_resp_s>().pack(bref));
      break;
    case types::f1_removal_resp:
      HANDLE_CODE(c.get<f1_removal_resp_s>().pack(bref));
      break;
    case types::bap_map_cfg_ack:
      HANDLE_CODE(c.get<bap_map_cfg_ack_s>().pack(bref));
      break;
    case types::gnb_du_res_cfg_ack:
      HANDLE_CODE(c.get<gnb_du_res_cfg_ack_s>().pack(bref));
      break;
    case types::iab_tnl_address_resp:
      HANDLE_CODE(c.get<iab_tnl_address_resp_s>().pack(bref));
      break;
    case types::iab_up_cfg_upd_resp:
      HANDLE_CODE(c.get<iab_up_cfg_upd_resp_s>().pack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().pack(bref));
      break;
    case types::positioning_meas_resp:
      HANDLE_CODE(c.get<positioning_meas_resp_s>().pack(bref));
      break;
    case types::trp_info_resp:
      HANDLE_CODE(c.get<trp_info_resp_s>().pack(bref));
      break;
    case types::positioning_info_resp:
      HANDLE_CODE(c.get<positioning_info_resp_s>().pack(bref));
      break;
    case types::positioning_activation_resp:
      HANDLE_CODE(c.get<positioning_activation_resp_s>().pack(bref));
      break;
    case types::e_c_id_meas_initiation_resp:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_resp_s>().pack(bref));
      break;
    case types::broadcast_context_setup_resp:
      HANDLE_CODE(c.get<broadcast_context_setup_resp_s>().pack(bref));
      break;
    case types::broadcast_context_release_complete:
      HANDLE_CODE(c.get<broadcast_context_release_complete_s>().pack(bref));
      break;
    case types::broadcast_context_mod_resp:
      HANDLE_CODE(c.get<broadcast_context_mod_resp_s>().pack(bref));
      break;
    case types::multicast_context_setup_resp:
      HANDLE_CODE(c.get<multicast_context_setup_resp_s>().pack(bref));
      break;
    case types::multicast_context_release_complete:
      HANDLE_CODE(c.get<multicast_context_release_complete_s>().pack(bref));
      break;
    case types::multicast_context_mod_resp:
      HANDLE_CODE(c.get<multicast_context_mod_resp_s>().pack(bref));
      break;
    case types::multicast_distribution_setup_resp:
      HANDLE_CODE(c.get<multicast_distribution_setup_resp_s>().pack(bref));
      break;
    case types::multicast_distribution_release_complete:
      HANDLE_CODE(c.get<multicast_distribution_release_complete_s>().pack(bref));
      break;
    case types::pdc_meas_initiation_resp:
      HANDLE_CODE(c.get<pdc_meas_initiation_resp_s>().pack(bref));
      break;
    case types::prs_cfg_resp:
      HANDLE_CODE(c.get<prs_cfg_resp_s>().pack(bref));
      break;
    case types::meas_precfg_confirm:
      HANDLE_CODE(c.get<meas_precfg_confirm_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE f1ap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::reset_ack:
      HANDLE_CODE(c.get<reset_ack_s>().unpack(bref));
      break;
    case types::f1_setup_resp:
      HANDLE_CODE(c.get<f1_setup_resp_s>().unpack(bref));
      break;
    case types::gnb_du_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_du_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::gnb_cu_cfg_upd_ack:
      HANDLE_CODE(c.get<gnb_cu_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::ue_context_setup_resp:
      HANDLE_CODE(c.get<ue_context_setup_resp_s>().unpack(bref));
      break;
    case types::ue_context_release_complete:
      HANDLE_CODE(c.get<ue_context_release_complete_s>().unpack(bref));
      break;
    case types::ue_context_mod_resp:
      HANDLE_CODE(c.get<ue_context_mod_resp_s>().unpack(bref));
      break;
    case types::ue_context_mod_confirm:
      HANDLE_CODE(c.get<ue_context_mod_confirm_s>().unpack(bref));
      break;
    case types::write_replace_warning_resp:
      HANDLE_CODE(c.get<write_replace_warning_resp_s>().unpack(bref));
      break;
    case types::pws_cancel_resp:
      HANDLE_CODE(c.get<pws_cancel_resp_s>().unpack(bref));
      break;
    case types::gnb_du_res_coordination_resp:
      HANDLE_CODE(c.get<gnb_du_res_coordination_resp_s>().unpack(bref));
      break;
    case types::f1_removal_resp:
      HANDLE_CODE(c.get<f1_removal_resp_s>().unpack(bref));
      break;
    case types::bap_map_cfg_ack:
      HANDLE_CODE(c.get<bap_map_cfg_ack_s>().unpack(bref));
      break;
    case types::gnb_du_res_cfg_ack:
      HANDLE_CODE(c.get<gnb_du_res_cfg_ack_s>().unpack(bref));
      break;
    case types::iab_tnl_address_resp:
      HANDLE_CODE(c.get<iab_tnl_address_resp_s>().unpack(bref));
      break;
    case types::iab_up_cfg_upd_resp:
      HANDLE_CODE(c.get<iab_up_cfg_upd_resp_s>().unpack(bref));
      break;
    case types::res_status_resp:
      HANDLE_CODE(c.get<res_status_resp_s>().unpack(bref));
      break;
    case types::positioning_meas_resp:
      HANDLE_CODE(c.get<positioning_meas_resp_s>().unpack(bref));
      break;
    case types::trp_info_resp:
      HANDLE_CODE(c.get<trp_info_resp_s>().unpack(bref));
      break;
    case types::positioning_info_resp:
      HANDLE_CODE(c.get<positioning_info_resp_s>().unpack(bref));
      break;
    case types::positioning_activation_resp:
      HANDLE_CODE(c.get<positioning_activation_resp_s>().unpack(bref));
      break;
    case types::e_c_id_meas_initiation_resp:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_resp_s>().unpack(bref));
      break;
    case types::broadcast_context_setup_resp:
      HANDLE_CODE(c.get<broadcast_context_setup_resp_s>().unpack(bref));
      break;
    case types::broadcast_context_release_complete:
      HANDLE_CODE(c.get<broadcast_context_release_complete_s>().unpack(bref));
      break;
    case types::broadcast_context_mod_resp:
      HANDLE_CODE(c.get<broadcast_context_mod_resp_s>().unpack(bref));
      break;
    case types::multicast_context_setup_resp:
      HANDLE_CODE(c.get<multicast_context_setup_resp_s>().unpack(bref));
      break;
    case types::multicast_context_release_complete:
      HANDLE_CODE(c.get<multicast_context_release_complete_s>().unpack(bref));
      break;
    case types::multicast_context_mod_resp:
      HANDLE_CODE(c.get<multicast_context_mod_resp_s>().unpack(bref));
      break;
    case types::multicast_distribution_setup_resp:
      HANDLE_CODE(c.get<multicast_distribution_setup_resp_s>().unpack(bref));
      break;
    case types::multicast_distribution_release_complete:
      HANDLE_CODE(c.get<multicast_distribution_release_complete_s>().unpack(bref));
      break;
    case types::pdc_meas_initiation_resp:
      HANDLE_CODE(c.get<pdc_meas_initiation_resp_s>().unpack(bref));
      break;
    case types::prs_cfg_resp:
      HANDLE_CODE(c.get<prs_cfg_resp_s>().unpack(bref));
      break;
    case types::meas_precfg_confirm:
      HANDLE_CODE(c.get<meas_precfg_confirm_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* f1ap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"ResetAcknowledge",
                                "F1SetupResponse",
                                "GNBDUConfigurationUpdateAcknowledge",
                                "GNBCUConfigurationUpdateAcknowledge",
                                "UEContextSetupResponse",
                                "UEContextReleaseComplete",
                                "UEContextModificationResponse",
                                "UEContextModificationConfirm",
                                "WriteReplaceWarningResponse",
                                "PWSCancelResponse",
                                "GNBDUResourceCoordinationResponse",
                                "F1RemovalResponse",
                                "BAPMappingConfigurationAcknowledge",
                                "GNBDUResourceConfigurationAcknowledge",
                                "IABTNLAddressResponse",
                                "IABUPConfigurationUpdateResponse",
                                "ResourceStatusResponse",
                                "PositioningMeasurementResponse",
                                "TRPInformationResponse",
                                "PositioningInformationResponse",
                                "PositioningActivationResponse",
                                "E-CIDMeasurementInitiationResponse",
                                "BroadcastContextSetupResponse",
                                "BroadcastContextReleaseComplete",
                                "BroadcastContextModificationResponse",
                                "MulticastContextSetupResponse",
                                "MulticastContextReleaseComplete",
                                "MulticastContextModificationResponse",
                                "MulticastDistributionSetupResponse",
                                "MulticastDistributionReleaseComplete",
                                "PDCMeasurementInitiationResponse",
                                "PRSConfigurationResponse",
                                "MeasurementPreconfigurationConfirm"};
  return convert_enum_idx(names, 33, value, "f1ap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void f1ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  switch (type_) {
    case types::f1_setup_fail:
      c = f1_setup_fail_s{};
      break;
    case types::gnb_du_cfg_upd_fail:
      c = gnb_du_cfg_upd_fail_s{};
      break;
    case types::gnb_cu_cfg_upd_fail:
      c = gnb_cu_cfg_upd_fail_s{};
      break;
    case types::ue_context_setup_fail:
      c = ue_context_setup_fail_s{};
      break;
    case types::ue_context_mod_fail:
      c = ue_context_mod_fail_s{};
      break;
    case types::ue_context_mod_refuse:
      c = ue_context_mod_refuse_s{};
      break;
    case types::f1_removal_fail:
      c = f1_removal_fail_s{};
      break;
    case types::bap_map_cfg_fail:
      c = bap_map_cfg_fail_s{};
      break;
    case types::gnb_du_res_cfg_fail:
      c = gnb_du_res_cfg_fail_s{};
      break;
    case types::iab_tnl_address_fail:
      c = iab_tnl_address_fail_s{};
      break;
    case types::iab_up_cfg_upd_fail:
      c = iab_up_cfg_upd_fail_s{};
      break;
    case types::res_status_fail:
      c = res_status_fail_s{};
      break;
    case types::positioning_meas_fail:
      c = positioning_meas_fail_s{};
      break;
    case types::trp_info_fail:
      c = trp_info_fail_s{};
      break;
    case types::positioning_info_fail:
      c = positioning_info_fail_s{};
      break;
    case types::positioning_activation_fail:
      c = positioning_activation_fail_s{};
      break;
    case types::e_c_id_meas_initiation_fail:
      c = e_c_id_meas_initiation_fail_s{};
      break;
    case types::broadcast_context_setup_fail:
      c = broadcast_context_setup_fail_s{};
      break;
    case types::broadcast_context_mod_fail:
      c = broadcast_context_mod_fail_s{};
      break;
    case types::multicast_context_setup_fail:
      c = multicast_context_setup_fail_s{};
      break;
    case types::multicast_context_mod_fail:
      c = multicast_context_mod_fail_s{};
      break;
    case types::multicast_distribution_setup_fail:
      c = multicast_distribution_setup_fail_s{};
      break;
    case types::pdc_meas_initiation_fail:
      c = pdc_meas_initiation_fail_s{};
      break;
    case types::prs_cfg_fail:
      c = prs_cfg_fail_s{};
      break;
    case types::meas_precfg_refuse:
      c = meas_precfg_refuse_s{};
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
f1_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_setup_fail()
{
  assert_choice_type(types::f1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_setup_fail_s>();
}
gnb_du_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_cfg_upd_fail()
{
  assert_choice_type(types::gnb_du_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_fail_s>();
}
gnb_cu_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_fail_s>();
}
ue_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_setup_fail()
{
  assert_choice_type(types::ue_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_setup_fail_s>();
}
ue_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail()
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
ue_context_mod_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_refuse()
{
  assert_choice_type(types::ue_context_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_refuse_s>();
}
f1_removal_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_removal_fail()
{
  assert_choice_type(types::f1_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_removal_fail_s>();
}
bap_map_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::bap_map_cfg_fail()
{
  assert_choice_type(types::bap_map_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<bap_map_cfg_fail_s>();
}
gnb_du_res_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_res_cfg_fail()
{
  assert_choice_type(types::gnb_du_res_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_res_cfg_fail_s>();
}
iab_tnl_address_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_tnl_address_fail()
{
  assert_choice_type(types::iab_tnl_address_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_tnl_address_fail_s>();
}
iab_up_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_cfg_upd_fail()
{
  assert_choice_type(types::iab_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_cfg_upd_fail_s>();
}
res_status_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
positioning_meas_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_meas_fail()
{
  assert_choice_type(types::positioning_meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_meas_fail_s>();
}
trp_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::trp_info_fail()
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
positioning_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail()
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
positioning_activation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail()
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
e_c_id_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail()
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
broadcast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_setup_fail()
{
  assert_choice_type(types::broadcast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_setup_fail_s>();
}
broadcast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_mod_fail()
{
  assert_choice_type(types::broadcast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_mod_fail_s>();
}
multicast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_setup_fail()
{
  assert_choice_type(types::multicast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_setup_fail_s>();
}
multicast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_mod_fail()
{
  assert_choice_type(types::multicast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_mod_fail_s>();
}
multicast_distribution_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_distribution_setup_fail()
{
  assert_choice_type(types::multicast_distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_distribution_setup_fail_s>();
}
pdc_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::pdc_meas_initiation_fail()
{
  assert_choice_type(types::pdc_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<pdc_meas_initiation_fail_s>();
}
prs_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail()
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
meas_precfg_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse()
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}
const f1_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_setup_fail() const
{
  assert_choice_type(types::f1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_setup_fail_s>();
}
const gnb_du_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_du_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_fail_s>();
}
const gnb_cu_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_fail_s>();
}
const ue_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_setup_fail() const
{
  assert_choice_type(types::ue_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_setup_fail_s>();
}
const ue_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail() const
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
const ue_context_mod_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_refuse() const
{
  assert_choice_type(types::ue_context_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_refuse_s>();
}
const f1_removal_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_removal_fail() const
{
  assert_choice_type(types::f1_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_removal_fail_s>();
}
const bap_map_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::bap_map_cfg_fail() const
{
  assert_choice_type(types::bap_map_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<bap_map_cfg_fail_s>();
}
const gnb_du_res_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_res_cfg_fail() const
{
  assert_choice_type(types::gnb_du_res_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_res_cfg_fail_s>();
}
const iab_tnl_address_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_tnl_address_fail() const
{
  assert_choice_type(types::iab_tnl_address_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_tnl_address_fail_s>();
}
const iab_up_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_cfg_upd_fail() const
{
  assert_choice_type(types::iab_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_cfg_upd_fail_s>();
}
const res_status_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const positioning_meas_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_meas_fail() const
{
  assert_choice_type(types::positioning_meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_meas_fail_s>();
}
const trp_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::trp_info_fail() const
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
const positioning_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail() const
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
const positioning_activation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail() const
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
const e_c_id_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail() const
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
const broadcast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_setup_fail() const
{
  assert_choice_type(types::broadcast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_setup_fail_s>();
}
const broadcast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_mod_fail() const
{
  assert_choice_type(types::broadcast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_mod_fail_s>();
}
const multicast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_setup_fail() const
{
  assert_choice_type(types::multicast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_setup_fail_s>();
}
const multicast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_mod_fail() const
{
  assert_choice_type(types::multicast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_mod_fail_s>();
}
const multicast_distribution_setup_fail_s&
f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_distribution_setup_fail() const
{
  assert_choice_type(types::multicast_distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_distribution_setup_fail_s>();
}
const pdc_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::pdc_meas_initiation_fail() const
{
  assert_choice_type(types::pdc_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<pdc_meas_initiation_fail_s>();
}
const prs_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail() const
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
const meas_precfg_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse() const
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}
void f1ap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::f1_setup_fail:
      j.write_fieldname("F1SetupFailure");
      c.get<f1_setup_fail_s>().to_json(j);
      break;
    case types::gnb_du_cfg_upd_fail:
      j.write_fieldname("GNBDUConfigurationUpdateFailure");
      c.get<gnb_du_cfg_upd_fail_s>().to_json(j);
      break;
    case types::gnb_cu_cfg_upd_fail:
      j.write_fieldname("GNBCUConfigurationUpdateFailure");
      c.get<gnb_cu_cfg_upd_fail_s>().to_json(j);
      break;
    case types::ue_context_setup_fail:
      j.write_fieldname("UEContextSetupFailure");
      c.get<ue_context_setup_fail_s>().to_json(j);
      break;
    case types::ue_context_mod_fail:
      j.write_fieldname("UEContextModificationFailure");
      c.get<ue_context_mod_fail_s>().to_json(j);
      break;
    case types::ue_context_mod_refuse:
      j.write_fieldname("UEContextModificationRefuse");
      c.get<ue_context_mod_refuse_s>().to_json(j);
      break;
    case types::f1_removal_fail:
      j.write_fieldname("F1RemovalFailure");
      c.get<f1_removal_fail_s>().to_json(j);
      break;
    case types::bap_map_cfg_fail:
      j.write_fieldname("BAPMappingConfigurationFailure");
      c.get<bap_map_cfg_fail_s>().to_json(j);
      break;
    case types::gnb_du_res_cfg_fail:
      j.write_fieldname("GNBDUResourceConfigurationFailure");
      c.get<gnb_du_res_cfg_fail_s>().to_json(j);
      break;
    case types::iab_tnl_address_fail:
      j.write_fieldname("IABTNLAddressFailure");
      c.get<iab_tnl_address_fail_s>().to_json(j);
      break;
    case types::iab_up_cfg_upd_fail:
      j.write_fieldname("IABUPConfigurationUpdateFailure");
      c.get<iab_up_cfg_upd_fail_s>().to_json(j);
      break;
    case types::res_status_fail:
      j.write_fieldname("ResourceStatusFailure");
      c.get<res_status_fail_s>().to_json(j);
      break;
    case types::positioning_meas_fail:
      j.write_fieldname("PositioningMeasurementFailure");
      c.get<positioning_meas_fail_s>().to_json(j);
      break;
    case types::trp_info_fail:
      j.write_fieldname("TRPInformationFailure");
      c.get<trp_info_fail_s>().to_json(j);
      break;
    case types::positioning_info_fail:
      j.write_fieldname("PositioningInformationFailure");
      c.get<positioning_info_fail_s>().to_json(j);
      break;
    case types::positioning_activation_fail:
      j.write_fieldname("PositioningActivationFailure");
      c.get<positioning_activation_fail_s>().to_json(j);
      break;
    case types::e_c_id_meas_initiation_fail:
      j.write_fieldname("E-CIDMeasurementInitiationFailure");
      c.get<e_c_id_meas_initiation_fail_s>().to_json(j);
      break;
    case types::broadcast_context_setup_fail:
      j.write_fieldname("BroadcastContextSetupFailure");
      c.get<broadcast_context_setup_fail_s>().to_json(j);
      break;
    case types::broadcast_context_mod_fail:
      j.write_fieldname("BroadcastContextModificationFailure");
      c.get<broadcast_context_mod_fail_s>().to_json(j);
      break;
    case types::multicast_context_setup_fail:
      j.write_fieldname("MulticastContextSetupFailure");
      c.get<multicast_context_setup_fail_s>().to_json(j);
      break;
    case types::multicast_context_mod_fail:
      j.write_fieldname("MulticastContextModificationFailure");
      c.get<multicast_context_mod_fail_s>().to_json(j);
      break;
    case types::multicast_distribution_setup_fail:
      j.write_fieldname("MulticastDistributionSetupFailure");
      c.get<multicast_distribution_setup_fail_s>().to_json(j);
      break;
    case types::pdc_meas_initiation_fail:
      j.write_fieldname("PDCMeasurementInitiationFailure");
      c.get<pdc_meas_initiation_fail_s>().to_json(j);
      break;
    case types::prs_cfg_fail:
      j.write_fieldname("PRSConfigurationFailure");
      c.get<prs_cfg_fail_s>().to_json(j);
      break;
    case types::meas_precfg_refuse:
      j.write_fieldname("MeasurementPreconfigurationRefuse");
      c.get<meas_precfg_refuse_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE f1ap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::f1_setup_fail:
      HANDLE_CODE(c.get<f1_setup_fail_s>().pack(bref));
      break;
    case types::gnb_du_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_du_cfg_upd_fail_s>().pack(bref));
      break;
    case types::gnb_cu_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_cfg_upd_fail_s>().pack(bref));
      break;
    case types::ue_context_setup_fail:
      HANDLE_CODE(c.get<ue_context_setup_fail_s>().pack(bref));
      break;
    case types::ue_context_mod_fail:
      HANDLE_CODE(c.get<ue_context_mod_fail_s>().pack(bref));
      break;
    case types::ue_context_mod_refuse:
      HANDLE_CODE(c.get<ue_context_mod_refuse_s>().pack(bref));
      break;
    case types::f1_removal_fail:
      HANDLE_CODE(c.get<f1_removal_fail_s>().pack(bref));
      break;
    case types::bap_map_cfg_fail:
      HANDLE_CODE(c.get<bap_map_cfg_fail_s>().pack(bref));
      break;
    case types::gnb_du_res_cfg_fail:
      HANDLE_CODE(c.get<gnb_du_res_cfg_fail_s>().pack(bref));
      break;
    case types::iab_tnl_address_fail:
      HANDLE_CODE(c.get<iab_tnl_address_fail_s>().pack(bref));
      break;
    case types::iab_up_cfg_upd_fail:
      HANDLE_CODE(c.get<iab_up_cfg_upd_fail_s>().pack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().pack(bref));
      break;
    case types::positioning_meas_fail:
      HANDLE_CODE(c.get<positioning_meas_fail_s>().pack(bref));
      break;
    case types::trp_info_fail:
      HANDLE_CODE(c.get<trp_info_fail_s>().pack(bref));
      break;
    case types::positioning_info_fail:
      HANDLE_CODE(c.get<positioning_info_fail_s>().pack(bref));
      break;
    case types::positioning_activation_fail:
      HANDLE_CODE(c.get<positioning_activation_fail_s>().pack(bref));
      break;
    case types::e_c_id_meas_initiation_fail:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_fail_s>().pack(bref));
      break;
    case types::broadcast_context_setup_fail:
      HANDLE_CODE(c.get<broadcast_context_setup_fail_s>().pack(bref));
      break;
    case types::broadcast_context_mod_fail:
      HANDLE_CODE(c.get<broadcast_context_mod_fail_s>().pack(bref));
      break;
    case types::multicast_context_setup_fail:
      HANDLE_CODE(c.get<multicast_context_setup_fail_s>().pack(bref));
      break;
    case types::multicast_context_mod_fail:
      HANDLE_CODE(c.get<multicast_context_mod_fail_s>().pack(bref));
      break;
    case types::multicast_distribution_setup_fail:
      HANDLE_CODE(c.get<multicast_distribution_setup_fail_s>().pack(bref));
      break;
    case types::pdc_meas_initiation_fail:
      HANDLE_CODE(c.get<pdc_meas_initiation_fail_s>().pack(bref));
      break;
    case types::prs_cfg_fail:
      HANDLE_CODE(c.get<prs_cfg_fail_s>().pack(bref));
      break;
    case types::meas_precfg_refuse:
      HANDLE_CODE(c.get<meas_precfg_refuse_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE f1ap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::f1_setup_fail:
      HANDLE_CODE(c.get<f1_setup_fail_s>().unpack(bref));
      break;
    case types::gnb_du_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_du_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::gnb_cu_cfg_upd_fail:
      HANDLE_CODE(c.get<gnb_cu_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::ue_context_setup_fail:
      HANDLE_CODE(c.get<ue_context_setup_fail_s>().unpack(bref));
      break;
    case types::ue_context_mod_fail:
      HANDLE_CODE(c.get<ue_context_mod_fail_s>().unpack(bref));
      break;
    case types::ue_context_mod_refuse:
      HANDLE_CODE(c.get<ue_context_mod_refuse_s>().unpack(bref));
      break;
    case types::f1_removal_fail:
      HANDLE_CODE(c.get<f1_removal_fail_s>().unpack(bref));
      break;
    case types::bap_map_cfg_fail:
      HANDLE_CODE(c.get<bap_map_cfg_fail_s>().unpack(bref));
      break;
    case types::gnb_du_res_cfg_fail:
      HANDLE_CODE(c.get<gnb_du_res_cfg_fail_s>().unpack(bref));
      break;
    case types::iab_tnl_address_fail:
      HANDLE_CODE(c.get<iab_tnl_address_fail_s>().unpack(bref));
      break;
    case types::iab_up_cfg_upd_fail:
      HANDLE_CODE(c.get<iab_up_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::res_status_fail:
      HANDLE_CODE(c.get<res_status_fail_s>().unpack(bref));
      break;
    case types::positioning_meas_fail:
      HANDLE_CODE(c.get<positioning_meas_fail_s>().unpack(bref));
      break;
    case types::trp_info_fail:
      HANDLE_CODE(c.get<trp_info_fail_s>().unpack(bref));
      break;
    case types::positioning_info_fail:
      HANDLE_CODE(c.get<positioning_info_fail_s>().unpack(bref));
      break;
    case types::positioning_activation_fail:
      HANDLE_CODE(c.get<positioning_activation_fail_s>().unpack(bref));
      break;
    case types::e_c_id_meas_initiation_fail:
      HANDLE_CODE(c.get<e_c_id_meas_initiation_fail_s>().unpack(bref));
      break;
    case types::broadcast_context_setup_fail:
      HANDLE_CODE(c.get<broadcast_context_setup_fail_s>().unpack(bref));
      break;
    case types::broadcast_context_mod_fail:
      HANDLE_CODE(c.get<broadcast_context_mod_fail_s>().unpack(bref));
      break;
    case types::multicast_context_setup_fail:
      HANDLE_CODE(c.get<multicast_context_setup_fail_s>().unpack(bref));
      break;
    case types::multicast_context_mod_fail:
      HANDLE_CODE(c.get<multicast_context_mod_fail_s>().unpack(bref));
      break;
    case types::multicast_distribution_setup_fail:
      HANDLE_CODE(c.get<multicast_distribution_setup_fail_s>().unpack(bref));
      break;
    case types::pdc_meas_initiation_fail:
      HANDLE_CODE(c.get<pdc_meas_initiation_fail_s>().unpack(bref));
      break;
    case types::prs_cfg_fail:
      HANDLE_CODE(c.get<prs_cfg_fail_s>().unpack(bref));
      break;
    case types::meas_precfg_refuse:
      HANDLE_CODE(c.get<meas_precfg_refuse_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"F1SetupFailure",
                                "GNBDUConfigurationUpdateFailure",
                                "GNBCUConfigurationUpdateFailure",
                                "UEContextSetupFailure",
                                "UEContextModificationFailure",
                                "UEContextModificationRefuse",
                                "F1RemovalFailure",
                                "BAPMappingConfigurationFailure",
                                "GNBDUResourceConfigurationFailure",
                                "IABTNLAddressFailure",
                                "IABUPConfigurationUpdateFailure",
                                "ResourceStatusFailure",
                                "PositioningMeasurementFailure",
                                "TRPInformationFailure",
                                "PositioningInformationFailure",
                                "PositioningActivationFailure",
                                "E-CIDMeasurementInitiationFailure",
                                "BroadcastContextSetupFailure",
                                "BroadcastContextModificationFailure",
                                "MulticastContextSetupFailure",
                                "MulticastContextModificationFailure",
                                "MulticastDistributionSetupFailure",
                                "PDCMeasurementInitiationFailure",
                                "PRSConfigurationFailure",
                                "MeasurementPreconfigurationRefuse"};
  return convert_enum_idx(names, 25, value, "f1ap_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != f1ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = f1ap_elem_procs_o::get_init_msg(proc_code);
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
  if (not f1ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = f1ap_elem_procs_o::get_crit(proc_code);
  value     = f1ap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != f1ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != f1ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = f1ap_elem_procs_o::get_successful_outcome(proc_code);
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
  if (not f1ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = f1ap_elem_procs_o::get_crit(proc_code);
  value     = f1ap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != f1ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != f1ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = f1ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
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
  if (not f1ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = f1ap_elem_procs_o::get_crit(proc_code);
  value     = f1ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// F1AP-PDU ::= CHOICE
void f1ap_pdu_c::destroy_()
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
    case types::choice_ext:
      c.destroy<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
      break;
    default:
      break;
  }
}
void f1ap_pdu_c::set(types::options e)
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
    case types::choice_ext:
      c.init<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_pdu_c");
  }
}
f1ap_pdu_c::f1ap_pdu_c(const f1ap_pdu_c& other)
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
    case types::choice_ext:
      c.init(other.c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_pdu_c");
  }
}
f1ap_pdu_c& f1ap_pdu_c::operator=(const f1ap_pdu_c& other)
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
    case types::choice_ext:
      c.set(other.c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_pdu_c");
  }

  return *this;
}
init_msg_s& f1ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& f1ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& f1ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& f1ap_pdu_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
}
void f1ap_pdu_c::to_json(json_writer& j) const
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
    case types::choice_ext:
      j.write_fieldname("choice-extension");
      c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_pdu_c");
  }
  j.end_obj();
}
SRSASN_CODE f1ap_pdu_c::pack(bit_ref& bref) const
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
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_pdu_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE f1ap_pdu_c::unpack(cbit_ref& bref)
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
    case types::choice_ext:
      HANDLE_CODE(c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_pdu_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* f1ap_pdu_c::types_opts::to_string() const
{
  static const char* names[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome", "choice-extension"};
  return convert_enum_idx(names, 4, value, "f1ap_pdu_c::types");
}

// ProtocolIE-FieldPair{F1AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{F1AP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
SRSASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, id, (uint32_t)0u, (uint32_t)65535u, false, true));
  warn_assert(first_crit != ies_set_paramT_::get_first_crit(id), __func__, __LINE__);
  HANDLE_CODE(first_crit.pack(bref));
  HANDLE_CODE(first_value.pack(bref));
  warn_assert(second_crit != ies_set_paramT_::get_second_crit(id), __func__, __LINE__);
  HANDLE_CODE(second_crit.pack(bref));
  HANDLE_CODE(second_value.pack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
SRSASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(first_crit.unpack(bref));
  first_value = ies_set_paramT_::get_first_value(id);
  HANDLE_CODE(first_value.unpack(bref));
  HANDLE_CODE(second_crit.unpack(bref));
  second_value = ies_set_paramT_::get_second_value(id);
  HANDLE_CODE(second_value.unpack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
void protocol_ie_field_pair_s<ies_set_paramT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", id);
  j.write_str("firstCriticality", first_crit.to_string());
  j.write_str("secondCriticality", second_crit.to_string());
  j.end_obj();
}
template <class ies_set_paramT_>
bool protocol_ie_field_pair_s<ies_set_paramT_>::load_info_obj(const uint32_t& id_)
{
  if (not ies_set_paramT_::is_id_valid(id_)) {
    return false;
  }
  id           = id_;
  first_crit   = ies_set_paramT_::get_first_crit(id);
  first_value  = ies_set_paramT_::get_first_value(id);
  second_crit  = ies_set_paramT_::get_second_crit(id);
  second_value = ies_set_paramT_::get_second_value(id);
  return first_value.type().value != ies_set_paramT_::first_value_c::types_opts::nulltype and
         second_value.type().value != ies_set_paramT_::second_value_c::types_opts::nulltype;
}
