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
#include <sstream>

using namespace asn1;
using namespace asn1::f1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

const char* access_and_mob_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "RACHReportInformationList",
                                "RLFReportInformationList",
                                "SuccessfulHOReportInformationList"};
  return convert_enum_idx(names, 4, value, "access_and_mob_ind_ies_o::value_c::types");
}
uint8_t access_and_mob_ind_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "access_and_mob_ind_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<access_and_mob_ind_ies_o>;

const char* access_success_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "NRCGI"};
  return convert_enum_idx(names, 3, value, "access_success_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<access_success_ies_o>;

const char* sul_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NRCarrierList", "FrequencyShift7p5khz"};
  return convert_enum_idx(names, 2, value, "sul_info_ext_ies_o::ext_c::types");
}
uint8_t sul_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  if (value == freq_shift7p5khz) {
    return 7;
  }
  invalid_enum_number(value, "sul_info_ext_ies_o::ext_c::types");
  return 0;
}

const char* nr_freq_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"FrequencyShift7p5khz"};
  return convert_enum_idx(names, 1, value, "nr_freq_info_ext_ies_o::ext_c::types");
}
uint8_t nr_freq_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {7};
  return map_enum_number(numbers, 1, value, "nr_freq_info_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<sul_info_ext_ies_o>;

const char* gnb_du_cell_res_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"RBSetConfiguration",
                                "Frequency-Domain-HSNA-Configuration-List",
                                "Child-IAB-Nodes-NA-Resource-List",
                                "Parent-IAB-Nodes-NA-Resource-Configuration-List"};
  return convert_enum_idx(names, 4, value, "gnb_du_cell_res_cfg_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<gnb_du_cell_res_cfg_ext_ies_o>;

const char* iab_du_cell_res_cfg_fdd_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {
      "NRFreqInfo", "Transmission-Bandwidth", "NRCarrierList", "NRFreqInfo", "Transmission-Bandwidth", "NRCarrierList"};
  return convert_enum_idx(names, 6, value, "iab_du_cell_res_cfg_fdd_info_ext_ies_o::ext_c::types");
}

const char* iab_du_cell_res_cfg_tdd_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NRFreqInfo", "Transmission-Bandwidth", "NRCarrierList"};
  return convert_enum_idx(names, 3, value, "iab_du_cell_res_cfg_tdd_info_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<iab_du_cell_res_cfg_fdd_info_ext_ies_o>;

template struct asn1::protocol_ext_field_s<iab_du_cell_res_cfg_tdd_info_ext_ies_o>;

const char* bh_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NonF1terminatingTopologyIndicator"};
  return convert_enum_idx(names, 1, value, "bh_info_ext_ies_o::ext_c::types");
}
uint8_t bh_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "bh_info_ext_ies_o::ext_c::types");
}

const char* add_pdcp_dupl_tnl_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BHInfo"};
  return convert_enum_idx(names, 1, value, "add_pdcp_dupl_tnl_item_ext_ies_o::ext_c::types");
}

const char* add_path_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"MultipleULAoA", "UL-SRS-RSRPP"};
  return convert_enum_idx(names, 2, value, "add_path_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<add_path_item_ext_ies_o>;

const char* associated_scell_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Associated-SCell-Item"};
  return convert_enum_idx(names, 1, value, "associated_scell_item_ies_o::value_c::types");
}

const char* bap_hdr_rewriting_added_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BAP-Header-Rewriting-Added-List-Item"};
  return convert_enum_idx(names, 1, value, "bap_hdr_rewriting_added_list_item_ies_o::value_c::types");
}

const char* bap_hdr_rewriting_remd_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BAP-Header-Rewriting-Removed-List-Item"};
  return convert_enum_idx(names, 1, value, "bap_hdr_rewriting_remd_list_item_ies_o::value_c::types");
}

const char* ba_player_bh_rlc_ch_map_info_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"IngressNonF1terminatingTopologyIndicator", "EgressNonF1terminatingTopologyIndicator"};
  return convert_enum_idx(names, 2, value, "ba_player_bh_rlc_ch_map_info_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<ba_player_bh_rlc_ch_map_info_item_ext_ies_o>;

const char* bh_routing_info_added_list_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NonF1terminatingTopologyIndicator"};
  return convert_enum_idx(names, 1, value, "bh_routing_info_added_list_item_ext_ies_o::ext_c::types");
}
uint8_t bh_routing_info_added_list_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "bh_routing_info_added_list_item_ext_ies_o::ext_c::types");
}

const char* bh_routing_info_added_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BH-Routing-Information-Added-List-Item"};
  return convert_enum_idx(names, 1, value, "bh_routing_info_added_list_item_ies_o::value_c::types");
}

const char* bh_routing_info_remd_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BH-Routing-Information-Removed-List-Item"};
  return convert_enum_idx(names, 1, value, "bh_routing_info_remd_list_item_ies_o::value_c::types");
}

const char* bap_map_cfg_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "BH-Routing-Information-Added-List",
                                "BH-Routing-Information-Removed-List",
                                "TrafficMappingInfo",
                                "INTEGER (0..16777215)",
                                "BAP-Header-Rewriting-Added-List",
                                "Re-routingEnableIndicator",
                                "BAP-Header-Rewriting-Removed-List"};
  return convert_enum_idx(names, 8, value, "bap_map_cfg_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bap_map_cfg_ies_o>;

const char* bap_map_cfg_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "bap_map_cfg_ack_ies_o::value_c::types");
}
uint8_t bap_map_cfg_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "bap_map_cfg_ack_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bap_map_cfg_ack_ies_o>;

const char* bap_map_cfg_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "bap_map_cfg_fail_ies_o::value_c::types");
}
uint8_t bap_map_cfg_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "bap_map_cfg_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<bap_map_cfg_fail_ies_o>;

const char* bh_chs_failed_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-FailedToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_failed_to_be_modified_item_ies_o::value_c::types");
}

const char* bh_chs_failed_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-FailedToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_failed_to_be_setup_item_ies_o::value_c::types");
}

const char* bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-FailedToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_failed_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* bh_chs_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-Modified-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_modified_item_ies_o::value_c::types");
}

const char* bh_chs_required_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-Required-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_required_to_be_released_item_ies_o::value_c::types");
}

const char* bh_chs_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-Setup-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_setup_item_ies_o::value_c::types");
}

const char* bh_chs_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-SetupMod-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_setup_mod_item_ies_o::value_c::types");
}

const char* dyn_5qi_descriptor_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..65535,...)", "INTEGER (1..65535,...)", "INTEGER (1..65535,...)"};
  return convert_enum_idx(names, 3, value, "dyn_5qi_descriptor_ext_ies_o::ext_c::types");
}

const char* non_dyn_5qi_descriptor_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..65535,...)", "INTEGER (1..65535,...)"};
  return convert_enum_idx(names, 2, value, "non_dyn_5qi_descriptor_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<dyn_5qi_descriptor_ext_ies_o>;

const char* gbr_qos_flow_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AlternativeQoSParaSetList"};
  return convert_enum_idx(names, 1, value, "gbr_qos_flow_info_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<non_dyn_5qi_descriptor_ext_ies_o>;

const char* eutran_qos_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "eutran_qos_ext_ies_o::ext_c::types");
}

const char* qos_flow_level_qos_params_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255)", "INTEGER (0..4000000000000,...)", "QosMonitoringRequest", "BIT STRING"};
  return convert_enum_idx(names, 4, value, "qos_flow_level_qos_params_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<qos_flow_level_qos_params_ext_ies_o>;

const char* bh_chs_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-ToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_to_be_modified_item_ies_o::value_c::types");
}

const char* bh_chs_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_to_be_released_item_ies_o::value_c::types");
}

const char* bh_chs_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_to_be_setup_item_ies_o::value_c::types");
}

const char* bh_chs_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BHChannels-ToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "bh_chs_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* bplmn_id_info_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ConfiguredTACIndication", "NPNBroadcastInformation"};
  return convert_enum_idx(names, 2, value, "bplmn_id_info_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<bplmn_id_info_item_ext_ies_o>;

const char* broadcast_to_be_cancelled_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Broadcast-To-Be-Cancelled-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_to_be_cancelled_list_item_ies_o::value_c::types");
}

const char* broadcast_context_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "broadcast_context_mod_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_mod_fail_ies_o>;

const char* broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-ToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_to_be_modified_item_ies_o::value_c::types");
}

const char* broadcast_m_rbs_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_to_be_released_item_ies_o::value_c::types");
}

const char* broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-ToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* broadcast_context_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MBS-ServiceArea",
                                "MBS-CUtoDURRCInformation",
                                "BroadcastMRBs-ToBeSetupMod-List",
                                "BroadcastMRBs-ToBeModified-List",
                                "BroadcastMRBs-ToBeReleased-List"};
  return convert_enum_idx(names, 7, value, "broadcast_context_mod_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_mod_request_ies_o>;

const char* broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-FailedToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types");
}

const char* broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-FailedToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* broadcast_m_rbs_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-Modified-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_modified_item_ies_o::value_c::types");
}

const char* broadcast_m_rbs_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-SetupMod-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_setup_mod_item_ies_o::value_c::types");
}

const char* broadcast_context_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "BroadcastMRBs-SetupMod-List",
                                "BroadcastMRBs-FailedToBeSetupMod-List",
                                "BroadcastMRBs-Modified-List",
                                "BroadcastMRBs-FailedToBeModified-List",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 7, value, "broadcast_context_mod_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_mod_resp_ies_o>;

const char* broadcast_context_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(names, 3, value, "broadcast_context_release_cmd_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_release_cmd_ies_o>;

const char* broadcast_context_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "broadcast_context_release_complete_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_release_complete_ies_o>;

const char* broadcast_context_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(names, 3, value, "broadcast_context_release_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_release_request_ies_o>;

const char* broadcast_context_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "broadcast_context_setup_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_setup_fail_ies_o>;

const char* broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_to_be_setup_item_ies_o::value_c::types");
}

const char* broadcast_context_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "MBS-Session-ID",
                                "MBS-ServiceArea",
                                "MBS-CUtoDURRCInformation",
                                "SNSSAI",
                                "BroadcastMRBs-ToBeSetup-List"};
  return convert_enum_idx(names, 6, value, "broadcast_context_setup_request_ies_o::value_c::types");
}
uint8_t broadcast_context_setup_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "broadcast_context_setup_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_setup_request_ies_o>;

const char* broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-FailedToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types");
}

const char* broadcast_m_rbs_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BroadcastMRBs-Setup-Item"};
  return convert_enum_idx(names, 1, value, "broadcast_m_rbs_setup_item_ies_o::value_c::types");
}

const char* broadcast_context_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "BroadcastMRBs-Setup-List",
                                "BroadcastMRBs-FailedToBeSetup-List",
                                "BroadcastAreaScope",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 6, value, "broadcast_context_setup_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_context_setup_resp_ies_o>;

const char* endpoint_ip_address_and_port_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "endpoint_ip_address_and_port_ext_ies_o::ext_c::types");
}

const char* cu_du_radio_info_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CUDURadioInformationType"};
  return convert_enum_idx(names, 2, value, "cu_du_radio_info_transfer_ies_o::value_c::types");
}
uint8_t cu_du_radio_info_transfer_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "cu_du_radio_info_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<cu_du_radio_info_transfer_ies_o>;

const char* cu_to_du_rrc_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING"};
  return convert_enum_idx(names, 14, value, "cu_to_du_rrc_info_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<cu_to_du_rrc_info_ext_ies_o>;

const char* candidate_sp_cell_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Candidate-SpCell-Item"};
  return convert_enum_idx(names, 1, value, "candidate_sp_cell_item_ies_o::value_c::types");
}

const char* composite_available_capacity_group_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"CompositeAvailableCapacity"};
  return convert_enum_idx(names, 1, value, "composite_available_capacity_group_ext_ies_o::ext_c::types");
}

const char* rr_status_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SliceRadioResourceStatus", "MIMOPRBusageInformation"};
  return convert_enum_idx(names, 2, value, "rr_status_ext_ies_o::ext_c::types");
}

const char* cell_meas_result_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NR-U-Channel-List"};
  return convert_enum_idx(names, 1, value, "cell_meas_result_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<rr_status_ext_ies_o>;

const char* cell_traffic_trace_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "BIT STRING",
                                "PrivacyIndicator",
                                "VisibleString"};
  return convert_enum_idx(names, 6, value, "cell_traffic_trace_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<cell_traffic_trace_ies_o>;

const char* cells_broadcast_cancelled_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-Broadcast-Cancelled-Item"};
  return convert_enum_idx(names, 1, value, "cells_broadcast_cancelled_list_item_ies_o::value_c::types");
}

const char* cells_broadcast_completed_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-Broadcast-Completed-Item"};
  return convert_enum_idx(names, 1, value, "cells_broadcast_completed_list_item_ies_o::value_c::types");
}

const char* cells_failed_to_be_activ_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-Failed-to-be-Activated-List-Item"};
  return convert_enum_idx(names, 1, value, "cells_failed_to_be_activ_list_item_ies_o::value_c::types");
}

const char* cells_status_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-Status-Item"};
  return convert_enum_idx(names, 1, value, "cells_status_item_ies_o::value_c::types");
}

const char* cells_to_be_broadcast_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-To-Be-Broadcast-Item"};
  return convert_enum_idx(names, 1, value, "cells_to_be_broadcast_list_item_ies_o::value_c::types");
}

const char* sibtypetobeupd_list_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AreaScope"};
  return convert_enum_idx(names, 1, value, "sibtypetobeupd_list_item_ext_ies_o::ext_c::types");
}

const char* gnb_cu_sys_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "gnb_cu_sys_info_ext_ies_o::ext_c::types");
}

const char* cells_to_be_activ_list_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-CUSystemInformation",
                                "AvailablePLMNList",
                                "ExtendedAvailablePLMN-List",
                                "IAB-Info-IAB-donor-CU",
                                "AvailableSNPN-ID-List",
                                "OCTET STRING"};
  return convert_enum_idx(names, 6, value, "cells_to_be_activ_list_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<cells_to_be_activ_list_item_ext_ies_o>;

const char* cells_to_be_activ_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-to-be-Activated-List-Item"};
  return convert_enum_idx(names, 1, value, "cells_to_be_activ_list_item_ies_o::value_c::types");
}

const char* cells_to_be_barred_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"IAB-Barred"};
  return convert_enum_idx(names, 1, value, "cells_to_be_barred_item_ext_ies_o::ext_c::types");
}

const char* cells_to_be_barred_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-to-be-Barred-Item"};
  return convert_enum_idx(names, 1, value, "cells_to_be_barred_item_ies_o::value_c::types");
}

const char* cells_to_be_deactiv_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cells-to-be-Deactivated-List-Item"};
  return convert_enum_idx(names, 1, value, "cells_to_be_deactiv_list_item_ies_o::value_c::types");
}

const char* freq_domain_len_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"L571Info", "L1151Info"};
  return convert_enum_idx(names, 2, value, "freq_domain_len_ext_ies_o::value_c::types");
}
uint16_t freq_domain_len_ext_ies_o::value_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {571, 1151};
  return map_enum_number(numbers, 2, value, "freq_domain_len_ext_ies_o::value_c::types");
}

const char* iab_mt_cell_list_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"DU-RX-MT-RX-Extend", "DU-TX-MT-TX-Extend", "DU-RX-MT-TX-Extend", "DU-TX-MT-RX-Extend"};
  return convert_enum_idx(names, 4, value, "iab_mt_cell_list_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<iab_mt_cell_list_item_ext_ies_o>;

const char* conditional_inter_du_mob_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..100)"};
  return convert_enum_idx(names, 1, value, "conditional_inter_du_mob_info_ext_ies_o::ext_c::types");
}
uint8_t conditional_inter_du_mob_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "conditional_inter_du_mob_info_ext_ies_o::ext_c::types");
}

const char* conditional_intra_du_mob_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..100)"};
  return convert_enum_idx(names, 1, value, "conditional_intra_du_mob_info_ext_ies_o::ext_c::types");
}
uint8_t conditional_intra_du_mob_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "conditional_intra_du_mob_info_ext_ies_o::ext_c::types");
}

const char* dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DL-UP-TNL-Address-to-Update-List-Item"};
  return convert_enum_idx(names, 1, value, "dl_up_tnl_address_to_upd_list_item_ies_o::value_c::types");
}

const char* dl_rrc_msg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (0..3,...)",
                                "ExecuteDuplication",
                                "OCTET STRING",
                                "RAT-FrequencyPriorityInformation",
                                "RRCDeliveryStatusRequest",
                                "UEContextNotRetrievable",
                                "OCTET STRING",
                                "OCTET STRING",
                                "INTEGER (0..4294967295)",
                                "BIT STRING",
                                "BIT STRING"};
  return convert_enum_idx(names, 14, value, "dl_rrc_msg_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_rrc_msg_transfer_ies_o>;

const char* drb_activity_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-Activity-Item"};
  return convert_enum_idx(names, 1, value, "drb_activity_item_ies_o::value_c::types");
}

const char* tsc_assist_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1920000,...)"};
  return convert_enum_idx(names, 1, value, "tsc_assist_info_ext_ies_o::ext_c::types");
}
uint8_t tsc_assist_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "tsc_assist_info_ext_ies_o::ext_c::types");
}

const char* flows_mapped_to_drb_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QoSFlowMappingIndication", "TSCTrafficCharacteristics"};
  return convert_enum_idx(names, 2, value, "flows_mapped_to_drb_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<flows_mapped_to_drb_item_ext_ies_o>;

const char* drb_notify_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..8,...)"};
  return convert_enum_idx(names, 1, value, "drb_notify_item_ext_ies_o::ext_c::types");
}
uint8_t drb_notify_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "drb_notify_item_ext_ies_o::ext_c::types");
}

const char* drb_notify_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-Notify-Item"};
  return convert_enum_idx(names, 1, value, "drb_notify_item_ies_o::value_c::types");
}

const char* drbs_failed_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-FailedToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "drbs_failed_to_be_modified_item_ies_o::value_c::types");
}

const char* drbs_failed_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-FailedToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "drbs_failed_to_be_setup_item_ies_o::value_c::types");
}

const char* drbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-FailedToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "drbs_failed_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* drbs_modified_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"RLC-Status", "AdditionalPDCPDuplicationTNL-List", "INTEGER (1..8,...)"};
  return convert_enum_idx(names, 3, value, "drbs_modified_item_ext_ies_o::ext_c::types");
}
uint8_t drbs_modified_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  if (value == current_qos_para_set_idx) {
    return 1;
  }
  invalid_enum_number(value, "drbs_modified_item_ext_ies_o::ext_c::types");
  return 0;
}

