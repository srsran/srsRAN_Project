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

#include "srsran/asn1/ngap/ngap.h"
#include <sstream>

using namespace asn1;
using namespace asn1::ngap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

const char* cp_transport_layer_info_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"EndpointIPAddressAndPort"};
  return convert_enum_idx(names, 1, value, "cp_transport_layer_info_ext_ies_o::value_c::types");
}

const char* amf_tnl_assoc_to_rem_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"CPTransportLayerInformation"};
  return convert_enum_idx(names, 1, value, "amf_tnl_assoc_to_rem_item_ext_ies_o::ext_c::types");
}

const char* amf_cp_relocation_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "S-NSSAI", "AllowedNSSAI"};
  return convert_enum_idx(names, 4, value, "amf_cp_relocation_ind_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<amf_cp_relocation_ind_ies_o>;

const char* plmn_support_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NPN-Support", "ExtendedSliceSupportList", "OnboardingSupport"};
  return convert_enum_idx(names, 3, value, "plmn_support_item_ext_ies_o::ext_c::types");
}

const char* served_guami_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"GUAMIType"};
  return convert_enum_idx(names, 1, value, "served_guami_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<plmn_support_item_ext_ies_o>;

const char* amf_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PrintableString",
                                "ServedGUAMIList",
                                "INTEGER (0..255)",
                                "PLMNSupportList",
                                "AMF-TNLAssociationToAddList",
                                "AMF-TNLAssociationToRemoveList",
                                "AMF-TNLAssociationToUpdateList",
                                "Extended-AMFName"};
  return convert_enum_idx(names, 8, value, "amf_cfg_upd_ies_o::value_c::types");
}
uint8_t amf_cfg_upd_ies_o::value_c::types_opts::to_number() const
{
  if (value == relative_amf_capacity) {
    return 0;
  }
  invalid_enum_number(value, "amf_cfg_upd_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<amf_cfg_upd_ies_o>;

const char* amf_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"AMF-TNLAssociationSetupList", "TNLAssociationList", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "amf_cfg_upd_ack_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<amf_cfg_upd_ack_ies_o>;

const char* amf_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "amf_cfg_upd_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<amf_cfg_upd_fail_ies_o>;

const char* global_ran_node_id_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GlobalTNGF-ID", "GlobalTWIF-ID", "GlobalW-AGF-ID"};
  return convert_enum_idx(names, 3, value, "global_ran_node_id_ext_ies_o::value_c::types");
}

const char* amf_status_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UnavailableGUAMIList"};
  return convert_enum_idx(names, 1, value, "amf_status_ind_ies_o::value_c::types");
}

const char* qos_flow_item_with_data_forwarding_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..8,...)"};
  return convert_enum_idx(names, 1, value, "qos_flow_item_with_data_forwarding_ext_ies_o::ext_c::types");
}
uint8_t qos_flow_item_with_data_forwarding_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "qos_flow_item_with_data_forwarding_ext_ies_o::ext_c::types");
}

const char* add_dl_up_tnl_info_for_ho_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UPTransportLayerInformation"};
  return convert_enum_idx(names, 1, value, "add_dl_up_tnl_info_for_ho_item_ext_ies_o::ext_c::types");
}

const char* assist_data_for_paging_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NPN-PagingAssistanceInformation", "PagingAssisDataforCEcapabUE"};
  return convert_enum_idx(names, 2, value, "assist_data_for_paging_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<assist_data_for_paging_ext_ies_o>;

const char* associated_qos_flow_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..8,...)"};
  return convert_enum_idx(names, 1, value, "associated_qos_flow_item_ext_ies_o::ext_c::types");
}
uint8_t associated_qos_flow_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "associated_qos_flow_item_ext_ies_o::ext_c::types");
}

const char* broadcast_plmn_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NPN-Support", "ExtendedSliceSupportList", "TAINSAGSupportList"};
  return convert_enum_idx(names, 3, value, "broadcast_plmn_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<broadcast_plmn_item_ext_ies_o>;

const char* broadcast_session_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "OCTET STRING", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "broadcast_session_mod_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_mod_fail_ies_o>;

const char* broadcast_session_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "MBS-ServiceArea", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "broadcast_session_mod_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_mod_request_ies_o>;

const char* broadcast_session_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "OCTET STRING", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "broadcast_session_mod_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_mod_resp_ies_o>;

const char* broadcast_session_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "Cause"};
  return convert_enum_idx(names, 2, value, "broadcast_session_release_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_release_request_ies_o>;

const char* broadcast_session_release_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "Cause"};
  return convert_enum_idx(names, 2, value, "broadcast_session_release_required_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_release_required_ies_o>;

const char* broadcast_session_release_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "OCTET STRING", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "broadcast_session_release_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_release_resp_ies_o>;

const char* broadcast_session_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "OCTET STRING", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "broadcast_session_setup_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_setup_fail_ies_o>;

const char* broadcast_session_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "S-NSSAI", "MBS-ServiceArea", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "broadcast_session_setup_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_setup_request_ies_o>;

const char* broadcast_session_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "OCTET STRING", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "broadcast_session_setup_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<broadcast_session_setup_resp_ies_o>;

const char* cell_traffic_trace_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "NGRAN-CGI",
                                "BIT STRING",
                                "PrivacyIndicator",
                                "VisibleString"};
  return convert_enum_idx(names, 7, value, "cell_traffic_trace_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<cell_traffic_trace_ies_o>;

const char* conn_establishment_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "EndIndication",
                                "S-NSSAI",
                                "AllowedNSSAI",
                                "UE-DifferentiationInfo",
                                "DL-CP-SecurityInformation",
                                "INTEGER (0..255,...)",
                                "Enhanced-CoverageRestriction",
                                "CEmodeBrestricted",
                                "OCTET STRING",
                                "BIT STRING"};
  return convert_enum_idx(names, 13, value, "conn_establishment_ind_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<conn_establishment_ind_ies_o>;

const char* ue_radio_cap_for_paging_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING"};
  return convert_enum_idx(names, 1, value, "ue_radio_cap_for_paging_ext_ies_o::ext_c::types");
}

const char* core_network_assist_info_for_inactive_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"EUTRA-PagingeDRXInformation",
                                "BIT STRING",
                                "UERadioCapabilityForPaging",
                                "MicoAllPLMN",
                                "NR-PagingeDRXInformation",
                                "PagingCauseIndicationForVoiceService",
                                "PEIPSassistanceInformation"};
  return convert_enum_idx(names, 7, value, "core_network_assist_info_for_inactive_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<core_network_assist_info_for_inactive_ext_ies_o>;

const char* drbs_subject_to_status_transfer_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AssociatedQosFlowList"};
  return convert_enum_idx(names, 1, value, "drbs_subject_to_status_transfer_item_ext_ies_o::ext_c::types");
}

const char* drbs_to_qos_flows_map_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"DAPSRequestInfo"};
  return convert_enum_idx(names, 1, value, "drbs_to_qos_flows_map_item_ext_ies_o::ext_c::types");
}

const char* deactiv_trace_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "deactiv_trace_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<deactiv_trace_ies_o>;

const char* distribution_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "INTEGER (0..65535,...)", "OCTET STRING", "Cause"};
  return convert_enum_idx(names, 4, value, "distribution_release_request_ies_o::value_c::types");
}
uint8_t distribution_release_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "distribution_release_request_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<distribution_release_request_ies_o>;

const char* distribution_release_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "INTEGER (0..65535,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "distribution_release_resp_ies_o::value_c::types");
}
uint8_t distribution_release_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "distribution_release_resp_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<distribution_release_resp_ies_o>;

const char* distribution_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "MBS-SessionID", "INTEGER (0..65535,...)", "OCTET STRING", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "distribution_setup_fail_ies_o::value_c::types");
}
uint8_t distribution_setup_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "distribution_setup_fail_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<distribution_setup_fail_ies_o>;

const char* distribution_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "INTEGER (0..65535,...)", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "distribution_setup_request_ies_o::value_c::types");
}
uint8_t distribution_setup_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "distribution_setup_request_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<distribution_setup_request_ies_o>;

const char* distribution_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "INTEGER (0..65535,...)", "OCTET STRING", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "distribution_setup_resp_ies_o::value_c::types");
}
uint8_t distribution_setup_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "distribution_setup_resp_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<distribution_setup_resp_ies_o>;

const char* rat_restricts_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExtendedRATRestrictionInformation"};
  return convert_enum_idx(names, 1, value, "rat_restricts_item_ext_ies_o::ext_c::types");
}

const char* mob_restrict_list_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {
      "OCTET STRING", "CNTypeRestrictionsForServing", "CNTypeRestrictionsForEquivalent", "NPN-MobilityInformation"};
  return convert_enum_idx(names, 4, value, "mob_restrict_list_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<mob_restrict_list_ext_ies_o>;

const char* dl_nas_transport_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PrintableString",
                                "INTEGER (1..256)",
                                "OCTET STRING",
                                "MobilityRestrictionList",
                                "INTEGER (1..256,...)",
                                "UEAggregateMaximumBitRate",
                                "AllowedNSSAI",
                                "SRVCCOperationPossible",
                                "Enhanced-CoverageRestriction",
                                "INTEGER (0..255)",
                                "UE-DifferentiationInfo",
                                "CEmodeBrestricted",
                                "OCTET STRING",
                                "UECapabilityInfoRequest",
                                "EndIndication",
                                "OCTET STRING",
                                "TargetNSSAIInformation",
                                "BIT STRING"};
  return convert_enum_idx(names, 20, value, "dl_nas_transport_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_nas_transport_ies_o>;

const char* dl_non_ue_associated_nrppa_transport_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 2, value, "dl_non_ue_associated_nrppa_transport_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_non_ue_associated_nrppa_transport_ies_o>;

const char* xn_ext_tla_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SCTP-TLAs"};
  return convert_enum_idx(names, 1, value, "xn_ext_tla_item_ext_ies_o::ext_c::types");
}

const char* son_info_report_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SuccessfulHandoverReportList"};
  return convert_enum_idx(names, 1, value, "son_info_report_ext_ies_o::value_c::types");
}

const char* intersys_son_info_report_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"IntersystemCellStateIndication", "IntersystemResourceStatusReport"};
  return convert_enum_idx(names, 2, value, "intersys_son_info_report_ext_ies_o::value_c::types");
}

const char* intersys_son_info_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"IntersystemSONInformationRequest", "IntersystemSONInformationReply"};
  return convert_enum_idx(names, 2, value, "intersys_son_info_ext_ies_o::value_c::types");
}

const char* son_info_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SONInformationReport"};
  return convert_enum_idx(names, 1, value, "son_info_ext_ies_o::value_c::types");
}

const char* target_ran_node_id_son_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NR-CGI"};
  return convert_enum_idx(names, 1, value, "target_ran_node_id_son_ext_ies_o::ext_c::types");
}

const char* dl_ran_cfg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SONConfigurationTransfer", "OCTET STRING", "IntersystemSONConfigurationTransfer"};
  return convert_enum_idx(names, 3, value, "dl_ran_cfg_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_ran_cfg_transfer_ies_o>;

const char* dl_ran_early_status_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "EarlyStatusTransfer-TransparentContainer"};
  return convert_enum_idx(names, 3, value, "dl_ran_early_status_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_ran_early_status_transfer_ies_o>;

const char* dl_ran_status_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "RANStatusTransfer-TransparentContainer"};
  return convert_enum_idx(names, 3, value, "dl_ran_status_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_ran_status_transfer_ies_o>;

const char* dl_rim_info_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RIMInformationTransfer"};
  return convert_enum_idx(names, 1, value, "dl_rim_info_transfer_ies_o::value_c::types");
}

const char* dl_ue_associated_nrppa_transport_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "dl_ue_associated_nrppa_transport_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<dl_ue_associated_nrppa_transport_ies_o>;

const char* dyn_5qi_descriptor_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..65535,...)", "INTEGER (1..65535,...)", "INTEGER (1..65535,...)"};
  return convert_enum_idx(names, 3, value, "dyn_5qi_descriptor_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<dyn_5qi_descriptor_ext_ies_o>;

const char* erab_info_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING", "BIT STRING"};
  return convert_enum_idx(names, 2, value, "erab_info_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<erab_info_item_ext_ies_o>;

const char* error_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics", "FiveG-S-TMSI"};
  return convert_enum_idx(names, 5, value, "error_ind_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<error_ind_ies_o>;

const char* gbr_qos_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AlternativeQoSParaSetList"};
  return convert_enum_idx(names, 1, value, "gbr_qos_info_ext_ies_o::ext_c::types");
}

const char* global_line_id_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"TAI"};
  return convert_enum_idx(names, 1, value, "global_line_id_ext_ies_o::ext_c::types");
}

const char* ho_cancel_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(names, 3, value, "ho_cancel_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_cancel_ies_o>;

const char* ho_cancel_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "ho_cancel_ack_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_cancel_ack_ies_o>;

const char* ho_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "HandoverType",
                                "OCTET STRING",
                                "PDUSessionResourceHandoverList",
                                "PDUSessionResourceToReleaseListHOCmd",
                                "OCTET STRING",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 8, value, "ho_cmd_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_cmd_ies_o>;

const char* up_transport_layer_info_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING"};
  return convert_enum_idx(names, 1, value, "up_transport_layer_info_item_ext_ies_o::ext_c::types");
}

const char* ho_cmd_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QosFlowPerTNLInformationList",
                                "UPTransportLayerInformation",
                                "UPTransportLayerInformationList",
                                "DataForwardingResponseERABList",
                                "QosFlowListWithCause"};
  return convert_enum_idx(names, 5, value, "ho_cmd_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<ho_cmd_transfer_ext_ies_o>;

const char* ho_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "Cause", "CriticalityDiagnostics", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "ho_fail_ies_o::value_c::types");
}
uint8_t ho_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ho_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_fail_ies_o>;

const char* user_location_info_tngf_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"TAI"};
  return convert_enum_idx(names, 1, value, "user_location_info_tngf_ext_ies_o::ext_c::types");
}

const char* user_location_info_twif_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"TAI"};
  return convert_enum_idx(names, 1, value, "user_location_info_twif_ext_ies_o::ext_c::types");
}

const char* user_location_info_w_agf_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING", "HFCNode-ID-new", "GlobalCable-ID-new"};
  return convert_enum_idx(names, 3, value, "user_location_info_w_agf_ext_ies_o::value_c::types");
}

const char* user_location_info_eutra_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NGRAN-CGI"};
  return convert_enum_idx(names, 1, value, "user_location_info_eutra_ext_ies_o::ext_c::types");
}

const char* user_location_info_n3_iwf_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"TAI"};
  return convert_enum_idx(names, 1, value, "user_location_info_n3_iwf_ext_ies_o::ext_c::types");
}

const char* user_location_info_nr_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NGRAN-CGI", "BIT STRING", "NRNTNTAIInformation"};
  return convert_enum_idx(names, 3, value, "user_location_info_nr_ext_ies_o::ext_c::types");
}

const char* user_location_info_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "UserLocationInformationTNGF", "UserLocationInformationTWIF", "UserLocationInformationW-AGF"};
  return convert_enum_idx(names, 3, value, "user_location_info_ext_ies_o::value_c::types");
}

template struct asn1::protocol_ext_field_s<user_location_info_nr_ext_ies_o>;

const char* ho_notify_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "UserLocationInformation", "NotifySourceNGRANNode"};
  return convert_enum_idx(names, 4, value, "ho_notify_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_notify_ies_o>;

const char* ho_prep_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics", "OCTET STRING"};
  return convert_enum_idx(names, 5, value, "ho_prep_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_prep_fail_ies_o>;

const char* m1_periodic_report_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExtendedReportIntervalMDT"};
  return convert_enum_idx(names, 1, value, "m1_periodic_report_ext_ies_o::ext_c::types");
}

const char* m1_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"IncludeBeamMeasurementsIndication", "BeamMeasurementsReportConfiguration"};
  return convert_enum_idx(names, 2, value, "m1_cfg_ext_ies_o::ext_c::types");
}

const char* m4_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"M4ReportAmountMDT"};
  return convert_enum_idx(names, 1, value, "m4_cfg_ext_ies_o::ext_c::types");
}
uint8_t m4_cfg_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {4};
  return map_enum_number(numbers, 1, value, "m4_cfg_ext_ies_o::ext_c::types");
}

const char* m5_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"M5ReportAmountMDT"};
  return convert_enum_idx(names, 1, value, "m5_cfg_ext_ies_o::ext_c::types");
}
uint8_t m5_cfg_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "m5_cfg_ext_ies_o::ext_c::types");
}

const char* m6_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"M6ReportAmountMDT", "ExcessPacketDelayThresholdConfiguration"};
  return convert_enum_idx(names, 2, value, "m6_cfg_ext_ies_o::ext_c::types");
}
uint8_t m6_cfg_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {6};
  return map_enum_number(numbers, 1, value, "m6_cfg_ext_ies_o::ext_c::types");
}

const char* m7_cfg_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"M7ReportAmountMDT"};
  return convert_enum_idx(names, 1, value, "m7_cfg_ext_ies_o::ext_c::types");
}
uint8_t m7_cfg_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {7};
  return map_enum_number(numbers, 1, value, "m7_cfg_ext_ies_o::ext_c::types");
}

const char* logged_mdt_nr_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"EarlyMeasurement"};
  return convert_enum_idx(names, 1, value, "logged_mdt_nr_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<m1_cfg_ext_ies_o>;

template struct asn1::protocol_ext_field_s<m6_cfg_ext_ies_o>;

const char* pdu_session_res_setup_item_ho_req_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExpectedUEActivityBehaviour"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_setup_item_ho_req_ext_ies_o::ext_c::types");
}

const char* location_report_request_type_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"LocationReportingAdditionalInfo"};
  return convert_enum_idx(names, 1, value, "location_report_request_type_ext_ies_o::ext_c::types");
}

const char* trace_activation_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"MDT-Configuration", "VisibleString"};
  return convert_enum_idx(names, 2, value, "trace_activation_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<trace_activation_ext_ies_o>;

const char* ho_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "HandoverType",
                                "Cause",
                                "UEAggregateMaximumBitRate",
                                "CoreNetworkAssistanceInformationForInactive",
                                "UESecurityCapabilities",
                                "SecurityContext",
                                "NewSecurityContextInd",
                                "OCTET STRING",
                                "PDUSessionResourceSetupListHOReq",
                                "AllowedNSSAI",
                                "TraceActivation",
                                "BIT STRING",
                                "OCTET STRING",
                                "MobilityRestrictionList",
                                "LocationReportingRequestType",
                                "RRCInactiveTransitionReportRequest",
                                "GUAMI",
                                "RedirectionVoiceFallback",
                                "CNAssistedRANTuning",
                                "SRVCCOperationPossible",
                                "IAB-Authorized",
                                "Enhanced-CoverageRestriction",
                                "UE-DifferentiationInfo",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "PC5QoSParameters",
                                "CEmodeBrestricted",
                                "UE-UP-CIoT-Support",
                                "MDTPLMNList",
                                "OCTET STRING",
                                "INTEGER (0..255)",
                                "TimeSyncAssistanceInfo",
                                "UESliceMaximumBitRateList",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "FiveG-ProSePC5QoSParameters"};
  return convert_enum_idx(names, 39, value, "ho_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_request_ies_o>;

const char* ho_request_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceAdmittedList",
                                "PDUSessionResourceFailedToSetupListHOAck",
                                "OCTET STRING",
                                "CriticalityDiagnostics",
                                "NPN-AccessInformation",
                                "RedCapIndication"};
  return convert_enum_idx(names, 8, value, "ho_request_ack_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_request_ack_ies_o>;

const char* redundant_pdu_session_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..255,...)"};
  return convert_enum_idx(names, 1, value, "redundant_pdu_session_info_ext_ies_o::ext_c::types");
}
uint8_t redundant_pdu_session_info_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "redundant_pdu_session_info_ext_ies_o::ext_c::types");
}

const char* ho_request_ack_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"AdditionalDLUPTNLInformationForHOList",
                                "UPTransportLayerInformation",
                                "UPTransportLayerInformationList",
                                "DataForwardingResponseERABList",
                                "UPTransportLayerInformation",
                                "RedundantPDUSessionInformation",
                                "GlobalRANNodeID",
                                "MBS-SupportIndicator"};
  return convert_enum_idx(names, 8, value, "ho_request_ack_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<ho_request_ack_transfer_ext_ies_o>;

const char* target_id_ext_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"TargetRNC-ID", "TargetHomeENB-ID"};
  return convert_enum_idx(names, 2, value, "target_id_ext_ies_o::value_c::types");
}

const char* ho_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "HandoverType",
                                "Cause",
                                "TargetID",
                                "DirectForwardingPathAvailability",
                                "PDUSessionResourceListHORqd",
                                "OCTET STRING"};
  return convert_enum_idx(names, 8, value, "ho_required_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_required_ies_o>;

const char* ho_success_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)"};
  return convert_enum_idx(names, 2, value, "ho_success_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<ho_success_ies_o>;

const char* init_context_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceFailedToSetupListCxtFail",
                                "Cause",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "init_context_setup_fail_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<init_context_setup_fail_ies_o>;

const char* pdu_session_res_setup_item_cxt_req_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExpectedUEActivityBehaviour"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_setup_item_cxt_req_ext_ies_o::ext_c::types");
}

const char* init_context_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PrintableString",
                                "UEAggregateMaximumBitRate",
                                "CoreNetworkAssistanceInformationForInactive",
                                "GUAMI",
                                "PDUSessionResourceSetupListCxtReq",
                                "AllowedNSSAI",
                                "UESecurityCapabilities",
                                "BIT STRING",
                                "TraceActivation",
                                "MobilityRestrictionList",
                                "OCTET STRING",
                                "INTEGER (1..256,...)",
                                "BIT STRING",
                                "OCTET STRING",
                                "EmergencyFallbackIndicator",
                                "RRCInactiveTransitionReportRequest",
                                "UERadioCapabilityForPaging",
                                "RedirectionVoiceFallback",
                                "LocationReportingRequestType",
                                "CNAssistedRANTuning",
                                "SRVCCOperationPossible",
                                "IAB-Authorized",
                                "Enhanced-CoverageRestriction",
                                "INTEGER (0..255)",
                                "UE-DifferentiationInfo",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "PC5QoSParameters",
                                "CEmodeBrestricted",
                                "UE-UP-CIoT-Support",
                                "OCTET STRING",
                                "MDTPLMNList",
                                "OCTET STRING",
                                "TimeSyncAssistanceInfo",
                                "QMCConfigInfo",
                                "TargetNSSAIInformation",
                                "UESliceMaximumBitRateList",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "FiveG-ProSePC5QoSParameters"};
  return convert_enum_idx(names, 44, value, "init_context_setup_request_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<init_context_setup_request_ies_o>;

const char* init_context_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceSetupListCxtRes",
                                "PDUSessionResourceFailedToSetupListCxtRes",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "init_context_setup_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<init_context_setup_resp_ies_o>;

const char* init_ue_msg_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "UserLocationInformation",
                                "RRCEstablishmentCause",
                                "FiveG-S-TMSI",
                                "BIT STRING",
                                "UEContextRequest",
                                "AllowedNSSAI",
                                "SourceToTarget-AMFInformationReroute",
                                "OCTET STRING",
                                "IABNodeIndication",
                                "CEmodeBSupport-Indicator",
                                "LTEM-Indication",
                                "EDT-Session",
                                "AuthenticatedIndication",
                                "NPN-AccessInformation",
                                "RedCapIndication"};
  return convert_enum_idx(names, 17, value, "init_ue_msg_ies_o::value_c::types");
}
uint8_t init_ue_msg_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "init_ue_msg_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<init_ue_msg_ies_o>;

const char* pdu_session_res_item_cxt_rel_cpl_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_item_cxt_rel_cpl_ext_ies_o::ext_c::types");
}

const char* pdu_session_res_modify_item_mod_req_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"S-NSSAI", "ExpectedUEActivityBehaviour"};
  return convert_enum_idx(names, 2, value, "pdu_session_res_modify_item_mod_req_ext_ies_o::ext_c::types");
}

const char* pdu_session_res_setup_item_su_req_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExpectedUEActivityBehaviour"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_setup_item_su_req_ext_ies_o::ext_c::types");
}

const char* pdu_session_res_switched_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ExpectedUEActivityBehaviour"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_switched_item_ext_ies_o::ext_c::types");
}

const char* private_ie_id_c::types_opts::to_string() const
{
  static const char* names[] = {"local", "global"};
  return convert_enum_idx(names, 2, value, "private_ie_id_c::types");
}

const char* supported_ta_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ConfiguredTACIndication", "RAT-Information"};
  return convert_enum_idx(names, 2, value, "supported_ta_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<pdu_session_res_modify_item_mod_req_ext_ies_o>;

template struct asn1::protocol_ext_field_s<supported_ta_item_ext_ies_o>;

const char* location_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "UserLocationInformation",
                                "UEPresenceInAreaOfInterestList",
                                "LocationReportingRequestType"};
  return convert_enum_idx(names, 5, value, "location_report_ies_o::value_c::types");
}

const char* location_report_ctrl_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "LocationReportingRequestType"};
  return convert_enum_idx(names, 3, value, "location_report_ctrl_ies_o::value_c::types");
}

const char* location_report_fail_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "Cause"};
  return convert_enum_idx(names, 3, value, "location_report_fail_ind_ies_o::value_c::types");
}

const char* multicast_group_paging_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "MBS-ServiceArea", "MulticastGroupPagingAreaList"};
  return convert_enum_idx(names, 3, value, "multicast_group_paging_ies_o::value_c::types");
}

const char* multicast_session_activation_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "multicast_session_activation_fail_ies_o::value_c::types");
}

const char* multicast_session_activation_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "OCTET STRING"};
  return convert_enum_idx(names, 2, value, "multicast_session_activation_request_ies_o::value_c::types");
}

const char* multicast_session_activation_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "multicast_session_activation_resp_ies_o::value_c::types");
}

const char* multicast_session_deactivation_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "OCTET STRING"};
  return convert_enum_idx(names, 2, value, "multicast_session_deactivation_request_ies_o::value_c::types");
}

const char* multicast_session_deactivation_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "multicast_session_deactivation_resp_ies_o::value_c::types");
}

const char* multicast_session_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "INTEGER (0..65535,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "multicast_session_upd_fail_ies_o::value_c::types");
}
uint8_t multicast_session_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "multicast_session_upd_fail_ies_o::value_c::types");
  return 0;
}

const char* multicast_session_upd_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "INTEGER (0..65535,...)", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "multicast_session_upd_request_ies_o::value_c::types");
}
uint8_t multicast_session_upd_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "multicast_session_upd_request_ies_o::value_c::types");
  return 0;
}

const char* multicast_session_upd_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionID", "INTEGER (0..65535,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "multicast_session_upd_resp_ies_o::value_c::types");
}
uint8_t multicast_session_upd_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_area_session_id) {
    return 0;
  }
  invalid_enum_number(value, "multicast_session_upd_resp_ies_o::value_c::types");
  return 0;
}

const char* nas_non_delivery_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "OCTET STRING", "Cause"};
  return convert_enum_idx(names, 4, value, "nas_non_delivery_ind_ies_o::value_c::types");
}

const char* ng_reset_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-associatedLogicalNG-connectionList", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 2, value, "ng_reset_ack_ies_o::value_c::types");
}

const char* ng_reset_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cause", "ResetType"};
  return convert_enum_idx(names, 2, value, "ng_reset_ies_o::value_c::types");
}

