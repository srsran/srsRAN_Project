/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/asn1/ngap/ngap.h"
#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

// Helper to create PDU from NGAP message
template <class T>
byte_buffer pack_into_pdu(const T& msg, const char* context_name = nullptr)
{
  context_name = context_name == nullptr ? __FUNCTION__ : context_name;
  byte_buffer   pdu{};
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    srslog::fetch_basic_logger("NGAP").error("Failed to pack message in {} - discarding it", context_name);
  }
  return pdu;
}

/// \brief  Convert CU-CP security result to NGAP security result.
/// \param security_result The CU-CP security result.
/// \return The NGAP security result.
inline asn1::ngap::security_result_s cu_cp_security_result_to_ngap_security_result(security_result_t security_result)
{
  asn1::ngap::security_result_s ngap_security_result;

  if (security_result.confidentiality_protection_result == "performed") {
    ngap_security_result.confidentiality_protection_result =
        asn1::ngap::confidentiality_protection_result_opts::options::performed;
  } else {
    ngap_security_result.confidentiality_protection_result =
        asn1::ngap::confidentiality_protection_result_opts::options::not_performed;
  }

  if (security_result.integrity_protection_result == "performed") {
    ngap_security_result.integrity_protection_result.value =
        asn1::ngap::integrity_protection_result_opts::options::performed;
  } else {
    ngap_security_result.integrity_protection_result =
        asn1::ngap::integrity_protection_result_opts::options::not_performed;
  }

  return ngap_security_result;
}

/// \brief Convert CU-CP Associated QoS Flow to NGAP Associated QoS Flow Item.
/// \param cu_cp_qos_flow The CU-CP Associated QoS Flow.
/// \return The NGAP Associated QoS Flow Item.
inline asn1::ngap::associated_qos_flow_item_s
cu_cp_assoc_qos_flow_to_ngap_assoc_qos_flow_item(cu_cp_associated_qos_flow cu_cp_qos_flow)
{
  asn1::ngap::associated_qos_flow_item_s asn1_assoc_qos_item;

  asn1_assoc_qos_item.qos_flow_id = qos_flow_id_to_uint(cu_cp_qos_flow.qos_flow_id);

  if (cu_cp_qos_flow.qos_flow_map_ind.has_value()) {
    asn1_assoc_qos_item.qos_flow_map_ind_present = true;

    if (cu_cp_qos_flow.qos_flow_map_ind.value() == "ul") {
      asn1_assoc_qos_item.qos_flow_map_ind.value =
          asn1::ngap::associated_qos_flow_item_s::qos_flow_map_ind_opts::options::ul;
    } else {
      asn1_assoc_qos_item.qos_flow_map_ind.value =
          asn1::ngap::associated_qos_flow_item_s::qos_flow_map_ind_opts::options::dl;
    }
  }

  return asn1_assoc_qos_item;
}

/// \brief Convert CU-CP QoS Flow Per TNL Info to NGAP QoS Flow Per TNL Info.
/// \param cu_cp_qos_flow_info The CU-CP QoS Flow Per TNL Info.
/// \return The NGAP QoS Flow Per TNL Info.
inline asn1::ngap::qos_flow_per_tnl_info_s
cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(cu_cp_qos_flow_per_tnl_information cu_cp_qos_flow_info)
{
  asn1::ngap::qos_flow_per_tnl_info_s ngap_qos_flow_info;

  up_transport_layer_info_to_asn1(ngap_qos_flow_info.up_transport_layer_info, cu_cp_qos_flow_info.up_tp_layer_info);

  for (const auto& cu_cp_assoc_qos_item : cu_cp_qos_flow_info.associated_qos_flow_list) {
    asn1::ngap::associated_qos_flow_item_s ngap_assoc_qos_item =
        cu_cp_assoc_qos_flow_to_ngap_assoc_qos_flow_item(cu_cp_assoc_qos_item);

    ngap_qos_flow_info.associated_qos_flow_list.push_back(ngap_assoc_qos_item);
  }

  return ngap_qos_flow_info;
}

/// \brief Convert \c cause_t type to NGAP cause.
/// \param cause The cause_t type.
/// \return The NGAP cause.
inline asn1::ngap::cause_c cause_to_ngap_cause(cause_t cause)
{
  asn1::ngap::cause_c ngap_cause;

  switch (cause) {
    case cause_t::radio_network:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::radio_network);
      break;
    case cause_t::transport:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::transport);
      break;
    case cause_t::protocol:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::protocol);
      break;
    case cause_t::misc:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::misc);
      break;
    default:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::nulltype);
      break;
  }

  return ngap_cause;
}