template struct asn1::protocol_ext_field_s<drbs_modified_item_ext_ies_o>;

const char* drbs_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-Modified-Item"};
  return convert_enum_idx(names, 1, value, "drbs_modified_item_ies_o::value_c::types");
}

const char* ul_up_tnl_info_to_be_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BHInfo", "BIT STRING"};
  return convert_enum_idx(names, 2, value, "ul_up_tnl_info_to_be_setup_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<ul_up_tnl_info_to_be_setup_item_ext_ies_o>;

const char* drbs_modified_conf_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AdditionalPDCPDuplicationTNL-List"};
  return convert_enum_idx(names, 1, value, "drbs_modified_conf_item_ext_ies_o::ext_c::types");
}

const char* drbs_modified_conf_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-ModifiedConf-Item"};
  return convert_enum_idx(names, 1, value, "drbs_modified_conf_item_ies_o::value_c::types");
}

const char* drbs_required_to_be_modified_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"RLC-Status", "AdditionalPDCPDuplicationTNL-List"};
  return convert_enum_idx(names, 2, value, "drbs_required_to_be_modified_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<drbs_required_to_be_modified_item_ext_ies_o>;

const char* drbs_required_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-Required-ToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "drbs_required_to_be_modified_item_ies_o::value_c::types");
}

const char* drbs_required_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-Required-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "drbs_required_to_be_released_item_ies_o::value_c::types");
}

const char* drbs_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AdditionalPDCPDuplicationTNL-List", "INTEGER (1..8,...)"};
  return convert_enum_idx(names, 2, value, "drbs_setup_item_ext_ies_o::ext_c::types");
}
uint8_t drbs_setup_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  if (value == current_qos_para_set_idx) {
    return 1;
  }
  invalid_enum_number(value, "drbs_setup_item_ext_ies_o::ext_c::types");
  return 0;
}

template struct asn1::protocol_ext_field_s<drbs_setup_item_ext_ies_o>;

const char* drbs_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-Setup-Item"};
  return convert_enum_idx(names, 1, value, "drbs_setup_item_ies_o::value_c::types");
}

const char* drbs_setup_mod_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AdditionalPDCPDuplicationTNL-List", "INTEGER (1..8,...)"};
  return convert_enum_idx(names, 2, value, "drbs_setup_mod_item_ext_ies_o::ext_c::types");
}
uint8_t drbs_setup_mod_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  if (value == current_qos_para_set_idx) {
    return 1;
  }
  invalid_enum_number(value, "drbs_setup_mod_item_ext_ies_o::ext_c::types");
  return 0;
}

template struct asn1::protocol_ext_field_s<drbs_setup_mod_item_ext_ies_o>;

const char* drbs_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-SetupMod-Item"};
  return convert_enum_idx(names, 1, value, "drbs_setup_mod_item_ies_o::value_c::types");
}

const char* qos_info_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRB-Information"};
  return convert_enum_idx(names, 1, value, "qos_info_ext_ies_o::value_c::types");
}

const char* drbs_to_be_modified_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"PDCPSNLength",
                                "PDCPSNLength",
                                "BearerTypeChange",
                                "RLCMode",
                                "DuplicationActivation",
                                "DCBasedDuplicationConfigured",
                                "DuplicationActivation",
                                "AdditionalPDCPDuplicationTNL-List",
                                "RLCDuplicationInformation",
                                "TransmissionStopIndicator",
                                "CG-SDTindicatorMod"};
  return convert_enum_idx(names, 11, value, "drbs_to_be_modified_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<drbs_to_be_modified_item_ext_ies_o>;

const char* drbs_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-ToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "drbs_to_be_modified_item_ies_o::value_c::types");
}

const char* drbs_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "drbs_to_be_released_item_ies_o::value_c::types");
}

const char* drbs_to_be_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"DCBasedDuplicationConfigured",
                                "DuplicationActivation",
                                "PDCPSNLength",
                                "PDCPSNLength",
                                "AdditionalPDCPDuplicationTNL-List",
                                "RLCDuplicationInformation",
                                "OCTET STRING"};
  return convert_enum_idx(names, 7, value, "drbs_to_be_setup_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<drbs_to_be_setup_item_ext_ies_o>;

const char* drbs_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "drbs_to_be_setup_item_ies_o::value_c::types");
}

const char* drbs_to_be_setup_mod_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"DCBasedDuplicationConfigured",
                                "DuplicationActivation",
                                "PDCPSNLength",
                                "PDCPSNLength",
                                "AdditionalPDCPDuplicationTNL-List",
                                "RLCDuplicationInformation",
                                "CG-SDTindicatorSetup"};
  return convert_enum_idx(names, 7, value, "drbs_to_be_setup_mod_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<drbs_to_be_setup_mod_item_ext_ies_o>;

const char* drbs_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"DRBs-ToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "drbs_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* du_cu_radio_info_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "DUCURadioInformationType"};
  return convert_enum_idx(names, 2, value, "du_cu_radio_info_transfer_ies_o::value_c::types");
}
uint8_t du_cu_radio_info_transfer_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "du_cu_radio_info_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<du_cu_radio_info_transfer_ies_o>;

const char* du_to_cu_rrc_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..10239)", "OCTET STRING",  "OCTET STRING",
                                "OCTET STRING",       "OCTET STRING",  "OCTET STRING",
                                "OCTET STRING",       "OCTET STRING",  "OCTET STRING",
                                "OCTET STRING",       "OCTET STRING",  "OCTET STRING",
                                "OCTET STRING",       "OCTET STRING",  "OCTET STRING",
                                "OCTET STRING",       "OCTET STRING",  "InterFrequencyConfig-NoGap",
                                "OCTET STRING",       "TwoPHRModeMCG", "TwoPHRModeSCG"};
  return convert_enum_idx(names, 21, value, "du_to_cu_rrc_info_ext_ies_o::ext_c::types");
}
uint8_t du_to_cu_rrc_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "du_to_cu_rrc_info_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<du_to_cu_rrc_info_ext_ies_o>;

const char* deactiv_trace_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "deactiv_trace_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<deactiv_trace_ies_o>;

const char* ded_si_delivery_needed_ue_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Dedicated-SIDelivery-NeededUE-Item"};
  return convert_enum_idx(names, 1, value, "ded_si_delivery_needed_ue_item_ies_o::value_c::types");
}

const char* e_c_id_measured_results_value_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..7690)"};
  return convert_enum_idx(names, 1, value, "e_c_id_measured_results_value_ext_ies_o::value_c::types");
}
uint8_t e_c_id_measured_results_value_ext_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "e_c_id_measured_results_value_ext_ies_o::value_c::types");
}

const char* e_c_id_meas_quantities_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"E-CID-MeasurementQuantities-Item"};
  return convert_enum_idx(names, 1, value, "e_c_id_meas_quantities_item_ies_o::value_c::types");
}

const char* geographical_coordinates_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ARPLocationInformation"};
  return convert_enum_idx(names, 1, value, "geographical_coordinates_ext_ies_o::ext_c::types");
}

const char* e_c_id_meas_fail_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (1..256,...)", "INTEGER (1..256,...)", "Cause"};
  return convert_enum_idx(names, 5, value, "e_c_id_meas_fail_ind_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e_c_id_meas_fail_ind_ies_o>;

const char* e_c_id_meas_initiation_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256,...)",
                                "INTEGER (1..256,...)",
                                "Cause",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 6, value, "e_c_id_meas_initiation_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e_c_id_meas_initiation_fail_ies_o>;

const char* e_c_id_meas_initiation_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256,...)",
                                "INTEGER (1..256,...)",
                                "E-CID-ReportCharacteristics",
                                "MeasurementPeriodicity",
                                "E-CID-MeasurementQuantities",
                                "PosMeasurementPeriodicityNR-AoA"};
  return convert_enum_idx(names, 8, value, "e_c_id_meas_initiation_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e_c_id_meas_initiation_request_ies_o>;

const char* e_c_id_meas_initiation_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256,...)",
                                "INTEGER (1..256,...)",
                                "E-CID-MeasurementResult",
                                "INTEGER (0..4095,...)",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 7, value, "e_c_id_meas_initiation_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e_c_id_meas_initiation_resp_ies_o>;

const char* e_c_id_meas_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256,...)",
                                "INTEGER (1..256,...)",
                                "E-CID-MeasurementResult",
                                "INTEGER (0..4095,...)"};
  return convert_enum_idx(names, 6, value, "e_c_id_meas_report_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e_c_id_meas_report_ies_o>;

const char* e_c_id_meas_termination_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (1..256,...)", "INTEGER (1..256,...)"};
  return convert_enum_idx(names, 4, value, "e_c_id_meas_termination_cmd_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e_c_id_meas_termination_cmd_ies_o>;

const char* error_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "error_ind_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<error_ind_ies_o>;

const char* extended_served_plmns_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NPNSupportInfo", "ExtendedSliceSupportList", "NSAGSupportList"};
  return convert_enum_idx(names, 3, value, "extended_served_plmns_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<extended_served_plmns_item_ext_ies_o>;

const char* num_dl_ul_symbols_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"Permutation"};
  return convert_enum_idx(names, 1, value, "num_dl_ul_symbols_ext_ies_o::ext_c::types");
}

const char* prs_angle_item_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..63)"};
  return convert_enum_idx(names, 1, value, "prs_angle_item_item_ext_ies_o::ext_c::types");
}
uint8_t prs_angle_item_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "prs_angle_item_item_ext_ies_o::ext_c::types");
}

const char* fdd_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NRCarrierList", "NRCarrierList"};
  return convert_enum_idx(names, 2, value, "fdd_info_ext_ies_o::ext_c::types");
}

const char* gnb_rx_tx_time_diff_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExtendedAdditionalPathList", "TRPTEGInformation"};
  return convert_enum_idx(names, 2, value, "gnb_rx_tx_time_diff_ext_ies_o::ext_c::types");
}

const char* srs_restype_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SRSPortIndex"};
  return convert_enum_idx(names, 1, value, "srs_restype_ext_ies_o::ext_c::types");
}

const char* served_plmns_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SliceSupportList", "NPNSupportInfo", "ExtendedSliceSupportList", "NSAGSupportList"};
  return convert_enum_idx(names, 4, value, "served_plmns_item_ext_ies_o::ext_c::types");
}

const char* tdd_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"IntendedTDD-DL-ULConfig", "OCTET STRING", "NRCarrierList"};
  return convert_enum_idx(names, 3, value, "tdd_info_ext_ies_o::ext_c::types");
}

const char* trp_info_type_resp_item_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"TRPType", "OnDemandPRS-Info", "TRPTxTEGAssociation", "TRPBeamAntennaInformation"};
  return convert_enum_idx(names, 4, value, "trp_info_type_resp_item_ext_ies_o::value_c::types");
}

const char* time_stamp_slot_idx_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..319)", "INTEGER (0..639)"};
  return convert_enum_idx(names, 2, value, "time_stamp_slot_idx_ext_ies_o::value_c::types");
}

const char* ul_rtoa_meas_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExtendedAdditionalPathList", "TRP-Rx-TEGInformation"};
  return convert_enum_idx(names, 2, value, "ul_rtoa_meas_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<fdd_info_ext_ies_o>;

template struct asn1::protocol_ext_field_s<gnb_rx_tx_time_diff_ext_ies_o>;

const char* measured_results_value_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"ZoAInformation", "MultipleULAoA", "UL-SRS-RSRPP"};
  return convert_enum_idx(names, 3, value, "measured_results_value_ext_ies_o::value_c::types");
}

const char* nr_mode_info_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"NR-U-Channel-Info-List"};
  return convert_enum_idx(names, 1, value, "nr_mode_info_ext_ies_o::value_c::types");
}

template struct asn1::protocol_ext_field_s<served_plmns_item_ext_ies_o>;

template struct asn1::protocol_ext_field_s<tdd_info_ext_ies_o>;

template struct asn1::protocol_ext_field_s<ul_rtoa_meas_ext_ies_o>;

const char* gnb_du_sys_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {
      "OCTET STRING", "OCTET STRING", "OCTET STRING", "OCTET STRING", "OCTET STRING", "OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 7, value, "gnb_du_sys_info_ext_ies_o::ext_c::types");
}

const char* m5_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"M5ReportAmount"};
  return convert_enum_idx(names, 1, value, "m5_cfg_ext_ies_o::ext_c::types");
}
uint8_t m5_cfg_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "m5_cfg_ext_ies_o::ext_c::types");
}

const char* m6_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"M6ReportAmount"};
  return convert_enum_idx(names, 1, value, "m6_cfg_ext_ies_o::ext_c::types");
}
uint8_t m6_cfg_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {6};
  return map_enum_number(numbers, 1, value, "m6_cfg_ext_ies_o::ext_c::types");
}

const char* m7_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"M7ReportAmount"};
  return convert_enum_idx(names, 1, value, "m7_cfg_ext_ies_o::ext_c::types");
}
uint8_t m7_cfg_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {7};
  return map_enum_number(numbers, 1, value, "m7_cfg_ext_ies_o::ext_c::types");
}

const char* pos_meas_result_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..16,...)", "SRSResourcetype", "LoS-NLoSInformation"};
  return convert_enum_idx(names, 3, value, "pos_meas_result_item_ext_ies_o::ext_c::types");
}
uint8_t pos_meas_result_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "pos_meas_result_item_ext_ies_o::ext_c::types");
}

const char* served_cell_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255)",
                                "ExtendedServedPLMNs-List",
                                "Cell-Direction",
                                "BPLMN-ID-Info-List",
                                "CellType",
                                "ConfiguredTACIndication",
                                "AggressorgNBSetID",
                                "VictimgNBSetID",
                                "IAB-Info-IAB-DU",
                                "SSB-PositionsInBurst",
                                "NRPRACHConfig",
                                "SFN-Offset",
                                "NPNBroadcastInformation",
                                "Supported-MBS-FSA-ID-List",
                                "BIT STRING"};
  return convert_enum_idx(names, 15, value, "served_cell_info_ext_ies_o::ext_c::types");
}
uint8_t served_cell_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "served_cell_info_ext_ies_o::ext_c::types");
}

const char* gnb_cu_tnl_assoc_to_rem_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"CP-TransportLayerAddress"};
  return convert_enum_idx(names, 1, value, "gnb_cu_tnl_assoc_to_rem_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<gnb_du_sys_info_ext_ies_o>;

const char* paging_cell_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"LastUsedCellIndication", "PEISubgroupingSupportIndication"};
  return convert_enum_idx(names, 2, value, "paging_cell_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<pos_meas_result_item_ext_ies_o>;

const char* scell_to_be_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..64,...)"};
  return convert_enum_idx(names, 1, value, "scell_to_be_setup_item_ext_ies_o::ext_c::types");
}
uint8_t scell_to_be_setup_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "scell_to_be_setup_item_ext_ies_o::ext_c::types");
}

const char* scell_to_be_setup_mod_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..64,...)"};
  return convert_enum_idx(names, 1, value, "scell_to_be_setup_mod_item_ext_ies_o::ext_c::types");
}
uint8_t scell_to_be_setup_mod_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "scell_to_be_setup_mod_item_ext_ies_o::ext_c::types");
}

const char* srbs_to_be_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AdditionalDuplicationIndication", "OCTET STRING", "BIT STRING"};
  return convert_enum_idx(names, 3, value, "srbs_to_be_setup_item_ext_ies_o::ext_c::types");
}

const char* srbs_to_be_setup_mod_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AdditionalDuplicationIndication", "BIT STRING", "CG-SDTindicatorSetup"};
  return convert_enum_idx(names, 3, value, "srbs_to_be_setup_mod_item_ext_ies_o::ext_c::types");
}

const char* srs_res_set_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SpatialRelationPerSRSResource"};
  return convert_enum_idx(names, 1, value, "srs_res_set_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<served_cell_info_ext_ies_o>;

const char* uac_plmn_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING"};
  return convert_enum_idx(names, 1, value, "uac_plmn_item_ext_ies_o::ext_c::types");
}

const char* ue_multicast_m_rbs_required_to_be_modified_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_required_to_be_modified_item_ext_ies_o::ext_c::types");
}

const char* ue_multicast_m_rbs_to_be_setup_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..512,...)"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_to_be_setup_item_ext_ies_o::ext_c::types");
}
uint8_t ue_multicast_m_rbs_to_be_setup_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_multicast_m_rbs_to_be_setup_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<paging_cell_item_ext_ies_o>;

const char* pos_meas_result_list_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NRCGI"};
  return convert_enum_idx(names, 1, value, "pos_meas_result_list_item_ext_ies_o::ext_c::types");
}

const char* private_ie_id_c::types_opts::to_string() const
{
  static const char* names[] = {"local", "global"};
  return convert_enum_idx(names, 2, value, "private_ie_id_c::types");
}

const char* res_coordination_eutra_cell_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"IgnorePRACHConfiguration"};
  return convert_enum_idx(names, 1, value, "res_coordination_eutra_cell_info_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<srbs_to_be_setup_item_ext_ies_o>;

template struct asn1::protocol_ext_field_s<srbs_to_be_setup_mod_item_ext_ies_o>;

const char* semipersistent_srs_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SpatialRelationPerSRSResource"};
  return convert_enum_idx(names, 1, value, "semipersistent_srs_ext_ies_o::ext_c::types");
}

const char* trp_meas_request_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NRCGI", "AoA-AssistanceInfo", "NumberOfTRPRxTEG", "NumberOfTRPRxTxTEG"};
  return convert_enum_idx(names, 4, value, "trp_meas_request_item_ext_ies_o::ext_c::types");
}

const char* trp_meas_upd_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NumberOfTRPRxTEG", "NumberOfTRPRxTxTEG"};
  return convert_enum_idx(names, 2, value, "trp_meas_upd_item_ext_ies_o::ext_c::types");
}

const char* ue_associated_lc_f1_conn_item_res_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-associatedLogicalF1-ConnectionItem"};
  return convert_enum_idx(names, 1, value, "ue_associated_lc_f1_conn_item_res_o::value_c::types");
}
uint8_t ue_associated_lc_f1_conn_item_res_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_associated_lc_f1_conn_item_res_o::value_c::types");
}

const char* gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-CU-TNL-Association-Failed-To-Setup-Item"};
  return convert_enum_idx(names, 1, value, "gnb_cu_tnl_assoc_failed_to_setup_item_ies_o::value_c::types");
}