const char* ng_setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "ng_setup_fail_ies_o::value_c::types");
}

const char* ng_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"GlobalRANNodeID",
                                "PrintableString",
                                "SupportedTAList",
                                "PagingDRX",
                                "UERetentionInformation",
                                "NB-IoT-DefaultPagingDRX",
                                "Extended-RANNodeName"};
  return convert_enum_idx(names, 7, value, "ng_setup_request_ies_o::value_c::types");
}

const char* ng_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PrintableString",
                                "ServedGUAMIList",
                                "INTEGER (0..255)",
                                "PLMNSupportList",
                                "CriticalityDiagnostics",
                                "UERetentionInformation",
                                "IAB-Supported",
                                "Extended-AMFName"};
  return convert_enum_idx(names, 8, value, "ng_setup_resp_ies_o::value_c::types");
}
uint8_t ng_setup_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == relative_amf_capacity) {
    return 0;
  }
  invalid_enum_number(value, "ng_setup_resp_ies_o::value_c::types");
  return 0;
}

const char* overload_start_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"OverloadResponse", "INTEGER (1..99)", "OverloadStartNSSAIList"};
  return convert_enum_idx(names, 3, value, "overload_start_ies_o::value_c::types");
}
uint8_t overload_start_ies_o::value_c::types_opts::to_number() const
{
  if (value == amf_traffic_load_reduction_ind) {
    return 1;
  }
  invalid_enum_number(value, "overload_start_ies_o::value_c::types");
  return 0;
}

const char* pdu_session_res_modify_confirm_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceModifyListModCfm",
                                "PDUSessionResourceFailedToModifyListModCfm",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "pdu_session_res_modify_confirm_ies_o::value_c::types");
}

const char* pdu_session_res_modify_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceModifyListModInd",
                                "UserLocationInformation"};
  return convert_enum_idx(names, 4, value, "pdu_session_res_modify_ind_ies_o::value_c::types");
}

const char* pdu_session_res_modify_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256)",
                                "PDUSessionResourceModifyListModReq"};
  return convert_enum_idx(names, 4, value, "pdu_session_res_modify_request_ies_o::value_c::types");
}

const char* pdu_session_res_modify_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceModifyListModRes",
                                "PDUSessionResourceFailedToModifyListModRes",
                                "UserLocationInformation",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 6, value, "pdu_session_res_modify_resp_ies_o::value_c::types");
}

const char* pdu_session_res_notify_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceNotifyList",
                                "PDUSessionResourceReleasedListNot",
                                "UserLocationInformation"};
  return convert_enum_idx(names, 5, value, "pdu_session_res_notify_ies_o::value_c::types");
}

const char* pdu_session_res_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256)",
                                "OCTET STRING",
                                "PDUSessionResourceToReleaseListRelCmd"};
  return convert_enum_idx(names, 5, value, "pdu_session_res_release_cmd_ies_o::value_c::types");
}

const char* pdu_session_res_release_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceReleasedListRelRes",
                                "UserLocationInformation",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "pdu_session_res_release_resp_ies_o::value_c::types");
}

const char* pdu_session_res_setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256)",
                                "OCTET STRING",
                                "PDUSessionResourceSetupListSUReq",
                                "UEAggregateMaximumBitRate",
                                "UESliceMaximumBitRateList"};
  return convert_enum_idx(names, 7, value, "pdu_session_res_setup_request_ies_o::value_c::types");
}

const char* pdu_session_res_setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceSetupListSURes",
                                "PDUSessionResourceFailedToSetupListSURes",
                                "CriticalityDiagnostics",
                                "UserLocationInformation"};
  return convert_enum_idx(names, 6, value, "pdu_session_res_setup_resp_ies_o::value_c::types");
}

const char* pws_cancel_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING", "BIT STRING", "WarningAreaList", "CancelAllWarningMessages"};
  return convert_enum_idx(names, 4, value, "pws_cancel_request_ies_o::value_c::types");
}

const char* pws_cancel_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING", "BIT STRING", "BroadcastCancelledAreaList", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "pws_cancel_resp_ies_o::value_c::types");
}

const char* pws_fail_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PWSFailedCellIDList", "GlobalRANNodeID"};
  return convert_enum_idx(names, 2, value, "pws_fail_ind_ies_o::value_c::types");
}

const char* pws_restart_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "CellIDListForRestart", "GlobalRANNodeID", "TAIListForRestart", "EmergencyAreaIDListForRestart"};
  return convert_enum_idx(names, 4, value, "pws_restart_ind_ies_o::value_c::types");
}

const char* paging_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UEPagingIdentity",
                                "PagingDRX",
                                "TAIListForPaging",
                                "PagingPriority",
                                "UERadioCapabilityForPaging",
                                "PagingOrigin",
                                "AssistanceDataForPaging",
                                "NB-IoT-Paging-eDRXInfo",
                                "NB-IoT-PagingDRX",
                                "Enhanced-CoverageRestriction",
                                "WUS-Assistance-Information",
                                "EUTRA-PagingeDRXInformation",
                                "CEmodeBrestricted",
                                "NR-PagingeDRXInformation",
                                "PagingCause",
                                "PEIPSassistanceInformation"};
  return convert_enum_idx(names, 16, value, "paging_ies_o::value_c::types");
}

const char* path_switch_request_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "UESecurityCapabilities",
                                "SecurityContext",
                                "NewSecurityContextInd",
                                "PDUSessionResourceSwitchedList",
                                "PDUSessionResourceReleasedListPSAck",
                                "AllowedNSSAI",
                                "CoreNetworkAssistanceInformationForInactive",
                                "RRCInactiveTransitionReportRequest",
                                "CriticalityDiagnostics",
                                "RedirectionVoiceFallback",
                                "CNAssistedRANTuning",
                                "SRVCCOperationPossible",
                                "Enhanced-CoverageRestriction",
                                "INTEGER (0..255)",
                                "UE-DifferentiationInfo",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "PC5QoSParameters",
                                "CEmodeBrestricted",
                                "UE-UP-CIoT-Support",
                                "OCTET STRING",
                                "MDTPLMNList",
                                "TimeSyncAssistanceInfo",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "FiveG-ProSePC5QoSParameters",
                                "MDTPLMNModificationList"};
  return convert_enum_idx(names, 31, value, "path_switch_request_ack_ies_o::value_c::types");
}

const char* path_switch_request_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceReleasedListPSFail",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "path_switch_request_fail_ies_o::value_c::types");
}

const char* path_switch_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..1099511627775)",
                                "UserLocationInformation",
                                "UESecurityCapabilities",
                                "PDUSessionResourceToBeSwitchedDLList",
                                "PDUSessionResourceFailedToSetupListPSReq",
                                "RRCEstablishmentCause",
                                "RedCapIndication"};
  return convert_enum_idx(names, 8, value, "path_switch_request_ies_o::value_c::types");
}

const char* ngap_private_ies_empty_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {};
  return convert_enum_idx(names, 0, value, "ngap_private_ies_empty_o::value_c::types");
}

const char* ran_cp_relocation_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..4294967295)", "FiveG-S-TMSI", "EUTRA-CGI", "TAI", "UL-CP-SecurityInformation"};
  return convert_enum_idx(names, 5, value, "ran_cp_relocation_ind_ies_o::value_c::types");
}
uint8_t ran_cp_relocation_ind_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "ran_cp_relocation_ind_ies_o::value_c::types");
}

const char* ran_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"CriticalityDiagnostics"};
  return convert_enum_idx(names, 1, value, "ran_cfg_upd_ack_ies_o::value_c::types");
}

const char* ran_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "ran_cfg_upd_fail_ies_o::value_c::types");
}

const char* ran_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PrintableString",
                                "SupportedTAList",
                                "PagingDRX",
                                "GlobalRANNodeID",
                                "NGRAN-TNLAssociationToRemoveList",
                                "NB-IoT-DefaultPagingDRX",
                                "Extended-RANNodeName"};
  return convert_enum_idx(names, 7, value, "ran_cfg_upd_ies_o::value_c::types");
}

const char* rrc_inactive_transition_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "RRCState", "UserLocationInformation"};
  return convert_enum_idx(names, 4, value, "rrc_inactive_transition_report_ies_o::value_c::types");
}

const char* reroute_nas_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "INTEGER (0..1099511627775)",
                                "OCTET STRING",
                                "BIT STRING",
                                "AllowedNSSAI",
                                "SourceToTarget-AMFInformationReroute"};
  return convert_enum_idx(names, 6, value, "reroute_nas_request_ies_o::value_c::types");
}

const char* retrieve_ue_info_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"FiveG-S-TMSI"};
  return convert_enum_idx(names, 1, value, "retrieve_ue_info_ies_o::value_c::types");
}

const char* secondary_rat_data_usage_report_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceSecondaryRATUsageList",
                                "HandoverFlag",
                                "UserLocationInformation"};
  return convert_enum_idx(names, 5, value, "secondary_rat_data_usage_report_ies_o::value_c::types");
}

const char* trace_fail_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "OCTET STRING", "Cause"};
  return convert_enum_idx(names, 4, value, "trace_fail_ind_ies_o::value_c::types");
}

const char* trace_start_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "TraceActivation"};
  return convert_enum_idx(names, 3, value, "trace_start_ies_o::value_c::types");
}

const char* ue_context_mod_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ue_context_mod_fail_ies_o::value_c::types");
}

const char* ue_context_mod_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "INTEGER (1..256)",
                                "BIT STRING",
                                "INTEGER (1..256,...)",
                                "UEAggregateMaximumBitRate",
                                "UESecurityCapabilities",
                                "CoreNetworkAssistanceInformationForInactive",
                                "EmergencyFallbackIndicator",
                                "INTEGER (0..1099511627775)",
                                "RRCInactiveTransitionReportRequest",
                                "GUAMI",
                                "CNAssistedRANTuning",
                                "SRVCCOperationPossible",
                                "IAB-Authorized",
                                "NRV2XServicesAuthorized",
                                "LTEV2XServicesAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "LTEUESidelinkAggregateMaximumBitrate",
                                "PC5QoSParameters",
                                "OCTET STRING",
                                "OCTET STRING",
                                "TimeSyncAssistanceInfo",
                                "QMCConfigInfo",
                                "QMCDeactivation",
                                "UESliceMaximumBitRateList",
                                "MDTPLMNModificationList",
                                "FiveG-ProSeAuthorized",
                                "NRUESidelinkAggregateMaximumBitrate",
                                "FiveG-ProSePC5QoSParameters"};
  return convert_enum_idx(names, 30, value, "ue_context_mod_request_ies_o::value_c::types");
}

const char* ue_context_mod_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "RRCState",
                                "UserLocationInformation",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 5, value, "ue_context_mod_resp_ies_o::value_c::types");
}

const char* ue_context_release_cmd_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"UE-NGAP-IDs", "Cause"};
  return convert_enum_idx(names, 2, value, "ue_context_release_cmd_ies_o::value_c::types");
}

const char* ue_context_release_complete_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "UserLocationInformation",
                                "InfoOnRecommendedCellsAndRANNodesForPaging",
                                "PDUSessionResourceListCxtRelCpl",
                                "CriticalityDiagnostics",
                                "PagingAssisDataforCEcapabUE"};
  return convert_enum_idx(names, 7, value, "ue_context_release_complete_ies_o::value_c::types");
}

const char* ue_context_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "PDUSessionResourceListCxtRelReq", "Cause"};
  return convert_enum_idx(names, 4, value, "ue_context_release_request_ies_o::value_c::types");
}

const char* ue_context_resume_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ue_context_resume_fail_ies_o::value_c::types");
}

const char* ue_context_resume_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "RRCEstablishmentCause",
                                "PDUSessionResourceResumeListRESReq",
                                "PDUSessionResourceFailedToResumeListRESReq",
                                "Suspend-Request-Indication",
                                "InfoOnRecommendedCellsAndRANNodesForPaging",
                                "PagingAssisDataforCEcapabUE"};
  return convert_enum_idx(names, 8, value, "ue_context_resume_request_ies_o::value_c::types");
}

const char* ue_context_resume_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "PDUSessionResourceResumeListRESRes",
                                "PDUSessionResourceFailedToResumeListRESRes",
                                "SecurityContext",
                                "Suspend-Response-Indication",
                                "INTEGER (0..255)",
                                "CriticalityDiagnostics"};
  return convert_enum_idx(names, 8, value, "ue_context_resume_resp_ies_o::value_c::types");
}

const char* ue_context_suspend_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ue_context_suspend_fail_ies_o::value_c::types");
}

const char* ue_context_suspend_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "InfoOnRecommendedCellsAndRANNodesForPaging",
                                "PagingAssisDataforCEcapabUE",
                                "PDUSessionResourceSuspendListSUSReq"};
  return convert_enum_idx(names, 5, value, "ue_context_suspend_request_ies_o::value_c::types");
}

const char* ue_context_suspend_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "SecurityContext", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ue_context_suspend_resp_ies_o::value_c::types");
}

const char* ue_info_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"FiveG-S-TMSI",
                                "INTEGER (0..255,...)",
                                "OCTET STRING",
                                "S-NSSAI",
                                "AllowedNSSAI",
                                "UE-DifferentiationInfo",
                                "BIT STRING"};
  return convert_enum_idx(names, 7, value, "ue_info_transfer_ies_o::value_c::types");
}
uint8_t ue_info_transfer_ies_o::value_c::types_opts::to_number() const
{
  if (value == nb_iot_ue_prio) {
    return 0;
  }
  invalid_enum_number(value, "ue_info_transfer_ies_o::value_c::types");
  return 0;
}

const char* ue_radio_cap_check_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "ue_radio_cap_check_request_ies_o::value_c::types");
}

const char* ue_radio_cap_check_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "IMSVoiceSupportIndicator", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "ue_radio_cap_check_resp_ies_o::value_c::types");
}

const char* ue_radio_cap_id_map_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING"};
  return convert_enum_idx(names, 1, value, "ue_radio_cap_id_map_request_ies_o::value_c::types");
}

const char* ue_radio_cap_id_map_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING", "OCTET STRING", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 3, value, "ue_radio_cap_id_map_resp_ies_o::value_c::types");
}

const char* ue_radio_cap_info_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "UERadioCapabilityForPaging",
                                "OCTET STRING"};
  return convert_enum_idx(names, 5, value, "ue_radio_cap_info_ind_ies_o::value_c::types");
}

const char* ue_tnla_binding_release_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)", "INTEGER (0..4294967295)"};
  return convert_enum_idx(names, 2, value, "ue_tnla_binding_release_request_ies_o::value_c::types");
}

const char* ul_nas_transport_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..1099511627775)",
                                "INTEGER (0..4294967295)",
                                "OCTET STRING",
                                "UserLocationInformation",
                                "OCTET STRING",
                                "OCTET STRING",
                                "OCTET STRING"};
  return convert_enum_idx(names, 7, value, "ul_nas_transport_ies_o::value_c::types");
}

const char* ul_non_ue_associated_nrppa_transport_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 2, value, "ul_non_ue_associated_nrppa_transport_ies_o::value_c::types");
}

const char* ul_ran_cfg_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"SONConfigurationTransfer", "OCTET STRING", "IntersystemSONConfigurationTransfer"};
  return convert_enum_idx(names, 3, value, "ul_ran_cfg_transfer_ies_o::value_c::types");
}

const char* ul_ran_early_status_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "EarlyStatusTransfer-TransparentContainer"};
  return convert_enum_idx(names, 3, value, "ul_ran_early_status_transfer_ies_o::value_c::types");
}

const char* ul_ran_status_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "RANStatusTransfer-TransparentContainer"};
  return convert_enum_idx(names, 3, value, "ul_ran_status_transfer_ies_o::value_c::types");
}

const char* ul_rim_info_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"RIMInformationTransfer"};
  return convert_enum_idx(names, 1, value, "ul_rim_info_transfer_ies_o::value_c::types");
}

const char* ul_ue_associated_nrppa_transport_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "INTEGER (0..1099511627775)", "INTEGER (0..4294967295)", "OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(names, 4, value, "ul_ue_associated_nrppa_transport_ies_o::value_c::types");
}

const char* write_replace_warning_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING",
                                "BIT STRING",
                                "WarningAreaList",
                                "INTEGER (0..131071)",
                                "INTEGER (0..65535)",
                                "OCTET STRING",
                                "OCTET STRING",
                                "BIT STRING",
                                "OCTET STRING",
                                "ConcurrentWarningMessageInd",
                                "OCTET STRING"};
  return convert_enum_idx(names, 11, value, "write_replace_warning_request_ies_o::value_c::types");
}

const char* write_replace_warning_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"BIT STRING", "BIT STRING", "BroadcastCompletedAreaList", "CriticalityDiagnostics"};
  return convert_enum_idx(names, 4, value, "write_replace_warning_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<location_report_ies_o>;

template struct asn1::protocol_ie_field_s<location_report_ctrl_ies_o>;

template struct asn1::protocol_ie_field_s<location_report_fail_ind_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_group_paging_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_activation_fail_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_activation_request_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_activation_resp_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_deactivation_request_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_deactivation_resp_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_upd_request_ies_o>;

template struct asn1::protocol_ie_field_s<multicast_session_upd_resp_ies_o>;

template struct asn1::protocol_ie_field_s<nas_non_delivery_ind_ies_o>;

template struct asn1::protocol_ie_field_s<ng_reset_ies_o>;

template struct asn1::protocol_ie_field_s<ng_reset_ack_ies_o>;

template struct asn1::protocol_ie_field_s<ng_setup_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ng_setup_request_ies_o>;

template struct asn1::protocol_ie_field_s<ng_setup_resp_ies_o>;

template struct asn1::protocol_ie_field_s<overload_start_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_modify_confirm_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_modify_ind_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_modify_request_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_modify_resp_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_notify_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_release_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_release_resp_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_setup_request_ies_o>;

template struct asn1::protocol_ie_field_s<pdu_session_res_setup_resp_ies_o>;

template struct asn1::protocol_ie_field_s<pws_cancel_request_ies_o>;

template struct asn1::protocol_ie_field_s<pws_cancel_resp_ies_o>;

template struct asn1::protocol_ie_field_s<pws_fail_ind_ies_o>;

template struct asn1::protocol_ie_field_s<pws_restart_ind_ies_o>;

template struct asn1::protocol_ie_field_s<paging_ies_o>;

template struct asn1::protocol_ie_field_s<path_switch_request_ies_o>;

template struct asn1::protocol_ie_field_s<path_switch_request_ack_ies_o>;

template struct asn1::protocol_ie_field_s<path_switch_request_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ran_cp_relocation_ind_ies_o>;

template struct asn1::protocol_ie_field_s<ran_cfg_upd_ies_o>;

template struct asn1::protocol_ie_field_s<ran_cfg_upd_fail_ies_o>;

template struct asn1::protocol_ie_field_s<rrc_inactive_transition_report_ies_o>;

template struct asn1::protocol_ie_field_s<reroute_nas_request_ies_o>;

template struct asn1::protocol_ie_field_s<secondary_rat_data_usage_report_ies_o>;

template struct asn1::protocol_ie_field_s<trace_fail_ind_ies_o>;

template struct asn1::protocol_ie_field_s<trace_start_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_mod_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_release_cmd_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_release_complete_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_release_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_resume_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_resume_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_resume_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_suspend_fail_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_suspend_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_context_suspend_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ue_info_transfer_ies_o>;

template struct asn1::protocol_ie_field_s<ue_radio_cap_check_request_ies_o>;

template struct asn1::protocol_ie_field_s<ue_radio_cap_check_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ue_radio_cap_id_map_resp_ies_o>;

template struct asn1::protocol_ie_field_s<ue_radio_cap_info_ind_ies_o>;

template struct asn1::protocol_ie_field_s<ue_tnla_binding_release_request_ies_o>;

template struct asn1::protocol_ie_field_s<ul_nas_transport_ies_o>;

template struct asn1::protocol_ie_field_s<ul_non_ue_associated_nrppa_transport_ies_o>;

template struct asn1::protocol_ie_field_s<ul_ran_cfg_transfer_ies_o>;

template struct asn1::protocol_ie_field_s<ul_ran_early_status_transfer_ies_o>;

template struct asn1::protocol_ie_field_s<ul_ran_status_transfer_ies_o>;

template struct asn1::protocol_ie_field_s<ul_ue_associated_nrppa_transport_ies_o>;

template struct asn1::protocol_ie_field_s<write_replace_warning_request_ies_o>;

template struct asn1::protocol_ie_field_s<write_replace_warning_resp_ies_o>;

// NGAP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NGAP-ELEMENTARY-PROCEDURE
uint16_t ngap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t names[] = {0,  66, 67, 68, 69, 70, 10, 12, 13, 14, 71, 72, 73, 20, 21, 25, 26, 27, 28,
                                   29, 32, 35, 40, 41, 58, 59, 43, 60, 51, 64, 1,  75, 2,  65, 3,  4,  5,  6,
                                   63, 7,  54, 8,  9,  11, 61, 15, 18, 16, 17, 74, 19, 22, 23, 24, 30, 31, 33,
                                   34, 57, 36, 55, 37, 52, 38, 39, 42, 56, 44, 45, 46, 47, 48, 62, 49, 53, 50};
  return map_enum_number(names, 76, idx, "proc_code");
}
bool ngap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t names[] = {0,  66, 67, 68, 69, 70, 10, 12, 13, 14, 71, 72, 73, 20, 21, 25, 26, 27, 28,
                                   29, 32, 35, 40, 41, 58, 59, 43, 60, 51, 64, 1,  75, 2,  65, 3,  4,  5,  6,
                                   63, 7,  54, 8,  9,  11, 61, 15, 18, 16, 17, 74, 19, 22, 23, 24, 30, 31, 33,
                                   34, 57, 36, 55, 37, 52, 38, 39, 42, 56, 44, 45, 46, 47, 48, 62, 49, 53, 50};
  for (const auto& o : names) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
ngap_elem_procs_o::init_msg_c ngap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(init_msg_c::types::amf_cfg_upd);
      break;
    case 66:
      ret.set(init_msg_c::types::broadcast_session_mod_request);
      break;
    case 67:
      ret.set(init_msg_c::types::broadcast_session_release_request);
      break;
    case 68:
      ret.set(init_msg_c::types::broadcast_session_setup_request);
      break;
    case 69:
      ret.set(init_msg_c::types::distribution_setup_request);
      break;
    case 70:
      ret.set(init_msg_c::types::distribution_release_request);
      break;
    case 10:
      ret.set(init_msg_c::types::ho_cancel);
      break;
    case 12:
      ret.set(init_msg_c::types::ho_required);
      break;
    case 13:
      ret.set(init_msg_c::types::ho_request);
      break;
    case 14:
      ret.set(init_msg_c::types::init_context_setup_request);
      break;
    case 71:
      ret.set(init_msg_c::types::multicast_session_activation_request);
      break;
    case 72:
      ret.set(init_msg_c::types::multicast_session_deactivation_request);
      break;
    case 73:
      ret.set(init_msg_c::types::multicast_session_upd_request);
      break;
    case 20:
      ret.set(init_msg_c::types::ng_reset);
      break;
    case 21:
      ret.set(init_msg_c::types::ng_setup_request);
      break;
    case 25:
      ret.set(init_msg_c::types::path_switch_request);
      break;
    case 26:
      ret.set(init_msg_c::types::pdu_session_res_modify_request);
      break;
    case 27:
      ret.set(init_msg_c::types::pdu_session_res_modify_ind);
      break;
    case 28:
      ret.set(init_msg_c::types::pdu_session_res_release_cmd);
      break;
    case 29:
      ret.set(init_msg_c::types::pdu_session_res_setup_request);
      break;
    case 32:
      ret.set(init_msg_c::types::pws_cancel_request);
      break;
    case 35:
      ret.set(init_msg_c::types::ran_cfg_upd);
      break;
    case 40:
      ret.set(init_msg_c::types::ue_context_mod_request);
      break;
    case 41:
      ret.set(init_msg_c::types::ue_context_release_cmd);
      break;
    case 58:
      ret.set(init_msg_c::types::ue_context_resume_request);
      break;
    case 59:
      ret.set(init_msg_c::types::ue_context_suspend_request);
      break;
    case 43:
      ret.set(init_msg_c::types::ue_radio_cap_check_request);
      break;
    case 60:
      ret.set(init_msg_c::types::ue_radio_cap_id_map_request);
      break;
    case 51:
      ret.set(init_msg_c::types::write_replace_warning_request);
      break;
    case 64:
      ret.set(init_msg_c::types::amf_cp_relocation_ind);
      break;
    case 1:
      ret.set(init_msg_c::types::amf_status_ind);
      break;
    case 75:
      ret.set(init_msg_c::types::broadcast_session_release_required);
      break;
    case 2:
      ret.set(init_msg_c::types::cell_traffic_trace);
      break;
    case 65:
      ret.set(init_msg_c::types::conn_establishment_ind);
      break;
    case 3:
      ret.set(init_msg_c::types::deactiv_trace);
      break;
    case 4:
      ret.set(init_msg_c::types::dl_nas_transport);
      break;
    case 5:
      ret.set(init_msg_c::types::dl_non_ue_associated_nrppa_transport);
      break;
    case 6:
      ret.set(init_msg_c::types::dl_ran_cfg_transfer);
      break;
    case 63:
      ret.set(init_msg_c::types::dl_ran_early_status_transfer);
      break;
    case 7:
      ret.set(init_msg_c::types::dl_ran_status_transfer);
      break;
    case 54:
      ret.set(init_msg_c::types::dl_rim_info_transfer);
      break;
    case 8:
      ret.set(init_msg_c::types::dl_ue_associated_nrppa_transport);
      break;
    case 9:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 11:
      ret.set(init_msg_c::types::ho_notify);
      break;
    case 61:
      ret.set(init_msg_c::types::ho_success);
      break;
    case 15:
      ret.set(init_msg_c::types::init_ue_msg);
      break;
    case 18:
      ret.set(init_msg_c::types::location_report);
      break;
    case 16:
      ret.set(init_msg_c::types::location_report_ctrl);
      break;
    case 17:
      ret.set(init_msg_c::types::location_report_fail_ind);
      break;
    case 74:
      ret.set(init_msg_c::types::multicast_group_paging);
      break;
    case 19:
      ret.set(init_msg_c::types::nas_non_delivery_ind);
      break;
    case 22:
      ret.set(init_msg_c::types::overload_start);
      break;
    case 23:
      ret.set(init_msg_c::types::overload_stop);
      break;
    case 24:
      ret.set(init_msg_c::types::paging);
      break;
    case 30:
      ret.set(init_msg_c::types::pdu_session_res_notify);
      break;
    case 31:
      ret.set(init_msg_c::types::private_msg);
      break;
    case 33:
      ret.set(init_msg_c::types::pws_fail_ind);
      break;
    case 34:
      ret.set(init_msg_c::types::pws_restart_ind);
      break;
    case 57:
      ret.set(init_msg_c::types::ran_cp_relocation_ind);
      break;
    case 36:
      ret.set(init_msg_c::types::reroute_nas_request);
      break;
    case 55:
      ret.set(init_msg_c::types::retrieve_ue_info);
      break;
    case 37:
      ret.set(init_msg_c::types::rrc_inactive_transition_report);
      break;
    case 52:
      ret.set(init_msg_c::types::secondary_rat_data_usage_report);
      break;
    case 38:
      ret.set(init_msg_c::types::trace_fail_ind);
      break;
    case 39:
      ret.set(init_msg_c::types::trace_start);
      break;
    case 42:
      ret.set(init_msg_c::types::ue_context_release_request);
      break;
    case 56:
      ret.set(init_msg_c::types::ue_info_transfer);
      break;
    case 44:
      ret.set(init_msg_c::types::ue_radio_cap_info_ind);
      break;
    case 45:
      ret.set(init_msg_c::types::ue_tnla_binding_release_request);
      break;
    case 46:
      ret.set(init_msg_c::types::ul_nas_transport);
      break;
    case 47:
      ret.set(init_msg_c::types::ul_non_ue_associated_nrppa_transport);
      break;
    case 48:
      ret.set(init_msg_c::types::ul_ran_cfg_transfer);
      break;
    case 62:
      ret.set(init_msg_c::types::ul_ran_early_status_transfer);
      break;
    case 49:
      ret.set(init_msg_c::types::ul_ran_status_transfer);
      break;
    case 53:
      ret.set(init_msg_c::types::ul_rim_info_transfer);
      break;
    case 50:
      ret.set(init_msg_c::types::ul_ue_associated_nrppa_transport);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