/// \brief Convert NGAP ASN1 cause to \c cause_t type.
/// \param ngap_cause The ASN1 NGAP cause.
/// \return The cause_t type.
inline cause_t ngap_cause_to_cause(asn1::ngap::cause_c ngap_cause)
{
  cause_t cause;

  switch (ngap_cause.type()) {
    case asn1::ngap::cause_c::types_opts::radio_network:
      cause = cause_t::radio_network;
      break;
    case asn1::ngap::cause_c::types_opts::transport:
      cause = cause_t::transport;
      break;
    case asn1::ngap::cause_c::types_opts::protocol:
      cause = cause_t::protocol;
      break;
    case asn1::ngap::cause_c::types_opts::nas:
      cause = cause_t::nas;
      break;
    case asn1::ngap::cause_c::types_opts::misc:
      cause = cause_t::misc;
      break;
    default:
      cause = cause_t::nulltype;
  }
  return cause;
}

/// \brief Convert CU-CP QoS Flow Failed to Setup Item to NGAP QoS Flow With Cause Item.
/// \param cu_cp_failed_item The CU-CP QoS Flow Failed to Setup Item.
/// \return The NGAP QoS Flow With Cause Item.
inline asn1::ngap::qos_flow_with_cause_item_s cu_cp_qos_flow_failed_to_setup_item_to_ngap_qos_flow_with_cause_item(
    cu_cp_qos_flow_failed_to_setup_item cu_cp_failed_item)
{
  asn1::ngap::qos_flow_with_cause_item_s asn1_failed_item;
  asn1_failed_item.qos_flow_id = qos_flow_id_to_uint(cu_cp_failed_item.qos_flow_id);
  asn1_failed_item.cause       = cause_to_ngap_cause(cu_cp_failed_item.cause);

  return asn1_failed_item;
}

/// \brief Convert CU-CP NRCGI to NR Cell Identity.
/// \param ngap_cgi The NGAP NRCGI.
/// \return The NR Cell Identity.
inline nr_cell_id_t cu_cp_nrcgi_to_nr_cell_identity(asn1::ngap::nr_cgi_s& ngap_cgi)
{
  return ngap_cgi.nr_cell_id.to_number();
}

/// \brief Convert CU-CP NRCGI to NR Cell Identity.
/// \param ngap_cgi The NGAP NRCGI.
/// \return The NR Cell Identity.
inline asn1::ngap::user_location_info_nr_s
cu_cp_user_location_info_to_asn1(const cu_cp_user_location_info_nr& cu_cp_user_location_info)
{
  asn1::ngap::user_location_info_nr_s asn1_user_location_info;

  // add nr cgi
  asn1_user_location_info.nr_cgi.nr_cell_id.from_number(cu_cp_user_location_info.nr_cgi.nci);
  asn1_user_location_info.nr_cgi.plmn_id.from_string(cu_cp_user_location_info.nr_cgi.plmn_hex);
  // add tai
  asn1_user_location_info.tai.plmn_id.from_string(cu_cp_user_location_info.tai.plmn_id);
  asn1_user_location_info.tai.tac.from_number(cu_cp_user_location_info.tai.tac);
  // add timestamp
  if (cu_cp_user_location_info.time_stamp.has_value()) {
    asn1_user_location_info.time_stamp_present = true;
    asn1_user_location_info.time_stamp.from_number(cu_cp_user_location_info.time_stamp.value());
  }

  return asn1_user_location_info;
}

/// \brief Convert ASN.1 cause to a human-readable string.
/// \param cause The ASN.1 cause.
/// \return The humand-readable string.
inline std::string asn1_cause_to_string(const asn1::ngap::cause_c& cause)
{
  std::string cause_str = "";

  switch (cause.type()) {
    case asn1::ngap::cause_c::types_opts::radio_network:
      cause_str = cause.radio_network().to_string();
      break;
    case asn1::ngap::cause_c::types_opts::transport:
      cause_str = cause.transport().to_string();
      break;
    case asn1::ngap::cause_c::types_opts::protocol:
      cause_str = cause.protocol().to_string();
      break;
    case asn1::ngap::cause_c::types_opts::nas:
      cause_str = cause.nas().to_string();
      break;
    case asn1::ngap::cause_c::types_opts::misc:
      cause_str = cause.misc().to_string();
      break;
    default:
      cause_str = "unknown";
      break;
  }

  return cause_str;
}