const char* gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-CU-TNL-Association-Setup-Item"};
  return convert_enum_idx(names, 1, value, "gnb_cu_tnl_assoc_setup_item_ies_o::value_c::types");
}

const char* gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-CU-TNL-Association-To-Add-Item"};
  return convert_enum_idx(names, 1, value, "gnb_cu_tnl_assoc_to_add_item_ies_o::value_c::types");
}

const char* gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-CU-TNL-Association-To-Remove-Item"};
  return convert_enum_idx(names, 1, value, "gnb_cu_tnl_assoc_to_rem_item_ies_o::value_c::types");
}

const char* gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-CU-TNL-Association-To-Update-Item"};
  return convert_enum_idx(names, 1, value, "gnb_cu_tnl_assoc_to_upd_item_ies_o::value_c::types");
}

const char* gnb_du_served_cells_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-DU-Served-Cells-Item"};
  return convert_enum_idx(names, 1, value, "gnb_du_served_cells_item_ies_o::value_c::types");
}

const char* gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GNB-DU-TNL-Association-To-Remove-Item"};
  return convert_enum_idx(names, 1, value, "gnb_du_tnl_assoc_to_rem_item_ies_o::value_c::types");
}

const char* iab_allocated_tnl_address_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"IAB-Allocated-TNL-Address-Item"};
  return convert_enum_idx(names, 1, value, "iab_allocated_tnl_address_list_item_ies_o::value_c::types");
}

const char* iab_tnl_addresses_to_rem_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"IAB-TNL-Addresses-To-Remove-Item"};
  return convert_enum_idx(names, 1, value, "iab_tnl_addresses_to_rem_item_ies_o::value_c::types");
}

const char* mc_paging_cell_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MC-PagingCell-Item"};
  return convert_enum_idx(names, 1, value, "mc_paging_cell_item_ies_o::value_c::types");
}

const char* multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastF1UContext-FailedToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types");
}
uint8_t multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "multicast_f1_u_context_failed_to_be_setup_item_ies_o::value_c::types");
}

const char* multicast_f1_u_context_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastF1UContext-Setup-Item"};
  return convert_enum_idx(names, 1, value, "multicast_f1_u_context_setup_item_ies_o::value_c::types");
}
uint8_t multicast_f1_u_context_setup_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "multicast_f1_u_context_setup_item_ies_o::value_c::types");
}

const char* multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastF1UContext-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types");
}
uint8_t multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "multicast_f1_u_context_to_be_setup_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-FailedToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_failed_to_be_modified_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-FailedToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_failed_to_be_setup_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-FailedToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_failed_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-Modified-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_modified_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-Setup-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_setup_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-SetupMod-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_setup_mod_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-ToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_to_be_modified_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_to_be_released_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_to_be_setup_item_ies_o::value_c::types");
}

const char* multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MulticastMRBs-ToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "multicast_m_rbs_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* nr_cgi_list_for_restart_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"NR-CGI-List-For-Restart-Item"};
  return convert_enum_idx(names, 1, value, "nr_cgi_list_for_restart_list_item_ies_o::value_c::types");
}

const char* neighbour_cell_info_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Neighbour-Cell-Information-Item"};
  return convert_enum_idx(names, 1, value, "neighbour_cell_info_item_ies_o::value_c::types");
}

const char* pdc_meas_quantities_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDCMeasurementQuantities-Item"};
  return convert_enum_idx(names, 1, value, "pdc_meas_quantities_item_ies_o::value_c::types");
}

const char* pws_failed_nr_cgi_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PWS-Failed-NR-CGI-Item"};
  return convert_enum_idx(names, 1, value, "pws_failed_nr_cgi_list_item_ies_o::value_c::types");
}

const char* pws_sys_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NotificationInformation", "AdditionalSIBMessageList"};
  return convert_enum_idx(names, 2, value, "pws_sys_info_ext_ies_o::ext_c::types");
}

const char* paging_cell_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PagingCell-Item"};
  return convert_enum_idx(names, 1, value, "paging_cell_item_ies_o::value_c::types");
}

const char* potential_sp_cell_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Potential-SpCell-Item"};
  return convert_enum_idx(names, 1, value, "potential_sp_cell_item_ies_o::value_c::types");
}

const char* protected_eutra_res_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Protected-EUTRA-Resources-Item"};
  return convert_enum_idx(names, 1, value, "protected_eutra_res_item_ies_o::value_c::types");
}

const char* rrc_version_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING"};
  return convert_enum_idx(names, 1, value, "rrc_version_ext_ies_o::ext_c::types");
}

const char* requested_srs_tx_characteristics_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..3279165)"};
  return convert_enum_idx(names, 1, value, "requested_srs_tx_characteristics_ext_ies_o::ext_c::types");
}
uint8_t requested_srs_tx_characteristics_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "requested_srs_tx_characteristics_ext_ies_o::ext_c::types");
}

const char* scell_failedto_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SCell-FailedtoSetup-Item"};
  return convert_enum_idx(names, 1, value, "scell_failedto_setup_item_ies_o::value_c::types");
}

const char* scell_failedto_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SCell-FailedtoSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "scell_failedto_setup_mod_item_ies_o::value_c::types");
}

const char* scell_to_be_remd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SCell-ToBeRemoved-Item"};
  return convert_enum_idx(names, 1, value, "scell_to_be_remd_item_ies_o::value_c::types");
}

const char* scell_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SCell-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "scell_to_be_setup_item_ies_o::value_c::types");
}

const char* scell_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SCell-ToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "scell_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* sl_drbs_failed_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-FailedToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_failed_to_be_modified_item_ies_o::value_c::types");
}

const char* sl_drbs_failed_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-FailedToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_failed_to_be_setup_item_ies_o::value_c::types");
}

const char* sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-FailedToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_failed_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* sl_drbs_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-Modified-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_modified_item_ies_o::value_c::types");
}

const char* sl_drbs_modified_conf_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-ModifiedConf-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_modified_conf_item_ies_o::value_c::types");
}

const char* sl_drbs_required_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-Required-ToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_required_to_be_modified_item_ies_o::value_c::types");
}

const char* sl_drbs_required_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-Required-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_required_to_be_released_item_ies_o::value_c::types");
}

const char* sl_drbs_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-Setup-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_setup_item_ies_o::value_c::types");
}

const char* sl_drbs_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-SetupMod-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_setup_mod_item_ies_o::value_c::types");
}

const char* sl_drbs_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-ToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_to_be_modified_item_ies_o::value_c::types");
}

const char* sl_drbs_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_to_be_released_item_ies_o::value_c::types");
}

const char* sl_drbs_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_to_be_setup_item_ies_o::value_c::types");
}

const char* sl_drbs_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SLDRBs-ToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "sl_drbs_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* srbs_failed_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-FailedToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "srbs_failed_to_be_setup_item_ies_o::value_c::types");
}

const char* srbs_failed_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-FailedToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "srbs_failed_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* srbs_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-Modified-Item"};
  return convert_enum_idx(names, 1, value, "srbs_modified_item_ies_o::value_c::types");
}

const char* srbs_required_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-Required-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "srbs_required_to_be_released_item_ies_o::value_c::types");
}

const char* srbs_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-Setup-Item"};
  return convert_enum_idx(names, 1, value, "srbs_setup_item_ies_o::value_c::types");
}

const char* srbs_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-SetupMod-Item"};
  return convert_enum_idx(names, 1, value, "srbs_setup_mod_item_ies_o::value_c::types");
}

const char* srbs_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "srbs_to_be_released_item_ies_o::value_c::types");
}

const char* srbs_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "srbs_to_be_setup_item_ies_o::value_c::types");
}

const char* srbs_to_be_setup_mod_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SRBs-ToBeSetupMod-Item"};
  return convert_enum_idx(names, 1, value, "srbs_to_be_setup_mod_item_ies_o::value_c::types");
}

const char* served_cells_to_add_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Served-Cells-To-Add-Item"};
  return convert_enum_idx(names, 1, value, "served_cells_to_add_item_ies_o::value_c::types");
}

const char* served_cells_to_delete_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Served-Cells-To-Delete-Item"};
  return convert_enum_idx(names, 1, value, "served_cells_to_delete_item_ies_o::value_c::types");
}

const char* served_cells_to_modify_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Served-Cells-To-Modify-Item"};
  return convert_enum_idx(names, 1, value, "served_cells_to_modify_item_ies_o::value_c::types");
}

const char* serving_cell_mo_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"ServingCellMO-List-Item"};
  return convert_enum_idx(names, 1, value, "serving_cell_mo_list_item_ies_o::value_c::types");
}

template struct asn1::protocol_ext_field_s<trp_meas_request_item_ext_ies_o>;

template struct asn1::protocol_ext_field_s<trp_meas_upd_item_ext_ies_o>;

const char* trp_info_item_trp_resp_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"TRPInformationItem"};
  return convert_enum_idx(names, 1, value, "trp_info_item_trp_resp_o::value_c::types");
}

const char* trp_info_type_item_trp_req_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"TRPInformationTypeItem"};
  return convert_enum_idx(names, 1, value, "trp_info_type_item_trp_req_o::value_c::types");
}

const char* trace_activation_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"MDTConfiguration", "VisibleString"};
  return convert_enum_idx(names, 2, value, "trace_activation_ext_ies_o::ext_c::types");
}

const char* ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-MulticastMRBs-ConfirmedToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o::value_c::types");
}

const char* ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-MulticastMRBs-RequiredToBeModified-Item"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_required_to_be_modified_item_ies_o::value_c::types");
}

const char* ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-MulticastMRBs-RequiredToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_required_to_be_released_item_ies_o::value_c::types");
}

const char* ue_multicast_m_rbs_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-MulticastMRBs-Setup-Item"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_setup_item_ies_o::value_c::types");
}

const char* ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-MulticastMRBs-ToBeReleased-Item"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_to_be_released_item_ies_o::value_c::types");
}

const char* ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-MulticastMRBs-ToBeSetup-Item"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_to_be_setup_item_ies_o::value_c::types");
}

const char* ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-MulticastMRBs-ToBeSetup-atModify-Item"};
  return convert_enum_idx(names, 1, value, "ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o::value_c::types");
}

const char* ue_associated_lc_f1_conn_item_res_ack_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-associatedLogicalF1-ConnectionItem"};
  return convert_enum_idx(names, 1, value, "ue_associated_lc_f1_conn_item_res_ack_o::value_c::types");
}
uint8_t ue_associated_lc_f1_conn_item_res_ack_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_associated_lc_f1_conn_item_res_ack_o::value_c::types");
}

const char* ue_id_list_for_paging_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UEIdentity-List-For-Paging-Item"};
  return convert_enum_idx(names, 1, value, "ue_id_list_for_paging_item_ies_o::value_c::types");
}

const char* ue_paging_cap_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"RedCapIndication"};
  return convert_enum_idx(names, 1, value, "ue_paging_cap_ext_ies_o::ext_c::types");
}

const char* ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UL-UP-TNL-Address-to-Update-List-Item"};
  return convert_enum_idx(names, 1, value, "ul_up_tnl_address_to_upd_list_item_ies_o::value_c::types");
}

const char* ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UL-UP-TNL-Information-to-Update-List-Item"};
  return convert_enum_idx(names, 1, value, "ul_up_tnl_info_to_upd_list_item_ies_o::value_c::types");
}

template struct asn1::protocol_ext_field_s<pws_sys_info_ext_ies_o>;

template struct asn1::protocol_ext_field_s<trace_activation_ext_ies_o>;

const char* f1_removal_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "f1_removal_fail_ies_o::value_c::types");
}
uint8_t f1_removal_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "f1_removal_fail_ies_o::value_c::types");
}

const char* f1_removal_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)"};
  return convert_enum_idx(names, 1, value, "f1_removal_request_ies_o::value_c::types");
}
uint8_t f1_removal_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "f1_removal_request_ies_o::value_c::types");
}

const char* f1_removal_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "f1_removal_resp_ies_o::value_c::types");
}
uint8_t f1_removal_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "f1_removal_resp_ies_o::value_c::types");
}

const char* f1_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "f1_setup_fail_ies_o::value_c::types");
}
uint8_t f1_setup_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "f1_setup_fail_ies_o::value_c::types");
}

const char* f1_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (0..68719476735)",
                                "PrintableString",
                                "GNB-DU-Served-Cells-List",
                                "RRC-Version",
                                "Transport-Layer-Address-Info",
                                "BIT STRING",
                                "Extended-GNB-DU-Name"};
  return convert_enum_idx(names, 8, value, "f1_setup_request_ies_o::value_c::types");
}

const char* f1_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "PrintableString",
                                "Cells-to-be-Activated-List",
                                "RRC-Version",
                                "Transport-Layer-Address-Info",
                                "UL-BH-Non-UP-Traffic-Mapping",
                                "BIT STRING",
                                "Extended-GNB-CU-Name"};
  return convert_enum_idx(names, 8, value, "f1_setup_resp_ies_o::value_c::types");
}
uint8_t f1_setup_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "f1_setup_resp_ies_o::value_c::types");
}

const char* gnb_cu_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "Cells-Failed-to-be-Activated-List",
                                "CriticalityDiagnostics",
                                "GNB-CU-TNL-Association-Setup-List",
                                "GNB-CU-TNL-Association-Failed-To-Setup-List",
                                "Dedicated-SIDelivery-NeededUE-List",
                                "Transport-Layer-Address-Info"};
  return convert_enum_idx(names, 7, value, "gnb_cu_cfg_upd_ack_ies_o::value_c::types");
}
uint8_t gnb_cu_cfg_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cfg_upd_ack_ies_o::value_c::types");
}

const char* gnb_cu_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "gnb_cu_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t gnb_cu_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cfg_upd_fail_ies_o::value_c::types");
}

const char* gnb_cu_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "Cells-to-be-Activated-List",
                                "Cells-to-be-Deactivated-List",
                                "GNB-CU-TNL-Association-To-Add-List",
                                "GNB-CU-TNL-Association-To-Remove-List",
                                "GNB-CU-TNL-Association-To-Update-List",
                                "Cells-to-be-Barred-List",
                                "Protected-EUTRA-Resources-List",
                                "Neighbour-Cell-Information-List",
                                "Transport-Layer-Address-Info",
                                "UL-BH-Non-UP-Traffic-Mapping",
                                "BIT STRING",
                                "CCO-Assistance-Information",
                                "CellsForSON-List",
                                "PrintableString",
                                "Extended-GNB-CU-Name"};
  return convert_enum_idx(names, 16, value, "gnb_cu_cfg_upd_ies_o::value_c::types");
}
uint8_t gnb_cu_cfg_upd_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_cu_cfg_upd_ies_o::value_c::types");
}

const char* gnb_du_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "Cells-to-be-Activated-List",
                                "CriticalityDiagnostics",
                                "Cells-to-be-Deactivated-List",
                                "Transport-Layer-Address-Info",
                                "UL-BH-Non-UP-Traffic-Mapping",
                                "BIT STRING",
                                "CellsForSON-List"};
  return convert_enum_idx(names, 8, value, "gnb_du_cfg_upd_ack_ies_o::value_c::types");
}
uint8_t gnb_du_cfg_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_cfg_upd_ack_ies_o::value_c::types");
}

const char* gnb_du_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "gnb_du_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t gnb_du_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_cfg_upd_fail_ies_o::value_c::types");
}

const char* gnb_du_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "Served-Cells-To-Add-List",
                                "Served-Cells-To-Modify-List",
                                "Served-Cells-To-Delete-List",
                                "Cells-Status-List",
                                "Dedicated-SIDelivery-NeededUE-List",
                                "INTEGER (0..68719476735)",
                                "GNB-DU-TNL-Association-To-Remove-List",
                                "Transport-Layer-Address-Info",
                                "Coverage-Modification-Notification",
                                "PrintableString",
                                "Extended-GNB-DU-Name"};
  return convert_enum_idx(names, 12, value, "gnb_du_cfg_upd_ies_o::value_c::types");
}

const char* gnb_du_res_cfg_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "gnb_du_res_cfg_ack_ies_o::value_c::types");
}
uint8_t gnb_du_res_cfg_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_res_cfg_ack_ies_o::value_c::types");
}

const char* gnb_du_res_cfg_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "gnb_du_res_cfg_fail_ies_o::value_c::types");
}
uint8_t gnb_du_res_cfg_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_res_cfg_fail_ies_o::value_c::types");
}

const char* gnb_du_res_cfg_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "Activated-Cells-to-be-Updated-List",
                                "Child-Nodes-List",
                                "Neighbour-Node-Cells-List",
                                "Serving-Cells-List"};
  return convert_enum_idx(names, 5, value, "gnb_du_res_cfg_ies_o::value_c::types");
}
uint8_t gnb_du_res_cfg_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_res_cfg_ies_o::value_c::types");
}

const char* gnb_du_res_coordination_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "RequestType", "OCTET STRING", "IgnoreResourceCoordinationContainer"};
  return convert_enum_idx(names, 4, value, "gnb_du_res_coordination_request_ies_o::value_c::types");
}
uint8_t gnb_du_res_coordination_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_res_coordination_request_ies_o::value_c::types");
}

const char* gnb_du_res_coordination_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "OCTET STRING"};
  return convert_enum_idx(names, 2, value, "gnb_du_res_coordination_resp_ies_o::value_c::types");
}
uint8_t gnb_du_res_coordination_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_res_coordination_resp_ies_o::value_c::types");
}

const char* gnb_du_status_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "GNBDUOverloadInformation", "IABCongestionIndication"};
  return convert_enum_idx(names, 3, value, "gnb_du_status_ind_ies_o::value_c::types");
}
uint8_t gnb_du_status_ind_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "gnb_du_status_ind_ies_o::value_c::types");
}

const char* iab_tnl_address_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "iab_tnl_address_fail_ies_o::value_c::types");
}
uint8_t iab_tnl_address_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_tnl_address_fail_ies_o::value_c::types");
}

const char* iab_tnl_address_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "IABv4AddressesRequested",
                                "IABIPv6RequestType",
                                "IAB-TNL-Addresses-To-Remove-List",
                                "IAB-TNL-Addresses-Exception"};
  return convert_enum_idx(names, 5, value, "iab_tnl_address_request_ies_o::value_c::types");
}
uint8_t iab_tnl_address_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 4, 6};
  return map_enum_number(numbers, 3, value, "iab_tnl_address_request_ies_o::value_c::types");
}

const char* iab_tnl_address_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "IAB-Allocated-TNL-Address-List"};
  return convert_enum_idx(names, 2, value, "iab_tnl_address_resp_ies_o::value_c::types");
}
uint8_t iab_tnl_address_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_tnl_address_resp_ies_o::value_c::types");
}