ngap_elem_procs_o::successful_outcome_c ngap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(successful_outcome_c::types::amf_cfg_upd_ack);
      break;
    case 66:
      ret.set(successful_outcome_c::types::broadcast_session_mod_resp);
      break;
    case 67:
      ret.set(successful_outcome_c::types::broadcast_session_release_resp);
      break;
    case 68:
      ret.set(successful_outcome_c::types::broadcast_session_setup_resp);
      break;
    case 69:
      ret.set(successful_outcome_c::types::distribution_setup_resp);
      break;
    case 70:
      ret.set(successful_outcome_c::types::distribution_release_resp);
      break;
    case 10:
      ret.set(successful_outcome_c::types::ho_cancel_ack);
      break;
    case 12:
      ret.set(successful_outcome_c::types::ho_cmd);
      break;
    case 13:
      ret.set(successful_outcome_c::types::ho_request_ack);
      break;
    case 14:
      ret.set(successful_outcome_c::types::init_context_setup_resp);
      break;
    case 71:
      ret.set(successful_outcome_c::types::multicast_session_activation_resp);
      break;
    case 72:
      ret.set(successful_outcome_c::types::multicast_session_deactivation_resp);
      break;
    case 73:
      ret.set(successful_outcome_c::types::multicast_session_upd_resp);
      break;
    case 20:
      ret.set(successful_outcome_c::types::ng_reset_ack);
      break;
    case 21:
      ret.set(successful_outcome_c::types::ng_setup_resp);
      break;
    case 25:
      ret.set(successful_outcome_c::types::path_switch_request_ack);
      break;
    case 26:
      ret.set(successful_outcome_c::types::pdu_session_res_modify_resp);
      break;
    case 27:
      ret.set(successful_outcome_c::types::pdu_session_res_modify_confirm);
      break;
    case 28:
      ret.set(successful_outcome_c::types::pdu_session_res_release_resp);
      break;
    case 29:
      ret.set(successful_outcome_c::types::pdu_session_res_setup_resp);
      break;
    case 32:
      ret.set(successful_outcome_c::types::pws_cancel_resp);
      break;
    case 35:
      ret.set(successful_outcome_c::types::ran_cfg_upd_ack);
      break;
    case 40:
      ret.set(successful_outcome_c::types::ue_context_mod_resp);
      break;
    case 41:
      ret.set(successful_outcome_c::types::ue_context_release_complete);
      break;
    case 58:
      ret.set(successful_outcome_c::types::ue_context_resume_resp);
      break;
    case 59:
      ret.set(successful_outcome_c::types::ue_context_suspend_resp);
      break;
    case 43:
      ret.set(successful_outcome_c::types::ue_radio_cap_check_resp);
      break;
    case 60:
      ret.set(successful_outcome_c::types::ue_radio_cap_id_map_resp);
      break;
    case 51:
      ret.set(successful_outcome_c::types::write_replace_warning_resp);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