/// \brief Convert common type Initial Context Setup Response message to NGAP Initial Context Setup Response
/// message.
/// \param[out] resp The ASN1 NGAP Initial Context Setup Response message.
/// \param[in] cu_cp_resp The CU-CP Initial Context Setup Response message.
template <typename template_asn1_item>
inline void pdu_session_res_setup_response_item_to_asn1(template_asn1_item&                             asn1_resp,
                                                        const cu_cp_pdu_session_res_setup_response_item resp)
{
  asn1_resp.pdu_session_id = pdu_session_id_to_uint(resp.pdu_session_id);

  asn1::ngap::pdu_session_res_setup_resp_transfer_s response_transfer;

  // Add dLQosFlowPerTNLInformation
  response_transfer.dl_qos_flow_per_tnl_info = cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(
      resp.pdu_session_resource_setup_response_transfer.dlqos_flow_per_tnl_info);

  // Add AdditionalDLQosFlowPerTNLInformation
  for (const auto& cu_cp_qos_flow_info :
       resp.pdu_session_resource_setup_response_transfer.add_dl_qos_flow_per_tnl_info) {
    asn1::ngap::qos_flow_per_tnl_info_item_s ngap_qos_flow_info;
    ngap_qos_flow_info.qos_flow_per_tnl_info =
        cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(cu_cp_qos_flow_info);
    response_transfer.add_dl_qos_flow_per_tnl_info.push_back(ngap_qos_flow_info);
  }

  // Add QosFlowFailedToSetupList
  for (const auto& failed_item : resp.pdu_session_resource_setup_response_transfer.qos_flow_failed_to_setup_list) {
    asn1::ngap::qos_flow_with_cause_item_s asn1_failed_item =
        cu_cp_qos_flow_failed_to_setup_item_to_ngap_qos_flow_with_cause_item(failed_item);
    response_transfer.qos_flow_failed_to_setup_list.push_back(asn1_failed_item);
  }

  // Add SecurityResult
  if (resp.pdu_session_resource_setup_response_transfer.security_result.has_value()) {
    response_transfer.security_result = cu_cp_security_result_to_ngap_security_result(
        resp.pdu_session_resource_setup_response_transfer.security_result.value());
  }

  // Pack pdu_session_res_setup_resp_transfer_s
  byte_buffer pdu = pack_into_pdu(response_transfer);

  asn1_resp.pdu_session_res_setup_resp_transfer.resize(pdu.length());
  std::copy(pdu.begin(), pdu.end(), asn1_resp.pdu_session_res_setup_resp_transfer.begin());
}

/// \brief Convert common type modify response item to ASN1 type message.
/// \param[out] asn1_resp The ASN1 NGAP struct.
/// \param[in] resp The common type struct.
template <typename template_asn1_item>
inline void pdu_session_res_modify_response_item_to_asn1(template_asn1_item& asn1_resp,
                                                         const cu_cp_pdu_session_resource_modify_response_item resp)
{
  asn1_resp.pdu_session_id = pdu_session_id_to_uint(resp.pdu_session_id);

  asn1::ngap::pdu_session_res_modify_resp_transfer_s response_transfer;

  // qos_flow_add_or_modify_resp_list
  if (resp.transfer.qos_flow_add_or_modify_response_list.has_value()) {
    for (const auto& qos_flow : resp.transfer.qos_flow_add_or_modify_response_list.value()) {
      asn1::ngap::qos_flow_add_or_modify_resp_item_s asn1_item;
      asn1_item.qos_flow_id = qos_flow_id_to_uint(qos_flow.qos_flow_id);
      response_transfer.qos_flow_add_or_modify_resp_list.push_back(asn1_item);
    }
  }

  // Pack pdu_session_res_modify_resp_transfer_s
  byte_buffer pdu = pack_into_pdu(response_transfer);

  asn1_resp.pdu_session_res_modify_resp_transfer.resize(pdu.length());
  std::copy(pdu.begin(), pdu.end(), asn1_resp.pdu_session_res_modify_resp_transfer.begin());
}

/// \brief Convert common type Initial Context Setup Response message to NGAP Initial Context Setup Response
/// message.
/// \param[out] resp The ASN1 NGAP Initial Context Setup Response message.
/// \param[in] cu_cp_resp The CU-CP Initial Context Setup Response message.
template <typename template_asn1_item>
inline void pdu_session_res_setup_failed_item_to_asn1(template_asn1_item&                           asn1_resp,
                                                      const cu_cp_pdu_session_res_setup_failed_item resp)
{
  asn1_resp.pdu_session_id = pdu_session_id_to_uint(resp.pdu_session_id);

  asn1::ngap::pdu_session_res_setup_unsuccessful_transfer_s setup_unsuccessful_transfer;
  setup_unsuccessful_transfer.cause = cause_to_ngap_cause(resp.pdu_session_resource_setup_unsuccessful_transfer.cause);

  // TODO: Add crit diagnostics

  // Pack pdu_session_res_setup_unsuccessful_transfer_s
  byte_buffer pdu = pack_into_pdu(setup_unsuccessful_transfer);

  asn1_resp.pdu_session_res_setup_unsuccessful_transfer.resize(pdu.length());
  std::copy(pdu.begin(), pdu.end(), asn1_resp.pdu_session_res_setup_unsuccessful_transfer.begin());
}

/// \brief Convert ASN.1 GUAMI to a common type.
/// \param asn1_guami The ASN.1 GUAMI.
/// \return The common type GUAMI.
inline guami_t asn1_guami_to_guami(const asn1::ngap::guami_s& asn1_guami)
{
  guami_t guami;
  guami.plmn          = asn1_guami.plmn_id.to_string();
  guami.amf_region_id = asn1_guami.amf_region_id.to_number();
  guami.amf_set_id    = asn1_guami.amf_set_id.to_number();
  guami.amf_pointer   = asn1_guami.amf_pointer.to_number();

  return guami;
}

} // namespace srs_cu_cp
} // namespace srsran