const char* iab_up_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "iab_up_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t iab_up_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_up_cfg_upd_fail_ies_o::value_c::types");
}

const char* iab_up_cfg_upd_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "UL-UP-TNL-Information-to-Update-List", "UL-UP-TNL-Address-to-Update-List"};
  return convert_enum_idx(names, 3, value, "iab_up_cfg_upd_request_ies_o::value_c::types");
}
uint8_t iab_up_cfg_upd_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_up_cfg_upd_request_ies_o::value_c::types");
}

const char* iab_up_cfg_upd_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics", "DL-UP-TNL-Address-to-Update-List"};
  return convert_enum_idx(names, 3, value, "iab_up_cfg_upd_resp_ies_o::value_c::types");
}
uint8_t iab_up_cfg_upd_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "iab_up_cfg_upd_resp_ies_o::value_c::types");
}

const char* init_ul_rrc_msg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "NRCGI",
                                "INTEGER (0..65535,...)",
                                "OCTET STRING",
                                "OCTET STRING",
                                "SULAccessIndication",
                                "INTEGER (0..255,...)",
                                "OCTET STRING",
                                "OCTET STRING",
                                "NRRedCapUEIndication",
                                "SDTInformation",
                                "SidelinkRelayConfiguration"};
  return convert_enum_idx(names, 12, value, "init_ul_rrc_msg_transfer_ies_o::value_c::types");
}

const char* meas_activation_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "ActivationRequestType", "PRS-Measurement-Info-List"};
  return convert_enum_idx(names, 4, value, "meas_activation_ies_o::value_c::types");
}

const char* meas_precfg_confirm_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "PosMeasGapPreConfigList", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "meas_precfg_confirm_ies_o::value_c::types");
}

const char* meas_precfg_refuse_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "meas_precfg_refuse_ies_o::value_c::types");
}

const char* meas_precfg_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "TRP-PRS-Info-List"};
  return convert_enum_idx(names, 3, value, "meas_precfg_required_ies_o::value_c::types");
}

const char* multicast_context_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "multicast_context_mod_fail_ies_o::value_c::types");
}

const char* multicast_context_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MBS-ServiceArea",
                                "MulticastMRBs-ToBeSetupMod-List",
                                "MulticastMRBs-ToBeModified-List",
                                "MulticastMRBs-ToBeReleased-List"};
  return convert_enum_idx(names, 6, value, "multicast_context_mod_request_ies_o::value_c::types");
}

const char* multicast_context_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MulticastMRBs-SetupMod-List",
                                "MulticastMRBs-FailedToBeSetupMod-List",
                                "MulticastMRBs-Modified-List",
                                "MulticastMRBs-FailedToBeModified-List",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 7, value, "multicast_context_mod_resp_ies_o::value_c::types");
}

const char* multicast_context_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(names, 3, value, "multicast_context_release_cmd_ies_o::value_c::types");
}

const char* multicast_context_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "multicast_context_release_complete_ies_o::value_c::types");
}

const char* multicast_context_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(names, 3, value, "multicast_context_release_request_ies_o::value_c::types");
}

const char* multicast_context_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "multicast_context_setup_fail_ies_o::value_c::types");
}

const char* multicast_context_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "MBS-Session-ID", "MBS-ServiceArea", "SNSSAI", "MulticastMRBs-ToBeSetup-List"};
  return convert_enum_idx(names, 5, value, "multicast_context_setup_request_ies_o::value_c::types");
}
uint8_t multicast_context_setup_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "multicast_context_setup_request_ies_o::value_c::types");
}

const char* multicast_context_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MulticastMRBs-Setup-List",
                                "MulticastMRBs-FailedToBeSetup-List",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "multicast_context_setup_resp_ies_o::value_c::types");
}

const char* multicast_distribution_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "MBSMulticastF1UContextDescriptor", "Cause"};
  return convert_enum_idx(names, 4, value, "multicast_distribution_release_cmd_ies_o::value_c::types");
}

const char* multicast_distribution_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MBSMulticastF1UContextDescriptor",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "multicast_distribution_release_complete_ies_o::value_c::types");
}

const char* multicast_distribution_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MBSMulticastF1UContextDescriptor",
                                "Cause",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "multicast_distribution_setup_fail_ies_o::value_c::types");
}

const char* multicast_distribution_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MBSMulticastF1UContextDescriptor",
                                "MulticastF1UContext-ToBeSetup-List"};
  return convert_enum_idx(names, 4, value, "multicast_distribution_setup_request_ies_o::value_c::types");
}

const char* multicast_distribution_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "MBSMulticastF1UContextDescriptor",
                                "MulticastF1UContext-Setup-List",
                                "MulticastF1UContext-FailedToBeSetup-List",
                                "CriticalityDiagnostics",
                                "OCTET STRING"};
  return convert_enum_idx(names, 7, value, "multicast_distribution_setup_resp_ies_o::value_c::types");
}

const char* multicast_group_paging_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-Session-ID", "UEIdentity-List-For-Paging-List", "MC-PagingCell-list"};
  return convert_enum_idx(names, 3, value, "multicast_group_paging_ies_o::value_c::types");
}

const char* network_access_rate_reduction_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "UAC-Assistance-Info"};
  return convert_enum_idx(names, 2, value, "network_access_rate_reduction_ies_o::value_c::types");
}
uint8_t network_access_rate_reduction_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "network_access_rate_reduction_ies_o::value_c::types");
}

const char* notify_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "DRB-Notify-List"};
  return convert_enum_idx(names, 3, value, "notify_ies_o::value_c::types");
}

const char* pdc_meas_fail_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (1..16,...)", "Cause"};
  return convert_enum_idx(names, 4, value, "pdc_meas_fail_ind_ies_o::value_c::types");
}

const char* pdc_meas_initiation_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (1..16,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "pdc_meas_initiation_fail_ies_o::value_c::types");
}

const char* pdc_meas_initiation_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..16,...)",
                                "PDCReportType",
                                "PDCMeasurementPeriodicity",
                                "PDCMeasurementQuantities"};
  return convert_enum_idx(names, 6, value, "pdc_meas_initiation_request_ies_o::value_c::types");
}

const char* pdc_meas_initiation_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..16,...)",
                                "PDCMeasurementResult",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "pdc_meas_initiation_resp_ies_o::value_c::types");
}

const char* pdc_meas_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (1..16,...)", "PDCMeasurementResult"};
  return convert_enum_idx(names, 4, value, "pdc_meas_report_ies_o::value_c::types");
}

const char* pdc_meas_termination_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "INTEGER (1..16,...)"};
  return convert_enum_idx(names, 3, value, "pdc_meas_termination_cmd_ies_o::value_c::types");
}

const char* prs_cfg_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "prs_cfg_fail_ies_o::value_c::types");
}
uint8_t prs_cfg_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "prs_cfg_fail_ies_o::value_c::types");
}

const char* prs_cfg_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PRSConfigRequestType", "PRSTRPList"};
  return convert_enum_idx(names, 2, value, "prs_cfg_request_ies_o::value_c::types");
}

const char* prs_cfg_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "PRSTransmissionTRPList", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "prs_cfg_resp_ies_o::value_c::types");
}
uint8_t prs_cfg_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "prs_cfg_resp_ies_o::value_c::types");
}

const char* pws_cancel_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (0..65535)",
                                "Broadcast-To-Be-Cancelled-List",
                                "Cancel-all-Warning-Messages-Indicator",
                                "NotificationInformation"};
  return convert_enum_idx(names, 5, value, "pws_cancel_request_ies_o::value_c::types");
}

const char* pws_cancel_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cells-Broadcast-Cancelled-List", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "pws_cancel_resp_ies_o::value_c::types");
}
uint8_t pws_cancel_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "pws_cancel_resp_ies_o::value_c::types");
}

const char* pws_fail_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "PWS-Failed-NR-CGI-List"};
  return convert_enum_idx(names, 2, value, "pws_fail_ind_ies_o::value_c::types");
}
uint8_t pws_fail_ind_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "pws_fail_ind_ies_o::value_c::types");
}

const char* pws_restart_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "NR-CGI-List-For-Restart-List"};
  return convert_enum_idx(names, 2, value, "pws_restart_ind_ies_o::value_c::types");
}
uint8_t pws_restart_ind_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "pws_restart_ind_ies_o::value_c::types");
}

const char* paging_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UEIdentityIndexValue",
                                "PagingIdentity",
                                "PagingDRX",
                                "PagingPriority",
                                "PagingCell-list",
                                "PagingOrigin",
                                "PagingDRX",
                                "PagingDRX",
                                "NRPagingeDRXInformation",
                                "NRPagingeDRXInformationforRRCINACTIVE",
                                "PagingCause",
                                "PEIPSAssistanceInfo",
                                "UEPagingCapability",
                                "BIT STRING"};
  return convert_enum_idx(names, 14, value, "paging_ies_o::value_c::types");
}

const char* pos_sys_info_delivery_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "NRCGI", "PosSItypeList", "INTEGER (0..4294967295)"};
  return convert_enum_idx(names, 4, value, "pos_sys_info_delivery_cmd_ies_o::value_c::types");
}

const char* positioning_activation_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "positioning_activation_fail_ies_o::value_c::types");
}

const char* positioning_activation_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "SRSType", "BIT STRING"};
  return convert_enum_idx(names, 4, value, "positioning_activation_request_ies_o::value_c::types");
}

const char* positioning_activation_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (0..1023)",
                                "INTEGER (0..79)",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "positioning_activation_resp_ies_o::value_c::types");
}

const char* positioning_assist_info_ctrl_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "OCTET STRING", "PosBroadcast", "PositioningBroadcastCells", "OCTET STRING"};
  return convert_enum_idx(names, 5, value, "positioning_assist_info_ctrl_ies_o::value_c::types");
}
uint8_t positioning_assist_info_ctrl_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "positioning_assist_info_ctrl_ies_o::value_c::types");
}

const char* positioning_assist_info_feedback_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "OCTET STRING", "PositioningBroadcastCells", "OCTET STRING", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "positioning_assist_info_feedback_ies_o::value_c::types");
}
uint8_t positioning_assist_info_feedback_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "positioning_assist_info_feedback_ies_o::value_c::types");
}

const char* positioning_deactivation_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "AbortTransmission"};
  return convert_enum_idx(names, 3, value, "positioning_deactivation_ies_o::value_c::types");
}

const char* positioning_info_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "positioning_info_fail_ies_o::value_c::types");
}

const char* positioning_info_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "RequestedSRSTransmissionCharacteristics",
                                "UEReportingInformation",
                                "SRSPosRRCInactiveQueryIndication"};
  return convert_enum_idx(names, 5, value, "positioning_info_request_ies_o::value_c::types");
}

const char* positioning_info_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "SRSConfig",
                                "BIT STRING",
                                "CriticalityDiagnostics",
                                "OCTET STRING"};
  return convert_enum_idx(names, 6, value, "positioning_info_resp_ies_o::value_c::types");
}

const char* positioning_info_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "SRSConfig", "BIT STRING"};
  return convert_enum_idx(names, 4, value, "positioning_info_upd_ies_o::value_c::types");
}

const char* positioning_meas_abort_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "INTEGER (1..65536,...)", "INTEGER (1..65536,...)"};
  return convert_enum_idx(names, 3, value, "positioning_meas_abort_ies_o::value_c::types");
}

const char* positioning_meas_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "INTEGER (1..65536,...)", "INTEGER (1..65536,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "positioning_meas_fail_ies_o::value_c::types");
}

const char* positioning_meas_fail_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "INTEGER (1..65536,...)", "INTEGER (1..65536,...)", "Cause"};
  return convert_enum_idx(names, 4, value, "positioning_meas_fail_ind_ies_o::value_c::types");
}

const char* positioning_meas_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "INTEGER (1..65536,...)", "INTEGER (1..65536,...)", "PosMeasurementResultList"};
  return convert_enum_idx(names, 4, value, "positioning_meas_report_ies_o::value_c::types");
}

const char* positioning_meas_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (1..65536,...)",
                                "INTEGER (1..65536,...)",
                                "TRP-MeasurementRequestList",
                                "PosReportCharacteristics",
                                "MeasurementPeriodicity",
                                "PosMeasurementQuantities",
                                "BIT STRING",
                                "SRSConfig",
                                "MeasurementBeamInfoRequest",
                                "INTEGER (0..1023)",
                                "INTEGER (0..79)",
                                "MeasurementPeriodicityExtended",
                                "ResponseTime",
                                "BIT STRING",
                                "MeasurementTimeOccasion",
                                "PosMeasurementAmount"};
  return convert_enum_idx(names, 17, value, "positioning_meas_request_ies_o::value_c::types");
}

const char* positioning_meas_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (1..65536,...)",
                                "INTEGER (1..65536,...)",
                                "PosMeasurementResultList",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "positioning_meas_resp_ies_o::value_c::types");
}

const char* positioning_meas_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (1..65536,...)",
                                "INTEGER (1..65536,...)",
                                "SRSConfig",
                                "TRP-MeasurementUpdateList",
                                "BIT STRING",
                                "MeasurementTimeOccasion"};
  return convert_enum_idx(names, 7, value, "positioning_meas_upd_ies_o::value_c::types");
}

const char* f1ap_private_ies_empty_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {};
  return convert_enum_idx(names, 0, value, "f1ap_private_ies_empty_o::value_c::types");
}

const char* qo_e_info_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "QoEInformation"};
  return convert_enum_idx(names, 3, value, "qo_e_info_transfer_ies_o::value_c::types");
}

const char* rrc_delivery_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "RRCDeliveryStatus", "INTEGER (0..3,...)"};
  return convert_enum_idx(names, 4, value, "rrc_delivery_report_ies_o::value_c::types");
}

const char* ref_time_info_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "TimeReferenceInformation"};
  return convert_enum_idx(names, 2, value, "ref_time_info_report_ies_o::value_c::types");
}
uint8_t ref_time_info_report_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ref_time_info_report_ies_o::value_c::types");
}

const char* ref_time_info_report_ctrl_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "ReportingRequestType"};
  return convert_enum_idx(names, 2, value, "ref_time_info_report_ctrl_ies_o::value_c::types");
}
uint8_t ref_time_info_report_ctrl_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ref_time_info_report_ctrl_ies_o::value_c::types");
}

const char* reset_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "UE-associatedLogicalF1-ConnectionListResAck", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "reset_ack_ies_o::value_c::types");
}
uint8_t reset_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "reset_ack_ies_o::value_c::types");
}

const char* reset_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "ResetType"};
  return convert_enum_idx(names, 3, value, "reset_ies_o::value_c::types");
}
uint8_t reset_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "reset_ies_o::value_c::types");
}

const char* res_status_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "INTEGER (0..4095,...)", "INTEGER (0..4095,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "res_status_fail_ies_o::value_c::types");
}

const char* res_status_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (0..4095,...)",
                                "INTEGER (0..4095,...)",
                                "RegistrationRequest",
                                "BIT STRING",
                                "CellToReportList",
                                "ReportingPeriodicity"};
  return convert_enum_idx(names, 7, value, "res_status_request_ies_o::value_c::types");
}

const char* res_status_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..255,...)", "INTEGER (0..4095,...)", "INTEGER (0..4095,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "res_status_resp_ies_o::value_c::types");
}

const char* res_status_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "INTEGER (0..4095,...)",
                                "INTEGER (0..4095,...)",
                                "HardwareLoadIndicator",
                                "TNLCapacityIndicator",
                                "CellMeasurementResultList"};
  return convert_enum_idx(names, 6, value, "res_status_upd_ies_o::value_c::types");
}

const char* sys_info_delivery_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "NRCGI", "SItype-List", "INTEGER (0..4294967295)"};
  return convert_enum_idx(names, 4, value, "sys_info_delivery_cmd_ies_o::value_c::types");
}

const char* trp_info_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "trp_info_fail_ies_o::value_c::types");
}
uint8_t trp_info_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "trp_info_fail_ies_o::value_c::types");
}

const char* trp_info_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "TRPList", "TRPInformationTypeListTRPReq"};
  return convert_enum_idx(names, 3, value, "trp_info_request_ies_o::value_c::types");
}
uint8_t trp_info_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "trp_info_request_ies_o::value_c::types");
}

const char* trp_info_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)", "TRPInformationListTRPResp", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "trp_info_resp_ies_o::value_c::types");
}
uint8_t trp_info_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "trp_info_resp_ies_o::value_c::types");
}

const char* trace_start_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "TraceActivation"};
  return convert_enum_idx(names, 3, value, "trace_start_ies_o::value_c::types");
}

const char* ue_context_mod_confirm_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "DRBs-ModifiedConf-List",
                                "OCTET STRING",
                                "CriticalityDiagnostics",
                                "ExecuteDuplication",
                                "ResourceCoordinationTransferInformation",
                                "SLDRBs-ModifiedConf-List",
                                "UuRLCChannelModifiedList",
                                "PC5RLCChannelModifiedList",
                                "UE-MulticastMRBs-ConfirmedToBeModified-List"};
  return convert_enum_idx(names, 12, value, "ue_context_mod_confirm_ies_o::value_c::types");
}

const char* ue_context_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics", "NRCGI"};
  return convert_enum_idx(names, 5, value, "ue_context_mod_fail_ies_o::value_c::types");
}

const char* ue_context_mod_refuse_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ue_context_mod_refuse_ies_o::value_c::types");
}