ngap_elem_procs_o::unsuccessful_outcome_c ngap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 0:
      ret.set(unsuccessful_outcome_c::types::amf_cfg_upd_fail);
      break;
    case 66:
      ret.set(unsuccessful_outcome_c::types::broadcast_session_mod_fail);
      break;
    case 68:
      ret.set(unsuccessful_outcome_c::types::broadcast_session_setup_fail);
      break;
    case 69:
      ret.set(unsuccessful_outcome_c::types::distribution_setup_fail);
      break;
    case 12:
      ret.set(unsuccessful_outcome_c::types::ho_prep_fail);
      break;
    case 13:
      ret.set(unsuccessful_outcome_c::types::ho_fail);
      break;
    case 14:
      ret.set(unsuccessful_outcome_c::types::init_context_setup_fail);
      break;
    case 71:
      ret.set(unsuccessful_outcome_c::types::multicast_session_activation_fail);
      break;
    case 73:
      ret.set(unsuccessful_outcome_c::types::multicast_session_upd_fail);
      break;
    case 21:
      ret.set(unsuccessful_outcome_c::types::ng_setup_fail);
      break;
    case 25:
      ret.set(unsuccessful_outcome_c::types::path_switch_request_fail);
      break;
    case 35:
      ret.set(unsuccessful_outcome_c::types::ran_cfg_upd_fail);
      break;
    case 40:
      ret.set(unsuccessful_outcome_c::types::ue_context_mod_fail);
      break;
    case 58:
      ret.set(unsuccessful_outcome_c::types::ue_context_resume_fail);
      break;
    case 59:
      ret.set(unsuccessful_outcome_c::types::ue_context_suspend_fail);
      break;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return ret;
}
crit_e ngap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 0:
      return crit_e::reject;
    case 66:
      return crit_e::reject;
    case 67:
      return crit_e::reject;
    case 68:
      return crit_e::reject;
    case 69:
      return crit_e::reject;
    case 70:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 12:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 14:
      return crit_e::reject;
    case 71:
      return crit_e::reject;
    case 72:
      return crit_e::reject;
    case 73:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 21:
      return crit_e::reject;
    case 25:
      return crit_e::reject;
    case 26:
      return crit_e::reject;
    case 27:
      return crit_e::reject;
    case 28:
      return crit_e::reject;
    case 29:
      return crit_e::reject;
    case 32:
      return crit_e::reject;
    case 35:
      return crit_e::reject;
    case 40:
      return crit_e::reject;
    case 41:
      return crit_e::reject;
    case 58:
      return crit_e::reject;
    case 59:
      return crit_e::reject;
    case 43:
      return crit_e::reject;
    case 60:
      return crit_e::reject;
    case 51:
      return crit_e::reject;
    case 64:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 75:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    case 65:
      return crit_e::reject;
    case 3:
      return crit_e::ignore;
    case 4:
      return crit_e::ignore;
    case 5:
      return crit_e::ignore;
    case 6:
      return crit_e::ignore;
    case 63:
      return crit_e::ignore;
    case 7:
      return crit_e::ignore;
    case 54:
      return crit_e::ignore;
    case 8:
      return crit_e::ignore;
    case 9:
      return crit_e::ignore;
    case 11:
      return crit_e::ignore;
    case 61:
      return crit_e::ignore;
    case 15:
      return crit_e::ignore;
    case 18:
      return crit_e::ignore;
    case 16:
      return crit_e::ignore;
    case 17:
      return crit_e::ignore;
    case 74:
      return crit_e::ignore;
    case 19:
      return crit_e::ignore;
    case 22:
      return crit_e::ignore;
    case 23:
      return crit_e::reject;
    case 24:
      return crit_e::ignore;
    case 30:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 33:
      return crit_e::ignore;
    case 34:
      return crit_e::ignore;
    case 57:
      return crit_e::reject;
    case 36:
      return crit_e::reject;
    case 55:
      return crit_e::reject;
    case 37:
      return crit_e::ignore;
    case 52:
      return crit_e::ignore;
    case 38:
      return crit_e::ignore;
    case 39:
      return crit_e::ignore;
    case 42:
      return crit_e::ignore;
    case 56:
      return crit_e::reject;
    case 44:
      return crit_e::ignore;
    case 45:
      return crit_e::ignore;
    case 46:
      return crit_e::ignore;
    case 47:
      return crit_e::ignore;
    case 48:
      return crit_e::ignore;
    case 62:
      return crit_e::reject;
    case 49:
      return crit_e::ignore;
    case 53:
      return crit_e::ignore;
    case 50:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code={} is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void ngap_elem_procs_o::init_msg_c::destroy_()
{
  switch (type_) {
    case types::amf_cfg_upd:
      c.destroy<amf_cfg_upd_s>();
      break;
    case types::broadcast_session_mod_request:
      c.destroy<broadcast_session_mod_request_s>();
      break;
    case types::broadcast_session_release_request:
      c.destroy<broadcast_session_release_request_s>();
      break;
    case types::broadcast_session_setup_request:
      c.destroy<broadcast_session_setup_request_s>();
      break;
    case types::distribution_setup_request:
      c.destroy<distribution_setup_request_s>();
      break;
    case types::distribution_release_request:
      c.destroy<distribution_release_request_s>();
      break;
    case types::ho_cancel:
      c.destroy<ho_cancel_s>();
      break;
    case types::ho_required:
      c.destroy<ho_required_s>();
      break;
    case types::ho_request:
      c.destroy<ho_request_s>();
      break;
    case types::init_context_setup_request:
      c.destroy<init_context_setup_request_s>();
      break;
    case types::multicast_session_activation_request:
      c.destroy<multicast_session_activation_request_s>();
      break;
    case types::multicast_session_deactivation_request:
      c.destroy<multicast_session_deactivation_request_s>();
      break;
    case types::multicast_session_upd_request:
      c.destroy<multicast_session_upd_request_s>();
      break;
    case types::ng_reset:
      c.destroy<ng_reset_s>();
      break;
    case types::ng_setup_request:
      c.destroy<ng_setup_request_s>();
      break;
    case types::path_switch_request:
      c.destroy<path_switch_request_s>();
      break;
    case types::pdu_session_res_modify_request:
      c.destroy<pdu_session_res_modify_request_s>();
      break;
    case types::pdu_session_res_modify_ind:
      c.destroy<pdu_session_res_modify_ind_s>();
      break;
    case types::pdu_session_res_release_cmd:
      c.destroy<pdu_session_res_release_cmd_s>();
      break;
    case types::pdu_session_res_setup_request:
      c.destroy<pdu_session_res_setup_request_s>();
      break;
    case types::pws_cancel_request:
      c.destroy<pws_cancel_request_s>();
      break;
    case types::ran_cfg_upd:
      c.destroy<ran_cfg_upd_s>();
      break;
    case types::ue_context_mod_request:
      c.destroy<ue_context_mod_request_s>();
      break;
    case types::ue_context_release_cmd:
      c.destroy<ue_context_release_cmd_s>();
      break;
    case types::ue_context_resume_request:
      c.destroy<ue_context_resume_request_s>();
      break;
    case types::ue_context_suspend_request:
      c.destroy<ue_context_suspend_request_s>();
      break;
    case types::ue_radio_cap_check_request:
      c.destroy<ue_radio_cap_check_request_s>();
      break;
    case types::ue_radio_cap_id_map_request:
      c.destroy<ue_radio_cap_id_map_request_s>();
      break;
    case types::write_replace_warning_request:
      c.destroy<write_replace_warning_request_s>();
      break;
    case types::amf_cp_relocation_ind:
      c.destroy<amf_cp_relocation_ind_s>();
      break;
    case types::amf_status_ind:
      c.destroy<amf_status_ind_s>();
      break;
    case types::broadcast_session_release_required:
      c.destroy<broadcast_session_release_required_s>();
      break;
    case types::cell_traffic_trace:
      c.destroy<cell_traffic_trace_s>();
      break;
    case types::conn_establishment_ind:
      c.destroy<conn_establishment_ind_s>();
      break;
    case types::deactiv_trace:
      c.destroy<deactiv_trace_s>();
      break;
    case types::dl_nas_transport:
      c.destroy<dl_nas_transport_s>();
      break;
    case types::dl_non_ue_associated_nrppa_transport:
      c.destroy<dl_non_ue_associated_nrppa_transport_s>();
      break;
    case types::dl_ran_cfg_transfer:
      c.destroy<dl_ran_cfg_transfer_s>();
      break;
    case types::dl_ran_early_status_transfer:
      c.destroy<dl_ran_early_status_transfer_s>();
      break;
    case types::dl_ran_status_transfer:
      c.destroy<dl_ran_status_transfer_s>();
      break;
    case types::dl_rim_info_transfer:
      c.destroy<dl_rim_info_transfer_s>();
      break;
    case types::dl_ue_associated_nrppa_transport:
      c.destroy<dl_ue_associated_nrppa_transport_s>();
      break;
    case types::error_ind:
      c.destroy<error_ind_s>();
      break;
    case types::ho_notify:
      c.destroy<ho_notify_s>();
      break;
    case types::ho_success:
      c.destroy<ho_success_s>();
      break;
    case types::init_ue_msg:
      c.destroy<init_ue_msg_s>();
      break;
    case types::location_report:
      c.destroy<location_report_s>();
      break;
    case types::location_report_ctrl:
      c.destroy<location_report_ctrl_s>();
      break;
    case types::location_report_fail_ind:
      c.destroy<location_report_fail_ind_s>();
      break;
    case types::multicast_group_paging:
      c.destroy<multicast_group_paging_s>();
      break;
    case types::nas_non_delivery_ind:
      c.destroy<nas_non_delivery_ind_s>();
      break;
    case types::overload_start:
      c.destroy<overload_start_s>();
      break;
    case types::overload_stop:
      c.destroy<overload_stop_s>();
      break;
    case types::paging:
      c.destroy<paging_s>();
      break;
    case types::pdu_session_res_notify:
      c.destroy<pdu_session_res_notify_s>();
      break;
    case types::private_msg:
      c.destroy<private_msg_s>();
      break;
    case types::pws_fail_ind:
      c.destroy<pws_fail_ind_s>();
      break;
    case types::pws_restart_ind:
      c.destroy<pws_restart_ind_s>();
      break;
    case types::ran_cp_relocation_ind:
      c.destroy<ran_cp_relocation_ind_s>();
      break;
    case types::reroute_nas_request:
      c.destroy<reroute_nas_request_s>();
      break;
    case types::retrieve_ue_info:
      c.destroy<retrieve_ue_info_s>();
      break;
    case types::rrc_inactive_transition_report:
      c.destroy<rrc_inactive_transition_report_s>();
      break;
    case types::secondary_rat_data_usage_report:
      c.destroy<secondary_rat_data_usage_report_s>();
      break;
    case types::trace_fail_ind:
      c.destroy<trace_fail_ind_s>();
      break;
    case types::trace_start:
      c.destroy<trace_start_s>();
      break;
    case types::ue_context_release_request:
      c.destroy<ue_context_release_request_s>();
      break;
    case types::ue_info_transfer:
      c.destroy<ue_info_transfer_s>();
      break;
    case types::ue_radio_cap_info_ind:
      c.destroy<ue_radio_cap_info_ind_s>();
      break;
    case types::ue_tnla_binding_release_request:
      c.destroy<ue_tnla_binding_release_request_s>();
      break;
    case types::ul_nas_transport:
      c.destroy<ul_nas_transport_s>();
      break;
    case types::ul_non_ue_associated_nrppa_transport:
      c.destroy<ul_non_ue_associated_nrppa_transport_s>();
      break;
    case types::ul_ran_cfg_transfer:
      c.destroy<ul_ran_cfg_transfer_s>();
      break;
    case types::ul_ran_early_status_transfer:
      c.destroy<ul_ran_early_status_transfer_s>();
      break;
    case types::ul_ran_status_transfer:
      c.destroy<ul_ran_status_transfer_s>();
      break;
    case types::ul_rim_info_transfer:
      c.destroy<ul_rim_info_transfer_s>();
      break;
    case types::ul_ue_associated_nrppa_transport:
      c.destroy<ul_ue_associated_nrppa_transport_s>();
      break;
    default:
      break;
  }
}
void ngap_elem_procs_o::init_msg_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::amf_cfg_upd:
      c.init<amf_cfg_upd_s>();
      break;
    case types::broadcast_session_mod_request:
      c.init<broadcast_session_mod_request_s>();
      break;
    case types::broadcast_session_release_request:
      c.init<broadcast_session_release_request_s>();
      break;
    case types::broadcast_session_setup_request:
      c.init<broadcast_session_setup_request_s>();
      break;
    case types::distribution_setup_request:
      c.init<distribution_setup_request_s>();
      break;
    case types::distribution_release_request:
      c.init<distribution_release_request_s>();
      break;
    case types::ho_cancel:
      c.init<ho_cancel_s>();
      break;
    case types::ho_required:
      c.init<ho_required_s>();
      break;
    case types::ho_request:
      c.init<ho_request_s>();
      break;
    case types::init_context_setup_request:
      c.init<init_context_setup_request_s>();
      break;
    case types::multicast_session_activation_request:
      c.init<multicast_session_activation_request_s>();
      break;
    case types::multicast_session_deactivation_request:
      c.init<multicast_session_deactivation_request_s>();
      break;
    case types::multicast_session_upd_request:
      c.init<multicast_session_upd_request_s>();
      break;
    case types::ng_reset:
      c.init<ng_reset_s>();
      break;
    case types::ng_setup_request:
      c.init<ng_setup_request_s>();
      break;
    case types::path_switch_request:
      c.init<path_switch_request_s>();
      break;
    case types::pdu_session_res_modify_request:
      c.init<pdu_session_res_modify_request_s>();
      break;
    case types::pdu_session_res_modify_ind:
      c.init<pdu_session_res_modify_ind_s>();
      break;
    case types::pdu_session_res_release_cmd:
      c.init<pdu_session_res_release_cmd_s>();
      break;
    case types::pdu_session_res_setup_request:
      c.init<pdu_session_res_setup_request_s>();
      break;
    case types::pws_cancel_request:
      c.init<pws_cancel_request_s>();
      break;
    case types::ran_cfg_upd:
      c.init<ran_cfg_upd_s>();
      break;
    case types::ue_context_mod_request:
      c.init<ue_context_mod_request_s>();
      break;
    case types::ue_context_release_cmd:
      c.init<ue_context_release_cmd_s>();
      break;
    case types::ue_context_resume_request:
      c.init<ue_context_resume_request_s>();
      break;
    case types::ue_context_suspend_request:
      c.init<ue_context_suspend_request_s>();
      break;
    case types::ue_radio_cap_check_request:
      c.init<ue_radio_cap_check_request_s>();
      break;
    case types::ue_radio_cap_id_map_request:
      c.init<ue_radio_cap_id_map_request_s>();
      break;
    case types::write_replace_warning_request:
      c.init<write_replace_warning_request_s>();
      break;
    case types::amf_cp_relocation_ind:
      c.init<amf_cp_relocation_ind_s>();
      break;
    case types::amf_status_ind:
      c.init<amf_status_ind_s>();
      break;
    case types::broadcast_session_release_required:
      c.init<broadcast_session_release_required_s>();
      break;
    case types::cell_traffic_trace:
      c.init<cell_traffic_trace_s>();
      break;
    case types::conn_establishment_ind:
      c.init<conn_establishment_ind_s>();
      break;
    case types::deactiv_trace:
      c.init<deactiv_trace_s>();
      break;
    case types::dl_nas_transport:
      c.init<dl_nas_transport_s>();
      break;
    case types::dl_non_ue_associated_nrppa_transport:
      c.init<dl_non_ue_associated_nrppa_transport_s>();
      break;
    case types::dl_ran_cfg_transfer:
      c.init<dl_ran_cfg_transfer_s>();
      break;
    case types::dl_ran_early_status_transfer:
      c.init<dl_ran_early_status_transfer_s>();
      break;
    case types::dl_ran_status_transfer:
      c.init<dl_ran_status_transfer_s>();
      break;
    case types::dl_rim_info_transfer:
      c.init<dl_rim_info_transfer_s>();
      break;
    case types::dl_ue_associated_nrppa_transport:
      c.init<dl_ue_associated_nrppa_transport_s>();
      break;
    case types::error_ind:
      c.init<error_ind_s>();
      break;
    case types::ho_notify:
      c.init<ho_notify_s>();
      break;
    case types::ho_success:
      c.init<ho_success_s>();
      break;
    case types::init_ue_msg:
      c.init<init_ue_msg_s>();
      break;
    case types::location_report:
      c.init<location_report_s>();
      break;
    case types::location_report_ctrl:
      c.init<location_report_ctrl_s>();
      break;
    case types::location_report_fail_ind:
      c.init<location_report_fail_ind_s>();
      break;
    case types::multicast_group_paging:
      c.init<multicast_group_paging_s>();
      break;
    case types::nas_non_delivery_ind:
      c.init<nas_non_delivery_ind_s>();
      break;
    case types::overload_start:
      c.init<overload_start_s>();
      break;
    case types::overload_stop:
      c.init<overload_stop_s>();
      break;
    case types::paging:
      c.init<paging_s>();
      break;
    case types::pdu_session_res_notify:
      c.init<pdu_session_res_notify_s>();
      break;
    case types::private_msg:
      c.init<private_msg_s>();
      break;
    case types::pws_fail_ind:
      c.init<pws_fail_ind_s>();
      break;
    case types::pws_restart_ind:
      c.init<pws_restart_ind_s>();
      break;
    case types::ran_cp_relocation_ind:
      c.init<ran_cp_relocation_ind_s>();
      break;
    case types::reroute_nas_request:
      c.init<reroute_nas_request_s>();
      break;
    case types::retrieve_ue_info:
      c.init<retrieve_ue_info_s>();
      break;
    case types::rrc_inactive_transition_report:
      c.init<rrc_inactive_transition_report_s>();
      break;
    case types::secondary_rat_data_usage_report:
      c.init<secondary_rat_data_usage_report_s>();
      break;
    case types::trace_fail_ind:
      c.init<trace_fail_ind_s>();
      break;
    case types::trace_start:
      c.init<trace_start_s>();
      break;
    case types::ue_context_release_request:
      c.init<ue_context_release_request_s>();
      break;
    case types::ue_info_transfer:
      c.init<ue_info_transfer_s>();
      break;
    case types::ue_radio_cap_info_ind:
      c.init<ue_radio_cap_info_ind_s>();
      break;
    case types::ue_tnla_binding_release_request:
      c.init<ue_tnla_binding_release_request_s>();
      break;
    case types::ul_nas_transport:
      c.init<ul_nas_transport_s>();
      break;
    case types::ul_non_ue_associated_nrppa_transport:
      c.init<ul_non_ue_associated_nrppa_transport_s>();
      break;
    case types::ul_ran_cfg_transfer:
      c.init<ul_ran_cfg_transfer_s>();
      break;
    case types::ul_ran_early_status_transfer:
      c.init<ul_ran_early_status_transfer_s>();
      break;
    case types::ul_ran_status_transfer:
      c.init<ul_ran_status_transfer_s>();
      break;
    case types::ul_rim_info_transfer:
      c.init<ul_rim_info_transfer_s>();
      break;
    case types::ul_ue_associated_nrppa_transport:
      c.init<ul_ue_associated_nrppa_transport_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::init_msg_c");
  }
}
ngap_elem_procs_o::init_msg_c::init_msg_c(const ngap_elem_procs_o::init_msg_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::amf_cfg_upd:
      c.init(other.c.get<amf_cfg_upd_s>());
      break;
    case types::broadcast_session_mod_request:
      c.init(other.c.get<broadcast_session_mod_request_s>());
      break;
    case types::broadcast_session_release_request:
      c.init(other.c.get<broadcast_session_release_request_s>());
      break;
    case types::broadcast_session_setup_request:
      c.init(other.c.get<broadcast_session_setup_request_s>());
      break;
    case types::distribution_setup_request:
      c.init(other.c.get<distribution_setup_request_s>());
      break;
    case types::distribution_release_request:
      c.init(other.c.get<distribution_release_request_s>());
      break;
    case types::ho_cancel:
      c.init(other.c.get<ho_cancel_s>());
      break;
    case types::ho_required:
      c.init(other.c.get<ho_required_s>());
      break;
    case types::ho_request:
      c.init(other.c.get<ho_request_s>());
      break;
    case types::init_context_setup_request:
      c.init(other.c.get<init_context_setup_request_s>());
      break;
    case types::multicast_session_activation_request:
      c.init(other.c.get<multicast_session_activation_request_s>());
      break;
    case types::multicast_session_deactivation_request:
      c.init(other.c.get<multicast_session_deactivation_request_s>());
      break;
    case types::multicast_session_upd_request:
      c.init(other.c.get<multicast_session_upd_request_s>());
      break;
    case types::ng_reset:
      c.init(other.c.get<ng_reset_s>());
      break;
    case types::ng_setup_request:
      c.init(other.c.get<ng_setup_request_s>());
      break;
    case types::path_switch_request:
      c.init(other.c.get<path_switch_request_s>());
      break;
    case types::pdu_session_res_modify_request:
      c.init(other.c.get<pdu_session_res_modify_request_s>());
      break;
    case types::pdu_session_res_modify_ind:
      c.init(other.c.get<pdu_session_res_modify_ind_s>());
      break;
    case types::pdu_session_res_release_cmd:
      c.init(other.c.get<pdu_session_res_release_cmd_s>());
      break;
    case types::pdu_session_res_setup_request:
      c.init(other.c.get<pdu_session_res_setup_request_s>());
      break;
    case types::pws_cancel_request:
      c.init(other.c.get<pws_cancel_request_s>());
      break;
    case types::ran_cfg_upd:
      c.init(other.c.get<ran_cfg_upd_s>());
      break;
    case types::ue_context_mod_request:
      c.init(other.c.get<ue_context_mod_request_s>());
      break;
    case types::ue_context_release_cmd:
      c.init(other.c.get<ue_context_release_cmd_s>());
      break;
    case types::ue_context_resume_request:
      c.init(other.c.get<ue_context_resume_request_s>());
      break;
    case types::ue_context_suspend_request:
      c.init(other.c.get<ue_context_suspend_request_s>());
      break;
    case types::ue_radio_cap_check_request:
      c.init(other.c.get<ue_radio_cap_check_request_s>());
      break;
    case types::ue_radio_cap_id_map_request:
      c.init(other.c.get<ue_radio_cap_id_map_request_s>());
      break;
    case types::write_replace_warning_request:
      c.init(other.c.get<write_replace_warning_request_s>());
      break;
    case types::amf_cp_relocation_ind:
      c.init(other.c.get<amf_cp_relocation_ind_s>());
      break;
    case types::amf_status_ind:
      c.init(other.c.get<amf_status_ind_s>());
      break;
    case types::broadcast_session_release_required:
      c.init(other.c.get<broadcast_session_release_required_s>());
      break;
    case types::cell_traffic_trace:
      c.init(other.c.get<cell_traffic_trace_s>());
      break;
    case types::conn_establishment_ind:
      c.init(other.c.get<conn_establishment_ind_s>());
      break;
    case types::deactiv_trace:
      c.init(other.c.get<deactiv_trace_s>());
      break;
    case types::dl_nas_transport:
      c.init(other.c.get<dl_nas_transport_s>());
      break;
    case types::dl_non_ue_associated_nrppa_transport:
      c.init(other.c.get<dl_non_ue_associated_nrppa_transport_s>());
      break;
    case types::dl_ran_cfg_transfer:
      c.init(other.c.get<dl_ran_cfg_transfer_s>());
      break;
    case types::dl_ran_early_status_transfer:
      c.init(other.c.get<dl_ran_early_status_transfer_s>());
      break;
    case types::dl_ran_status_transfer:
      c.init(other.c.get<dl_ran_status_transfer_s>());
      break;
    case types::dl_rim_info_transfer:
      c.init(other.c.get<dl_rim_info_transfer_s>());
      break;
    case types::dl_ue_associated_nrppa_transport:
      c.init(other.c.get<dl_ue_associated_nrppa_transport_s>());
      break;
    case types::error_ind:
      c.init(other.c.get<error_ind_s>());
      break;
    case types::ho_notify:
      c.init(other.c.get<ho_notify_s>());
      break;
    case types::ho_success:
      c.init(other.c.get<ho_success_s>());
      break;
    case types::init_ue_msg:
      c.init(other.c.get<init_ue_msg_s>());
      break;
    case types::location_report:
      c.init(other.c.get<location_report_s>());
      break;
    case types::location_report_ctrl:
      c.init(other.c.get<location_report_ctrl_s>());
      break;
    case types::location_report_fail_ind:
      c.init(other.c.get<location_report_fail_ind_s>());
      break;
    case types::multicast_group_paging:
      c.init(other.c.get<multicast_group_paging_s>());
      break;
    case types::nas_non_delivery_ind:
      c.init(other.c.get<nas_non_delivery_ind_s>());
      break;
    case types::overload_start:
      c.init(other.c.get<overload_start_s>());
      break;
    case types::overload_stop:
      c.init(other.c.get<overload_stop_s>());
      break;
    case types::paging:
      c.init(other.c.get<paging_s>());
      break;
    case types::pdu_session_res_notify:
      c.init(other.c.get<pdu_session_res_notify_s>());
      break;
    case types::private_msg:
      c.init(other.c.get<private_msg_s>());
      break;
    case types::pws_fail_ind:
      c.init(other.c.get<pws_fail_ind_s>());
      break;
    case types::pws_restart_ind:
      c.init(other.c.get<pws_restart_ind_s>());
      break;
    case types::ran_cp_relocation_ind:
      c.init(other.c.get<ran_cp_relocation_ind_s>());
      break;
    case types::reroute_nas_request:
      c.init(other.c.get<reroute_nas_request_s>());
      break;
    case types::retrieve_ue_info:
      c.init(other.c.get<retrieve_ue_info_s>());
      break;
    case types::rrc_inactive_transition_report:
      c.init(other.c.get<rrc_inactive_transition_report_s>());
      break;
    case types::secondary_rat_data_usage_report:
      c.init(other.c.get<secondary_rat_data_usage_report_s>());
      break;
    case types::trace_fail_ind:
      c.init(other.c.get<trace_fail_ind_s>());
      break;
    case types::trace_start:
      c.init(other.c.get<trace_start_s>());
      break;
    case types::ue_context_release_request:
      c.init(other.c.get<ue_context_release_request_s>());
      break;
    case types::ue_info_transfer:
      c.init(other.c.get<ue_info_transfer_s>());
      break;
    case types::ue_radio_cap_info_ind:
      c.init(other.c.get<ue_radio_cap_info_ind_s>());
      break;
    case types::ue_tnla_binding_release_request:
      c.init(other.c.get<ue_tnla_binding_release_request_s>());
      break;
    case types::ul_nas_transport:
      c.init(other.c.get<ul_nas_transport_s>());
      break;
    case types::ul_non_ue_associated_nrppa_transport:
      c.init(other.c.get<ul_non_ue_associated_nrppa_transport_s>());
      break;
    case types::ul_ran_cfg_transfer:
      c.init(other.c.get<ul_ran_cfg_transfer_s>());
      break;
    case types::ul_ran_early_status_transfer:
      c.init(other.c.get<ul_ran_early_status_transfer_s>());
      break;
    case types::ul_ran_status_transfer:
      c.init(other.c.get<ul_ran_status_transfer_s>());
      break;
    case types::ul_rim_info_transfer:
      c.init(other.c.get<ul_rim_info_transfer_s>());
      break;
    case types::ul_ue_associated_nrppa_transport:
      c.init(other.c.get<ul_ue_associated_nrppa_transport_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::init_msg_c");
  }
}
ngap_elem_procs_o::init_msg_c& ngap_elem_procs_o::init_msg_c::operator=(const ngap_elem_procs_o::init_msg_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::amf_cfg_upd:
      c.set(other.c.get<amf_cfg_upd_s>());
      break;
    case types::broadcast_session_mod_request:
      c.set(other.c.get<broadcast_session_mod_request_s>());
      break;
    case types::broadcast_session_release_request:
      c.set(other.c.get<broadcast_session_release_request_s>());
      break;
    case types::broadcast_session_setup_request:
      c.set(other.c.get<broadcast_session_setup_request_s>());
      break;
    case types::distribution_setup_request:
      c.set(other.c.get<distribution_setup_request_s>());
      break;
    case types::distribution_release_request:
      c.set(other.c.get<distribution_release_request_s>());
      break;
    case types::ho_cancel:
      c.set(other.c.get<ho_cancel_s>());
      break;
    case types::ho_required:
      c.set(other.c.get<ho_required_s>());
      break;
    case types::ho_request:
      c.set(other.c.get<ho_request_s>());
      break;
    case types::init_context_setup_request:
      c.set(other.c.get<init_context_setup_request_s>());
      break;
    case types::multicast_session_activation_request:
      c.set(other.c.get<multicast_session_activation_request_s>());
      break;
    case types::multicast_session_deactivation_request:
      c.set(other.c.get<multicast_session_deactivation_request_s>());
      break;
    case types::multicast_session_upd_request:
      c.set(other.c.get<multicast_session_upd_request_s>());
      break;
    case types::ng_reset:
      c.set(other.c.get<ng_reset_s>());
      break;
    case types::ng_setup_request:
      c.set(other.c.get<ng_setup_request_s>());
      break;
    case types::path_switch_request:
      c.set(other.c.get<path_switch_request_s>());
      break;
    case types::pdu_session_res_modify_request:
      c.set(other.c.get<pdu_session_res_modify_request_s>());
      break;
    case types::pdu_session_res_modify_ind:
      c.set(other.c.get<pdu_session_res_modify_ind_s>());
      break;
    case types::pdu_session_res_release_cmd:
      c.set(other.c.get<pdu_session_res_release_cmd_s>());
      break;
    case types::pdu_session_res_setup_request:
      c.set(other.c.get<pdu_session_res_setup_request_s>());
      break;
    case types::pws_cancel_request:
      c.set(other.c.get<pws_cancel_request_s>());
      break;
    case types::ran_cfg_upd:
      c.set(other.c.get<ran_cfg_upd_s>());
      break;
    case types::ue_context_mod_request:
      c.set(other.c.get<ue_context_mod_request_s>());
      break;
    case types::ue_context_release_cmd:
      c.set(other.c.get<ue_context_release_cmd_s>());
      break;
    case types::ue_context_resume_request:
      c.set(other.c.get<ue_context_resume_request_s>());
      break;
    case types::ue_context_suspend_request:
      c.set(other.c.get<ue_context_suspend_request_s>());
      break;
    case types::ue_radio_cap_check_request:
      c.set(other.c.get<ue_radio_cap_check_request_s>());
      break;
    case types::ue_radio_cap_id_map_request:
      c.set(other.c.get<ue_radio_cap_id_map_request_s>());
      break;
    case types::write_replace_warning_request:
      c.set(other.c.get<write_replace_warning_request_s>());
      break;
    case types::amf_cp_relocation_ind:
      c.set(other.c.get<amf_cp_relocation_ind_s>());
      break;
    case types::amf_status_ind:
      c.set(other.c.get<amf_status_ind_s>());
      break;
    case types::broadcast_session_release_required:
      c.set(other.c.get<broadcast_session_release_required_s>());
      break;
    case types::cell_traffic_trace:
      c.set(other.c.get<cell_traffic_trace_s>());
      break;
    case types::conn_establishment_ind:
      c.set(other.c.get<conn_establishment_ind_s>());
      break;
    case types::deactiv_trace:
      c.set(other.c.get<deactiv_trace_s>());
      break;
    case types::dl_nas_transport:
      c.set(other.c.get<dl_nas_transport_s>());
      break;
    case types::dl_non_ue_associated_nrppa_transport:
      c.set(other.c.get<dl_non_ue_associated_nrppa_transport_s>());
      break;
    case types::dl_ran_cfg_transfer:
      c.set(other.c.get<dl_ran_cfg_transfer_s>());
      break;
    case types::dl_ran_early_status_transfer:
      c.set(other.c.get<dl_ran_early_status_transfer_s>());
      break;
    case types::dl_ran_status_transfer:
      c.set(other.c.get<dl_ran_status_transfer_s>());
      break;
    case types::dl_rim_info_transfer:
      c.set(other.c.get<dl_rim_info_transfer_s>());
      break;
    case types::dl_ue_associated_nrppa_transport:
      c.set(other.c.get<dl_ue_associated_nrppa_transport_s>());
      break;
    case types::error_ind:
      c.set(other.c.get<error_ind_s>());
      break;
    case types::ho_notify:
      c.set(other.c.get<ho_notify_s>());
      break;
    case types::ho_success:
      c.set(other.c.get<ho_success_s>());
      break;
    case types::init_ue_msg:
      c.set(other.c.get<init_ue_msg_s>());
      break;
    case types::location_report:
      c.set(other.c.get<location_report_s>());
      break;
    case types::location_report_ctrl:
      c.set(other.c.get<location_report_ctrl_s>());
      break;
    case types::location_report_fail_ind:
      c.set(other.c.get<location_report_fail_ind_s>());
      break;
    case types::multicast_group_paging:
      c.set(other.c.get<multicast_group_paging_s>());
      break;
    case types::nas_non_delivery_ind:
      c.set(other.c.get<nas_non_delivery_ind_s>());
      break;
    case types::overload_start:
      c.set(other.c.get<overload_start_s>());
      break;
    case types::overload_stop:
      c.set(other.c.get<overload_stop_s>());
      break;
    case types::paging:
      c.set(other.c.get<paging_s>());
      break;
    case types::pdu_session_res_notify:
      c.set(other.c.get<pdu_session_res_notify_s>());
      break;
    case types::private_msg:
      c.set(other.c.get<private_msg_s>());
      break;
    case types::pws_fail_ind:
      c.set(other.c.get<pws_fail_ind_s>());
      break;
    case types::pws_restart_ind:
      c.set(other.c.get<pws_restart_ind_s>());
      break;
    case types::ran_cp_relocation_ind:
      c.set(other.c.get<ran_cp_relocation_ind_s>());
      break;
    case types::reroute_nas_request:
      c.set(other.c.get<reroute_nas_request_s>());
      break;
    case types::retrieve_ue_info:
      c.set(other.c.get<retrieve_ue_info_s>());
      break;
    case types::rrc_inactive_transition_report:
      c.set(other.c.get<rrc_inactive_transition_report_s>());
      break;
    case types::secondary_rat_data_usage_report:
      c.set(other.c.get<secondary_rat_data_usage_report_s>());
      break;
    case types::trace_fail_ind:
      c.set(other.c.get<trace_fail_ind_s>());
      break;
    case types::trace_start:
      c.set(other.c.get<trace_start_s>());
      break;
    case types::ue_context_release_request:
      c.set(other.c.get<ue_context_release_request_s>());
      break;
    case types::ue_info_transfer:
      c.set(other.c.get<ue_info_transfer_s>());
      break;
    case types::ue_radio_cap_info_ind:
      c.set(other.c.get<ue_radio_cap_info_ind_s>());
      break;
    case types::ue_tnla_binding_release_request:
      c.set(other.c.get<ue_tnla_binding_release_request_s>());
      break;
    case types::ul_nas_transport:
      c.set(other.c.get<ul_nas_transport_s>());
      break;
    case types::ul_non_ue_associated_nrppa_transport:
      c.set(other.c.get<ul_non_ue_associated_nrppa_transport_s>());
      break;
    case types::ul_ran_cfg_transfer:
      c.set(other.c.get<ul_ran_cfg_transfer_s>());
      break;
    case types::ul_ran_early_status_transfer:
      c.set(other.c.get<ul_ran_early_status_transfer_s>());
      break;
    case types::ul_ran_status_transfer:
      c.set(other.c.get<ul_ran_status_transfer_s>());
      break;
    case types::ul_rim_info_transfer:
      c.set(other.c.get<ul_rim_info_transfer_s>());
      break;
    case types::ul_ue_associated_nrppa_transport:
      c.set(other.c.get<ul_ue_associated_nrppa_transport_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::init_msg_c");
  }

  return *this;
}
amf_cfg_upd_s& ngap_elem_procs_o::init_msg_c::amf_cfg_upd()
{
  assert_choice_type(types::amf_cfg_upd, type_, "InitiatingMessage");
  return c.get<amf_cfg_upd_s>();
}
broadcast_session_mod_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_mod_request()
{
  assert_choice_type(types::broadcast_session_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_mod_request_s>();
}
broadcast_session_release_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_request()
{
  assert_choice_type(types::broadcast_session_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_request_s>();
}
broadcast_session_setup_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_setup_request()
{
  assert_choice_type(types::broadcast_session_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_setup_request_s>();
}
distribution_setup_request_s& ngap_elem_procs_o::init_msg_c::distribution_setup_request()
{
  assert_choice_type(types::distribution_setup_request, type_, "InitiatingMessage");
  return c.get<distribution_setup_request_s>();
}
distribution_release_request_s& ngap_elem_procs_o::init_msg_c::distribution_release_request()
{
  assert_choice_type(types::distribution_release_request, type_, "InitiatingMessage");
  return c.get<distribution_release_request_s>();
}
ho_cancel_s& ngap_elem_procs_o::init_msg_c::ho_cancel()
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
ho_required_s& ngap_elem_procs_o::init_msg_c::ho_required()
{
  assert_choice_type(types::ho_required, type_, "InitiatingMessage");
  return c.get<ho_required_s>();
}
ho_request_s& ngap_elem_procs_o::init_msg_c::ho_request()
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
init_context_setup_request_s& ngap_elem_procs_o::init_msg_c::init_context_setup_request()
{
  assert_choice_type(types::init_context_setup_request, type_, "InitiatingMessage");
  return c.get<init_context_setup_request_s>();
}
multicast_session_activation_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_activation_request()
{
  assert_choice_type(types::multicast_session_activation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_activation_request_s>();
}
multicast_session_deactivation_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_deactivation_request()
{
  assert_choice_type(types::multicast_session_deactivation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_deactivation_request_s>();
}
multicast_session_upd_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_upd_request()
{
  assert_choice_type(types::multicast_session_upd_request, type_, "InitiatingMessage");
  return c.get<multicast_session_upd_request_s>();
}
ng_reset_s& ngap_elem_procs_o::init_msg_c::ng_reset()
{
  assert_choice_type(types::ng_reset, type_, "InitiatingMessage");
  return c.get<ng_reset_s>();
}
ng_setup_request_s& ngap_elem_procs_o::init_msg_c::ng_setup_request()
{
  assert_choice_type(types::ng_setup_request, type_, "InitiatingMessage");
  return c.get<ng_setup_request_s>();
}
path_switch_request_s& ngap_elem_procs_o::init_msg_c::path_switch_request()
{
  assert_choice_type(types::path_switch_request, type_, "InitiatingMessage");
  return c.get<path_switch_request_s>();
}
pdu_session_res_modify_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_request()
{
  assert_choice_type(types::pdu_session_res_modify_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_request_s>();
}
pdu_session_res_modify_ind_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_ind()
{
  assert_choice_type(types::pdu_session_res_modify_ind, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_ind_s>();
}
pdu_session_res_release_cmd_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_release_cmd()
{
  assert_choice_type(types::pdu_session_res_release_cmd, type_, "InitiatingMessage");
  return c.get<pdu_session_res_release_cmd_s>();
}
pdu_session_res_setup_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_setup_request()
{
  assert_choice_type(types::pdu_session_res_setup_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_setup_request_s>();
}
pws_cancel_request_s& ngap_elem_procs_o::init_msg_c::pws_cancel_request()
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
ran_cfg_upd_s& ngap_elem_procs_o::init_msg_c::ran_cfg_upd()
{
  assert_choice_type(types::ran_cfg_upd, type_, "InitiatingMessage");
  return c.get<ran_cfg_upd_s>();
}
ue_context_mod_request_s& ngap_elem_procs_o::init_msg_c::ue_context_mod_request()
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
ue_context_release_cmd_s& ngap_elem_procs_o::init_msg_c::ue_context_release_cmd()
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
ue_context_resume_request_s& ngap_elem_procs_o::init_msg_c::ue_context_resume_request()
{
  assert_choice_type(types::ue_context_resume_request, type_, "InitiatingMessage");
  return c.get<ue_context_resume_request_s>();
}
ue_context_suspend_request_s& ngap_elem_procs_o::init_msg_c::ue_context_suspend_request()
{
  assert_choice_type(types::ue_context_suspend_request, type_, "InitiatingMessage");
  return c.get<ue_context_suspend_request_s>();
}
ue_radio_cap_check_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_check_request()
{
  assert_choice_type(types::ue_radio_cap_check_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_check_request_s>();
}
ue_radio_cap_id_map_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_id_map_request()
{
  assert_choice_type(types::ue_radio_cap_id_map_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_id_map_request_s>();
}
write_replace_warning_request_s& ngap_elem_procs_o::init_msg_c::write_replace_warning_request()
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
amf_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::amf_cp_relocation_ind()
{
  assert_choice_type(types::amf_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<amf_cp_relocation_ind_s>();
}
amf_status_ind_s& ngap_elem_procs_o::init_msg_c::amf_status_ind()
{
  assert_choice_type(types::amf_status_ind, type_, "InitiatingMessage");
  return c.get<amf_status_ind_s>();
}
broadcast_session_release_required_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_required()
{
  assert_choice_type(types::broadcast_session_release_required, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_required_s>();
}
cell_traffic_trace_s& ngap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
conn_establishment_ind_s& ngap_elem_procs_o::init_msg_c::conn_establishment_ind()
{
  assert_choice_type(types::conn_establishment_ind, type_, "InitiatingMessage");
  return c.get<conn_establishment_ind_s>();
}
deactiv_trace_s& ngap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
dl_nas_transport_s& ngap_elem_procs_o::init_msg_c::dl_nas_transport()
{
  assert_choice_type(types::dl_nas_transport, type_, "InitiatingMessage");
  return c.get<dl_nas_transport_s>();
}
dl_non_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::dl_non_ue_associated_nrppa_transport()
{
  assert_choice_type(types::dl_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_non_ue_associated_nrppa_transport_s>();
}
dl_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_cfg_transfer()
{
  assert_choice_type(types::dl_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_cfg_transfer_s>();
}
dl_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_early_status_transfer()
{
  assert_choice_type(types::dl_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_early_status_transfer_s>();
}
dl_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_status_transfer()
{
  assert_choice_type(types::dl_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_status_transfer_s>();
}
dl_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::dl_rim_info_transfer()
{
  assert_choice_type(types::dl_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<dl_rim_info_transfer_s>();
}
dl_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::dl_ue_associated_nrppa_transport()
{
  assert_choice_type(types::dl_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_ue_associated_nrppa_transport_s>();
}
error_ind_s& ngap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ho_notify_s& ngap_elem_procs_o::init_msg_c::ho_notify()
{
  assert_choice_type(types::ho_notify, type_, "InitiatingMessage");
  return c.get<ho_notify_s>();
}
ho_success_s& ngap_elem_procs_o::init_msg_c::ho_success()
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
init_ue_msg_s& ngap_elem_procs_o::init_msg_c::init_ue_msg()
{
  assert_choice_type(types::init_ue_msg, type_, "InitiatingMessage");
  return c.get<init_ue_msg_s>();
}
location_report_s& ngap_elem_procs_o::init_msg_c::location_report()
{
  assert_choice_type(types::location_report, type_, "InitiatingMessage");
  return c.get<location_report_s>();
}
location_report_ctrl_s& ngap_elem_procs_o::init_msg_c::location_report_ctrl()
{
  assert_choice_type(types::location_report_ctrl, type_, "InitiatingMessage");
  return c.get<location_report_ctrl_s>();
}
location_report_fail_ind_s& ngap_elem_procs_o::init_msg_c::location_report_fail_ind()
{
  assert_choice_type(types::location_report_fail_ind, type_, "InitiatingMessage");
  return c.get<location_report_fail_ind_s>();
}
multicast_group_paging_s& ngap_elem_procs_o::init_msg_c::multicast_group_paging()
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
nas_non_delivery_ind_s& ngap_elem_procs_o::init_msg_c::nas_non_delivery_ind()
{
  assert_choice_type(types::nas_non_delivery_ind, type_, "InitiatingMessage");
  return c.get<nas_non_delivery_ind_s>();
}
overload_start_s& ngap_elem_procs_o::init_msg_c::overload_start()
{
  assert_choice_type(types::overload_start, type_, "InitiatingMessage");
  return c.get<overload_start_s>();
}
overload_stop_s& ngap_elem_procs_o::init_msg_c::overload_stop()
{
  assert_choice_type(types::overload_stop, type_, "InitiatingMessage");
  return c.get<overload_stop_s>();
}
paging_s& ngap_elem_procs_o::init_msg_c::paging()
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
pdu_session_res_notify_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_notify()
{
  assert_choice_type(types::pdu_session_res_notify, type_, "InitiatingMessage");
  return c.get<pdu_session_res_notify_s>();
}
private_msg_s& ngap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
pws_fail_ind_s& ngap_elem_procs_o::init_msg_c::pws_fail_ind()
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
pws_restart_ind_s& ngap_elem_procs_o::init_msg_c::pws_restart_ind()
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
ran_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::ran_cp_relocation_ind()
{
  assert_choice_type(types::ran_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<ran_cp_relocation_ind_s>();
}
reroute_nas_request_s& ngap_elem_procs_o::init_msg_c::reroute_nas_request()
{
  assert_choice_type(types::reroute_nas_request, type_, "InitiatingMessage");
  return c.get<reroute_nas_request_s>();
}
retrieve_ue_info_s& ngap_elem_procs_o::init_msg_c::retrieve_ue_info()
{
  assert_choice_type(types::retrieve_ue_info, type_, "InitiatingMessage");
  return c.get<retrieve_ue_info_s>();
}
rrc_inactive_transition_report_s& ngap_elem_procs_o::init_msg_c::rrc_inactive_transition_report()
{
  assert_choice_type(types::rrc_inactive_transition_report, type_, "InitiatingMessage");
  return c.get<rrc_inactive_transition_report_s>();
}
secondary_rat_data_usage_report_s& ngap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report()
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
trace_fail_ind_s& ngap_elem_procs_o::init_msg_c::trace_fail_ind()
{
  assert_choice_type(types::trace_fail_ind, type_, "InitiatingMessage");
  return c.get<trace_fail_ind_s>();
}
trace_start_s& ngap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
ue_context_release_request_s& ngap_elem_procs_o::init_msg_c::ue_context_release_request()
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
ue_info_transfer_s& ngap_elem_procs_o::init_msg_c::ue_info_transfer()
{
  assert_choice_type(types::ue_info_transfer, type_, "InitiatingMessage");
  return c.get<ue_info_transfer_s>();
}
ue_radio_cap_info_ind_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_info_ind()
{
  assert_choice_type(types::ue_radio_cap_info_ind, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_info_ind_s>();
}
ue_tnla_binding_release_request_s& ngap_elem_procs_o::init_msg_c::ue_tnla_binding_release_request()
{
  assert_choice_type(types::ue_tnla_binding_release_request, type_, "InitiatingMessage");
  return c.get<ue_tnla_binding_release_request_s>();
}
ul_nas_transport_s& ngap_elem_procs_o::init_msg_c::ul_nas_transport()
{
  assert_choice_type(types::ul_nas_transport, type_, "InitiatingMessage");
  return c.get<ul_nas_transport_s>();
}
ul_non_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::ul_non_ue_associated_nrppa_transport()
{
  assert_choice_type(types::ul_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_non_ue_associated_nrppa_transport_s>();
}
ul_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_cfg_transfer()
{
  assert_choice_type(types::ul_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_cfg_transfer_s>();
}
ul_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_early_status_transfer()
{
  assert_choice_type(types::ul_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_early_status_transfer_s>();
}
ul_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_status_transfer()
{
  assert_choice_type(types::ul_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_status_transfer_s>();
}
ul_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::ul_rim_info_transfer()
{
  assert_choice_type(types::ul_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<ul_rim_info_transfer_s>();
}
ul_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::ul_ue_associated_nrppa_transport()
{
  assert_choice_type(types::ul_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_ue_associated_nrppa_transport_s>();
}
const amf_cfg_upd_s& ngap_elem_procs_o::init_msg_c::amf_cfg_upd() const
{
  assert_choice_type(types::amf_cfg_upd, type_, "InitiatingMessage");
  return c.get<amf_cfg_upd_s>();
}
const broadcast_session_mod_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_mod_request() const
{
  assert_choice_type(types::broadcast_session_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_mod_request_s>();
}
const broadcast_session_release_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_request() const
{
  assert_choice_type(types::broadcast_session_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_request_s>();
}
const broadcast_session_setup_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_setup_request() const
{
  assert_choice_type(types::broadcast_session_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_setup_request_s>();
}
const distribution_setup_request_s& ngap_elem_procs_o::init_msg_c::distribution_setup_request() const
{
  assert_choice_type(types::distribution_setup_request, type_, "InitiatingMessage");
  return c.get<distribution_setup_request_s>();
}
const distribution_release_request_s& ngap_elem_procs_o::init_msg_c::distribution_release_request() const
{
  assert_choice_type(types::distribution_release_request, type_, "InitiatingMessage");
  return c.get<distribution_release_request_s>();
}
const ho_cancel_s& ngap_elem_procs_o::init_msg_c::ho_cancel() const
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
const ho_required_s& ngap_elem_procs_o::init_msg_c::ho_required() const
{
  assert_choice_type(types::ho_required, type_, "InitiatingMessage");
  return c.get<ho_required_s>();
}
const ho_request_s& ngap_elem_procs_o::init_msg_c::ho_request() const
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
const init_context_setup_request_s& ngap_elem_procs_o::init_msg_c::init_context_setup_request() const
{
  assert_choice_type(types::init_context_setup_request, type_, "InitiatingMessage");
  return c.get<init_context_setup_request_s>();
}
const multicast_session_activation_request_s&
ngap_elem_procs_o::init_msg_c::multicast_session_activation_request() const
{
  assert_choice_type(types::multicast_session_activation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_activation_request_s>();
}
const multicast_session_deactivation_request_s&
ngap_elem_procs_o::init_msg_c::multicast_session_deactivation_request() const
{
  assert_choice_type(types::multicast_session_deactivation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_deactivation_request_s>();
}
const multicast_session_upd_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_upd_request() const
{
  assert_choice_type(types::multicast_session_upd_request, type_, "InitiatingMessage");
  return c.get<multicast_session_upd_request_s>();
}
const ng_reset_s& ngap_elem_procs_o::init_msg_c::ng_reset() const
{
  assert_choice_type(types::ng_reset, type_, "InitiatingMessage");
  return c.get<ng_reset_s>();
}
const ng_setup_request_s& ngap_elem_procs_o::init_msg_c::ng_setup_request() const
{
  assert_choice_type(types::ng_setup_request, type_, "InitiatingMessage");
  return c.get<ng_setup_request_s>();
}
const path_switch_request_s& ngap_elem_procs_o::init_msg_c::path_switch_request() const
{
  assert_choice_type(types::path_switch_request, type_, "InitiatingMessage");
  return c.get<path_switch_request_s>();
}
const pdu_session_res_modify_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_request() const
{
  assert_choice_type(types::pdu_session_res_modify_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_request_s>();
}
const pdu_session_res_modify_ind_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_ind() const
{
  assert_choice_type(types::pdu_session_res_modify_ind, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_ind_s>();
}
const pdu_session_res_release_cmd_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_release_cmd() const
{
  assert_choice_type(types::pdu_session_res_release_cmd, type_, "InitiatingMessage");
  return c.get<pdu_session_res_release_cmd_s>();
}
const pdu_session_res_setup_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_setup_request() const
{
  assert_choice_type(types::pdu_session_res_setup_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_setup_request_s>();
}
const pws_cancel_request_s& ngap_elem_procs_o::init_msg_c::pws_cancel_request() const
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
const ran_cfg_upd_s& ngap_elem_procs_o::init_msg_c::ran_cfg_upd() const
{
  assert_choice_type(types::ran_cfg_upd, type_, "InitiatingMessage");
  return c.get<ran_cfg_upd_s>();
}
const ue_context_mod_request_s& ngap_elem_procs_o::init_msg_c::ue_context_mod_request() const
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
const ue_context_release_cmd_s& ngap_elem_procs_o::init_msg_c::ue_context_release_cmd() const
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
const ue_context_resume_request_s& ngap_elem_procs_o::init_msg_c::ue_context_resume_request() const
{
  assert_choice_type(types::ue_context_resume_request, type_, "InitiatingMessage");
  return c.get<ue_context_resume_request_s>();
}
const ue_context_suspend_request_s& ngap_elem_procs_o::init_msg_c::ue_context_suspend_request() const
{
  assert_choice_type(types::ue_context_suspend_request, type_, "InitiatingMessage");
  return c.get<ue_context_suspend_request_s>();
}
const ue_radio_cap_check_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_check_request() const
{
  assert_choice_type(types::ue_radio_cap_check_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_check_request_s>();
}
const ue_radio_cap_id_map_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_id_map_request() const
{
  assert_choice_type(types::ue_radio_cap_id_map_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_id_map_request_s>();
}
const write_replace_warning_request_s& ngap_elem_procs_o::init_msg_c::write_replace_warning_request() const
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
const amf_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::amf_cp_relocation_ind() const
{
  assert_choice_type(types::amf_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<amf_cp_relocation_ind_s>();
}
const amf_status_ind_s& ngap_elem_procs_o::init_msg_c::amf_status_ind() const
{
  assert_choice_type(types::amf_status_ind, type_, "InitiatingMessage");
  return c.get<amf_status_ind_s>();
}
const broadcast_session_release_required_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_required() const
{
  assert_choice_type(types::broadcast_session_release_required, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_required_s>();
}
const cell_traffic_trace_s& ngap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const conn_establishment_ind_s& ngap_elem_procs_o::init_msg_c::conn_establishment_ind() const
{
  assert_choice_type(types::conn_establishment_ind, type_, "InitiatingMessage");
  return c.get<conn_establishment_ind_s>();
}
const deactiv_trace_s& ngap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const dl_nas_transport_s& ngap_elem_procs_o::init_msg_c::dl_nas_transport() const
{
  assert_choice_type(types::dl_nas_transport, type_, "InitiatingMessage");
  return c.get<dl_nas_transport_s>();
}
const dl_non_ue_associated_nrppa_transport_s&
ngap_elem_procs_o::init_msg_c::dl_non_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::dl_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_non_ue_associated_nrppa_transport_s>();
}
const dl_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_cfg_transfer() const
{
  assert_choice_type(types::dl_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_cfg_transfer_s>();
}
const dl_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_early_status_transfer() const
{
  assert_choice_type(types::dl_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_early_status_transfer_s>();
}
const dl_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_status_transfer() const
{
  assert_choice_type(types::dl_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_status_transfer_s>();
}
const dl_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::dl_rim_info_transfer() const
{
  assert_choice_type(types::dl_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<dl_rim_info_transfer_s>();
}
const dl_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::dl_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::dl_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_ue_associated_nrppa_transport_s>();
}
const error_ind_s& ngap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ho_notify_s& ngap_elem_procs_o::init_msg_c::ho_notify() const
{
  assert_choice_type(types::ho_notify, type_, "InitiatingMessage");
  return c.get<ho_notify_s>();
}
const ho_success_s& ngap_elem_procs_o::init_msg_c::ho_success() const
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
const init_ue_msg_s& ngap_elem_procs_o::init_msg_c::init_ue_msg() const
{
  assert_choice_type(types::init_ue_msg, type_, "InitiatingMessage");
  return c.get<init_ue_msg_s>();
}
const location_report_s& ngap_elem_procs_o::init_msg_c::location_report() const
{
  assert_choice_type(types::location_report, type_, "InitiatingMessage");
  return c.get<location_report_s>();
}
const location_report_ctrl_s& ngap_elem_procs_o::init_msg_c::location_report_ctrl() const
{
  assert_choice_type(types::location_report_ctrl, type_, "InitiatingMessage");
  return c.get<location_report_ctrl_s>();
}
const location_report_fail_ind_s& ngap_elem_procs_o::init_msg_c::location_report_fail_ind() const
{
  assert_choice_type(types::location_report_fail_ind, type_, "InitiatingMessage");
  return c.get<location_report_fail_ind_s>();
}
const multicast_group_paging_s& ngap_elem_procs_o::init_msg_c::multicast_group_paging() const
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
const nas_non_delivery_ind_s& ngap_elem_procs_o::init_msg_c::nas_non_delivery_ind() const
{
  assert_choice_type(types::nas_non_delivery_ind, type_, "InitiatingMessage");
  return c.get<nas_non_delivery_ind_s>();
}
const overload_start_s& ngap_elem_procs_o::init_msg_c::overload_start() const
{
  assert_choice_type(types::overload_start, type_, "InitiatingMessage");
  return c.get<overload_start_s>();
}
const overload_stop_s& ngap_elem_procs_o::init_msg_c::overload_stop() const
{
  assert_choice_type(types::overload_stop, type_, "InitiatingMessage");
  return c.get<overload_stop_s>();
}
const paging_s& ngap_elem_procs_o::init_msg_c::paging() const
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
const pdu_session_res_notify_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_notify() const
{
  assert_choice_type(types::pdu_session_res_notify, type_, "InitiatingMessage");
  return c.get<pdu_session_res_notify_s>();
}
const private_msg_s& ngap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const pws_fail_ind_s& ngap_elem_procs_o::init_msg_c::pws_fail_ind() const
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
const pws_restart_ind_s& ngap_elem_procs_o::init_msg_c::pws_restart_ind() const
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
const ran_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::ran_cp_relocation_ind() const
{
  assert_choice_type(types::ran_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<ran_cp_relocation_ind_s>();
}
const reroute_nas_request_s& ngap_elem_procs_o::init_msg_c::reroute_nas_request() const
{
  assert_choice_type(types::reroute_nas_request, type_, "InitiatingMessage");
  return c.get<reroute_nas_request_s>();
}
const retrieve_ue_info_s& ngap_elem_procs_o::init_msg_c::retrieve_ue_info() const
{
  assert_choice_type(types::retrieve_ue_info, type_, "InitiatingMessage");
  return c.get<retrieve_ue_info_s>();
}
const rrc_inactive_transition_report_s& ngap_elem_procs_o::init_msg_c::rrc_inactive_transition_report() const
{
  assert_choice_type(types::rrc_inactive_transition_report, type_, "InitiatingMessage");
  return c.get<rrc_inactive_transition_report_s>();
}
const secondary_rat_data_usage_report_s& ngap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report() const
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
const trace_fail_ind_s& ngap_elem_procs_o::init_msg_c::trace_fail_ind() const
{
  assert_choice_type(types::trace_fail_ind, type_, "InitiatingMessage");
  return c.get<trace_fail_ind_s>();
}
const trace_start_s& ngap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const ue_context_release_request_s& ngap_elem_procs_o::init_msg_c::ue_context_release_request() const
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
const ue_info_transfer_s& ngap_elem_procs_o::init_msg_c::ue_info_transfer() const
{
  assert_choice_type(types::ue_info_transfer, type_, "InitiatingMessage");
  return c.get<ue_info_transfer_s>();
}
const ue_radio_cap_info_ind_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_info_ind() const
{
  assert_choice_type(types::ue_radio_cap_info_ind, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_info_ind_s>();
}
const ue_tnla_binding_release_request_s& ngap_elem_procs_o::init_msg_c::ue_tnla_binding_release_request() const
{
  assert_choice_type(types::ue_tnla_binding_release_request, type_, "InitiatingMessage");
  return c.get<ue_tnla_binding_release_request_s>();
}
const ul_nas_transport_s& ngap_elem_procs_o::init_msg_c::ul_nas_transport() const
{
  assert_choice_type(types::ul_nas_transport, type_, "InitiatingMessage");
  return c.get<ul_nas_transport_s>();
}
const ul_non_ue_associated_nrppa_transport_s&
ngap_elem_procs_o::init_msg_c::ul_non_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::ul_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_non_ue_associated_nrppa_transport_s>();
}
const ul_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_cfg_transfer() const
{
  assert_choice_type(types::ul_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_cfg_transfer_s>();
}
const ul_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_early_status_transfer() const
{
  assert_choice_type(types::ul_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_early_status_transfer_s>();
}
const ul_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_status_transfer() const
{
  assert_choice_type(types::ul_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_status_transfer_s>();
}
const ul_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::ul_rim_info_transfer() const
{
  assert_choice_type(types::ul_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<ul_rim_info_transfer_s>();
}
const ul_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::ul_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::ul_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_ue_associated_nrppa_transport_s>();
}
void ngap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::amf_cfg_upd:
      j.write_fieldname("AMFConfigurationUpdate");
      c.get<amf_cfg_upd_s>().to_json(j);
      break;
    case types::broadcast_session_mod_request:
      j.write_fieldname("BroadcastSessionModificationRequest");
      c.get<broadcast_session_mod_request_s>().to_json(j);
      break;
    case types::broadcast_session_release_request:
      j.write_fieldname("BroadcastSessionReleaseRequest");
      c.get<broadcast_session_release_request_s>().to_json(j);
      break;
    case types::broadcast_session_setup_request:
      j.write_fieldname("BroadcastSessionSetupRequest");
      c.get<broadcast_session_setup_request_s>().to_json(j);
      break;
    case types::distribution_setup_request:
      j.write_fieldname("DistributionSetupRequest");
      c.get<distribution_setup_request_s>().to_json(j);
      break;
    case types::distribution_release_request:
      j.write_fieldname("DistributionReleaseRequest");
      c.get<distribution_release_request_s>().to_json(j);
      break;
    case types::ho_cancel:
      j.write_fieldname("HandoverCancel");
      c.get<ho_cancel_s>().to_json(j);
      break;
    case types::ho_required:
      j.write_fieldname("HandoverRequired");
      c.get<ho_required_s>().to_json(j);
      break;
    case types::ho_request:
      j.write_fieldname("HandoverRequest");
      c.get<ho_request_s>().to_json(j);
      break;
    case types::init_context_setup_request:
      j.write_fieldname("InitialContextSetupRequest");
      c.get<init_context_setup_request_s>().to_json(j);
      break;
    case types::multicast_session_activation_request:
      j.write_fieldname("MulticastSessionActivationRequest");
      c.get<multicast_session_activation_request_s>().to_json(j);
      break;
    case types::multicast_session_deactivation_request:
      j.write_fieldname("MulticastSessionDeactivationRequest");
      c.get<multicast_session_deactivation_request_s>().to_json(j);
      break;
    case types::multicast_session_upd_request:
      j.write_fieldname("MulticastSessionUpdateRequest");
      c.get<multicast_session_upd_request_s>().to_json(j);
      break;
    case types::ng_reset:
      j.write_fieldname("NGReset");
      c.get<ng_reset_s>().to_json(j);
      break;
    case types::ng_setup_request:
      j.write_fieldname("NGSetupRequest");
      c.get<ng_setup_request_s>().to_json(j);
      break;
    case types::path_switch_request:
      j.write_fieldname("PathSwitchRequest");
      c.get<path_switch_request_s>().to_json(j);
      break;
    case types::pdu_session_res_modify_request:
      j.write_fieldname("PDUSessionResourceModifyRequest");
      c.get<pdu_session_res_modify_request_s>().to_json(j);
      break;
    case types::pdu_session_res_modify_ind:
      j.write_fieldname("PDUSessionResourceModifyIndication");
      c.get<pdu_session_res_modify_ind_s>().to_json(j);
      break;
    case types::pdu_session_res_release_cmd:
      j.write_fieldname("PDUSessionResourceReleaseCommand");
      c.get<pdu_session_res_release_cmd_s>().to_json(j);
      break;
    case types::pdu_session_res_setup_request:
      j.write_fieldname("PDUSessionResourceSetupRequest");
      c.get<pdu_session_res_setup_request_s>().to_json(j);
      break;
    case types::pws_cancel_request:
      j.write_fieldname("PWSCancelRequest");
      c.get<pws_cancel_request_s>().to_json(j);
      break;
    case types::ran_cfg_upd:
      j.write_fieldname("RANConfigurationUpdate");
      c.get<ran_cfg_upd_s>().to_json(j);
      break;
    case types::ue_context_mod_request:
      j.write_fieldname("UEContextModificationRequest");
      c.get<ue_context_mod_request_s>().to_json(j);
      break;
    case types::ue_context_release_cmd:
      j.write_fieldname("UEContextReleaseCommand");
      c.get<ue_context_release_cmd_s>().to_json(j);
      break;
    case types::ue_context_resume_request:
      j.write_fieldname("UEContextResumeRequest");
      c.get<ue_context_resume_request_s>().to_json(j);
      break;
    case types::ue_context_suspend_request:
      j.write_fieldname("UEContextSuspendRequest");
      c.get<ue_context_suspend_request_s>().to_json(j);
      break;
    case types::ue_radio_cap_check_request:
      j.write_fieldname("UERadioCapabilityCheckRequest");
      c.get<ue_radio_cap_check_request_s>().to_json(j);
      break;
    case types::ue_radio_cap_id_map_request:
      j.write_fieldname("UERadioCapabilityIDMappingRequest");
      c.get<ue_radio_cap_id_map_request_s>().to_json(j);
      break;
    case types::write_replace_warning_request:
      j.write_fieldname("WriteReplaceWarningRequest");
      c.get<write_replace_warning_request_s>().to_json(j);
      break;
    case types::amf_cp_relocation_ind:
      j.write_fieldname("AMFCPRelocationIndication");
      c.get<amf_cp_relocation_ind_s>().to_json(j);
      break;
    case types::amf_status_ind:
      j.write_fieldname("AMFStatusIndication");
      c.get<amf_status_ind_s>().to_json(j);
      break;
    case types::broadcast_session_release_required:
      j.write_fieldname("BroadcastSessionReleaseRequired");
      c.get<broadcast_session_release_required_s>().to_json(j);
      break;
    case types::cell_traffic_trace:
      j.write_fieldname("CellTrafficTrace");
      c.get<cell_traffic_trace_s>().to_json(j);
      break;
    case types::conn_establishment_ind:
      j.write_fieldname("ConnectionEstablishmentIndication");
      c.get<conn_establishment_ind_s>().to_json(j);
      break;
    case types::deactiv_trace:
      j.write_fieldname("DeactivateTrace");
      c.get<deactiv_trace_s>().to_json(j);
      break;
    case types::dl_nas_transport:
      j.write_fieldname("DownlinkNASTransport");
      c.get<dl_nas_transport_s>().to_json(j);
      break;
    case types::dl_non_ue_associated_nrppa_transport:
      j.write_fieldname("DownlinkNonUEAssociatedNRPPaTransport");
      c.get<dl_non_ue_associated_nrppa_transport_s>().to_json(j);
      break;
    case types::dl_ran_cfg_transfer:
      j.write_fieldname("DownlinkRANConfigurationTransfer");
      c.get<dl_ran_cfg_transfer_s>().to_json(j);
      break;
    case types::dl_ran_early_status_transfer:
      j.write_fieldname("DownlinkRANEarlyStatusTransfer");
      c.get<dl_ran_early_status_transfer_s>().to_json(j);
      break;
    case types::dl_ran_status_transfer:
      j.write_fieldname("DownlinkRANStatusTransfer");
      c.get<dl_ran_status_transfer_s>().to_json(j);
      break;
    case types::dl_rim_info_transfer:
      j.write_fieldname("DownlinkRIMInformationTransfer");
      c.get<dl_rim_info_transfer_s>().to_json(j);
      break;
    case types::dl_ue_associated_nrppa_transport:
      j.write_fieldname("DownlinkUEAssociatedNRPPaTransport");
      c.get<dl_ue_associated_nrppa_transport_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::ho_notify:
      j.write_fieldname("HandoverNotify");
      c.get<ho_notify_s>().to_json(j);
      break;
    case types::ho_success:
      j.write_fieldname("HandoverSuccess");
      c.get<ho_success_s>().to_json(j);
      break;
    case types::init_ue_msg:
      j.write_fieldname("InitialUEMessage");
      c.get<init_ue_msg_s>().to_json(j);
      break;
    case types::location_report:
      j.write_fieldname("LocationReport");
      c.get<location_report_s>().to_json(j);
      break;
    case types::location_report_ctrl:
      j.write_fieldname("LocationReportingControl");
      c.get<location_report_ctrl_s>().to_json(j);
      break;
    case types::location_report_fail_ind:
      j.write_fieldname("LocationReportingFailureIndication");
      c.get<location_report_fail_ind_s>().to_json(j);
      break;
    case types::multicast_group_paging:
      j.write_fieldname("MulticastGroupPaging");
      c.get<multicast_group_paging_s>().to_json(j);
      break;
    case types::nas_non_delivery_ind:
      j.write_fieldname("NASNonDeliveryIndication");
      c.get<nas_non_delivery_ind_s>().to_json(j);
      break;
    case types::overload_start:
      j.write_fieldname("OverloadStart");
      c.get<overload_start_s>().to_json(j);
      break;
    case types::overload_stop:
      j.write_fieldname("OverloadStop");
      c.get<overload_stop_s>().to_json(j);
      break;
    case types::paging:
      j.write_fieldname("Paging");
      c.get<paging_s>().to_json(j);
      break;
    case types::pdu_session_res_notify:
      j.write_fieldname("PDUSessionResourceNotify");
      c.get<pdu_session_res_notify_s>().to_json(j);
      break;
    case types::private_msg:
      j.write_fieldname("PrivateMessage");
      c.get<private_msg_s>().to_json(j);
      break;
    case types::pws_fail_ind:
      j.write_fieldname("PWSFailureIndication");
      c.get<pws_fail_ind_s>().to_json(j);
      break;
    case types::pws_restart_ind:
      j.write_fieldname("PWSRestartIndication");
      c.get<pws_restart_ind_s>().to_json(j);
      break;
    case types::ran_cp_relocation_ind:
      j.write_fieldname("RANCPRelocationIndication");
      c.get<ran_cp_relocation_ind_s>().to_json(j);
      break;
    case types::reroute_nas_request:
      j.write_fieldname("RerouteNASRequest");
      c.get<reroute_nas_request_s>().to_json(j);
      break;
    case types::retrieve_ue_info:
      j.write_fieldname("RetrieveUEInformation");
      c.get<retrieve_ue_info_s>().to_json(j);
      break;
    case types::rrc_inactive_transition_report:
      j.write_fieldname("RRCInactiveTransitionReport");
      c.get<rrc_inactive_transition_report_s>().to_json(j);
      break;
    case types::secondary_rat_data_usage_report:
      j.write_fieldname("SecondaryRATDataUsageReport");
      c.get<secondary_rat_data_usage_report_s>().to_json(j);
      break;
    case types::trace_fail_ind:
      j.write_fieldname("TraceFailureIndication");
      c.get<trace_fail_ind_s>().to_json(j);
      break;
    case types::trace_start:
      j.write_fieldname("TraceStart");
      c.get<trace_start_s>().to_json(j);
      break;
    case types::ue_context_release_request:
      j.write_fieldname("UEContextReleaseRequest");
      c.get<ue_context_release_request_s>().to_json(j);
      break;
    case types::ue_info_transfer:
      j.write_fieldname("UEInformationTransfer");
      c.get<ue_info_transfer_s>().to_json(j);
      break;
    case types::ue_radio_cap_info_ind:
      j.write_fieldname("UERadioCapabilityInfoIndication");
      c.get<ue_radio_cap_info_ind_s>().to_json(j);
      break;
    case types::ue_tnla_binding_release_request:
      j.write_fieldname("UETNLABindingReleaseRequest");
      c.get<ue_tnla_binding_release_request_s>().to_json(j);
      break;
    case types::ul_nas_transport:
      j.write_fieldname("UplinkNASTransport");
      c.get<ul_nas_transport_s>().to_json(j);
      break;
    case types::ul_non_ue_associated_nrppa_transport:
      j.write_fieldname("UplinkNonUEAssociatedNRPPaTransport");
      c.get<ul_non_ue_associated_nrppa_transport_s>().to_json(j);
      break;
    case types::ul_ran_cfg_transfer:
      j.write_fieldname("UplinkRANConfigurationTransfer");
      c.get<ul_ran_cfg_transfer_s>().to_json(j);
      break;
    case types::ul_ran_early_status_transfer:
      j.write_fieldname("UplinkRANEarlyStatusTransfer");
      c.get<ul_ran_early_status_transfer_s>().to_json(j);
      break;
    case types::ul_ran_status_transfer:
      j.write_fieldname("UplinkRANStatusTransfer");
      c.get<ul_ran_status_transfer_s>().to_json(j);
      break;
    case types::ul_rim_info_transfer:
      j.write_fieldname("UplinkRIMInformationTransfer");
      c.get<ul_rim_info_transfer_s>().to_json(j);
      break;
    case types::ul_ue_associated_nrppa_transport:
      j.write_fieldname("UplinkUEAssociatedNRPPaTransport");
      c.get<ul_ue_associated_nrppa_transport_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE ngap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::amf_cfg_upd:
      HANDLE_CODE(c.get<amf_cfg_upd_s>().pack(bref));
      break;
    case types::broadcast_session_mod_request:
      HANDLE_CODE(c.get<broadcast_session_mod_request_s>().pack(bref));
      break;
    case types::broadcast_session_release_request:
      HANDLE_CODE(c.get<broadcast_session_release_request_s>().pack(bref));
      break;
    case types::broadcast_session_setup_request:
      HANDLE_CODE(c.get<broadcast_session_setup_request_s>().pack(bref));
      break;
    case types::distribution_setup_request:
      HANDLE_CODE(c.get<distribution_setup_request_s>().pack(bref));
      break;
    case types::distribution_release_request:
      HANDLE_CODE(c.get<distribution_release_request_s>().pack(bref));
      break;
    case types::ho_cancel:
      HANDLE_CODE(c.get<ho_cancel_s>().pack(bref));
      break;
    case types::ho_required:
      HANDLE_CODE(c.get<ho_required_s>().pack(bref));
      break;
    case types::ho_request:
      HANDLE_CODE(c.get<ho_request_s>().pack(bref));
      break;
    case types::init_context_setup_request:
      HANDLE_CODE(c.get<init_context_setup_request_s>().pack(bref));
      break;
    case types::multicast_session_activation_request:
      HANDLE_CODE(c.get<multicast_session_activation_request_s>().pack(bref));
      break;
    case types::multicast_session_deactivation_request:
      HANDLE_CODE(c.get<multicast_session_deactivation_request_s>().pack(bref));
      break;
    case types::multicast_session_upd_request:
      HANDLE_CODE(c.get<multicast_session_upd_request_s>().pack(bref));
      break;
    case types::ng_reset:
      HANDLE_CODE(c.get<ng_reset_s>().pack(bref));
      break;
    case types::ng_setup_request:
      HANDLE_CODE(c.get<ng_setup_request_s>().pack(bref));
      break;
    case types::path_switch_request:
      HANDLE_CODE(c.get<path_switch_request_s>().pack(bref));
      break;
    case types::pdu_session_res_modify_request:
      HANDLE_CODE(c.get<pdu_session_res_modify_request_s>().pack(bref));
      break;
    case types::pdu_session_res_modify_ind:
      HANDLE_CODE(c.get<pdu_session_res_modify_ind_s>().pack(bref));
      break;
    case types::pdu_session_res_release_cmd:
      HANDLE_CODE(c.get<pdu_session_res_release_cmd_s>().pack(bref));
      break;
    case types::pdu_session_res_setup_request:
      HANDLE_CODE(c.get<pdu_session_res_setup_request_s>().pack(bref));
      break;
    case types::pws_cancel_request:
      HANDLE_CODE(c.get<pws_cancel_request_s>().pack(bref));
      break;
    case types::ran_cfg_upd:
      HANDLE_CODE(c.get<ran_cfg_upd_s>().pack(bref));
      break;
    case types::ue_context_mod_request:
      HANDLE_CODE(c.get<ue_context_mod_request_s>().pack(bref));
      break;
    case types::ue_context_release_cmd:
      HANDLE_CODE(c.get<ue_context_release_cmd_s>().pack(bref));
      break;
    case types::ue_context_resume_request:
      HANDLE_CODE(c.get<ue_context_resume_request_s>().pack(bref));
      break;
    case types::ue_context_suspend_request:
      HANDLE_CODE(c.get<ue_context_suspend_request_s>().pack(bref));
      break;
    case types::ue_radio_cap_check_request:
      HANDLE_CODE(c.get<ue_radio_cap_check_request_s>().pack(bref));
      break;
    case types::ue_radio_cap_id_map_request:
      HANDLE_CODE(c.get<ue_radio_cap_id_map_request_s>().pack(bref));
      break;
    case types::write_replace_warning_request:
      HANDLE_CODE(c.get<write_replace_warning_request_s>().pack(bref));
      break;
    case types::amf_cp_relocation_ind:
      HANDLE_CODE(c.get<amf_cp_relocation_ind_s>().pack(bref));
      break;
    case types::amf_status_ind:
      HANDLE_CODE(c.get<amf_status_ind_s>().pack(bref));
      break;
    case types::broadcast_session_release_required:
      HANDLE_CODE(c.get<broadcast_session_release_required_s>().pack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().pack(bref));
      break;
    case types::conn_establishment_ind:
      HANDLE_CODE(c.get<conn_establishment_ind_s>().pack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().pack(bref));
      break;
    case types::dl_nas_transport:
      HANDLE_CODE(c.get<dl_nas_transport_s>().pack(bref));
      break;
    case types::dl_non_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<dl_non_ue_associated_nrppa_transport_s>().pack(bref));
      break;
    case types::dl_ran_cfg_transfer:
      HANDLE_CODE(c.get<dl_ran_cfg_transfer_s>().pack(bref));
      break;
    case types::dl_ran_early_status_transfer:
      HANDLE_CODE(c.get<dl_ran_early_status_transfer_s>().pack(bref));
      break;
    case types::dl_ran_status_transfer:
      HANDLE_CODE(c.get<dl_ran_status_transfer_s>().pack(bref));
      break;
    case types::dl_rim_info_transfer:
      HANDLE_CODE(c.get<dl_rim_info_transfer_s>().pack(bref));
      break;
    case types::dl_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<dl_ue_associated_nrppa_transport_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::ho_notify:
      HANDLE_CODE(c.get<ho_notify_s>().pack(bref));
      break;
    case types::ho_success:
      HANDLE_CODE(c.get<ho_success_s>().pack(bref));
      break;
    case types::init_ue_msg:
      HANDLE_CODE(c.get<init_ue_msg_s>().pack(bref));
      break;
    case types::location_report:
      HANDLE_CODE(c.get<location_report_s>().pack(bref));
      break;
    case types::location_report_ctrl:
      HANDLE_CODE(c.get<location_report_ctrl_s>().pack(bref));
      break;
    case types::location_report_fail_ind:
      HANDLE_CODE(c.get<location_report_fail_ind_s>().pack(bref));
      break;
    case types::multicast_group_paging:
      HANDLE_CODE(c.get<multicast_group_paging_s>().pack(bref));
      break;
    case types::nas_non_delivery_ind:
      HANDLE_CODE(c.get<nas_non_delivery_ind_s>().pack(bref));
      break;
    case types::overload_start:
      HANDLE_CODE(c.get<overload_start_s>().pack(bref));
      break;
    case types::overload_stop:
      HANDLE_CODE(c.get<overload_stop_s>().pack(bref));
      break;
    case types::paging:
      HANDLE_CODE(c.get<paging_s>().pack(bref));
      break;
    case types::pdu_session_res_notify:
      HANDLE_CODE(c.get<pdu_session_res_notify_s>().pack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().pack(bref));
      break;
    case types::pws_fail_ind:
      HANDLE_CODE(c.get<pws_fail_ind_s>().pack(bref));
      break;
    case types::pws_restart_ind:
      HANDLE_CODE(c.get<pws_restart_ind_s>().pack(bref));
      break;
    case types::ran_cp_relocation_ind:
      HANDLE_CODE(c.get<ran_cp_relocation_ind_s>().pack(bref));
      break;
    case types::reroute_nas_request:
      HANDLE_CODE(c.get<reroute_nas_request_s>().pack(bref));
      break;
    case types::retrieve_ue_info:
      HANDLE_CODE(c.get<retrieve_ue_info_s>().pack(bref));
      break;
    case types::rrc_inactive_transition_report:
      HANDLE_CODE(c.get<rrc_inactive_transition_report_s>().pack(bref));
      break;
    case types::secondary_rat_data_usage_report:
      HANDLE_CODE(c.get<secondary_rat_data_usage_report_s>().pack(bref));
      break;
    case types::trace_fail_ind:
      HANDLE_CODE(c.get<trace_fail_ind_s>().pack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().pack(bref));
      break;
    case types::ue_context_release_request:
      HANDLE_CODE(c.get<ue_context_release_request_s>().pack(bref));
      break;
    case types::ue_info_transfer:
      HANDLE_CODE(c.get<ue_info_transfer_s>().pack(bref));
      break;
    case types::ue_radio_cap_info_ind:
      HANDLE_CODE(c.get<ue_radio_cap_info_ind_s>().pack(bref));
      break;
    case types::ue_tnla_binding_release_request:
      HANDLE_CODE(c.get<ue_tnla_binding_release_request_s>().pack(bref));
      break;
    case types::ul_nas_transport:
      HANDLE_CODE(c.get<ul_nas_transport_s>().pack(bref));
      break;
    case types::ul_non_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<ul_non_ue_associated_nrppa_transport_s>().pack(bref));
      break;
    case types::ul_ran_cfg_transfer:
      HANDLE_CODE(c.get<ul_ran_cfg_transfer_s>().pack(bref));
      break;
    case types::ul_ran_early_status_transfer:
      HANDLE_CODE(c.get<ul_ran_early_status_transfer_s>().pack(bref));
      break;
    case types::ul_ran_status_transfer:
      HANDLE_CODE(c.get<ul_ran_status_transfer_s>().pack(bref));
      break;
    case types::ul_rim_info_transfer:
      HANDLE_CODE(c.get<ul_rim_info_transfer_s>().pack(bref));
      break;
    case types::ul_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<ul_ue_associated_nrppa_transport_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ngap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::amf_cfg_upd:
      HANDLE_CODE(c.get<amf_cfg_upd_s>().unpack(bref));
      break;
    case types::broadcast_session_mod_request:
      HANDLE_CODE(c.get<broadcast_session_mod_request_s>().unpack(bref));
      break;
    case types::broadcast_session_release_request:
      HANDLE_CODE(c.get<broadcast_session_release_request_s>().unpack(bref));
      break;
    case types::broadcast_session_setup_request:
      HANDLE_CODE(c.get<broadcast_session_setup_request_s>().unpack(bref));
      break;
    case types::distribution_setup_request:
      HANDLE_CODE(c.get<distribution_setup_request_s>().unpack(bref));
      break;
    case types::distribution_release_request:
      HANDLE_CODE(c.get<distribution_release_request_s>().unpack(bref));
      break;
    case types::ho_cancel:
      HANDLE_CODE(c.get<ho_cancel_s>().unpack(bref));
      break;
    case types::ho_required:
      HANDLE_CODE(c.get<ho_required_s>().unpack(bref));
      break;
    case types::ho_request:
      HANDLE_CODE(c.get<ho_request_s>().unpack(bref));
      break;
    case types::init_context_setup_request:
      HANDLE_CODE(c.get<init_context_setup_request_s>().unpack(bref));
      break;
    case types::multicast_session_activation_request:
      HANDLE_CODE(c.get<multicast_session_activation_request_s>().unpack(bref));
      break;
    case types::multicast_session_deactivation_request:
      HANDLE_CODE(c.get<multicast_session_deactivation_request_s>().unpack(bref));
      break;
    case types::multicast_session_upd_request:
      HANDLE_CODE(c.get<multicast_session_upd_request_s>().unpack(bref));
      break;
    case types::ng_reset:
      HANDLE_CODE(c.get<ng_reset_s>().unpack(bref));
      break;
    case types::ng_setup_request:
      HANDLE_CODE(c.get<ng_setup_request_s>().unpack(bref));
      break;
    case types::path_switch_request:
      HANDLE_CODE(c.get<path_switch_request_s>().unpack(bref));
      break;
    case types::pdu_session_res_modify_request:
      HANDLE_CODE(c.get<pdu_session_res_modify_request_s>().unpack(bref));
      break;
    case types::pdu_session_res_modify_ind:
      HANDLE_CODE(c.get<pdu_session_res_modify_ind_s>().unpack(bref));
      break;
    case types::pdu_session_res_release_cmd:
      HANDLE_CODE(c.get<pdu_session_res_release_cmd_s>().unpack(bref));
      break;
    case types::pdu_session_res_setup_request:
      HANDLE_CODE(c.get<pdu_session_res_setup_request_s>().unpack(bref));
      break;
    case types::pws_cancel_request:
      HANDLE_CODE(c.get<pws_cancel_request_s>().unpack(bref));
      break;
    case types::ran_cfg_upd:
      HANDLE_CODE(c.get<ran_cfg_upd_s>().unpack(bref));
      break;
    case types::ue_context_mod_request:
      HANDLE_CODE(c.get<ue_context_mod_request_s>().unpack(bref));
      break;
    case types::ue_context_release_cmd:
      HANDLE_CODE(c.get<ue_context_release_cmd_s>().unpack(bref));
      break;
    case types::ue_context_resume_request:
      HANDLE_CODE(c.get<ue_context_resume_request_s>().unpack(bref));
      break;
    case types::ue_context_suspend_request:
      HANDLE_CODE(c.get<ue_context_suspend_request_s>().unpack(bref));
      break;
    case types::ue_radio_cap_check_request:
      HANDLE_CODE(c.get<ue_radio_cap_check_request_s>().unpack(bref));
      break;
    case types::ue_radio_cap_id_map_request:
      HANDLE_CODE(c.get<ue_radio_cap_id_map_request_s>().unpack(bref));
      break;
    case types::write_replace_warning_request:
      HANDLE_CODE(c.get<write_replace_warning_request_s>().unpack(bref));
      break;
    case types::amf_cp_relocation_ind:
      HANDLE_CODE(c.get<amf_cp_relocation_ind_s>().unpack(bref));
      break;
    case types::amf_status_ind:
      HANDLE_CODE(c.get<amf_status_ind_s>().unpack(bref));
      break;
    case types::broadcast_session_release_required:
      HANDLE_CODE(c.get<broadcast_session_release_required_s>().unpack(bref));
      break;
    case types::cell_traffic_trace:
      HANDLE_CODE(c.get<cell_traffic_trace_s>().unpack(bref));
      break;
    case types::conn_establishment_ind:
      HANDLE_CODE(c.get<conn_establishment_ind_s>().unpack(bref));
      break;
    case types::deactiv_trace:
      HANDLE_CODE(c.get<deactiv_trace_s>().unpack(bref));
      break;
    case types::dl_nas_transport:
      HANDLE_CODE(c.get<dl_nas_transport_s>().unpack(bref));
      break;
    case types::dl_non_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<dl_non_ue_associated_nrppa_transport_s>().unpack(bref));
      break;
    case types::dl_ran_cfg_transfer:
      HANDLE_CODE(c.get<dl_ran_cfg_transfer_s>().unpack(bref));
      break;
    case types::dl_ran_early_status_transfer:
      HANDLE_CODE(c.get<dl_ran_early_status_transfer_s>().unpack(bref));
      break;
    case types::dl_ran_status_transfer:
      HANDLE_CODE(c.get<dl_ran_status_transfer_s>().unpack(bref));
      break;
    case types::dl_rim_info_transfer:
      HANDLE_CODE(c.get<dl_rim_info_transfer_s>().unpack(bref));
      break;
    case types::dl_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<dl_ue_associated_nrppa_transport_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::ho_notify:
      HANDLE_CODE(c.get<ho_notify_s>().unpack(bref));
      break;
    case types::ho_success:
      HANDLE_CODE(c.get<ho_success_s>().unpack(bref));
      break;
    case types::init_ue_msg:
      HANDLE_CODE(c.get<init_ue_msg_s>().unpack(bref));
      break;
    case types::location_report:
      HANDLE_CODE(c.get<location_report_s>().unpack(bref));
      break;
    case types::location_report_ctrl:
      HANDLE_CODE(c.get<location_report_ctrl_s>().unpack(bref));
      break;
    case types::location_report_fail_ind:
      HANDLE_CODE(c.get<location_report_fail_ind_s>().unpack(bref));
      break;
    case types::multicast_group_paging:
      HANDLE_CODE(c.get<multicast_group_paging_s>().unpack(bref));
      break;
    case types::nas_non_delivery_ind:
      HANDLE_CODE(c.get<nas_non_delivery_ind_s>().unpack(bref));
      break;
    case types::overload_start:
      HANDLE_CODE(c.get<overload_start_s>().unpack(bref));
      break;
    case types::overload_stop:
      HANDLE_CODE(c.get<overload_stop_s>().unpack(bref));
      break;
    case types::paging:
      HANDLE_CODE(c.get<paging_s>().unpack(bref));
      break;
    case types::pdu_session_res_notify:
      HANDLE_CODE(c.get<pdu_session_res_notify_s>().unpack(bref));
      break;
    case types::private_msg:
      HANDLE_CODE(c.get<private_msg_s>().unpack(bref));
      break;
    case types::pws_fail_ind:
      HANDLE_CODE(c.get<pws_fail_ind_s>().unpack(bref));
      break;
    case types::pws_restart_ind:
      HANDLE_CODE(c.get<pws_restart_ind_s>().unpack(bref));
      break;
    case types::ran_cp_relocation_ind:
      HANDLE_CODE(c.get<ran_cp_relocation_ind_s>().unpack(bref));
      break;
    case types::reroute_nas_request:
      HANDLE_CODE(c.get<reroute_nas_request_s>().unpack(bref));
      break;
    case types::retrieve_ue_info:
      HANDLE_CODE(c.get<retrieve_ue_info_s>().unpack(bref));
      break;
    case types::rrc_inactive_transition_report:
      HANDLE_CODE(c.get<rrc_inactive_transition_report_s>().unpack(bref));
      break;
    case types::secondary_rat_data_usage_report:
      HANDLE_CODE(c.get<secondary_rat_data_usage_report_s>().unpack(bref));
      break;
    case types::trace_fail_ind:
      HANDLE_CODE(c.get<trace_fail_ind_s>().unpack(bref));
      break;
    case types::trace_start:
      HANDLE_CODE(c.get<trace_start_s>().unpack(bref));
      break;
    case types::ue_context_release_request:
      HANDLE_CODE(c.get<ue_context_release_request_s>().unpack(bref));
      break;
    case types::ue_info_transfer:
      HANDLE_CODE(c.get<ue_info_transfer_s>().unpack(bref));
      break;
    case types::ue_radio_cap_info_ind:
      HANDLE_CODE(c.get<ue_radio_cap_info_ind_s>().unpack(bref));
      break;
    case types::ue_tnla_binding_release_request:
      HANDLE_CODE(c.get<ue_tnla_binding_release_request_s>().unpack(bref));
      break;
    case types::ul_nas_transport:
      HANDLE_CODE(c.get<ul_nas_transport_s>().unpack(bref));
      break;
    case types::ul_non_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<ul_non_ue_associated_nrppa_transport_s>().unpack(bref));
      break;
    case types::ul_ran_cfg_transfer:
      HANDLE_CODE(c.get<ul_ran_cfg_transfer_s>().unpack(bref));
      break;
    case types::ul_ran_early_status_transfer:
      HANDLE_CODE(c.get<ul_ran_early_status_transfer_s>().unpack(bref));
      break;
    case types::ul_ran_status_transfer:
      HANDLE_CODE(c.get<ul_ran_status_transfer_s>().unpack(bref));
      break;
    case types::ul_rim_info_transfer:
      HANDLE_CODE(c.get<ul_rim_info_transfer_s>().unpack(bref));
      break;
    case types::ul_ue_associated_nrppa_transport:
      HANDLE_CODE(c.get<ul_ue_associated_nrppa_transport_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ngap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* names[] = {"AMFConfigurationUpdate",
                                "BroadcastSessionModificationRequest",
                                "BroadcastSessionReleaseRequest",
                                "BroadcastSessionSetupRequest",
                                "DistributionSetupRequest",
                                "DistributionReleaseRequest",
                                "HandoverCancel",
                                "HandoverRequired",
                                "HandoverRequest",
                                "InitialContextSetupRequest",
                                "MulticastSessionActivationRequest",
                                "MulticastSessionDeactivationRequest",
                                "MulticastSessionUpdateRequest",
                                "NGReset",
                                "NGSetupRequest",
                                "PathSwitchRequest",
                                "PDUSessionResourceModifyRequest",
                                "PDUSessionResourceModifyIndication",
                                "PDUSessionResourceReleaseCommand",
                                "PDUSessionResourceSetupRequest",
                                "PWSCancelRequest",
                                "RANConfigurationUpdate",
                                "UEContextModificationRequest",
                                "UEContextReleaseCommand",
                                "UEContextResumeRequest",
                                "UEContextSuspendRequest",
                                "UERadioCapabilityCheckRequest",
                                "UERadioCapabilityIDMappingRequest",
                                "WriteReplaceWarningRequest",
                                "AMFCPRelocationIndication",
                                "AMFStatusIndication",
                                "BroadcastSessionReleaseRequired",
                                "CellTrafficTrace",
                                "ConnectionEstablishmentIndication",
                                "DeactivateTrace",
                                "DownlinkNASTransport",
                                "DownlinkNonUEAssociatedNRPPaTransport",
                                "DownlinkRANConfigurationTransfer",
                                "DownlinkRANEarlyStatusTransfer",
                                "DownlinkRANStatusTransfer",
                                "DownlinkRIMInformationTransfer",
                                "DownlinkUEAssociatedNRPPaTransport",
                                "ErrorIndication",
                                "HandoverNotify",
                                "HandoverSuccess",
                                "InitialUEMessage",
                                "LocationReport",
                                "LocationReportingControl",
                                "LocationReportingFailureIndication",
                                "MulticastGroupPaging",
                                "NASNonDeliveryIndication",
                                "OverloadStart",
                                "OverloadStop",
                                "Paging",
                                "PDUSessionResourceNotify",
                                "PrivateMessage",
                                "PWSFailureIndication",
                                "PWSRestartIndication",
                                "RANCPRelocationIndication",
                                "RerouteNASRequest",
                                "RetrieveUEInformation",
                                "RRCInactiveTransitionReport",
                                "SecondaryRATDataUsageReport",
                                "TraceFailureIndication",
                                "TraceStart",
                                "UEContextReleaseRequest",
                                "UEInformationTransfer",
                                "UERadioCapabilityInfoIndication",
                                "UETNLABindingReleaseRequest",
                                "UplinkNASTransport",
                                "UplinkNonUEAssociatedNRPPaTransport",
                                "UplinkRANConfigurationTransfer",
                                "UplinkRANEarlyStatusTransfer",
                                "UplinkRANStatusTransfer",
                                "UplinkRIMInformationTransfer",
                                "UplinkUEAssociatedNRPPaTransport"};
  return convert_enum_idx(names, 76, value, "ngap_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void ngap_elem_procs_o::successful_outcome_c::destroy_()
{
  switch (type_) {
    case types::amf_cfg_upd_ack:
      c.destroy<amf_cfg_upd_ack_s>();
      break;
    case types::broadcast_session_mod_resp:
      c.destroy<broadcast_session_mod_resp_s>();
      break;
    case types::broadcast_session_release_resp:
      c.destroy<broadcast_session_release_resp_s>();
      break;
    case types::broadcast_session_setup_resp:
      c.destroy<broadcast_session_setup_resp_s>();
      break;
    case types::distribution_setup_resp:
      c.destroy<distribution_setup_resp_s>();
      break;
    case types::distribution_release_resp:
      c.destroy<distribution_release_resp_s>();
      break;
    case types::ho_cancel_ack:
      c.destroy<ho_cancel_ack_s>();
      break;
    case types::ho_cmd:
      c.destroy<ho_cmd_s>();
      break;
    case types::ho_request_ack:
      c.destroy<ho_request_ack_s>();
      break;
    case types::init_context_setup_resp:
      c.destroy<init_context_setup_resp_s>();
      break;
    case types::multicast_session_activation_resp:
      c.destroy<multicast_session_activation_resp_s>();
      break;
    case types::multicast_session_deactivation_resp:
      c.destroy<multicast_session_deactivation_resp_s>();
      break;
    case types::multicast_session_upd_resp:
      c.destroy<multicast_session_upd_resp_s>();
      break;
    case types::ng_reset_ack:
      c.destroy<ng_reset_ack_s>();
      break;
    case types::ng_setup_resp:
      c.destroy<ng_setup_resp_s>();
      break;
    case types::path_switch_request_ack:
      c.destroy<path_switch_request_ack_s>();
      break;
    case types::pdu_session_res_modify_resp:
      c.destroy<pdu_session_res_modify_resp_s>();
      break;
    case types::pdu_session_res_modify_confirm:
      c.destroy<pdu_session_res_modify_confirm_s>();
      break;
    case types::pdu_session_res_release_resp:
      c.destroy<pdu_session_res_release_resp_s>();
      break;
    case types::pdu_session_res_setup_resp:
      c.destroy<pdu_session_res_setup_resp_s>();
      break;
    case types::pws_cancel_resp:
      c.destroy<pws_cancel_resp_s>();
      break;
    case types::ran_cfg_upd_ack:
      c.destroy<ran_cfg_upd_ack_s>();
      break;
    case types::ue_context_mod_resp:
      c.destroy<ue_context_mod_resp_s>();
      break;
    case types::ue_context_release_complete:
      c.destroy<ue_context_release_complete_s>();
      break;
    case types::ue_context_resume_resp:
      c.destroy<ue_context_resume_resp_s>();
      break;
    case types::ue_context_suspend_resp:
      c.destroy<ue_context_suspend_resp_s>();
      break;
    case types::ue_radio_cap_check_resp:
      c.destroy<ue_radio_cap_check_resp_s>();
      break;
    case types::ue_radio_cap_id_map_resp:
      c.destroy<ue_radio_cap_id_map_resp_s>();
      break;
    case types::write_replace_warning_resp:
      c.destroy<write_replace_warning_resp_s>();
      break;
    default:
      break;
  }
}
void ngap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::amf_cfg_upd_ack:
      c.init<amf_cfg_upd_ack_s>();
      break;
    case types::broadcast_session_mod_resp:
      c.init<broadcast_session_mod_resp_s>();
      break;
    case types::broadcast_session_release_resp:
      c.init<broadcast_session_release_resp_s>();
      break;
    case types::broadcast_session_setup_resp:
      c.init<broadcast_session_setup_resp_s>();
      break;
    case types::distribution_setup_resp:
      c.init<distribution_setup_resp_s>();
      break;
    case types::distribution_release_resp:
      c.init<distribution_release_resp_s>();
      break;
    case types::ho_cancel_ack:
      c.init<ho_cancel_ack_s>();
      break;
    case types::ho_cmd:
      c.init<ho_cmd_s>();
      break;
    case types::ho_request_ack:
      c.init<ho_request_ack_s>();
      break;
    case types::init_context_setup_resp:
      c.init<init_context_setup_resp_s>();
      break;
    case types::multicast_session_activation_resp:
      c.init<multicast_session_activation_resp_s>();
      break;
    case types::multicast_session_deactivation_resp:
      c.init<multicast_session_deactivation_resp_s>();
      break;
    case types::multicast_session_upd_resp:
      c.init<multicast_session_upd_resp_s>();
      break;
    case types::ng_reset_ack:
      c.init<ng_reset_ack_s>();
      break;
    case types::ng_setup_resp:
      c.init<ng_setup_resp_s>();
      break;
    case types::path_switch_request_ack:
      c.init<path_switch_request_ack_s>();
      break;
    case types::pdu_session_res_modify_resp:
      c.init<pdu_session_res_modify_resp_s>();
      break;
    case types::pdu_session_res_modify_confirm:
      c.init<pdu_session_res_modify_confirm_s>();
      break;
    case types::pdu_session_res_release_resp:
      c.init<pdu_session_res_release_resp_s>();
      break;
    case types::pdu_session_res_setup_resp:
      c.init<pdu_session_res_setup_resp_s>();
      break;
    case types::pws_cancel_resp:
      c.init<pws_cancel_resp_s>();
      break;
    case types::ran_cfg_upd_ack:
      c.init<ran_cfg_upd_ack_s>();
      break;
    case types::ue_context_mod_resp:
      c.init<ue_context_mod_resp_s>();
      break;
    case types::ue_context_release_complete:
      c.init<ue_context_release_complete_s>();
      break;
    case types::ue_context_resume_resp:
      c.init<ue_context_resume_resp_s>();
      break;
    case types::ue_context_suspend_resp:
      c.init<ue_context_suspend_resp_s>();
      break;
    case types::ue_radio_cap_check_resp:
      c.init<ue_radio_cap_check_resp_s>();
      break;
    case types::ue_radio_cap_id_map_resp:
      c.init<ue_radio_cap_id_map_resp_s>();
      break;
    case types::write_replace_warning_resp:
      c.init<write_replace_warning_resp_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::successful_outcome_c");
  }
}
ngap_elem_procs_o::successful_outcome_c::successful_outcome_c(const ngap_elem_procs_o::successful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::amf_cfg_upd_ack:
      c.init(other.c.get<amf_cfg_upd_ack_s>());
      break;
    case types::broadcast_session_mod_resp:
      c.init(other.c.get<broadcast_session_mod_resp_s>());
      break;
    case types::broadcast_session_release_resp:
      c.init(other.c.get<broadcast_session_release_resp_s>());
      break;
    case types::broadcast_session_setup_resp:
      c.init(other.c.get<broadcast_session_setup_resp_s>());
      break;
    case types::distribution_setup_resp:
      c.init(other.c.get<distribution_setup_resp_s>());
      break;
    case types::distribution_release_resp:
      c.init(other.c.get<distribution_release_resp_s>());
      break;
    case types::ho_cancel_ack:
      c.init(other.c.get<ho_cancel_ack_s>());
      break;
    case types::ho_cmd:
      c.init(other.c.get<ho_cmd_s>());
      break;
    case types::ho_request_ack:
      c.init(other.c.get<ho_request_ack_s>());
      break;
    case types::init_context_setup_resp:
      c.init(other.c.get<init_context_setup_resp_s>());
      break;
    case types::multicast_session_activation_resp:
      c.init(other.c.get<multicast_session_activation_resp_s>());
      break;
    case types::multicast_session_deactivation_resp:
      c.init(other.c.get<multicast_session_deactivation_resp_s>());
      break;
    case types::multicast_session_upd_resp:
      c.init(other.c.get<multicast_session_upd_resp_s>());
      break;
    case types::ng_reset_ack:
      c.init(other.c.get<ng_reset_ack_s>());
      break;
    case types::ng_setup_resp:
      c.init(other.c.get<ng_setup_resp_s>());
      break;
    case types::path_switch_request_ack:
      c.init(other.c.get<path_switch_request_ack_s>());
      break;
    case types::pdu_session_res_modify_resp:
      c.init(other.c.get<pdu_session_res_modify_resp_s>());
      break;
    case types::pdu_session_res_modify_confirm:
      c.init(other.c.get<pdu_session_res_modify_confirm_s>());
      break;
    case types::pdu_session_res_release_resp:
      c.init(other.c.get<pdu_session_res_release_resp_s>());
      break;
    case types::pdu_session_res_setup_resp:
      c.init(other.c.get<pdu_session_res_setup_resp_s>());
      break;
    case types::pws_cancel_resp:
      c.init(other.c.get<pws_cancel_resp_s>());
      break;
    case types::ran_cfg_upd_ack:
      c.init(other.c.get<ran_cfg_upd_ack_s>());
      break;
    case types::ue_context_mod_resp:
      c.init(other.c.get<ue_context_mod_resp_s>());
      break;
    case types::ue_context_release_complete:
      c.init(other.c.get<ue_context_release_complete_s>());
      break;
    case types::ue_context_resume_resp:
      c.init(other.c.get<ue_context_resume_resp_s>());
      break;
    case types::ue_context_suspend_resp:
      c.init(other.c.get<ue_context_suspend_resp_s>());
      break;
    case types::ue_radio_cap_check_resp:
      c.init(other.c.get<ue_radio_cap_check_resp_s>());
      break;
    case types::ue_radio_cap_id_map_resp:
      c.init(other.c.get<ue_radio_cap_id_map_resp_s>());
      break;
    case types::write_replace_warning_resp:
      c.init(other.c.get<write_replace_warning_resp_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::successful_outcome_c");
  }
}
ngap_elem_procs_o::successful_outcome_c&
ngap_elem_procs_o::successful_outcome_c::operator=(const ngap_elem_procs_o::successful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::amf_cfg_upd_ack:
      c.set(other.c.get<amf_cfg_upd_ack_s>());
      break;
    case types::broadcast_session_mod_resp:
      c.set(other.c.get<broadcast_session_mod_resp_s>());
      break;
    case types::broadcast_session_release_resp:
      c.set(other.c.get<broadcast_session_release_resp_s>());
      break;
    case types::broadcast_session_setup_resp:
      c.set(other.c.get<broadcast_session_setup_resp_s>());
      break;
    case types::distribution_setup_resp:
      c.set(other.c.get<distribution_setup_resp_s>());
      break;
    case types::distribution_release_resp:
      c.set(other.c.get<distribution_release_resp_s>());
      break;
    case types::ho_cancel_ack:
      c.set(other.c.get<ho_cancel_ack_s>());
      break;
    case types::ho_cmd:
      c.set(other.c.get<ho_cmd_s>());
      break;
    case types::ho_request_ack:
      c.set(other.c.get<ho_request_ack_s>());
      break;
    case types::init_context_setup_resp:
      c.set(other.c.get<init_context_setup_resp_s>());
      break;
    case types::multicast_session_activation_resp:
      c.set(other.c.get<multicast_session_activation_resp_s>());
      break;
    case types::multicast_session_deactivation_resp:
      c.set(other.c.get<multicast_session_deactivation_resp_s>());
      break;
    case types::multicast_session_upd_resp:
      c.set(other.c.get<multicast_session_upd_resp_s>());
      break;
    case types::ng_reset_ack:
      c.set(other.c.get<ng_reset_ack_s>());
      break;
    case types::ng_setup_resp:
      c.set(other.c.get<ng_setup_resp_s>());
      break;
    case types::path_switch_request_ack:
      c.set(other.c.get<path_switch_request_ack_s>());
      break;
    case types::pdu_session_res_modify_resp:
      c.set(other.c.get<pdu_session_res_modify_resp_s>());
      break;
    case types::pdu_session_res_modify_confirm:
      c.set(other.c.get<pdu_session_res_modify_confirm_s>());
      break;
    case types::pdu_session_res_release_resp:
      c.set(other.c.get<pdu_session_res_release_resp_s>());
      break;
    case types::pdu_session_res_setup_resp:
      c.set(other.c.get<pdu_session_res_setup_resp_s>());
      break;
    case types::pws_cancel_resp:
      c.set(other.c.get<pws_cancel_resp_s>());
      break;
    case types::ran_cfg_upd_ack:
      c.set(other.c.get<ran_cfg_upd_ack_s>());
      break;
    case types::ue_context_mod_resp:
      c.set(other.c.get<ue_context_mod_resp_s>());
      break;
    case types::ue_context_release_complete:
      c.set(other.c.get<ue_context_release_complete_s>());
      break;
    case types::ue_context_resume_resp:
      c.set(other.c.get<ue_context_resume_resp_s>());
      break;
    case types::ue_context_suspend_resp:
      c.set(other.c.get<ue_context_suspend_resp_s>());
      break;
    case types::ue_radio_cap_check_resp:
      c.set(other.c.get<ue_radio_cap_check_resp_s>());
      break;
    case types::ue_radio_cap_id_map_resp:
      c.set(other.c.get<ue_radio_cap_id_map_resp_s>());
      break;
    case types::write_replace_warning_resp:
      c.set(other.c.get<write_replace_warning_resp_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::successful_outcome_c");
  }

  return *this;
}
amf_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::amf_cfg_upd_ack()
{
  assert_choice_type(types::amf_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<amf_cfg_upd_ack_s>();
}
broadcast_session_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_mod_resp()
{
  assert_choice_type(types::broadcast_session_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_mod_resp_s>();
}
broadcast_session_release_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_release_resp()
{
  assert_choice_type(types::broadcast_session_release_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_release_resp_s>();
}
broadcast_session_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_setup_resp()
{
  assert_choice_type(types::broadcast_session_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_setup_resp_s>();
}
distribution_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_setup_resp()
{
  assert_choice_type(types::distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_setup_resp_s>();
}
distribution_release_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_release_resp()
{
  assert_choice_type(types::distribution_release_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_release_resp_s>();
}
ho_cancel_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_cancel_ack()
{
  assert_choice_type(types::ho_cancel_ack, type_, "SuccessfulOutcome");
  return c.get<ho_cancel_ack_s>();
}
ho_cmd_s& ngap_elem_procs_o::successful_outcome_c::ho_cmd()
{
  assert_choice_type(types::ho_cmd, type_, "SuccessfulOutcome");
  return c.get<ho_cmd_s>();
}
ho_request_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_request_ack()
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
init_context_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::init_context_setup_resp()
{
  assert_choice_type(types::init_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<init_context_setup_resp_s>();
}
multicast_session_activation_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_activation_resp()
{
  assert_choice_type(types::multicast_session_activation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_activation_resp_s>();
}
multicast_session_deactivation_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_deactivation_resp()
{
  assert_choice_type(types::multicast_session_deactivation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_deactivation_resp_s>();
}
multicast_session_upd_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_upd_resp()
{
  assert_choice_type(types::multicast_session_upd_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_upd_resp_s>();
}
ng_reset_ack_s& ngap_elem_procs_o::successful_outcome_c::ng_reset_ack()
{
  assert_choice_type(types::ng_reset_ack, type_, "SuccessfulOutcome");
  return c.get<ng_reset_ack_s>();
}
ng_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::ng_setup_resp()
{
  assert_choice_type(types::ng_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ng_setup_resp_s>();
}
path_switch_request_ack_s& ngap_elem_procs_o::successful_outcome_c::path_switch_request_ack()
{
  assert_choice_type(types::path_switch_request_ack, type_, "SuccessfulOutcome");
  return c.get<path_switch_request_ack_s>();
}
pdu_session_res_modify_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_resp()
{
  assert_choice_type(types::pdu_session_res_modify_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_resp_s>();
}
pdu_session_res_modify_confirm_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_confirm()
{
  assert_choice_type(types::pdu_session_res_modify_confirm, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_confirm_s>();
}
pdu_session_res_release_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_release_resp()
{
  assert_choice_type(types::pdu_session_res_release_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_release_resp_s>();
}
pdu_session_res_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_setup_resp()
{
  assert_choice_type(types::pdu_session_res_setup_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_setup_resp_s>();
}
pws_cancel_resp_s& ngap_elem_procs_o::successful_outcome_c::pws_cancel_resp()
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
ran_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::ran_cfg_upd_ack()
{
  assert_choice_type(types::ran_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ran_cfg_upd_ack_s>();
}
ue_context_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_mod_resp()
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
ue_context_release_complete_s& ngap_elem_procs_o::successful_outcome_c::ue_context_release_complete()
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
ue_context_resume_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_resume_resp()
{
  assert_choice_type(types::ue_context_resume_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_resume_resp_s>();
}
ue_context_suspend_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_suspend_resp()
{
  assert_choice_type(types::ue_context_suspend_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_suspend_resp_s>();
}
ue_radio_cap_check_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_check_resp()
{
  assert_choice_type(types::ue_radio_cap_check_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_check_resp_s>();
}
ue_radio_cap_id_map_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_id_map_resp()
{
  assert_choice_type(types::ue_radio_cap_id_map_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_id_map_resp_s>();
}
write_replace_warning_resp_s& ngap_elem_procs_o::successful_outcome_c::write_replace_warning_resp()
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}
const amf_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::amf_cfg_upd_ack() const
{
  assert_choice_type(types::amf_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<amf_cfg_upd_ack_s>();
}
const broadcast_session_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_mod_resp() const
{
  assert_choice_type(types::broadcast_session_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_mod_resp_s>();
}
const broadcast_session_release_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_release_resp() const
{
  assert_choice_type(types::broadcast_session_release_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_release_resp_s>();
}
const broadcast_session_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_setup_resp() const
{
  assert_choice_type(types::broadcast_session_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_setup_resp_s>();
}
const distribution_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_setup_resp() const
{
  assert_choice_type(types::distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_setup_resp_s>();
}
const distribution_release_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_release_resp() const
{
  assert_choice_type(types::distribution_release_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_release_resp_s>();
}
const ho_cancel_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_cancel_ack() const
{
  assert_choice_type(types::ho_cancel_ack, type_, "SuccessfulOutcome");
  return c.get<ho_cancel_ack_s>();
}
const ho_cmd_s& ngap_elem_procs_o::successful_outcome_c::ho_cmd() const
{
  assert_choice_type(types::ho_cmd, type_, "SuccessfulOutcome");
  return c.get<ho_cmd_s>();
}
const ho_request_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_request_ack() const
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
const init_context_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::init_context_setup_resp() const
{
  assert_choice_type(types::init_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<init_context_setup_resp_s>();
}
const multicast_session_activation_resp_s&
ngap_elem_procs_o::successful_outcome_c::multicast_session_activation_resp() const
{
  assert_choice_type(types::multicast_session_activation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_activation_resp_s>();
}
const multicast_session_deactivation_resp_s&
ngap_elem_procs_o::successful_outcome_c::multicast_session_deactivation_resp() const
{
  assert_choice_type(types::multicast_session_deactivation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_deactivation_resp_s>();
}
const multicast_session_upd_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_upd_resp() const
{
  assert_choice_type(types::multicast_session_upd_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_upd_resp_s>();
}
const ng_reset_ack_s& ngap_elem_procs_o::successful_outcome_c::ng_reset_ack() const
{
  assert_choice_type(types::ng_reset_ack, type_, "SuccessfulOutcome");
  return c.get<ng_reset_ack_s>();
}
const ng_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::ng_setup_resp() const
{
  assert_choice_type(types::ng_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ng_setup_resp_s>();
}
const path_switch_request_ack_s& ngap_elem_procs_o::successful_outcome_c::path_switch_request_ack() const
{
  assert_choice_type(types::path_switch_request_ack, type_, "SuccessfulOutcome");
  return c.get<path_switch_request_ack_s>();
}
const pdu_session_res_modify_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_resp() const
{
  assert_choice_type(types::pdu_session_res_modify_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_resp_s>();
}
const pdu_session_res_modify_confirm_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_confirm() const
{
  assert_choice_type(types::pdu_session_res_modify_confirm, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_confirm_s>();
}
const pdu_session_res_release_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_release_resp() const
{
  assert_choice_type(types::pdu_session_res_release_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_release_resp_s>();
}
const pdu_session_res_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_setup_resp() const
{
  assert_choice_type(types::pdu_session_res_setup_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_setup_resp_s>();
}
const pws_cancel_resp_s& ngap_elem_procs_o::successful_outcome_c::pws_cancel_resp() const
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
const ran_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::ran_cfg_upd_ack() const
{
  assert_choice_type(types::ran_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ran_cfg_upd_ack_s>();
}
const ue_context_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_mod_resp() const
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
const ue_context_release_complete_s& ngap_elem_procs_o::successful_outcome_c::ue_context_release_complete() const
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
const ue_context_resume_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_resume_resp() const
{
  assert_choice_type(types::ue_context_resume_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_resume_resp_s>();
}
const ue_context_suspend_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_suspend_resp() const
{
  assert_choice_type(types::ue_context_suspend_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_suspend_resp_s>();
}
const ue_radio_cap_check_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_check_resp() const
{
  assert_choice_type(types::ue_radio_cap_check_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_check_resp_s>();
}
const ue_radio_cap_id_map_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_id_map_resp() const
{
  assert_choice_type(types::ue_radio_cap_id_map_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_id_map_resp_s>();
}
const write_replace_warning_resp_s& ngap_elem_procs_o::successful_outcome_c::write_replace_warning_resp() const
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}
void ngap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::amf_cfg_upd_ack:
      j.write_fieldname("AMFConfigurationUpdateAcknowledge");
      c.get<amf_cfg_upd_ack_s>().to_json(j);
      break;
    case types::broadcast_session_mod_resp:
      j.write_fieldname("BroadcastSessionModificationResponse");
      c.get<broadcast_session_mod_resp_s>().to_json(j);
      break;
    case types::broadcast_session_release_resp:
      j.write_fieldname("BroadcastSessionReleaseResponse");
      c.get<broadcast_session_release_resp_s>().to_json(j);
      break;
    case types::broadcast_session_setup_resp:
      j.write_fieldname("BroadcastSessionSetupResponse");
      c.get<broadcast_session_setup_resp_s>().to_json(j);
      break;
    case types::distribution_setup_resp:
      j.write_fieldname("DistributionSetupResponse");
      c.get<distribution_setup_resp_s>().to_json(j);
      break;
    case types::distribution_release_resp:
      j.write_fieldname("DistributionReleaseResponse");
      c.get<distribution_release_resp_s>().to_json(j);
      break;
    case types::ho_cancel_ack:
      j.write_fieldname("HandoverCancelAcknowledge");
      c.get<ho_cancel_ack_s>().to_json(j);
      break;
    case types::ho_cmd:
      j.write_fieldname("HandoverCommand");
      c.get<ho_cmd_s>().to_json(j);
      break;
    case types::ho_request_ack:
      j.write_fieldname("HandoverRequestAcknowledge");
      c.get<ho_request_ack_s>().to_json(j);
      break;
    case types::init_context_setup_resp:
      j.write_fieldname("InitialContextSetupResponse");
      c.get<init_context_setup_resp_s>().to_json(j);
      break;
    case types::multicast_session_activation_resp:
      j.write_fieldname("MulticastSessionActivationResponse");
      c.get<multicast_session_activation_resp_s>().to_json(j);
      break;
    case types::multicast_session_deactivation_resp:
      j.write_fieldname("MulticastSessionDeactivationResponse");
      c.get<multicast_session_deactivation_resp_s>().to_json(j);
      break;
    case types::multicast_session_upd_resp:
      j.write_fieldname("MulticastSessionUpdateResponse");
      c.get<multicast_session_upd_resp_s>().to_json(j);
      break;
    case types::ng_reset_ack:
      j.write_fieldname("NGResetAcknowledge");
      c.get<ng_reset_ack_s>().to_json(j);
      break;
    case types::ng_setup_resp:
      j.write_fieldname("NGSetupResponse");
      c.get<ng_setup_resp_s>().to_json(j);
      break;
    case types::path_switch_request_ack:
      j.write_fieldname("PathSwitchRequestAcknowledge");
      c.get<path_switch_request_ack_s>().to_json(j);
      break;
    case types::pdu_session_res_modify_resp:
      j.write_fieldname("PDUSessionResourceModifyResponse");
      c.get<pdu_session_res_modify_resp_s>().to_json(j);
      break;
    case types::pdu_session_res_modify_confirm:
      j.write_fieldname("PDUSessionResourceModifyConfirm");
      c.get<pdu_session_res_modify_confirm_s>().to_json(j);
      break;
    case types::pdu_session_res_release_resp:
      j.write_fieldname("PDUSessionResourceReleaseResponse");
      c.get<pdu_session_res_release_resp_s>().to_json(j);
      break;
    case types::pdu_session_res_setup_resp:
      j.write_fieldname("PDUSessionResourceSetupResponse");
      c.get<pdu_session_res_setup_resp_s>().to_json(j);
      break;
    case types::pws_cancel_resp:
      j.write_fieldname("PWSCancelResponse");
      c.get<pws_cancel_resp_s>().to_json(j);
      break;
    case types::ran_cfg_upd_ack:
      j.write_fieldname("RANConfigurationUpdateAcknowledge");
      c.get<ran_cfg_upd_ack_s>().to_json(j);
      break;
    case types::ue_context_mod_resp:
      j.write_fieldname("UEContextModificationResponse");
      c.get<ue_context_mod_resp_s>().to_json(j);
      break;
    case types::ue_context_release_complete:
      j.write_fieldname("UEContextReleaseComplete");
      c.get<ue_context_release_complete_s>().to_json(j);
      break;
    case types::ue_context_resume_resp:
      j.write_fieldname("UEContextResumeResponse");
      c.get<ue_context_resume_resp_s>().to_json(j);
      break;
    case types::ue_context_suspend_resp:
      j.write_fieldname("UEContextSuspendResponse");
      c.get<ue_context_suspend_resp_s>().to_json(j);
      break;
    case types::ue_radio_cap_check_resp:
      j.write_fieldname("UERadioCapabilityCheckResponse");
      c.get<ue_radio_cap_check_resp_s>().to_json(j);
      break;
    case types::ue_radio_cap_id_map_resp:
      j.write_fieldname("UERadioCapabilityIDMappingResponse");
      c.get<ue_radio_cap_id_map_resp_s>().to_json(j);
      break;
    case types::write_replace_warning_resp:
      j.write_fieldname("WriteReplaceWarningResponse");
      c.get<write_replace_warning_resp_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE ngap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::amf_cfg_upd_ack:
      HANDLE_CODE(c.get<amf_cfg_upd_ack_s>().pack(bref));
      break;
    case types::broadcast_session_mod_resp:
      HANDLE_CODE(c.get<broadcast_session_mod_resp_s>().pack(bref));
      break;
    case types::broadcast_session_release_resp:
      HANDLE_CODE(c.get<broadcast_session_release_resp_s>().pack(bref));
      break;
    case types::broadcast_session_setup_resp:
      HANDLE_CODE(c.get<broadcast_session_setup_resp_s>().pack(bref));
      break;
    case types::distribution_setup_resp:
      HANDLE_CODE(c.get<distribution_setup_resp_s>().pack(bref));
      break;
    case types::distribution_release_resp:
      HANDLE_CODE(c.get<distribution_release_resp_s>().pack(bref));
      break;
    case types::ho_cancel_ack:
      HANDLE_CODE(c.get<ho_cancel_ack_s>().pack(bref));
      break;
    case types::ho_cmd:
      HANDLE_CODE(c.get<ho_cmd_s>().pack(bref));
      break;
    case types::ho_request_ack:
      HANDLE_CODE(c.get<ho_request_ack_s>().pack(bref));
      break;
    case types::init_context_setup_resp:
      HANDLE_CODE(c.get<init_context_setup_resp_s>().pack(bref));
      break;
    case types::multicast_session_activation_resp:
      HANDLE_CODE(c.get<multicast_session_activation_resp_s>().pack(bref));
      break;
    case types::multicast_session_deactivation_resp:
      HANDLE_CODE(c.get<multicast_session_deactivation_resp_s>().pack(bref));
      break;
    case types::multicast_session_upd_resp:
      HANDLE_CODE(c.get<multicast_session_upd_resp_s>().pack(bref));
      break;
    case types::ng_reset_ack:
      HANDLE_CODE(c.get<ng_reset_ack_s>().pack(bref));
      break;
    case types::ng_setup_resp:
      HANDLE_CODE(c.get<ng_setup_resp_s>().pack(bref));
      break;
    case types::path_switch_request_ack:
      HANDLE_CODE(c.get<path_switch_request_ack_s>().pack(bref));
      break;
    case types::pdu_session_res_modify_resp:
      HANDLE_CODE(c.get<pdu_session_res_modify_resp_s>().pack(bref));
      break;
    case types::pdu_session_res_modify_confirm:
      HANDLE_CODE(c.get<pdu_session_res_modify_confirm_s>().pack(bref));
      break;
    case types::pdu_session_res_release_resp:
      HANDLE_CODE(c.get<pdu_session_res_release_resp_s>().pack(bref));
      break;
    case types::pdu_session_res_setup_resp:
      HANDLE_CODE(c.get<pdu_session_res_setup_resp_s>().pack(bref));
      break;
    case types::pws_cancel_resp:
      HANDLE_CODE(c.get<pws_cancel_resp_s>().pack(bref));
      break;
    case types::ran_cfg_upd_ack:
      HANDLE_CODE(c.get<ran_cfg_upd_ack_s>().pack(bref));
      break;
    case types::ue_context_mod_resp:
      HANDLE_CODE(c.get<ue_context_mod_resp_s>().pack(bref));
      break;
    case types::ue_context_release_complete:
      HANDLE_CODE(c.get<ue_context_release_complete_s>().pack(bref));
      break;
    case types::ue_context_resume_resp:
      HANDLE_CODE(c.get<ue_context_resume_resp_s>().pack(bref));
      break;
    case types::ue_context_suspend_resp:
      HANDLE_CODE(c.get<ue_context_suspend_resp_s>().pack(bref));
      break;
    case types::ue_radio_cap_check_resp:
      HANDLE_CODE(c.get<ue_radio_cap_check_resp_s>().pack(bref));
      break;
    case types::ue_radio_cap_id_map_resp:
      HANDLE_CODE(c.get<ue_radio_cap_id_map_resp_s>().pack(bref));
      break;
    case types::write_replace_warning_resp:
      HANDLE_CODE(c.get<write_replace_warning_resp_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ngap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::amf_cfg_upd_ack:
      HANDLE_CODE(c.get<amf_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::broadcast_session_mod_resp:
      HANDLE_CODE(c.get<broadcast_session_mod_resp_s>().unpack(bref));
      break;
    case types::broadcast_session_release_resp:
      HANDLE_CODE(c.get<broadcast_session_release_resp_s>().unpack(bref));
      break;
    case types::broadcast_session_setup_resp:
      HANDLE_CODE(c.get<broadcast_session_setup_resp_s>().unpack(bref));
      break;
    case types::distribution_setup_resp:
      HANDLE_CODE(c.get<distribution_setup_resp_s>().unpack(bref));
      break;
    case types::distribution_release_resp:
      HANDLE_CODE(c.get<distribution_release_resp_s>().unpack(bref));
      break;
    case types::ho_cancel_ack:
      HANDLE_CODE(c.get<ho_cancel_ack_s>().unpack(bref));
      break;
    case types::ho_cmd:
      HANDLE_CODE(c.get<ho_cmd_s>().unpack(bref));
      break;
    case types::ho_request_ack:
      HANDLE_CODE(c.get<ho_request_ack_s>().unpack(bref));
      break;
    case types::init_context_setup_resp:
      HANDLE_CODE(c.get<init_context_setup_resp_s>().unpack(bref));
      break;
    case types::multicast_session_activation_resp:
      HANDLE_CODE(c.get<multicast_session_activation_resp_s>().unpack(bref));
      break;
    case types::multicast_session_deactivation_resp:
      HANDLE_CODE(c.get<multicast_session_deactivation_resp_s>().unpack(bref));
      break;
    case types::multicast_session_upd_resp:
      HANDLE_CODE(c.get<multicast_session_upd_resp_s>().unpack(bref));
      break;
    case types::ng_reset_ack:
      HANDLE_CODE(c.get<ng_reset_ack_s>().unpack(bref));
      break;
    case types::ng_setup_resp:
      HANDLE_CODE(c.get<ng_setup_resp_s>().unpack(bref));
      break;
    case types::path_switch_request_ack:
      HANDLE_CODE(c.get<path_switch_request_ack_s>().unpack(bref));
      break;
    case types::pdu_session_res_modify_resp:
      HANDLE_CODE(c.get<pdu_session_res_modify_resp_s>().unpack(bref));
      break;
    case types::pdu_session_res_modify_confirm:
      HANDLE_CODE(c.get<pdu_session_res_modify_confirm_s>().unpack(bref));
      break;
    case types::pdu_session_res_release_resp:
      HANDLE_CODE(c.get<pdu_session_res_release_resp_s>().unpack(bref));
      break;
    case types::pdu_session_res_setup_resp:
      HANDLE_CODE(c.get<pdu_session_res_setup_resp_s>().unpack(bref));
      break;
    case types::pws_cancel_resp:
      HANDLE_CODE(c.get<pws_cancel_resp_s>().unpack(bref));
      break;
    case types::ran_cfg_upd_ack:
      HANDLE_CODE(c.get<ran_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::ue_context_mod_resp:
      HANDLE_CODE(c.get<ue_context_mod_resp_s>().unpack(bref));
      break;
    case types::ue_context_release_complete:
      HANDLE_CODE(c.get<ue_context_release_complete_s>().unpack(bref));
      break;
    case types::ue_context_resume_resp:
      HANDLE_CODE(c.get<ue_context_resume_resp_s>().unpack(bref));
      break;
    case types::ue_context_suspend_resp:
      HANDLE_CODE(c.get<ue_context_suspend_resp_s>().unpack(bref));
      break;
    case types::ue_radio_cap_check_resp:
      HANDLE_CODE(c.get<ue_radio_cap_check_resp_s>().unpack(bref));
      break;
    case types::ue_radio_cap_id_map_resp:
      HANDLE_CODE(c.get<ue_radio_cap_id_map_resp_s>().unpack(bref));
      break;
    case types::write_replace_warning_resp:
      HANDLE_CODE(c.get<write_replace_warning_resp_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ngap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"AMFConfigurationUpdateAcknowledge",
                                "BroadcastSessionModificationResponse",
                                "BroadcastSessionReleaseResponse",
                                "BroadcastSessionSetupResponse",
                                "DistributionSetupResponse",
                                "DistributionReleaseResponse",
                                "HandoverCancelAcknowledge",
                                "HandoverCommand",
                                "HandoverRequestAcknowledge",
                                "InitialContextSetupResponse",
                                "MulticastSessionActivationResponse",
                                "MulticastSessionDeactivationResponse",
                                "MulticastSessionUpdateResponse",
                                "NGResetAcknowledge",
                                "NGSetupResponse",
                                "PathSwitchRequestAcknowledge",
                                "PDUSessionResourceModifyResponse",
                                "PDUSessionResourceModifyConfirm",
                                "PDUSessionResourceReleaseResponse",
                                "PDUSessionResourceSetupResponse",
                                "PWSCancelResponse",
                                "RANConfigurationUpdateAcknowledge",
                                "UEContextModificationResponse",
                                "UEContextReleaseComplete",
                                "UEContextResumeResponse",
                                "UEContextSuspendResponse",
                                "UERadioCapabilityCheckResponse",
                                "UERadioCapabilityIDMappingResponse",
                                "WriteReplaceWarningResponse"};
  return convert_enum_idx(names, 29, value, "ngap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void ngap_elem_procs_o::unsuccessful_outcome_c::destroy_()
{
  switch (type_) {
    case types::amf_cfg_upd_fail:
      c.destroy<amf_cfg_upd_fail_s>();
      break;
    case types::broadcast_session_mod_fail:
      c.destroy<broadcast_session_mod_fail_s>();
      break;
    case types::broadcast_session_setup_fail:
      c.destroy<broadcast_session_setup_fail_s>();
      break;
    case types::distribution_setup_fail:
      c.destroy<distribution_setup_fail_s>();
      break;
    case types::ho_prep_fail:
      c.destroy<ho_prep_fail_s>();
      break;
    case types::ho_fail:
      c.destroy<ho_fail_s>();
      break;
    case types::init_context_setup_fail:
      c.destroy<init_context_setup_fail_s>();
      break;
    case types::multicast_session_activation_fail:
      c.destroy<multicast_session_activation_fail_s>();
      break;
    case types::multicast_session_upd_fail:
      c.destroy<multicast_session_upd_fail_s>();
      break;
    case types::ng_setup_fail:
      c.destroy<ng_setup_fail_s>();
      break;
    case types::path_switch_request_fail:
      c.destroy<path_switch_request_fail_s>();
      break;
    case types::ran_cfg_upd_fail:
      c.destroy<ran_cfg_upd_fail_s>();
      break;
    case types::ue_context_mod_fail:
      c.destroy<ue_context_mod_fail_s>();
      break;
    case types::ue_context_resume_fail:
      c.destroy<ue_context_resume_fail_s>();
      break;
    case types::ue_context_suspend_fail:
      c.destroy<ue_context_suspend_fail_s>();
      break;
    default:
      break;
  }
}
void ngap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::amf_cfg_upd_fail:
      c.init<amf_cfg_upd_fail_s>();
      break;
    case types::broadcast_session_mod_fail:
      c.init<broadcast_session_mod_fail_s>();
      break;
    case types::broadcast_session_setup_fail:
      c.init<broadcast_session_setup_fail_s>();
      break;
    case types::distribution_setup_fail:
      c.init<distribution_setup_fail_s>();
      break;
    case types::ho_prep_fail:
      c.init<ho_prep_fail_s>();
      break;
    case types::ho_fail:
      c.init<ho_fail_s>();
      break;
    case types::init_context_setup_fail:
      c.init<init_context_setup_fail_s>();
      break;
    case types::multicast_session_activation_fail:
      c.init<multicast_session_activation_fail_s>();
      break;
    case types::multicast_session_upd_fail:
      c.init<multicast_session_upd_fail_s>();
      break;
    case types::ng_setup_fail:
      c.init<ng_setup_fail_s>();
      break;
    case types::path_switch_request_fail:
      c.init<path_switch_request_fail_s>();
      break;
    case types::ran_cfg_upd_fail:
      c.init<ran_cfg_upd_fail_s>();
      break;
    case types::ue_context_mod_fail:
      c.init<ue_context_mod_fail_s>();
      break;
    case types::ue_context_resume_fail:
      c.init<ue_context_resume_fail_s>();
      break;
    case types::ue_context_suspend_fail:
      c.init<ue_context_suspend_fail_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::unsuccessful_outcome_c");
  }
}
ngap_elem_procs_o::unsuccessful_outcome_c::unsuccessful_outcome_c(
    const ngap_elem_procs_o::unsuccessful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::amf_cfg_upd_fail:
      c.init(other.c.get<amf_cfg_upd_fail_s>());
      break;
    case types::broadcast_session_mod_fail:
      c.init(other.c.get<broadcast_session_mod_fail_s>());
      break;
    case types::broadcast_session_setup_fail:
      c.init(other.c.get<broadcast_session_setup_fail_s>());
      break;
    case types::distribution_setup_fail:
      c.init(other.c.get<distribution_setup_fail_s>());
      break;
    case types::ho_prep_fail:
      c.init(other.c.get<ho_prep_fail_s>());
      break;
    case types::ho_fail:
      c.init(other.c.get<ho_fail_s>());
      break;
    case types::init_context_setup_fail:
      c.init(other.c.get<init_context_setup_fail_s>());
      break;
    case types::multicast_session_activation_fail:
      c.init(other.c.get<multicast_session_activation_fail_s>());
      break;
    case types::multicast_session_upd_fail:
      c.init(other.c.get<multicast_session_upd_fail_s>());
      break;
    case types::ng_setup_fail:
      c.init(other.c.get<ng_setup_fail_s>());
      break;
    case types::path_switch_request_fail:
      c.init(other.c.get<path_switch_request_fail_s>());
      break;
    case types::ran_cfg_upd_fail:
      c.init(other.c.get<ran_cfg_upd_fail_s>());
      break;
    case types::ue_context_mod_fail:
      c.init(other.c.get<ue_context_mod_fail_s>());
      break;
    case types::ue_context_resume_fail:
      c.init(other.c.get<ue_context_resume_fail_s>());
      break;
    case types::ue_context_suspend_fail:
      c.init(other.c.get<ue_context_suspend_fail_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::unsuccessful_outcome_c");
  }
}
ngap_elem_procs_o::unsuccessful_outcome_c&
ngap_elem_procs_o::unsuccessful_outcome_c::operator=(const ngap_elem_procs_o::unsuccessful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::amf_cfg_upd_fail:
      c.set(other.c.get<amf_cfg_upd_fail_s>());
      break;
    case types::broadcast_session_mod_fail:
      c.set(other.c.get<broadcast_session_mod_fail_s>());
      break;
    case types::broadcast_session_setup_fail:
      c.set(other.c.get<broadcast_session_setup_fail_s>());
      break;
    case types::distribution_setup_fail:
      c.set(other.c.get<distribution_setup_fail_s>());
      break;
    case types::ho_prep_fail:
      c.set(other.c.get<ho_prep_fail_s>());
      break;
    case types::ho_fail:
      c.set(other.c.get<ho_fail_s>());
      break;
    case types::init_context_setup_fail:
      c.set(other.c.get<init_context_setup_fail_s>());
      break;
    case types::multicast_session_activation_fail:
      c.set(other.c.get<multicast_session_activation_fail_s>());
      break;
    case types::multicast_session_upd_fail:
      c.set(other.c.get<multicast_session_upd_fail_s>());
      break;
    case types::ng_setup_fail:
      c.set(other.c.get<ng_setup_fail_s>());
      break;
    case types::path_switch_request_fail:
      c.set(other.c.get<path_switch_request_fail_s>());
      break;
    case types::ran_cfg_upd_fail:
      c.set(other.c.get<ran_cfg_upd_fail_s>());
      break;
    case types::ue_context_mod_fail:
      c.set(other.c.get<ue_context_mod_fail_s>());
      break;
    case types::ue_context_resume_fail:
      c.set(other.c.get<ue_context_resume_fail_s>());
      break;
    case types::ue_context_suspend_fail:
      c.set(other.c.get<ue_context_suspend_fail_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::unsuccessful_outcome_c");
  }

  return *this;
}
amf_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::amf_cfg_upd_fail()
{
  assert_choice_type(types::amf_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<amf_cfg_upd_fail_s>();
}
broadcast_session_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_mod_fail()
{
  assert_choice_type(types::broadcast_session_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_mod_fail_s>();
}
broadcast_session_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_setup_fail()
{
  assert_choice_type(types::broadcast_session_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_setup_fail_s>();
}
distribution_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::distribution_setup_fail()
{
  assert_choice_type(types::distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<distribution_setup_fail_s>();
}
ho_prep_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail()
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
ho_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_fail()
{
  assert_choice_type(types::ho_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_fail_s>();
}
init_context_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::init_context_setup_fail()
{
  assert_choice_type(types::init_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<init_context_setup_fail_s>();
}
multicast_session_activation_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_activation_fail()
{
  assert_choice_type(types::multicast_session_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_activation_fail_s>();
}
multicast_session_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_upd_fail()
{
  assert_choice_type(types::multicast_session_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_upd_fail_s>();
}
ng_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ng_setup_fail()
{
  assert_choice_type(types::ng_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ng_setup_fail_s>();
}
path_switch_request_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::path_switch_request_fail()
{
  assert_choice_type(types::path_switch_request_fail, type_, "UnsuccessfulOutcome");
  return c.get<path_switch_request_fail_s>();
}
ran_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ran_cfg_upd_fail()
{
  assert_choice_type(types::ran_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ran_cfg_upd_fail_s>();
}
ue_context_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail()
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
ue_context_resume_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_resume_fail()
{
  assert_choice_type(types::ue_context_resume_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_resume_fail_s>();
}
ue_context_suspend_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_suspend_fail()
{
  assert_choice_type(types::ue_context_suspend_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_suspend_fail_s>();
}
const amf_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::amf_cfg_upd_fail() const
{
  assert_choice_type(types::amf_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<amf_cfg_upd_fail_s>();
}
const broadcast_session_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_mod_fail() const
{
  assert_choice_type(types::broadcast_session_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_mod_fail_s>();
}
const broadcast_session_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_setup_fail() const
{
  assert_choice_type(types::broadcast_session_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_setup_fail_s>();
}
const distribution_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::distribution_setup_fail() const
{
  assert_choice_type(types::distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<distribution_setup_fail_s>();
}
const ho_prep_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail() const
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
const ho_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_fail() const
{
  assert_choice_type(types::ho_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_fail_s>();
}
const init_context_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::init_context_setup_fail() const
{
  assert_choice_type(types::init_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<init_context_setup_fail_s>();
}
const multicast_session_activation_fail_s&
ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_activation_fail() const
{
  assert_choice_type(types::multicast_session_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_activation_fail_s>();
}
const multicast_session_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_upd_fail() const
{
  assert_choice_type(types::multicast_session_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_upd_fail_s>();
}
const ng_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ng_setup_fail() const
{
  assert_choice_type(types::ng_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ng_setup_fail_s>();
}
const path_switch_request_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::path_switch_request_fail() const
{
  assert_choice_type(types::path_switch_request_fail, type_, "UnsuccessfulOutcome");
  return c.get<path_switch_request_fail_s>();
}
const ran_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ran_cfg_upd_fail() const
{
  assert_choice_type(types::ran_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ran_cfg_upd_fail_s>();
}
const ue_context_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail() const
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
const ue_context_resume_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_resume_fail() const
{
  assert_choice_type(types::ue_context_resume_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_resume_fail_s>();
}
const ue_context_suspend_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_suspend_fail() const
{
  assert_choice_type(types::ue_context_suspend_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_suspend_fail_s>();
}
void ngap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::amf_cfg_upd_fail:
      j.write_fieldname("AMFConfigurationUpdateFailure");
      c.get<amf_cfg_upd_fail_s>().to_json(j);
      break;
    case types::broadcast_session_mod_fail:
      j.write_fieldname("BroadcastSessionModificationFailure");
      c.get<broadcast_session_mod_fail_s>().to_json(j);
      break;
    case types::broadcast_session_setup_fail:
      j.write_fieldname("BroadcastSessionSetupFailure");
      c.get<broadcast_session_setup_fail_s>().to_json(j);
      break;
    case types::distribution_setup_fail:
      j.write_fieldname("DistributionSetupFailure");
      c.get<distribution_setup_fail_s>().to_json(j);
      break;
    case types::ho_prep_fail:
      j.write_fieldname("HandoverPreparationFailure");
      c.get<ho_prep_fail_s>().to_json(j);
      break;
    case types::ho_fail:
      j.write_fieldname("HandoverFailure");
      c.get<ho_fail_s>().to_json(j);
      break;
    case types::init_context_setup_fail:
      j.write_fieldname("InitialContextSetupFailure");
      c.get<init_context_setup_fail_s>().to_json(j);
      break;
    case types::multicast_session_activation_fail:
      j.write_fieldname("MulticastSessionActivationFailure");
      c.get<multicast_session_activation_fail_s>().to_json(j);
      break;
    case types::multicast_session_upd_fail:
      j.write_fieldname("MulticastSessionUpdateFailure");
      c.get<multicast_session_upd_fail_s>().to_json(j);
      break;
    case types::ng_setup_fail:
      j.write_fieldname("NGSetupFailure");
      c.get<ng_setup_fail_s>().to_json(j);
      break;
    case types::path_switch_request_fail:
      j.write_fieldname("PathSwitchRequestFailure");
      c.get<path_switch_request_fail_s>().to_json(j);
      break;
    case types::ran_cfg_upd_fail:
      j.write_fieldname("RANConfigurationUpdateFailure");
      c.get<ran_cfg_upd_fail_s>().to_json(j);
      break;
    case types::ue_context_mod_fail:
      j.write_fieldname("UEContextModificationFailure");
      c.get<ue_context_mod_fail_s>().to_json(j);
      break;
    case types::ue_context_resume_fail:
      j.write_fieldname("UEContextResumeFailure");
      c.get<ue_context_resume_fail_s>().to_json(j);
      break;
    case types::ue_context_suspend_fail:
      j.write_fieldname("UEContextSuspendFailure");
      c.get<ue_context_suspend_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE ngap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::amf_cfg_upd_fail:
      HANDLE_CODE(c.get<amf_cfg_upd_fail_s>().pack(bref));
      break;
    case types::broadcast_session_mod_fail:
      HANDLE_CODE(c.get<broadcast_session_mod_fail_s>().pack(bref));
      break;
    case types::broadcast_session_setup_fail:
      HANDLE_CODE(c.get<broadcast_session_setup_fail_s>().pack(bref));
      break;
    case types::distribution_setup_fail:
      HANDLE_CODE(c.get<distribution_setup_fail_s>().pack(bref));
      break;
    case types::ho_prep_fail:
      HANDLE_CODE(c.get<ho_prep_fail_s>().pack(bref));
      break;
    case types::ho_fail:
      HANDLE_CODE(c.get<ho_fail_s>().pack(bref));
      break;
    case types::init_context_setup_fail:
      HANDLE_CODE(c.get<init_context_setup_fail_s>().pack(bref));
      break;
    case types::multicast_session_activation_fail:
      HANDLE_CODE(c.get<multicast_session_activation_fail_s>().pack(bref));
      break;
    case types::multicast_session_upd_fail:
      HANDLE_CODE(c.get<multicast_session_upd_fail_s>().pack(bref));
      break;
    case types::ng_setup_fail:
      HANDLE_CODE(c.get<ng_setup_fail_s>().pack(bref));
      break;
    case types::path_switch_request_fail:
      HANDLE_CODE(c.get<path_switch_request_fail_s>().pack(bref));
      break;
    case types::ran_cfg_upd_fail:
      HANDLE_CODE(c.get<ran_cfg_upd_fail_s>().pack(bref));
      break;
    case types::ue_context_mod_fail:
      HANDLE_CODE(c.get<ue_context_mod_fail_s>().pack(bref));
      break;
    case types::ue_context_resume_fail:
      HANDLE_CODE(c.get<ue_context_resume_fail_s>().pack(bref));
      break;
    case types::ue_context_suspend_fail:
      HANDLE_CODE(c.get<ue_context_suspend_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ngap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::amf_cfg_upd_fail:
      HANDLE_CODE(c.get<amf_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::broadcast_session_mod_fail:
      HANDLE_CODE(c.get<broadcast_session_mod_fail_s>().unpack(bref));
      break;
    case types::broadcast_session_setup_fail:
      HANDLE_CODE(c.get<broadcast_session_setup_fail_s>().unpack(bref));
      break;
    case types::distribution_setup_fail:
      HANDLE_CODE(c.get<distribution_setup_fail_s>().unpack(bref));
      break;
    case types::ho_prep_fail:
      HANDLE_CODE(c.get<ho_prep_fail_s>().unpack(bref));
      break;
    case types::ho_fail:
      HANDLE_CODE(c.get<ho_fail_s>().unpack(bref));
      break;
    case types::init_context_setup_fail:
      HANDLE_CODE(c.get<init_context_setup_fail_s>().unpack(bref));
      break;
    case types::multicast_session_activation_fail:
      HANDLE_CODE(c.get<multicast_session_activation_fail_s>().unpack(bref));
      break;
    case types::multicast_session_upd_fail:
      HANDLE_CODE(c.get<multicast_session_upd_fail_s>().unpack(bref));
      break;
    case types::ng_setup_fail:
      HANDLE_CODE(c.get<ng_setup_fail_s>().unpack(bref));
      break;
    case types::path_switch_request_fail:
      HANDLE_CODE(c.get<path_switch_request_fail_s>().unpack(bref));
      break;
    case types::ran_cfg_upd_fail:
      HANDLE_CODE(c.get<ran_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::ue_context_mod_fail:
      HANDLE_CODE(c.get<ue_context_mod_fail_s>().unpack(bref));
      break;
    case types::ue_context_resume_fail:
      HANDLE_CODE(c.get<ue_context_resume_fail_s>().unpack(bref));
      break;
    case types::ue_context_suspend_fail:
      HANDLE_CODE(c.get<ue_context_suspend_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ngap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* names[] = {"AMFConfigurationUpdateFailure",
                                "BroadcastSessionModificationFailure",
                                "BroadcastSessionSetupFailure",
                                "DistributionSetupFailure",
                                "HandoverPreparationFailure",
                                "HandoverFailure",
                                "InitialContextSetupFailure",
                                "MulticastSessionActivationFailure",
                                "MulticastSessionUpdateFailure",
                                "NGSetupFailure",
                                "PathSwitchRequestFailure",
                                "RANConfigurationUpdateFailure",
                                "UEContextModificationFailure",
                                "UEContextResumeFailure",
                                "UEContextSuspendFailure"};
  return convert_enum_idx(names, 15, value, "ngap_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != ngap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = ngap_elem_procs_o::get_init_msg(proc_code);
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
  if (not ngap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = ngap_elem_procs_o::get_crit(proc_code);
  value     = ngap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != ngap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

const char* last_visited_ngran_cell_info_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"LastVisitedPSCellList"};
  return convert_enum_idx(names, 1, value, "last_visited_ngran_cell_info_ext_ies_o::ext_c::types");
}

const char* non_dyn_5qi_descriptor_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..65535,...)", "INTEGER (1..65535,...)"};
  return convert_enum_idx(names, 2, value, "non_dyn_5qi_descriptor_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<non_dyn_5qi_descriptor_ext_ies_o>;

const char* qos_flow_level_qos_params_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QosMonitoringRequest", "INTEGER (1..1800,...)"};
  return convert_enum_idx(names, 2, value, "qos_flow_level_qos_params_ext_ies_o::ext_c::types");
}
uint8_t qos_flow_level_qos_params_ext_ies_o::ext_c::types_opts::to_number() const
{
  if (value == qos_monitoring_report_freq) {
    return 1;
  }
  invalid_enum_number(value, "qos_flow_level_qos_params_ext_ies_o::ext_c::types");
  return 0;
}

template struct asn1::protocol_ext_field_s<qos_flow_level_qos_params_ext_ies_o>;

const char* mbs_session_setup_or_mod_request_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"MBS-SessionTNLInfo5GC", "MBS-QoSFlowsToBeSetupList", "MBS-SessionFSAIDList"};
  return convert_enum_idx(names, 3, value, "mbs_session_setup_or_mod_request_transfer_ies_o::value_c::types");
}
uint8_t mbs_session_setup_or_mod_request_transfer_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "mbs_session_setup_or_mod_request_transfer_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<mbs_session_setup_or_mod_request_transfer_ies_o>;

const char* multicast_session_upd_request_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {
      "MBS-SessionID", "MBS-ServiceArea", "MBS-QoSFlowsToBeSetupList", "QosFlowListWithCause", "MBS-SessionTNLInfo5GC"};
  return convert_enum_idx(names, 5, value, "multicast_session_upd_request_transfer_ies_o::value_c::types");
}
uint8_t multicast_session_upd_request_transfer_ies_o::value_c::types_opts::to_number() const
{
  if (value == mbs_session_tnl_info5_gc) {
    return 5;
  }
  invalid_enum_number(value, "multicast_session_upd_request_transfer_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<multicast_session_upd_request_transfer_ies_o>;

// SuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != ngap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = ngap_elem_procs_o::get_successful_outcome(proc_code);
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
  if (not ngap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = ngap_elem_procs_o::get_crit(proc_code);
  value     = ngap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != ngap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != ngap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = ngap_elem_procs_o::get_unsuccessful_outcome(proc_code);
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
  if (not ngap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = ngap_elem_procs_o::get_crit(proc_code);
  value     = ngap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// NGAP-PDU ::= CHOICE
void ngap_pdu_c::destroy_()
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
void ngap_pdu_c::set(types::options e)
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
      log_invalid_choice_id(type_, "ngap_pdu_c");
  }
}
ngap_pdu_c::ngap_pdu_c(const ngap_pdu_c& other)
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
      log_invalid_choice_id(type_, "ngap_pdu_c");
  }
}
ngap_pdu_c& ngap_pdu_c::operator=(const ngap_pdu_c& other)
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
      log_invalid_choice_id(type_, "ngap_pdu_c");
  }

  return *this;
}
init_msg_s& ngap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& ngap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& ngap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void ngap_pdu_c::to_json(json_writer& j) const
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
      log_invalid_choice_id(type_, "ngap_pdu_c");
  }
  j.end_obj();
}
SRSASN_CODE ngap_pdu_c::pack(bit_ref& bref) const
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
      log_invalid_choice_id(type_, "ngap_pdu_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ngap_pdu_c::unpack(cbit_ref& bref)
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
      log_invalid_choice_id(type_, "ngap_pdu_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ngap_pdu_c::types_opts::to_string() const
{
  static const char* names[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(names, 3, value, "ngap_pdu_c::types");
}

const char* qos_flow_info_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"ULForwarding", "BIT STRING", "BIT STRING"};
  return convert_enum_idx(names, 3, value, "qos_flow_info_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<qos_flow_info_item_ext_ies_o>;

const char* pdu_session_res_modify_confirm_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UPTransportLayerInformation", "UPTransportLayerInformationPairList"};
  return convert_enum_idx(names, 2, value, "pdu_session_res_modify_confirm_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<pdu_session_res_modify_confirm_transfer_ext_ies_o>;

const char* pdu_session_res_modify_ind_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SecondaryRATUsageInformation",
                                "SecurityResult",
                                "QosFlowPerTNLInformation",
                                "QosFlowPerTNLInformationList",
                                "GlobalRANNodeID"};
  return convert_enum_idx(names, 5, value, "pdu_session_res_modify_ind_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<pdu_session_res_modify_ind_transfer_ext_ies_o>;

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

const char* qos_flow_add_or_modify_request_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"TSCTrafficCharacteristics", "RedundantQosFlowIndicator"};
  return convert_enum_idx(names, 2, value, "qos_flow_add_or_modify_request_item_ext_ies_o::ext_c::types");
}

const char* ul_ngu_up_tnl_modify_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UPTransportLayerInformation", "UPTransportLayerInformation"};
  return convert_enum_idx(names, 2, value, "ul_ngu_up_tnl_modify_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<qos_flow_add_or_modify_request_item_ext_ies_o>;

const char* security_ind_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"MaximumIntegrityProtectedDataRate"};
  return convert_enum_idx(names, 1, value, "security_ind_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<ul_ngu_up_tnl_modify_item_ext_ies_o>;

const char* pdu_session_res_modify_request_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDUSessionAggregateMaximumBitRate",
                                "UL-NGU-UP-TNLModifyList",
                                "INTEGER (1..256,...)",
                                "QosFlowAddOrModifyRequestList",
                                "QosFlowListWithCause",
                                "UPTransportLayerInformationList",
                                "OCTET STRING",
                                "UPTransportLayerInformationList",
                                "OCTET STRING",
                                "UPTransportLayerInformation",
                                "SecurityIndication",
                                "MBSSessionSetuporModifyRequestList",
                                "MBSSessionToReleaseList"};
  return convert_enum_idx(names, 13, value, "pdu_session_res_modify_request_transfer_ies_o::value_c::types");
}
uint8_t pdu_session_res_modify_request_transfer_ies_o::value_c::types_opts::to_number() const
{
  if (value == network_instance) {
    return 1;
  }
  invalid_enum_number(value, "pdu_session_res_modify_request_transfer_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<pdu_session_res_modify_request_transfer_ies_o>;

const char* qos_flow_add_or_modify_resp_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..8,...)"};
  return convert_enum_idx(names, 1, value, "qos_flow_add_or_modify_resp_item_ext_ies_o::ext_c::types");
}
uint8_t qos_flow_add_or_modify_resp_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "qos_flow_add_or_modify_resp_item_ext_ies_o::ext_c::types");
}

const char* pdu_session_res_modify_resp_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UPTransportLayerInformationPairList",
                                "UPTransportLayerInformation",
                                "UPTransportLayerInformation",
                                "QosFlowPerTNLInformationList",
                                "UPTransportLayerInformationPairList",
                                "SecondaryRATUsageInformation",
                                "MBS-SupportIndicator",
                                "MBSSessionSetupResponseList",
                                "MBSSessionFailedtoSetupList"};
  return convert_enum_idx(names, 9, value, "pdu_session_res_modify_resp_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<pdu_session_res_modify_resp_transfer_ext_ies_o>;

const char* pdu_session_res_notify_released_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SecondaryRATUsageInformation"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_notify_released_transfer_ext_ies_o::ext_c::types");
}

const char* qos_flow_notify_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..8,...)"};
  return convert_enum_idx(names, 1, value, "qos_flow_notify_item_ext_ies_o::ext_c::types");
}
uint8_t qos_flow_notify_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "qos_flow_notify_item_ext_ies_o::ext_c::types");
}

const char* pdu_session_res_notify_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SecondaryRATUsageInformation", "QosFlowFeedbackList"};
  return convert_enum_idx(names, 2, value, "pdu_session_res_notify_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<pdu_session_res_notify_transfer_ext_ies_o>;

const char* pdu_session_res_release_resp_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"SecondaryRATUsageInformation"};
  return convert_enum_idx(names, 1, value, "pdu_session_res_release_resp_transfer_ext_ies_o::ext_c::types");
}

const char* qos_flow_setup_request_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"TSCTrafficCharacteristics", "RedundantQosFlowIndicator"};
  return convert_enum_idx(names, 2, value, "qos_flow_setup_request_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<qos_flow_setup_request_item_ext_ies_o>;

const char* pdu_session_res_setup_request_transfer_ies_o::value_c::types_opts::to_string() const
{
  static const char* names[] = {"PDUSessionAggregateMaximumBitRate",
                                "UPTransportLayerInformation",
                                "UPTransportLayerInformationList",
                                "DataForwardingNotPossible",
                                "PDUSessionType",
                                "SecurityIndication",
                                "INTEGER (1..256,...)",
                                "QosFlowSetupRequestList",
                                "OCTET STRING",
                                "DirectForwardingPathAvailability",
                                "UPTransportLayerInformation",
                                "UPTransportLayerInformationList",
                                "OCTET STRING",
                                "RedundantPDUSessionInformation",
                                "MBSSessionSetupRequestList"};
  return convert_enum_idx(names, 15, value, "pdu_session_res_setup_request_transfer_ies_o::value_c::types");
}
uint8_t pdu_session_res_setup_request_transfer_ies_o::value_c::types_opts::to_number() const
{
  if (value == network_instance) {
    return 1;
  }
  invalid_enum_number(value, "pdu_session_res_setup_request_transfer_ies_o::value_c::types");
  return 0;
}

template struct asn1::protocol_ie_field_s<pdu_session_res_setup_request_transfer_ies_o>;

const char* pdu_session_res_setup_resp_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QosFlowPerTNLInformation",
                                "QosFlowPerTNLInformationList",
                                "RedundantPDUSessionInformation",
                                "GlobalRANNodeID",
                                "MBS-SupportIndicator",
                                "MBSSessionSetupResponseList",
                                "MBSSessionFailedtoSetupList"};
  return convert_enum_idx(names, 7, value, "pdu_session_res_setup_resp_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<pdu_session_res_setup_resp_transfer_ext_ies_o>;

const char* qos_flow_params_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..65535,...)", "INTEGER (1..65535,...)", "OCTET STRING"};
  return convert_enum_idx(names, 3, value, "qos_flow_params_item_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<qos_flow_params_item_ext_ies_o>;

const char* path_switch_request_ack_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"UPTransportLayerInformationPairList",
                                "UPTransportLayerInformation",
                                "UPTransportLayerInformationPairList",
                                "QosFlowParametersList"};
  return convert_enum_idx(names, 4, value, "path_switch_request_ack_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<path_switch_request_ack_transfer_ext_ies_o>;

const char* qos_flow_accepted_item_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (1..8,...)"};
  return convert_enum_idx(names, 1, value, "qos_flow_accepted_item_ext_ies_o::ext_c::types");
}
uint8_t qos_flow_accepted_item_ext_ies_o::ext_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "qos_flow_accepted_item_ext_ies_o::ext_c::types");
}

const char* path_switch_request_transfer_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"QosFlowPerTNLInformationList",
                                "UPTransportLayerInformation",
                                "DL-NGU-TNLInformationReused",
                                "QosFlowPerTNLInformationList",
                                "RedundantPDUSessionInformation",
                                "GlobalRANNodeID",
                                "MBS-SupportIndicator"};
  return convert_enum_idx(names, 7, value, "path_switch_request_transfer_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<path_switch_request_transfer_ext_ies_o>;

// ProtocolIE-FieldPair{NGAP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{NGAP-PROTOCOL-IES-PAIR}}
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

const char* source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"INTEGER (0..4294967295)",
                                "UEHistoryInformationFromTheUE",
                                "SourceNodeID",
                                "INTEGER (0..4294967295)",
                                "MBS-ActiveSessionInformation-SourcetoTargetList",
                                "QMCConfigInfo",
                                "NGAPIESupportInformationRequestList"};
  return convert_enum_idx(
      names, 7, value, "source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_o>;

const char*
target_ngran_node_to_source_ngran_node_fail_transparent_container_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"NGAPIESupportInformationResponseList"};
  return convert_enum_idx(
      names, 1, value, "target_ngran_node_to_source_ngran_node_fail_transparent_container_ext_ies_o::ext_c::types");
}

const char* target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_o::ext_c::types_opts::to_string() const
{
  static const char* names[] = {"DAPSResponseInfoList",
                                "DirectForwardingPathAvailability",
                                "MBS-ActiveSessionInformation-TargettoSourceList",
                                "NGAPIESupportInformationResponseList"};
  return convert_enum_idx(
      names, 4, value, "target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_o::ext_c::types");
}

template struct asn1::protocol_ext_field_s<target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_o>;