const char* ue_context_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "NRCGI",
                                "INTEGER (0..31,...)",
                                "CellULConfigured",
                                "DRXCycle",
                                "CUtoDURRCInformation",
                                "TransmissionActionIndicator",
                                "OCTET STRING",
                                "RRCReconfigurationCompleteIndicator",
                                "OCTET STRING",
                                "SCell-ToBeSetupMod-List",
                                "SCell-ToBeRemoved-List",
                                "SRBs-ToBeSetupMod-List",
                                "DRBs-ToBeSetupMod-List",
                                "DRBs-ToBeModified-List",
                                "SRBs-ToBeReleased-List",
                                "DRBs-ToBeReleased-List",
                                "InactivityMonitoringRequest",
                                "RAT-FrequencyPriorityInformation",
                                "DRXConfigurationIndicator",
                                "RLCFailureIndication",
                                "OCTET STRING",
                                "GNB-DUConfigurationQuery",
                                "INTEGER (0..4000000000000,...)",
                                "ExecuteDuplication",
                                "RRCDeliveryStatusRequest",
                                "ResourceCoordinationTransferInformation",
                                "INTEGER (1..64,...)",
                                "NeedforGap",
                                "FullConfiguration",
                                "BIT STRING",
                                "LowerLayerPresenceStatusChange",
                                "BHChannels-ToBeSetupMod-List",
                                "BHChannels-ToBeModified-List",
                                "BHChannels-ToBeReleased-List",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "SLDRBs-ToBeSetupMod-List",
                                "SLDRBs-ToBeModified-List",
                                "SLDRBs-ToBeReleased-List",
                                "ConditionalIntraDUMobilityInformation",
                                "F1CTransferPath",
                                "SCGIndicator",
                                "OCTET STRING",
                                "IABConditionalRRCMessageDeliveryIndication",
                                "F1CTransferPathNRDC",
                                "MDTPollutedMeasurementIndicator",
                                "SCGActivationRequest",
                                "CG-SDTQueryIndication",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "INTEGER (0..255,...)",
                                "UuRLCChannelToBeSetupList",
                                "UuRLCChannelToBeModifiedList",
                                "UuRLCChannelToBeReleasedList",
                                "PC5RLCChannelToBeSetupList",
                                "PC5RLCChannelToBeModifiedList",
                                "PC5RLCChannelToBeReleasedList",
                                "PathSwitchConfiguration",
                                "GNBDUUESliceMaximumBitRateList",
                                "MulticastMBSSessionList",
                                "MulticastMBSSessionList",
                                "UE-MulticastMRBs-ToBeSetup-atModify-List",
                                "UE-MulticastMRBs-ToBeReleased-List",
                                "SLDRXCycleList",
                                "MDTPLMNModificationList",
                                "SDTBearerConfigurationQueryIndication",
                                "DAPS-HO-Status",
                                "ServingCellMO-List",
                                "OCTET STRING",
                                "CPACMCGInformation"};
  return convert_enum_idx(names, 76, value, "ue_context_mod_request_ies_o::value_c::types");
}

const char* ue_context_mod_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "DUtoCURRCInformation",
                                "DRBs-Required-ToBeModified-List",
                                "SRBs-Required-ToBeReleased-List",
                                "DRBs-Required-ToBeReleased-List",
                                "Cause",
                                "BHChannels-Required-ToBeReleased-List",
                                "SLDRBs-Required-ToBeModified-List",
                                "SLDRBs-Required-ToBeReleased-List",
                                "TargetCellList",
                                "UuRLCChannelRequiredToBeModifiedList",
                                "UuRLCChannelRequiredToBeReleasedList",
                                "PC5RLCChannelRequiredToBeModifiedList",
                                "PC5RLCChannelRequiredToBeReleasedList",
                                "UE-MulticastMRBs-RequiredToBeModified-List",
                                "UE-MulticastMRBs-RequiredToBeReleased-List"};
  return convert_enum_idx(names, 18, value, "ue_context_mod_required_ies_o::value_c::types");
}

const char* ue_context_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "DUtoCURRCInformation",
                                "DRBs-SetupMod-List",
                                "DRBs-Modified-List",
                                "SRBs-FailedToBeSetupMod-List",
                                "DRBs-FailedToBeSetupMod-List",
                                "SCell-FailedtoSetupMod-List",
                                "DRBs-FailedToBeModified-List",
                                "InactivityMonitoringResponse",
                                "CriticalityDiagnostics",
                                "INTEGER (0..65535,...)",
                                "Associated-SCell-List",
                                "SRBs-SetupMod-List",
                                "SRBs-Modified-List",
                                "FullConfiguration",
                                "BHChannels-SetupMod-List",
                                "BHChannels-Modified-List",
                                "BHChannels-FailedToBeSetupMod-List",
                                "BHChannels-FailedToBeModified-List",
                                "SLDRBs-SetupMod-List",
                                "SLDRBs-Modified-List",
                                "SLDRBs-FailedToBeSetupMod-List",
                                "SLDRBs-FailedToBeModified-List",
                                "NRCGI",
                                "SCGActivationStatus",
                                "UuRLCChannelSetupList",
                                "UuRLCChannelFailedToBeSetupList",
                                "UuRLCChannelModifiedList",
                                "UuRLCChannelFailedToBeModifiedList",
                                "PC5RLCChannelSetupList",
                                "PC5RLCChannelFailedToBeSetupList",
                                "PC5RLCChannelModifiedList",
                                "PC5RLCChannelFailedToBeModifiedList",
                                "SDTBearerConfigurationInfo",
                                "UE-MulticastMRBs-Setup-List",
                                "ServingCellMO-encoded-in-CGC-List"};
  return convert_enum_idx(names, 38, value, "ue_context_mod_resp_ies_o::value_c::types");
}

const char* ue_context_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "Cause",
                                "OCTET STRING",
                                "INTEGER (0..3,...)",
                                "INTEGER (0..4294967295)",
                                "ExecuteDuplication",
                                "RRCDeliveryStatusRequest",
                                "TargetCellList",
                                "PosConextRevIndication",
                                "CG-SDTKeptIndicator"};
  return convert_enum_idx(names, 11, value, "ue_context_release_cmd_ies_o::value_c::types");
}

const char* ue_context_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "ue_context_release_complete_ies_o::value_c::types");
}

const char* ue_context_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "Cause", "TargetCellList"};
  return convert_enum_idx(names, 4, value, "ue_context_release_request_ies_o::value_c::types");
}

const char* ue_context_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "Cause",
                                "CriticalityDiagnostics",
                                "Potential-SpCell-List",
                                "NRCGI"};
  return convert_enum_idx(names, 6, value, "ue_context_setup_fail_ies_o::value_c::types");
}

const char* ue_context_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "NRCGI",
                                "INTEGER (0..31,...)",
                                "CellULConfigured",
                                "CUtoDURRCInformation",
                                "Candidate-SpCell-List",
                                "DRXCycle",
                                "OCTET STRING",
                                "SCell-ToBeSetup-List",
                                "SRBs-ToBeSetup-List",
                                "DRBs-ToBeSetup-List",
                                "InactivityMonitoringRequest",
                                "RAT-FrequencyPriorityInformation",
                                "OCTET STRING",
                                "BIT STRING",
                                "OCTET STRING",
                                "INTEGER (0..4000000000000,...)",
                                "RRCDeliveryStatusRequest",
                                "ResourceCoordinationTransferInformation",
                                "INTEGER (1..64,...)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "TraceActivation",
                                "BIT STRING",
                                "BHChannels-ToBeSetup-List",
                                "BIT STRING",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "SLDRBs-ToBeSetup-List",
                                "ConditionalInterDUMobilityInformation",
                                "MDTPLMNList",
                                "BIT STRING",
                                "F1CTransferPath",
                                "F1CTransferPathNRDC",
                                "MDTPollutedMeasurementIndicator",
                                "SCGActivationRequest",
                                "CG-SDTSessionInfo",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "INTEGER (0..4000000000000,...)",
                                "UuRLCChannelToBeSetupList",
                                "PC5RLCChannelToBeSetupList",
                                "PathSwitchConfiguration",
                                "GNBDUUESliceMaximumBitRateList",
                                "MulticastMBSSessionList",
                                "UE-MulticastMRBs-ToBeSetup-List",
                                "ServingCellMO-List"};
  return convert_enum_idx(names, 51, value, "ue_context_setup_request_ies_o::value_c::types");
}

const char* ue_context_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "DUtoCURRCInformation",
                                "INTEGER (0..65535,...)",
                                "OCTET STRING",
                                "FullConfiguration",
                                "DRBs-Setup-List",
                                "SRBs-FailedToBeSetup-List",
                                "DRBs-FailedToBeSetup-List",
                                "SCell-FailedtoSetup-List",
                                "InactivityMonitoringResponse",
                                "CriticalityDiagnostics",
                                "SRBs-Setup-List",
                                "BHChannels-Setup-List",
                                "BHChannels-FailedToBeSetup-List",
                                "SLDRBs-Setup-List",
                                "SLDRBs-FailedToBeSetup-List",
                                "NRCGI",
                                "SCGActivationStatus",
                                "UuRLCChannelSetupList",
                                "UuRLCChannelFailedToBeSetupList",
                                "PC5RLCChannelSetupList",
                                "PC5RLCChannelFailedToBeSetupList",
                                "ServingCellMO-encoded-in-CGC-List"};
  return convert_enum_idx(names, 24, value, "ue_context_setup_resp_ies_o::value_c::types");
}

const char* ue_inactivity_notif_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "INTEGER (0..4294967295)", "DRB-Activity-List", "SDT-Termination-Request"};
  return convert_enum_idx(names, 4, value, "ue_inactivity_notif_ies_o::value_c::types");
}

const char* ul_rrc_msg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (0..3,...)",
                                "OCTET STRING",
                                "OCTET STRING",
                                "INTEGER (0..4294967295)"};
  return convert_enum_idx(names, 6, value, "ul_rrc_msg_transfer_ies_o::value_c::types");
}

const char* write_replace_warning_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "PWSSystemInformation",
                                "INTEGER (0..131071,...)",
                                "INTEGER (0..65535)",
                                "Cells-To-Be-Broadcast-List"};
  return convert_enum_idx(names, 5, value, "write_replace_warning_request_ies_o::value_c::types");
}

const char* write_replace_warning_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)",
                                "Cells-Broadcast-Completed-List",
                                "CriticalityDiagnostics",
                                "Dedicated-SIDelivery-NeededUE-List"};
  return convert_enum_idx(names, 4, value, "write_replace_warning_resp_ies_o::value_c::types");
}
uint8_t write_replace_warning_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "write_replace_warning_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<f1_removal_fail_ies_o>;

template struct asn1::protocol_ie_field_s<f1_removal_resp_ies_o>;

template struct asn1::protocol_ie_field_s<f1_setup_fail_ies_o>;

template struct asn1::protocol_ie_field_s<f1_setup_request_ies_o>;

template struct asn1::protocol_ie_field_s<f1_setup_resp_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_cu_cfg_upd_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_cu_cfg_upd_ack_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_cu_cfg_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_cfg_upd_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_cfg_upd_ack_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_cfg_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_res_cfg_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_res_cfg_ack_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_res_cfg_fail_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_res_coordination_request_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_res_coordination_resp_ies_o>;

template struct asn1::protocol_ie_field_s<gnb_du_status_ind_ies_o>;

template struct asn1::protocol_ie_field_s<iab_tnl_address_fail_ies_o>;

template struct asn1::protocol_ie_field_s<iab_tnl_address_request_ies_o>;

template struct asn1::protocol_ie_field_s<iab_tnl_address_resp_ies_o>;

template struct asn1::protocol_ie_field_s<iab_up_cfg_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<iab_up_cfg_upd_request_ies_o>;

template struct asn1::protocol_ie_field_s<iab_up_cfg_upd_resp_ies_o>;

template struct asn1::protocol_ie_field_s<init_ul_rrc_msg_transfer_ies_o>;

template struct asn1::protocol_ie_field_s<meas_activation_ies_o>;

template struct asn1::protocol_ie_field_s<meas_precfg_confirm_ies_o>;

template struct asn1::protocol_ie_field_s<meas_precfg_refuse_ies_o>;

template struct asn1::protocol_ie_field_s<meas_precfg_required_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_mod_fail_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_mod_request_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_mod_resp_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_release_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_release_complete_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_release_request_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_setup_fail_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_setup_request_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_context_setup_resp_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_distribution_release_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_distribution_release_complete_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_distribution_setup_fail_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_distribution_setup_request_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_distribution_setup_resp_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_group_paging_ies_o>;

template struct asn1::protocol_ie_field_s<network_access_rate_reduction_ies_o>;

template struct asn1::protocol_ie_field_s<notify_ies_o>;

template struct asn1::protocol_ie_field_s<pdc_meas_fail_ind_ies_o>;

template struct asn1::protocol_ie_field_s<pdc_meas_initiation_fail_ies_o>;

template struct asn1::protocol_ie_field_s<pdc_meas_initiation_request_ies_o>;

template struct asn1::protocol_ie_field_s<pdc_meas_initiation_resp_ies_o>;

template struct asn1::protocol_ie_field_s<pdc_meas_report_ies_o>;

template struct asn1::protocol_ie_field_s<pdc_meas_termination_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<prs_cfg_fail_ies_o>;

template struct asn1::protocol_ie_field_s<prs_cfg_request_ies_o>;

template struct asn1::protocol_ie_field_s<prs_cfg_resp_ies_o>;

template struct asn1::protocol_ie_field_s<pws_cancel_request_ies_o>;

template struct asn1::protocol_ie_field_s<pws_cancel_resp_ies_o>;

template struct asn1::protocol_ie_field_s<pws_fail_ind_ies_o>;

template struct asn1::protocol_ie_field_s<pws_restart_ind_ies_o>;

template struct asn1::protocol_ie_field_s<paging_ies_o>;

template struct asn1::protocol_ie_field_s<pos_sys_info_delivery_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_activation_fail_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_activation_request_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_activation_resp_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_assist_info_ctrl_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_assist_info_feedback_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_deactivation_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_info_fail_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_info_request_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_info_resp_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_info_upd_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_meas_abort_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_meas_fail_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_meas_fail_ind_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_meas_report_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_meas_request_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_meas_resp_ies_o>;

template struct asn1::protocol_ie_field_s<positioning_meas_upd_ies_o>;

template struct asn1::protocol_ie_field_s<qo_e_info_transfer_ies_o>;

template struct asn1::protocol_ie_field_s<rrc_delivery_report_ies_o>;

template struct asn1::protocol_ie_field_s<ref_time_info_report_ies_o>;

template struct asn1::protocol_ie_field_s<ref_time_info_report_ctrl_ies_o>;

template struct asn1::protocol_ie_field_s<reset_ies_o>;

template struct asn1::protocol_ie_field_s<reset_ack_ies_o>;

template struct asn1::protocol_ie_field_s<res_status_fail_ies_o>;

template struct asn1::protocol_ie_field_s<res_status_request_ies_o>;

template struct asn1::protocol_ie_field_s<res_status_resp_ies_o>;

template struct asn1::protocol_ie_field_s<res_status_upd_ies_o>;

template struct asn1::protocol_ie_field_s<sys_info_delivery_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<trp_info_fail_ies_o>;

template struct asn1::protocol_ie_field_s<trp_info_request_ies_o>;

template struct asn1::protocol_ie_field_s<trp_info_resp_ies_o>;

template struct asn1::protocol_ie_field_s<trace_start_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_confirm_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_refuse_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_required_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_release_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_release_complete_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_release_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_setup_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_setup_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_setup_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ue_inactivity_notif_ies_o>;

template struct asn1::protocol_ie_field_s<ul_rrc_msg_transfer_ies_o>;

template struct asn1::protocol_ie_field_s<write_replace_warning_request_ies_o>;

template struct asn1::protocol_ie_field_s<write_replace_warning_resp_ies_o>;

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
void f1ap_elem_procs_o::init_msg_c::destroy_()
{
  switch (type_) {
    case types::reset:
      c.destroy<reset_s>();
      break;
    case types::f1_setup_request:
      c.destroy<f1_setup_request_s>();
      break;
    case types::gnb_du_cfg_upd:
      c.destroy<gnb_du_cfg_upd_s>();
      break;
    case types::gnb_cu_cfg_upd:
      c.destroy<gnb_cu_cfg_upd_s>();
      break;
    case types::ue_context_setup_request:
      c.destroy<ue_context_setup_request_s>();
      break;
    case types::ue_context_release_cmd:
      c.destroy<ue_context_release_cmd_s>();
      break;
    case types::ue_context_mod_request:
      c.destroy<ue_context_mod_request_s>();
      break;
    case types::ue_context_mod_required:
      c.destroy<ue_context_mod_required_s>();
      break;
    case types::write_replace_warning_request:
      c.destroy<write_replace_warning_request_s>();
      break;
    case types::pws_cancel_request:
      c.destroy<pws_cancel_request_s>();
      break;
    case types::gnb_du_res_coordination_request:
      c.destroy<gnb_du_res_coordination_request_s>();
      break;
    case types::f1_removal_request:
      c.destroy<f1_removal_request_s>();
      break;
    case types::bap_map_cfg:
      c.destroy<bap_map_cfg_s>();
      break;
    case types::gnb_du_res_cfg:
      c.destroy<gnb_du_res_cfg_s>();
      break;
    case types::iab_tnl_address_request:
      c.destroy<iab_tnl_address_request_s>();
      break;
    case types::iab_up_cfg_upd_request:
      c.destroy<iab_up_cfg_upd_request_s>();
      break;
    case types::res_status_request:
      c.destroy<res_status_request_s>();
      break;
    case types::positioning_meas_request:
      c.destroy<positioning_meas_request_s>();
      break;
    case types::trp_info_request:
      c.destroy<trp_info_request_s>();
      break;
    case types::positioning_info_request:
      c.destroy<positioning_info_request_s>();
      break;
    case types::positioning_activation_request:
      c.destroy<positioning_activation_request_s>();
      break;
    case types::e_c_id_meas_initiation_request:
      c.destroy<e_c_id_meas_initiation_request_s>();
      break;
    case types::broadcast_context_setup_request:
      c.destroy<broadcast_context_setup_request_s>();
      break;
    case types::broadcast_context_release_cmd:
      c.destroy<broadcast_context_release_cmd_s>();
      break;
    case types::broadcast_context_mod_request:
      c.destroy<broadcast_context_mod_request_s>();
      break;
    case types::multicast_context_setup_request:
      c.destroy<multicast_context_setup_request_s>();
      break;
    case types::multicast_context_release_cmd:
      c.destroy<multicast_context_release_cmd_s>();
      break;
    case types::multicast_context_mod_request:
      c.destroy<multicast_context_mod_request_s>();
      break;
    case types::multicast_distribution_setup_request:
      c.destroy<multicast_distribution_setup_request_s>();
      break;
    case types::multicast_distribution_release_cmd:
      c.destroy<multicast_distribution_release_cmd_s>();
      break;
    case types::pdc_meas_initiation_request:
      c.destroy<pdc_meas_initiation_request_s>();
      break;
    case types::prs_cfg_request:
      c.destroy<prs_cfg_request_s>();
      break;
    case types::meas_precfg_required:
      c.destroy<meas_precfg_required_s>();
      break;
    case types::error_ind:
      c.destroy<error_ind_s>();
      break;
    case types::ue_context_release_request:
      c.destroy<ue_context_release_request_s>();
      break;
    case types::dl_rrc_msg_transfer:
      c.destroy<dl_rrc_msg_transfer_s>();
      break;
    case types::ul_rrc_msg_transfer:
      c.destroy<ul_rrc_msg_transfer_s>();
      break;
    case types::ue_inactivity_notif:
      c.destroy<ue_inactivity_notif_s>();
      break;
    case types::private_msg:
      c.destroy<private_msg_s>();
      break;
    case types::init_ul_rrc_msg_transfer:
      c.destroy<init_ul_rrc_msg_transfer_s>();
      break;
    case types::sys_info_delivery_cmd:
      c.destroy<sys_info_delivery_cmd_s>();
      break;
    case types::paging:
      c.destroy<paging_s>();
      break;
    case types::notify:
      c.destroy<notify_s>();
      break;
    case types::pws_restart_ind:
      c.destroy<pws_restart_ind_s>();
      break;
    case types::pws_fail_ind:
      c.destroy<pws_fail_ind_s>();
      break;
    case types::gnb_du_status_ind:
      c.destroy<gnb_du_status_ind_s>();
      break;
    case types::rrc_delivery_report:
      c.destroy<rrc_delivery_report_s>();
      break;
    case types::network_access_rate_reduction:
      c.destroy<network_access_rate_reduction_s>();
      break;
    case types::trace_start:
      c.destroy<trace_start_s>();
      break;
    case types::deactiv_trace:
      c.destroy<deactiv_trace_s>();
      break;
    case types::du_cu_radio_info_transfer:
      c.destroy<du_cu_radio_info_transfer_s>();
      break;
    case types::cu_du_radio_info_transfer:
      c.destroy<cu_du_radio_info_transfer_s>();
      break;
    case types::res_status_upd:
      c.destroy<res_status_upd_s>();
      break;
    case types::access_and_mob_ind:
      c.destroy<access_and_mob_ind_s>();
      break;
    case types::ref_time_info_report_ctrl:
      c.destroy<ref_time_info_report_ctrl_s>();
      break;
    case types::ref_time_info_report:
      c.destroy<ref_time_info_report_s>();
      break;
    case types::access_success:
      c.destroy<access_success_s>();
      break;
    case types::cell_traffic_trace:
      c.destroy<cell_traffic_trace_s>();
      break;
    case types::positioning_assist_info_ctrl:
      c.destroy<positioning_assist_info_ctrl_s>();
      break;
    case types::positioning_assist_info_feedback:
      c.destroy<positioning_assist_info_feedback_s>();
      break;
    case types::positioning_meas_report:
      c.destroy<positioning_meas_report_s>();
      break;
    case types::positioning_meas_abort:
      c.destroy<positioning_meas_abort_s>();
      break;
    case types::positioning_meas_fail_ind:
      c.destroy<positioning_meas_fail_ind_s>();
      break;
    case types::positioning_meas_upd:
      c.destroy<positioning_meas_upd_s>();
      break;
    case types::positioning_deactivation:
      c.destroy<positioning_deactivation_s>();
      break;
    case types::e_c_id_meas_fail_ind:
      c.destroy<e_c_id_meas_fail_ind_s>();
      break;
    case types::e_c_id_meas_report:
      c.destroy<e_c_id_meas_report_s>();
      break;
    case types::e_c_id_meas_termination_cmd:
      c.destroy<e_c_id_meas_termination_cmd_s>();
      break;
    case types::positioning_info_upd:
      c.destroy<positioning_info_upd_s>();
      break;
    case types::multicast_group_paging:
      c.destroy<multicast_group_paging_s>();
      break;
    case types::broadcast_context_release_request:
      c.destroy<broadcast_context_release_request_s>();
      break;
    case types::multicast_context_release_request:
      c.destroy<multicast_context_release_request_s>();
      break;
    case types::pdc_meas_report:
      c.destroy<pdc_meas_report_s>();
      break;
    case types::pdc_meas_termination_cmd:
      c.destroy<pdc_meas_termination_cmd_s>();
      break;
    case types::pdc_meas_fail_ind:
      c.destroy<pdc_meas_fail_ind_s>();
      break;
    case types::meas_activation:
      c.destroy<meas_activation_s>();
      break;
    case types::qo_e_info_transfer:
      c.destroy<qo_e_info_transfer_s>();
      break;
    case types::pos_sys_info_delivery_cmd:
      c.destroy<pos_sys_info_delivery_cmd_s>();
      break;
    default:
      break;
  }
}
void f1ap_elem_procs_o::init_msg_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::reset:
      c.init<reset_s>();
      break;
    case types::f1_setup_request:
      c.init<f1_setup_request_s>();
      break;
    case types::gnb_du_cfg_upd:
      c.init<gnb_du_cfg_upd_s>();
      break;
    case types::gnb_cu_cfg_upd:
      c.init<gnb_cu_cfg_upd_s>();
      break;
    case types::ue_context_setup_request:
      c.init<ue_context_setup_request_s>();
      break;
    case types::ue_context_release_cmd:
      c.init<ue_context_release_cmd_s>();
      break;
    case types::ue_context_mod_request:
      c.init<ue_context_mod_request_s>();
      break;
    case types::ue_context_mod_required:
      c.init<ue_context_mod_required_s>();
      break;
    case types::write_replace_warning_request:
      c.init<write_replace_warning_request_s>();
      break;
    case types::pws_cancel_request:
      c.init<pws_cancel_request_s>();
      break;
    case types::gnb_du_res_coordination_request:
      c.init<gnb_du_res_coordination_request_s>();
      break;
    case types::f1_removal_request:
      c.init<f1_removal_request_s>();
      break;
    case types::bap_map_cfg:
      c.init<bap_map_cfg_s>();
      break;
    case types::gnb_du_res_cfg:
      c.init<gnb_du_res_cfg_s>();
      break;
    case types::iab_tnl_address_request:
      c.init<iab_tnl_address_request_s>();
      break;
    case types::iab_up_cfg_upd_request:
      c.init<iab_up_cfg_upd_request_s>();
      break;
    case types::res_status_request:
      c.init<res_status_request_s>();
      break;
    case types::positioning_meas_request:
      c.init<positioning_meas_request_s>();
      break;
    case types::trp_info_request:
      c.init<trp_info_request_s>();
      break;
    case types::positioning_info_request:
      c.init<positioning_info_request_s>();
      break;
    case types::positioning_activation_request:
      c.init<positioning_activation_request_s>();
      break;
    case types::e_c_id_meas_initiation_request:
      c.init<e_c_id_meas_initiation_request_s>();
      break;
    case types::broadcast_context_setup_request:
      c.init<broadcast_context_setup_request_s>();
      break;
    case types::broadcast_context_release_cmd:
      c.init<broadcast_context_release_cmd_s>();
      break;
    case types::broadcast_context_mod_request:
      c.init<broadcast_context_mod_request_s>();
      break;
    case types::multicast_context_setup_request:
      c.init<multicast_context_setup_request_s>();
      break;
    case types::multicast_context_release_cmd:
      c.init<multicast_context_release_cmd_s>();
      break;
    case types::multicast_context_mod_request:
      c.init<multicast_context_mod_request_s>();
      break;
    case types::multicast_distribution_setup_request:
      c.init<multicast_distribution_setup_request_s>();
      break;
    case types::multicast_distribution_release_cmd:
      c.init<multicast_distribution_release_cmd_s>();
      break;
    case types::pdc_meas_initiation_request:
      c.init<pdc_meas_initiation_request_s>();
      break;
    case types::prs_cfg_request:
      c.init<prs_cfg_request_s>();
      break;
    case types::meas_precfg_required:
      c.init<meas_precfg_required_s>();
      break;
    case types::error_ind:
      c.init<error_ind_s>();
      break;
    case types::ue_context_release_request:
      c.init<ue_context_release_request_s>();
      break;
    case types::dl_rrc_msg_transfer:
      c.init<dl_rrc_msg_transfer_s>();
      break;
    case types::ul_rrc_msg_transfer:
      c.init<ul_rrc_msg_transfer_s>();
      break;
    case types::ue_inactivity_notif:
      c.init<ue_inactivity_notif_s>();
      break;
    case types::private_msg:
      c.init<private_msg_s>();
      break;
    case types::init_ul_rrc_msg_transfer:
      c.init<init_ul_rrc_msg_transfer_s>();
      break;
    case types::sys_info_delivery_cmd:
      c.init<sys_info_delivery_cmd_s>();
      break;
    case types::paging:
      c.init<paging_s>();
      break;
    case types::notify:
      c.init<notify_s>();
      break;
    case types::pws_restart_ind:
      c.init<pws_restart_ind_s>();
      break;
    case types::pws_fail_ind:
      c.init<pws_fail_ind_s>();
      break;
    case types::gnb_du_status_ind:
      c.init<gnb_du_status_ind_s>();
      break;
    case types::rrc_delivery_report:
      c.init<rrc_delivery_report_s>();
      break;
    case types::network_access_rate_reduction:
      c.init<network_access_rate_reduction_s>();
      break;
    case types::trace_start:
      c.init<trace_start_s>();
      break;
    case types::deactiv_trace:
      c.init<deactiv_trace_s>();
      break;
    case types::du_cu_radio_info_transfer:
      c.init<du_cu_radio_info_transfer_s>();
      break;
    case types::cu_du_radio_info_transfer:
      c.init<cu_du_radio_info_transfer_s>();
      break;
    case types::res_status_upd:
      c.init<res_status_upd_s>();
      break;
    case types::access_and_mob_ind:
      c.init<access_and_mob_ind_s>();
      break;
    case types::ref_time_info_report_ctrl:
      c.init<ref_time_info_report_ctrl_s>();
      break;
    case types::ref_time_info_report:
      c.init<ref_time_info_report_s>();
      break;
    case types::access_success:
      c.init<access_success_s>();
      break;
    case types::cell_traffic_trace:
      c.init<cell_traffic_trace_s>();
      break;
    case types::positioning_assist_info_ctrl:
      c.init<positioning_assist_info_ctrl_s>();
      break;
    case types::positioning_assist_info_feedback:
      c.init<positioning_assist_info_feedback_s>();
      break;
    case types::positioning_meas_report:
      c.init<positioning_meas_report_s>();
      break;
    case types::positioning_meas_abort:
      c.init<positioning_meas_abort_s>();
      break;
    case types::positioning_meas_fail_ind:
      c.init<positioning_meas_fail_ind_s>();
      break;
    case types::positioning_meas_upd:
      c.init<positioning_meas_upd_s>();
      break;
    case types::positioning_deactivation:
      c.init<positioning_deactivation_s>();
      break;
    case types::e_c_id_meas_fail_ind:
      c.init<e_c_id_meas_fail_ind_s>();
      break;
    case types::e_c_id_meas_report:
      c.init<e_c_id_meas_report_s>();
      break;
    case types::e_c_id_meas_termination_cmd:
      c.init<e_c_id_meas_termination_cmd_s>();
      break;
    case types::positioning_info_upd:
      c.init<positioning_info_upd_s>();
      break;
    case types::multicast_group_paging:
      c.init<multicast_group_paging_s>();
      break;
    case types::broadcast_context_release_request:
      c.init<broadcast_context_release_request_s>();
      break;
    case types::multicast_context_release_request:
      c.init<multicast_context_release_request_s>();
      break;
    case types::pdc_meas_report:
      c.init<pdc_meas_report_s>();
      break;
    case types::pdc_meas_termination_cmd:
      c.init<pdc_meas_termination_cmd_s>();
      break;
    case types::pdc_meas_fail_ind:
      c.init<pdc_meas_fail_ind_s>();
      break;
    case types::meas_activation:
      c.init<meas_activation_s>();
      break;
    case types::qo_e_info_transfer:
      c.init<qo_e_info_transfer_s>();
      break;
    case types::pos_sys_info_delivery_cmd:
      c.init<pos_sys_info_delivery_cmd_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::init_msg_c");
  }
}
f1ap_elem_procs_o::init_msg_c::init_msg_c(const f1ap_elem_procs_o::init_msg_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::reset:
      c.init(other.c.get<reset_s>());
      break;
    case types::f1_setup_request:
      c.init(other.c.get<f1_setup_request_s>());
      break;
    case types::gnb_du_cfg_upd:
      c.init(other.c.get<gnb_du_cfg_upd_s>());
      break;
    case types::gnb_cu_cfg_upd:
      c.init(other.c.get<gnb_cu_cfg_upd_s>());
      break;
    case types::ue_context_setup_request:
      c.init(other.c.get<ue_context_setup_request_s>());
      break;
    case types::ue_context_release_cmd:
      c.init(other.c.get<ue_context_release_cmd_s>());
      break;
    case types::ue_context_mod_request:
      c.init(other.c.get<ue_context_mod_request_s>());
      break;
    case types::ue_context_mod_required:
      c.init(other.c.get<ue_context_mod_required_s>());
      break;
    case types::write_replace_warning_request:
      c.init(other.c.get<write_replace_warning_request_s>());
      break;
    case types::pws_cancel_request:
      c.init(other.c.get<pws_cancel_request_s>());
      break;
    case types::gnb_du_res_coordination_request:
      c.init(other.c.get<gnb_du_res_coordination_request_s>());
      break;
    case types::f1_removal_request:
      c.init(other.c.get<f1_removal_request_s>());
      break;
    case types::bap_map_cfg:
      c.init(other.c.get<bap_map_cfg_s>());
      break;
    case types::gnb_du_res_cfg:
      c.init(other.c.get<gnb_du_res_cfg_s>());
      break;
    case types::iab_tnl_address_request:
      c.init(other.c.get<iab_tnl_address_request_s>());
      break;
    case types::iab_up_cfg_upd_request:
      c.init(other.c.get<iab_up_cfg_upd_request_s>());
      break;
    case types::res_status_request:
      c.init(other.c.get<res_status_request_s>());
      break;
    case types::positioning_meas_request:
      c.init(other.c.get<positioning_meas_request_s>());
      break;
    case types::trp_info_request:
      c.init(other.c.get<trp_info_request_s>());
      break;
    case types::positioning_info_request:
      c.init(other.c.get<positioning_info_request_s>());
      break;
    case types::positioning_activation_request:
      c.init(other.c.get<positioning_activation_request_s>());
      break;
    case types::e_c_id_meas_initiation_request:
      c.init(other.c.get<e_c_id_meas_initiation_request_s>());
      break;
    case types::broadcast_context_setup_request:
      c.init(other.c.get<broadcast_context_setup_request_s>());
      break;
    case types::broadcast_context_release_cmd:
      c.init(other.c.get<broadcast_context_release_cmd_s>());
      break;
    case types::broadcast_context_mod_request:
      c.init(other.c.get<broadcast_context_mod_request_s>());
      break;
    case types::multicast_context_setup_request:
      c.init(other.c.get<multicast_context_setup_request_s>());
      break;
    case types::multicast_context_release_cmd:
      c.init(other.c.get<multicast_context_release_cmd_s>());
      break;
    case types::multicast_context_mod_request:
      c.init(other.c.get<multicast_context_mod_request_s>());
      break;
    case types::multicast_distribution_setup_request:
      c.init(other.c.get<multicast_distribution_setup_request_s>());
      break;
    case types::multicast_distribution_release_cmd:
      c.init(other.c.get<multicast_distribution_release_cmd_s>());
      break;
    case types::pdc_meas_initiation_request:
      c.init(other.c.get<pdc_meas_initiation_request_s>());
      break;
    case types::prs_cfg_request:
      c.init(other.c.get<prs_cfg_request_s>());
      break;
    case types::meas_precfg_required:
      c.init(other.c.get<meas_precfg_required_s>());
      break;
    case types::error_ind:
      c.init(other.c.get<error_ind_s>());
      break;
    case types::ue_context_release_request:
      c.init(other.c.get<ue_context_release_request_s>());
      break;
    case types::dl_rrc_msg_transfer:
      c.init(other.c.get<dl_rrc_msg_transfer_s>());
      break;
    case types::ul_rrc_msg_transfer:
      c.init(other.c.get<ul_rrc_msg_transfer_s>());
      break;
    case types::ue_inactivity_notif:
      c.init(other.c.get<ue_inactivity_notif_s>());
      break;
    case types::private_msg:
      c.init(other.c.get<private_msg_s>());
      break;
    case types::init_ul_rrc_msg_transfer:
      c.init(other.c.get<init_ul_rrc_msg_transfer_s>());
      break;
    case types::sys_info_delivery_cmd:
      c.init(other.c.get<sys_info_delivery_cmd_s>());
      break;
    case types::paging:
      c.init(other.c.get<paging_s>());
      break;
    case types::notify:
      c.init(other.c.get<notify_s>());
      break;
    case types::pws_restart_ind:
      c.init(other.c.get<pws_restart_ind_s>());
      break;
    case types::pws_fail_ind:
      c.init(other.c.get<pws_fail_ind_s>());
      break;
    case types::gnb_du_status_ind:
      c.init(other.c.get<gnb_du_status_ind_s>());
      break;
    case types::rrc_delivery_report:
      c.init(other.c.get<rrc_delivery_report_s>());
      break;
    case types::network_access_rate_reduction:
      c.init(other.c.get<network_access_rate_reduction_s>());
      break;
    case types::trace_start:
      c.init(other.c.get<trace_start_s>());
      break;
    case types::deactiv_trace:
      c.init(other.c.get<deactiv_trace_s>());
      break;
    case types::du_cu_radio_info_transfer:
      c.init(other.c.get<du_cu_radio_info_transfer_s>());
      break;
    case types::cu_du_radio_info_transfer:
      c.init(other.c.get<cu_du_radio_info_transfer_s>());
      break;
    case types::res_status_upd:
      c.init(other.c.get<res_status_upd_s>());
      break;
    case types::access_and_mob_ind:
      c.init(other.c.get<access_and_mob_ind_s>());
      break;
    case types::ref_time_info_report_ctrl:
      c.init(other.c.get<ref_time_info_report_ctrl_s>());
      break;
    case types::ref_time_info_report:
      c.init(other.c.get<ref_time_info_report_s>());
      break;
    case types::access_success:
      c.init(other.c.get<access_success_s>());
      break;
    case types::cell_traffic_trace:
      c.init(other.c.get<cell_traffic_trace_s>());
      break;
    case types::positioning_assist_info_ctrl:
      c.init(other.c.get<positioning_assist_info_ctrl_s>());
      break;
    case types::positioning_assist_info_feedback:
      c.init(other.c.get<positioning_assist_info_feedback_s>());
      break;
    case types::positioning_meas_report:
      c.init(other.c.get<positioning_meas_report_s>());
      break;
    case types::positioning_meas_abort:
      c.init(other.c.get<positioning_meas_abort_s>());
      break;
    case types::positioning_meas_fail_ind:
      c.init(other.c.get<positioning_meas_fail_ind_s>());
      break;
    case types::positioning_meas_upd:
      c.init(other.c.get<positioning_meas_upd_s>());
      break;
    case types::positioning_deactivation:
      c.init(other.c.get<positioning_deactivation_s>());
      break;
    case types::e_c_id_meas_fail_ind:
      c.init(other.c.get<e_c_id_meas_fail_ind_s>());
      break;
    case types::e_c_id_meas_report:
      c.init(other.c.get<e_c_id_meas_report_s>());
      break;
    case types::e_c_id_meas_termination_cmd:
      c.init(other.c.get<e_c_id_meas_termination_cmd_s>());
      break;
    case types::positioning_info_upd:
      c.init(other.c.get<positioning_info_upd_s>());
      break;
    case types::multicast_group_paging:
      c.init(other.c.get<multicast_group_paging_s>());
      break;
    case types::broadcast_context_release_request:
      c.init(other.c.get<broadcast_context_release_request_s>());
      break;
    case types::multicast_context_release_request:
      c.init(other.c.get<multicast_context_release_request_s>());
      break;
    case types::pdc_meas_report:
      c.init(other.c.get<pdc_meas_report_s>());
      break;
    case types::pdc_meas_termination_cmd:
      c.init(other.c.get<pdc_meas_termination_cmd_s>());
      break;
    case types::pdc_meas_fail_ind:
      c.init(other.c.get<pdc_meas_fail_ind_s>());
      break;
    case types::meas_activation:
      c.init(other.c.get<meas_activation_s>());
      break;
    case types::qo_e_info_transfer:
      c.init(other.c.get<qo_e_info_transfer_s>());
      break;
    case types::pos_sys_info_delivery_cmd:
      c.init(other.c.get<pos_sys_info_delivery_cmd_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::init_msg_c");
  }
}
f1ap_elem_procs_o::init_msg_c& f1ap_elem_procs_o::init_msg_c::operator=(const f1ap_elem_procs_o::init_msg_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::reset:
      c.set(other.c.get<reset_s>());
      break;
    case types::f1_setup_request:
      c.set(other.c.get<f1_setup_request_s>());
      break;
    case types::gnb_du_cfg_upd:
      c.set(other.c.get<gnb_du_cfg_upd_s>());
      break;
    case types::gnb_cu_cfg_upd:
      c.set(other.c.get<gnb_cu_cfg_upd_s>());
      break;
    case types::ue_context_setup_request:
      c.set(other.c.get<ue_context_setup_request_s>());
      break;
    case types::ue_context_release_cmd:
      c.set(other.c.get<ue_context_release_cmd_s>());
      break;
    case types::ue_context_mod_request:
      c.set(other.c.get<ue_context_mod_request_s>());
      break;
    case types::ue_context_mod_required:
      c.set(other.c.get<ue_context_mod_required_s>());
      break;
    case types::write_replace_warning_request:
      c.set(other.c.get<write_replace_warning_request_s>());
      break;
    case types::pws_cancel_request:
      c.set(other.c.get<pws_cancel_request_s>());
      break;
    case types::gnb_du_res_coordination_request:
      c.set(other.c.get<gnb_du_res_coordination_request_s>());
      break;
    case types::f1_removal_request:
      c.set(other.c.get<f1_removal_request_s>());
      break;
    case types::bap_map_cfg:
      c.set(other.c.get<bap_map_cfg_s>());
      break;
    case types::gnb_du_res_cfg:
      c.set(other.c.get<gnb_du_res_cfg_s>());
      break;
    case types::iab_tnl_address_request:
      c.set(other.c.get<iab_tnl_address_request_s>());
      break;
    case types::iab_up_cfg_upd_request:
      c.set(other.c.get<iab_up_cfg_upd_request_s>());
      break;
    case types::res_status_request:
      c.set(other.c.get<res_status_request_s>());
      break;
    case types::positioning_meas_request:
      c.set(other.c.get<positioning_meas_request_s>());
      break;
    case types::trp_info_request:
      c.set(other.c.get<trp_info_request_s>());
      break;
    case types::positioning_info_request:
      c.set(other.c.get<positioning_info_request_s>());
      break;
    case types::positioning_activation_request:
      c.set(other.c.get<positioning_activation_request_s>());
      break;
    case types::e_c_id_meas_initiation_request:
      c.set(other.c.get<e_c_id_meas_initiation_request_s>());
      break;
    case types::broadcast_context_setup_request:
      c.set(other.c.get<broadcast_context_setup_request_s>());
      break;
    case types::broadcast_context_release_cmd:
      c.set(other.c.get<broadcast_context_release_cmd_s>());
      break;
    case types::broadcast_context_mod_request:
      c.set(other.c.get<broadcast_context_mod_request_s>());
      break;
    case types::multicast_context_setup_request:
      c.set(other.c.get<multicast_context_setup_request_s>());
      break;
    case types::multicast_context_release_cmd:
      c.set(other.c.get<multicast_context_release_cmd_s>());
      break;
    case types::multicast_context_mod_request:
      c.set(other.c.get<multicast_context_mod_request_s>());
      break;
    case types::multicast_distribution_setup_request:
      c.set(other.c.get<multicast_distribution_setup_request_s>());
      break;
    case types::multicast_distribution_release_cmd:
      c.set(other.c.get<multicast_distribution_release_cmd_s>());
      break;
    case types::pdc_meas_initiation_request:
      c.set(other.c.get<pdc_meas_initiation_request_s>());
      break;
    case types::prs_cfg_request:
      c.set(other.c.get<prs_cfg_request_s>());
      break;
    case types::meas_precfg_required:
      c.set(other.c.get<meas_precfg_required_s>());
      break;
    case types::error_ind:
      c.set(other.c.get<error_ind_s>());
      break;
    case types::ue_context_release_request:
      c.set(other.c.get<ue_context_release_request_s>());
      break;
    case types::dl_rrc_msg_transfer:
      c.set(other.c.get<dl_rrc_msg_transfer_s>());
      break;
    case types::ul_rrc_msg_transfer:
      c.set(other.c.get<ul_rrc_msg_transfer_s>());
      break;
    case types::ue_inactivity_notif:
      c.set(other.c.get<ue_inactivity_notif_s>());
      break;
    case types::private_msg:
      c.set(other.c.get<private_msg_s>());
      break;
    case types::init_ul_rrc_msg_transfer:
      c.set(other.c.get<init_ul_rrc_msg_transfer_s>());
      break;
    case types::sys_info_delivery_cmd:
      c.set(other.c.get<sys_info_delivery_cmd_s>());
      break;
    case types::paging:
      c.set(other.c.get<paging_s>());
      break;
    case types::notify:
      c.set(other.c.get<notify_s>());
      break;
    case types::pws_restart_ind:
      c.set(other.c.get<pws_restart_ind_s>());
      break;
    case types::pws_fail_ind:
      c.set(other.c.get<pws_fail_ind_s>());
      break;
    case types::gnb_du_status_ind:
      c.set(other.c.get<gnb_du_status_ind_s>());
      break;
    case types::rrc_delivery_report:
      c.set(other.c.get<rrc_delivery_report_s>());
      break;
    case types::network_access_rate_reduction:
      c.set(other.c.get<network_access_rate_reduction_s>());
      break;
    case types::trace_start:
      c.set(other.c.get<trace_start_s>());
      break;
    case types::deactiv_trace:
      c.set(other.c.get<deactiv_trace_s>());
      break;
    case types::du_cu_radio_info_transfer:
      c.set(other.c.get<du_cu_radio_info_transfer_s>());
      break;
    case types::cu_du_radio_info_transfer:
      c.set(other.c.get<cu_du_radio_info_transfer_s>());
      break;
    case types::res_status_upd:
      c.set(other.c.get<res_status_upd_s>());
      break;
    case types::access_and_mob_ind:
      c.set(other.c.get<access_and_mob_ind_s>());
      break;
    case types::ref_time_info_report_ctrl:
      c.set(other.c.get<ref_time_info_report_ctrl_s>());
      break;
    case types::ref_time_info_report:
      c.set(other.c.get<ref_time_info_report_s>());
      break;
    case types::access_success:
      c.set(other.c.get<access_success_s>());
      break;
    case types::cell_traffic_trace:
      c.set(other.c.get<cell_traffic_trace_s>());
      break;
    case types::positioning_assist_info_ctrl:
      c.set(other.c.get<positioning_assist_info_ctrl_s>());
      break;
    case types::positioning_assist_info_feedback:
      c.set(other.c.get<positioning_assist_info_feedback_s>());
      break;
    case types::positioning_meas_report:
      c.set(other.c.get<positioning_meas_report_s>());
      break;
    case types::positioning_meas_abort:
      c.set(other.c.get<positioning_meas_abort_s>());
      break;
    case types::positioning_meas_fail_ind:
      c.set(other.c.get<positioning_meas_fail_ind_s>());
      break;
    case types::positioning_meas_upd:
      c.set(other.c.get<positioning_meas_upd_s>());
      break;
    case types::positioning_deactivation:
      c.set(other.c.get<positioning_deactivation_s>());
      break;
    case types::e_c_id_meas_fail_ind:
      c.set(other.c.get<e_c_id_meas_fail_ind_s>());
      break;
    case types::e_c_id_meas_report:
      c.set(other.c.get<e_c_id_meas_report_s>());
      break;
    case types::e_c_id_meas_termination_cmd:
      c.set(other.c.get<e_c_id_meas_termination_cmd_s>());
      break;
    case types::positioning_info_upd:
      c.set(other.c.get<positioning_info_upd_s>());
      break;
    case types::multicast_group_paging:
      c.set(other.c.get<multicast_group_paging_s>());
      break;
    case types::broadcast_context_release_request:
      c.set(other.c.get<broadcast_context_release_request_s>());
      break;
    case types::multicast_context_release_request:
      c.set(other.c.get<multicast_context_release_request_s>());
      break;
    case types::pdc_meas_report:
      c.set(other.c.get<pdc_meas_report_s>());
      break;
    case types::pdc_meas_termination_cmd:
      c.set(other.c.get<pdc_meas_termination_cmd_s>());
      break;
    case types::pdc_meas_fail_ind:
      c.set(other.c.get<pdc_meas_fail_ind_s>());
      break;
    case types::meas_activation:
      c.set(other.c.get<meas_activation_s>());
      break;
    case types::qo_e_info_transfer:
      c.set(other.c.get<qo_e_info_transfer_s>());
      break;
    case types::pos_sys_info_delivery_cmd:
      c.set(other.c.get<pos_sys_info_delivery_cmd_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::init_msg_c");
  }

  return *this;
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
void f1ap_elem_procs_o::successful_outcome_c::destroy_()
{
  switch (type_) {
    case types::reset_ack:
      c.destroy<reset_ack_s>();
      break;
    case types::f1_setup_resp:
      c.destroy<f1_setup_resp_s>();
      break;
    case types::gnb_du_cfg_upd_ack:
      c.destroy<gnb_du_cfg_upd_ack_s>();
      break;
    case types::gnb_cu_cfg_upd_ack:
      c.destroy<gnb_cu_cfg_upd_ack_s>();
      break;
    case types::ue_context_setup_resp:
      c.destroy<ue_context_setup_resp_s>();
      break;
    case types::ue_context_release_complete:
      c.destroy<ue_context_release_complete_s>();
      break;
    case types::ue_context_mod_resp:
      c.destroy<ue_context_mod_resp_s>();
      break;
    case types::ue_context_mod_confirm:
      c.destroy<ue_context_mod_confirm_s>();
      break;
    case types::write_replace_warning_resp:
      c.destroy<write_replace_warning_resp_s>();
      break;
    case types::pws_cancel_resp:
      c.destroy<pws_cancel_resp_s>();
      break;
    case types::gnb_du_res_coordination_resp:
      c.destroy<gnb_du_res_coordination_resp_s>();
      break;
    case types::f1_removal_resp:
      c.destroy<f1_removal_resp_s>();
      break;
    case types::bap_map_cfg_ack:
      c.destroy<bap_map_cfg_ack_s>();
      break;
    case types::gnb_du_res_cfg_ack:
      c.destroy<gnb_du_res_cfg_ack_s>();
      break;
    case types::iab_tnl_address_resp:
      c.destroy<iab_tnl_address_resp_s>();
      break;
    case types::iab_up_cfg_upd_resp:
      c.destroy<iab_up_cfg_upd_resp_s>();
      break;
    case types::res_status_resp:
      c.destroy<res_status_resp_s>();
      break;
    case types::positioning_meas_resp:
      c.destroy<positioning_meas_resp_s>();
      break;
    case types::trp_info_resp:
      c.destroy<trp_info_resp_s>();
      break;
    case types::positioning_info_resp:
      c.destroy<positioning_info_resp_s>();
      break;
    case types::positioning_activation_resp:
      c.destroy<positioning_activation_resp_s>();
      break;
    case types::e_c_id_meas_initiation_resp:
      c.destroy<e_c_id_meas_initiation_resp_s>();
      break;
    case types::broadcast_context_setup_resp:
      c.destroy<broadcast_context_setup_resp_s>();
      break;
    case types::broadcast_context_release_complete:
      c.destroy<broadcast_context_release_complete_s>();
      break;
    case types::broadcast_context_mod_resp:
      c.destroy<broadcast_context_mod_resp_s>();
      break;
    case types::multicast_context_setup_resp:
      c.destroy<multicast_context_setup_resp_s>();
      break;
    case types::multicast_context_release_complete:
      c.destroy<multicast_context_release_complete_s>();
      break;
    case types::multicast_context_mod_resp:
      c.destroy<multicast_context_mod_resp_s>();
      break;
    case types::multicast_distribution_setup_resp:
      c.destroy<multicast_distribution_setup_resp_s>();
      break;
    case types::multicast_distribution_release_complete:
      c.destroy<multicast_distribution_release_complete_s>();
      break;
    case types::pdc_meas_initiation_resp:
      c.destroy<pdc_meas_initiation_resp_s>();
      break;
    case types::prs_cfg_resp:
      c.destroy<prs_cfg_resp_s>();
      break;
    case types::meas_precfg_confirm:
      c.destroy<meas_precfg_confirm_s>();
      break;
    default:
      break;
  }
}
void f1ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::reset_ack:
      c.init<reset_ack_s>();
      break;
    case types::f1_setup_resp:
      c.init<f1_setup_resp_s>();
      break;
    case types::gnb_du_cfg_upd_ack:
      c.init<gnb_du_cfg_upd_ack_s>();
      break;
    case types::gnb_cu_cfg_upd_ack:
      c.init<gnb_cu_cfg_upd_ack_s>();
      break;
    case types::ue_context_setup_resp:
      c.init<ue_context_setup_resp_s>();
      break;
    case types::ue_context_release_complete:
      c.init<ue_context_release_complete_s>();
      break;
    case types::ue_context_mod_resp:
      c.init<ue_context_mod_resp_s>();
      break;
    case types::ue_context_mod_confirm:
      c.init<ue_context_mod_confirm_s>();
      break;
    case types::write_replace_warning_resp:
      c.init<write_replace_warning_resp_s>();
      break;
    case types::pws_cancel_resp:
      c.init<pws_cancel_resp_s>();
      break;
    case types::gnb_du_res_coordination_resp:
      c.init<gnb_du_res_coordination_resp_s>();
      break;
    case types::f1_removal_resp:
      c.init<f1_removal_resp_s>();
      break;
    case types::bap_map_cfg_ack:
      c.init<bap_map_cfg_ack_s>();
      break;
    case types::gnb_du_res_cfg_ack:
      c.init<gnb_du_res_cfg_ack_s>();
      break;
    case types::iab_tnl_address_resp:
      c.init<iab_tnl_address_resp_s>();
      break;
    case types::iab_up_cfg_upd_resp:
      c.init<iab_up_cfg_upd_resp_s>();
      break;
    case types::res_status_resp:
      c.init<res_status_resp_s>();
      break;
    case types::positioning_meas_resp:
      c.init<positioning_meas_resp_s>();
      break;
    case types::trp_info_resp:
      c.init<trp_info_resp_s>();
      break;
    case types::positioning_info_resp:
      c.init<positioning_info_resp_s>();
      break;
    case types::positioning_activation_resp:
      c.init<positioning_activation_resp_s>();
      break;
    case types::e_c_id_meas_initiation_resp:
      c.init<e_c_id_meas_initiation_resp_s>();
      break;
    case types::broadcast_context_setup_resp:
      c.init<broadcast_context_setup_resp_s>();
      break;
    case types::broadcast_context_release_complete:
      c.init<broadcast_context_release_complete_s>();
      break;
    case types::broadcast_context_mod_resp:
      c.init<broadcast_context_mod_resp_s>();
      break;
    case types::multicast_context_setup_resp:
      c.init<multicast_context_setup_resp_s>();
      break;
    case types::multicast_context_release_complete:
      c.init<multicast_context_release_complete_s>();
      break;
    case types::multicast_context_mod_resp:
      c.init<multicast_context_mod_resp_s>();
      break;
    case types::multicast_distribution_setup_resp:
      c.init<multicast_distribution_setup_resp_s>();
      break;
    case types::multicast_distribution_release_complete:
      c.init<multicast_distribution_release_complete_s>();
      break;
    case types::pdc_meas_initiation_resp:
      c.init<pdc_meas_initiation_resp_s>();
      break;
    case types::prs_cfg_resp:
      c.init<prs_cfg_resp_s>();
      break;
    case types::meas_precfg_confirm:
      c.init<meas_precfg_confirm_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::successful_outcome_c");
  }
}
f1ap_elem_procs_o::successful_outcome_c::successful_outcome_c(const f1ap_elem_procs_o::successful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::reset_ack:
      c.init(other.c.get<reset_ack_s>());
      break;
    case types::f1_setup_resp:
      c.init(other.c.get<f1_setup_resp_s>());
      break;
    case types::gnb_du_cfg_upd_ack:
      c.init(other.c.get<gnb_du_cfg_upd_ack_s>());
      break;
    case types::gnb_cu_cfg_upd_ack:
      c.init(other.c.get<gnb_cu_cfg_upd_ack_s>());
      break;
    case types::ue_context_setup_resp:
      c.init(other.c.get<ue_context_setup_resp_s>());
      break;
    case types::ue_context_release_complete:
      c.init(other.c.get<ue_context_release_complete_s>());
      break;
    case types::ue_context_mod_resp:
      c.init(other.c.get<ue_context_mod_resp_s>());
      break;
    case types::ue_context_mod_confirm:
      c.init(other.c.get<ue_context_mod_confirm_s>());
      break;
    case types::write_replace_warning_resp:
      c.init(other.c.get<write_replace_warning_resp_s>());
      break;
    case types::pws_cancel_resp:
      c.init(other.c.get<pws_cancel_resp_s>());
      break;
    case types::gnb_du_res_coordination_resp:
      c.init(other.c.get<gnb_du_res_coordination_resp_s>());
      break;
    case types::f1_removal_resp:
      c.init(other.c.get<f1_removal_resp_s>());
      break;
    case types::bap_map_cfg_ack:
      c.init(other.c.get<bap_map_cfg_ack_s>());
      break;
    case types::gnb_du_res_cfg_ack:
      c.init(other.c.get<gnb_du_res_cfg_ack_s>());
      break;
    case types::iab_tnl_address_resp:
      c.init(other.c.get<iab_tnl_address_resp_s>());
      break;
    case types::iab_up_cfg_upd_resp:
      c.init(other.c.get<iab_up_cfg_upd_resp_s>());
      break;
    case types::res_status_resp:
      c.init(other.c.get<res_status_resp_s>());
      break;
    case types::positioning_meas_resp:
      c.init(other.c.get<positioning_meas_resp_s>());
      break;
    case types::trp_info_resp:
      c.init(other.c.get<trp_info_resp_s>());
      break;
    case types::positioning_info_resp:
      c.init(other.c.get<positioning_info_resp_s>());
      break;
    case types::positioning_activation_resp:
      c.init(other.c.get<positioning_activation_resp_s>());
      break;
    case types::e_c_id_meas_initiation_resp:
      c.init(other.c.get<e_c_id_meas_initiation_resp_s>());
      break;
    case types::broadcast_context_setup_resp:
      c.init(other.c.get<broadcast_context_setup_resp_s>());
      break;
    case types::broadcast_context_release_complete:
      c.init(other.c.get<broadcast_context_release_complete_s>());
      break;
    case types::broadcast_context_mod_resp:
      c.init(other.c.get<broadcast_context_mod_resp_s>());
      break;
    case types::multicast_context_setup_resp:
      c.init(other.c.get<multicast_context_setup_resp_s>());
      break;
    case types::multicast_context_release_complete:
      c.init(other.c.get<multicast_context_release_complete_s>());
      break;
    case types::multicast_context_mod_resp:
      c.init(other.c.get<multicast_context_mod_resp_s>());
      break;
    case types::multicast_distribution_setup_resp:
      c.init(other.c.get<multicast_distribution_setup_resp_s>());
      break;
    case types::multicast_distribution_release_complete:
      c.init(other.c.get<multicast_distribution_release_complete_s>());
      break;
    case types::pdc_meas_initiation_resp:
      c.init(other.c.get<pdc_meas_initiation_resp_s>());
      break;
    case types::prs_cfg_resp:
      c.init(other.c.get<prs_cfg_resp_s>());
      break;
    case types::meas_precfg_confirm:
      c.init(other.c.get<meas_precfg_confirm_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::successful_outcome_c");
  }
}
f1ap_elem_procs_o::successful_outcome_c&
f1ap_elem_procs_o::successful_outcome_c::operator=(const f1ap_elem_procs_o::successful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::reset_ack:
      c.set(other.c.get<reset_ack_s>());
      break;
    case types::f1_setup_resp:
      c.set(other.c.get<f1_setup_resp_s>());
      break;
    case types::gnb_du_cfg_upd_ack:
      c.set(other.c.get<gnb_du_cfg_upd_ack_s>());
      break;
    case types::gnb_cu_cfg_upd_ack:
      c.set(other.c.get<gnb_cu_cfg_upd_ack_s>());
      break;
    case types::ue_context_setup_resp:
      c.set(other.c.get<ue_context_setup_resp_s>());
      break;
    case types::ue_context_release_complete:
      c.set(other.c.get<ue_context_release_complete_s>());
      break;
    case types::ue_context_mod_resp:
      c.set(other.c.get<ue_context_mod_resp_s>());
      break;
    case types::ue_context_mod_confirm:
      c.set(other.c.get<ue_context_mod_confirm_s>());
      break;
    case types::write_replace_warning_resp:
      c.set(other.c.get<write_replace_warning_resp_s>());
      break;
    case types::pws_cancel_resp:
      c.set(other.c.get<pws_cancel_resp_s>());
      break;
    case types::gnb_du_res_coordination_resp:
      c.set(other.c.get<gnb_du_res_coordination_resp_s>());
      break;
    case types::f1_removal_resp:
      c.set(other.c.get<f1_removal_resp_s>());
      break;
    case types::bap_map_cfg_ack:
      c.set(other.c.get<bap_map_cfg_ack_s>());
      break;
    case types::gnb_du_res_cfg_ack:
      c.set(other.c.get<gnb_du_res_cfg_ack_s>());
      break;
    case types::iab_tnl_address_resp:
      c.set(other.c.get<iab_tnl_address_resp_s>());
      break;
    case types::iab_up_cfg_upd_resp:
      c.set(other.c.get<iab_up_cfg_upd_resp_s>());
      break;
    case types::res_status_resp:
      c.set(other.c.get<res_status_resp_s>());
      break;
    case types::positioning_meas_resp:
      c.set(other.c.get<positioning_meas_resp_s>());
      break;
    case types::trp_info_resp:
      c.set(other.c.get<trp_info_resp_s>());
      break;
    case types::positioning_info_resp:
      c.set(other.c.get<positioning_info_resp_s>());
      break;
    case types::positioning_activation_resp:
      c.set(other.c.get<positioning_activation_resp_s>());
      break;
    case types::e_c_id_meas_initiation_resp:
      c.set(other.c.get<e_c_id_meas_initiation_resp_s>());
      break;
    case types::broadcast_context_setup_resp:
      c.set(other.c.get<broadcast_context_setup_resp_s>());
      break;
    case types::broadcast_context_release_complete:
      c.set(other.c.get<broadcast_context_release_complete_s>());
      break;
    case types::broadcast_context_mod_resp:
      c.set(other.c.get<broadcast_context_mod_resp_s>());
      break;
    case types::multicast_context_setup_resp:
      c.set(other.c.get<multicast_context_setup_resp_s>());
      break;
    case types::multicast_context_release_complete:
      c.set(other.c.get<multicast_context_release_complete_s>());
      break;
    case types::multicast_context_mod_resp:
      c.set(other.c.get<multicast_context_mod_resp_s>());
      break;
    case types::multicast_distribution_setup_resp:
      c.set(other.c.get<multicast_distribution_setup_resp_s>());
      break;
    case types::multicast_distribution_release_complete:
      c.set(other.c.get<multicast_distribution_release_complete_s>());
      break;
    case types::pdc_meas_initiation_resp:
      c.set(other.c.get<pdc_meas_initiation_resp_s>());
      break;
    case types::prs_cfg_resp:
      c.set(other.c.get<prs_cfg_resp_s>());
      break;
    case types::meas_precfg_confirm:
      c.set(other.c.get<meas_precfg_confirm_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::successful_outcome_c");
  }

  return *this;
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
void f1ap_elem_procs_o::unsuccessful_outcome_c::destroy_()
{
  switch (type_) {
    case types::f1_setup_fail:
      c.destroy<f1_setup_fail_s>();
      break;
    case types::gnb_du_cfg_upd_fail:
      c.destroy<gnb_du_cfg_upd_fail_s>();
      break;
    case types::gnb_cu_cfg_upd_fail:
      c.destroy<gnb_cu_cfg_upd_fail_s>();
      break;
    case types::ue_context_setup_fail:
      c.destroy<ue_context_setup_fail_s>();
      break;
    case types::ue_context_mod_fail:
      c.destroy<ue_context_mod_fail_s>();
      break;
    case types::ue_context_mod_refuse:
      c.destroy<ue_context_mod_refuse_s>();
      break;
    case types::f1_removal_fail:
      c.destroy<f1_removal_fail_s>();
      break;
    case types::bap_map_cfg_fail:
      c.destroy<bap_map_cfg_fail_s>();
      break;
    case types::gnb_du_res_cfg_fail:
      c.destroy<gnb_du_res_cfg_fail_s>();
      break;
    case types::iab_tnl_address_fail:
      c.destroy<iab_tnl_address_fail_s>();
      break;
    case types::iab_up_cfg_upd_fail:
      c.destroy<iab_up_cfg_upd_fail_s>();
      break;
    case types::res_status_fail:
      c.destroy<res_status_fail_s>();
      break;
    case types::positioning_meas_fail:
      c.destroy<positioning_meas_fail_s>();
      break;
    case types::trp_info_fail:
      c.destroy<trp_info_fail_s>();
      break;
    case types::positioning_info_fail:
      c.destroy<positioning_info_fail_s>();
      break;
    case types::positioning_activation_fail:
      c.destroy<positioning_activation_fail_s>();
      break;
    case types::e_c_id_meas_initiation_fail:
      c.destroy<e_c_id_meas_initiation_fail_s>();
      break;
    case types::broadcast_context_setup_fail:
      c.destroy<broadcast_context_setup_fail_s>();
      break;
    case types::broadcast_context_mod_fail:
      c.destroy<broadcast_context_mod_fail_s>();
      break;
    case types::multicast_context_setup_fail:
      c.destroy<multicast_context_setup_fail_s>();
      break;
    case types::multicast_context_mod_fail:
      c.destroy<multicast_context_mod_fail_s>();
      break;
    case types::multicast_distribution_setup_fail:
      c.destroy<multicast_distribution_setup_fail_s>();
      break;
    case types::pdc_meas_initiation_fail:
      c.destroy<pdc_meas_initiation_fail_s>();
      break;
    case types::prs_cfg_fail:
      c.destroy<prs_cfg_fail_s>();
      break;
    case types::meas_precfg_refuse:
      c.destroy<meas_precfg_refuse_s>();
      break;
    default:
      break;
  }
}
void f1ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::f1_setup_fail:
      c.init<f1_setup_fail_s>();
      break;
    case types::gnb_du_cfg_upd_fail:
      c.init<gnb_du_cfg_upd_fail_s>();
      break;
    case types::gnb_cu_cfg_upd_fail:
      c.init<gnb_cu_cfg_upd_fail_s>();
      break;
    case types::ue_context_setup_fail:
      c.init<ue_context_setup_fail_s>();
      break;
    case types::ue_context_mod_fail:
      c.init<ue_context_mod_fail_s>();
      break;
    case types::ue_context_mod_refuse:
      c.init<ue_context_mod_refuse_s>();
      break;
    case types::f1_removal_fail:
      c.init<f1_removal_fail_s>();
      break;
    case types::bap_map_cfg_fail:
      c.init<bap_map_cfg_fail_s>();
      break;
    case types::gnb_du_res_cfg_fail:
      c.init<gnb_du_res_cfg_fail_s>();
      break;
    case types::iab_tnl_address_fail:
      c.init<iab_tnl_address_fail_s>();
      break;
    case types::iab_up_cfg_upd_fail:
      c.init<iab_up_cfg_upd_fail_s>();
      break;
    case types::res_status_fail:
      c.init<res_status_fail_s>();
      break;
    case types::positioning_meas_fail:
      c.init<positioning_meas_fail_s>();
      break;
    case types::trp_info_fail:
      c.init<trp_info_fail_s>();
      break;
    case types::positioning_info_fail:
      c.init<positioning_info_fail_s>();
      break;
    case types::positioning_activation_fail:
      c.init<positioning_activation_fail_s>();
      break;
    case types::e_c_id_meas_initiation_fail:
      c.init<e_c_id_meas_initiation_fail_s>();
      break;
    case types::broadcast_context_setup_fail:
      c.init<broadcast_context_setup_fail_s>();
      break;
    case types::broadcast_context_mod_fail:
      c.init<broadcast_context_mod_fail_s>();
      break;
    case types::multicast_context_setup_fail:
      c.init<multicast_context_setup_fail_s>();
      break;
    case types::multicast_context_mod_fail:
      c.init<multicast_context_mod_fail_s>();
      break;
    case types::multicast_distribution_setup_fail:
      c.init<multicast_distribution_setup_fail_s>();
      break;
    case types::pdc_meas_initiation_fail:
      c.init<pdc_meas_initiation_fail_s>();
      break;
    case types::prs_cfg_fail:
      c.init<prs_cfg_fail_s>();
      break;
    case types::meas_precfg_refuse:
      c.init<meas_precfg_refuse_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
f1ap_elem_procs_o::unsuccessful_outcome_c::unsuccessful_outcome_c(
    const f1ap_elem_procs_o::unsuccessful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::f1_setup_fail:
      c.init(other.c.get<f1_setup_fail_s>());
      break;
    case types::gnb_du_cfg_upd_fail:
      c.init(other.c.get<gnb_du_cfg_upd_fail_s>());
      break;
    case types::gnb_cu_cfg_upd_fail:
      c.init(other.c.get<gnb_cu_cfg_upd_fail_s>());
      break;
    case types::ue_context_setup_fail:
      c.init(other.c.get<ue_context_setup_fail_s>());
      break;
    case types::ue_context_mod_fail:
      c.init(other.c.get<ue_context_mod_fail_s>());
      break;
    case types::ue_context_mod_refuse:
      c.init(other.c.get<ue_context_mod_refuse_s>());
      break;
    case types::f1_removal_fail:
      c.init(other.c.get<f1_removal_fail_s>());
      break;
    case types::bap_map_cfg_fail:
      c.init(other.c.get<bap_map_cfg_fail_s>());
      break;
    case types::gnb_du_res_cfg_fail:
      c.init(other.c.get<gnb_du_res_cfg_fail_s>());
      break;
    case types::iab_tnl_address_fail:
      c.init(other.c.get<iab_tnl_address_fail_s>());
      break;
    case types::iab_up_cfg_upd_fail:
      c.init(other.c.get<iab_up_cfg_upd_fail_s>());
      break;
    case types::res_status_fail:
      c.init(other.c.get<res_status_fail_s>());
      break;
    case types::positioning_meas_fail:
      c.init(other.c.get<positioning_meas_fail_s>());
      break;
    case types::trp_info_fail:
      c.init(other.c.get<trp_info_fail_s>());
      break;
    case types::positioning_info_fail:
      c.init(other.c.get<positioning_info_fail_s>());
      break;
    case types::positioning_activation_fail:
      c.init(other.c.get<positioning_activation_fail_s>());
      break;
    case types::e_c_id_meas_initiation_fail:
      c.init(other.c.get<e_c_id_meas_initiation_fail_s>());
      break;
    case types::broadcast_context_setup_fail:
      c.init(other.c.get<broadcast_context_setup_fail_s>());
      break;
    case types::broadcast_context_mod_fail:
      c.init(other.c.get<broadcast_context_mod_fail_s>());
      break;
    case types::multicast_context_setup_fail:
      c.init(other.c.get<multicast_context_setup_fail_s>());
      break;
    case types::multicast_context_mod_fail:
      c.init(other.c.get<multicast_context_mod_fail_s>());
      break;
    case types::multicast_distribution_setup_fail:
      c.init(other.c.get<multicast_distribution_setup_fail_s>());
      break;
    case types::pdc_meas_initiation_fail:
      c.init(other.c.get<pdc_meas_initiation_fail_s>());
      break;
    case types::prs_cfg_fail:
      c.init(other.c.get<prs_cfg_fail_s>());
      break;
    case types::meas_precfg_refuse:
      c.init(other.c.get<meas_precfg_refuse_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
f1ap_elem_procs_o::unsuccessful_outcome_c&
f1ap_elem_procs_o::unsuccessful_outcome_c::operator=(const f1ap_elem_procs_o::unsuccessful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::f1_setup_fail:
      c.set(other.c.get<f1_setup_fail_s>());
      break;
    case types::gnb_du_cfg_upd_fail:
      c.set(other.c.get<gnb_du_cfg_upd_fail_s>());
      break;
    case types::gnb_cu_cfg_upd_fail:
      c.set(other.c.get<gnb_cu_cfg_upd_fail_s>());
      break;
    case types::ue_context_setup_fail:
      c.set(other.c.get<ue_context_setup_fail_s>());
      break;
    case types::ue_context_mod_fail:
      c.set(other.c.get<ue_context_mod_fail_s>());
      break;
    case types::ue_context_mod_refuse:
      c.set(other.c.get<ue_context_mod_refuse_s>());
      break;
    case types::f1_removal_fail:
      c.set(other.c.get<f1_removal_fail_s>());
      break;
    case types::bap_map_cfg_fail:
      c.set(other.c.get<bap_map_cfg_fail_s>());
      break;
    case types::gnb_du_res_cfg_fail:
      c.set(other.c.get<gnb_du_res_cfg_fail_s>());
      break;
    case types::iab_tnl_address_fail:
      c.set(other.c.get<iab_tnl_address_fail_s>());
      break;
    case types::iab_up_cfg_upd_fail:
      c.set(other.c.get<iab_up_cfg_upd_fail_s>());
      break;
    case types::res_status_fail:
      c.set(other.c.get<res_status_fail_s>());
      break;
    case types::positioning_meas_fail:
      c.set(other.c.get<positioning_meas_fail_s>());
      break;
    case types::trp_info_fail:
      c.set(other.c.get<trp_info_fail_s>());
      break;
    case types::positioning_info_fail:
      c.set(other.c.get<positioning_info_fail_s>());
      break;
    case types::positioning_activation_fail:
      c.set(other.c.get<positioning_activation_fail_s>());
      break;
    case types::e_c_id_meas_initiation_fail:
      c.set(other.c.get<e_c_id_meas_initiation_fail_s>());
      break;
    case types::broadcast_context_setup_fail:
      c.set(other.c.get<broadcast_context_setup_fail_s>());
      break;
    case types::broadcast_context_mod_fail:
      c.set(other.c.get<broadcast_context_mod_fail_s>());
      break;
    case types::multicast_context_setup_fail:
      c.set(other.c.get<multicast_context_setup_fail_s>());
      break;
    case types::multicast_context_mod_fail:
      c.set(other.c.get<multicast_context_mod_fail_s>());
      break;
    case types::multicast_distribution_setup_fail:
      c.set(other.c.get<multicast_distribution_setup_fail_s>());
      break;
    case types::pdc_meas_initiation_fail:
      c.set(other.c.get<pdc_meas_initiation_fail_s>());
      break;
    case types::prs_cfg_fail:
      c.set(other.c.get<prs_cfg_fail_s>());
      break;
    case types::meas_precfg_refuse:
      c.set(other.c.get<meas_precfg_refuse_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "f1ap_elem_procs_o::unsuccessful_outcome_c");
  }

  return *this;
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
