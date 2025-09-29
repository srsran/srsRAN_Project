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

#include "ngap_test_message_validators.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;

using srs_cu_cp::ngap_message;

#define TRUE_OR_RETURN(cond)                                                                                           \
  if (not(cond))                                                                                                       \
    return false;

bool srsran::test_helpers::is_valid_init_ue_message(const ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_INIT_UE_MSG);
  return true;
}

bool srsran::test_helpers::is_valid_ul_nas_transport_message(const ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_UL_NAS_TRANSPORT);
  return true;
}

bool srsran::test_helpers::is_valid_initial_context_setup_response(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  return true;
}

bool srsran::test_helpers::is_valid_initial_context_setup_failure(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::unsuccessful_outcome);
  TRUE_OR_RETURN(msg.pdu.unsuccessful_outcome().proc_code == ASN1_NGAP_ID_INIT_CONTEXT_SETUP);
  return true;
}

bool srsran::test_helpers::is_valid_ue_context_release_request(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_UE_CONTEXT_RELEASE_REQUEST);
  return true;
}

bool srsran::test_helpers::is_valid_ue_radio_capability_info_indication(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_UE_RADIO_CAP_INFO_IND);
  return true;
}

bool srsran::test_helpers::is_valid_ue_context_release_complete(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_NGAP_ID_UE_CONTEXT_RELEASE);
  return true;
}

bool srsran::test_helpers::is_valid_pdu_session_resource_setup_response(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_NGAP_ID_PDU_SESSION_RES_SETUP);
  return true;
}

bool srsran::test_helpers::is_valid_pdu_session_resource_release_response(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_NGAP_ID_PDU_SESSION_RES_RELEASE);
  return true;
}

bool srsran::test_helpers::is_valid_pdu_session_resource_modify_response(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_NGAP_ID_PDU_SESSION_RES_MODIFY);
  return true;
}

bool srsran::test_helpers::is_valid_error_indication(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_ERROR_IND);
  return true;
}

bool srsran::test_helpers::is_valid_handover_request_ack(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  TRUE_OR_RETURN(msg.pdu.successful_outcome().proc_code == ASN1_NGAP_ID_HO_RES_ALLOC);
  return true;
}

bool srsran::test_helpers::is_valid_handover_notify(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_HO_NOTIF);
  return true;
}

bool srsran::test_helpers::is_valid_handover_required(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_HO_PREP);
  return true;
}

bool srsran::test_helpers::is_valid_handover_cancel(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_HO_CANCEL);
  return true;
}

bool srsran::test_helpers::is_valid_ul_ran_status_transfer(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_UL_RAN_STATUS_TRANSFER);
  return true;
}

bool srsran::test_helpers::is_valid_ng_reset(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_NG_RESET);
  return true;
}

bool srsran::test_helpers::is_valid_ul_ue_associated_nrppa_transport(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_UL_UE_ASSOCIATED_NRPPA_TRANSPORT);
  return true;
}

bool srsran::test_helpers::is_valid_ul_non_ue_associated_nrppa_transport(const srs_cu_cp::ngap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_UL_NON_UE_ASSOCIATED_NRPPA_TRANSPORT);
  return true;
}

bool srsran::test_helpers::is_expected_pdu_session_resource_setup_response(
    const ngap_message&                  ngap_pdu,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_to_setup,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_failed_to_setup)
{
  // Check failed PDU sessions
  if (expected_pdu_sessions_failed_to_setup.empty() && ngap_pdu.pdu.successful_outcome()
                                                           .value.pdu_session_res_setup_resp()
                                                           ->pdu_session_res_failed_to_setup_list_su_res_present) {
    return false;
  }

  if (!expected_pdu_sessions_failed_to_setup.empty() && !ngap_pdu.pdu.successful_outcome()
                                                             .value.pdu_session_res_setup_resp()
                                                             ->pdu_session_res_failed_to_setup_list_su_res_present) {
    return false;
  }

  if (expected_pdu_sessions_failed_to_setup.size() != ngap_pdu.pdu.successful_outcome()
                                                          .value.pdu_session_res_setup_resp()
                                                          ->pdu_session_res_failed_to_setup_list_su_res.size()) {
    return false;
  }

  for (const auto& asn1_failed_item : ngap_pdu.pdu.successful_outcome()
                                          .value.pdu_session_res_setup_resp()
                                          ->pdu_session_res_failed_to_setup_list_su_res) {
    if (std::find(expected_pdu_sessions_failed_to_setup.begin(),
                  expected_pdu_sessions_failed_to_setup.end(),
                  uint_to_pdu_session_id(asn1_failed_item.pdu_session_id)) ==
        expected_pdu_sessions_failed_to_setup.end()) {
      return false;
    }
  }

  // Check successful PDU sessions
  if (expected_pdu_sessions_to_setup.empty() &&
      ngap_pdu.pdu.successful_outcome().value.pdu_session_res_setup_resp()->pdu_session_res_setup_list_su_res_present) {
    return false;
  }

  if (!expected_pdu_sessions_to_setup.empty() && !ngap_pdu.pdu.successful_outcome()
                                                      .value.pdu_session_res_setup_resp()
                                                      ->pdu_session_res_setup_list_su_res_present) {
    return false;
  }

  if (expected_pdu_sessions_to_setup.size() !=
      ngap_pdu.pdu.successful_outcome().value.pdu_session_res_setup_resp()->pdu_session_res_setup_list_su_res.size()) {
    return false;
  }

  for (const auto& asn1_setup_item :
       ngap_pdu.pdu.successful_outcome().value.pdu_session_res_setup_resp()->pdu_session_res_setup_list_su_res) {
    if (std::find(expected_pdu_sessions_to_setup.begin(),
                  expected_pdu_sessions_to_setup.end(),
                  uint_to_pdu_session_id(asn1_setup_item.pdu_session_id)) == expected_pdu_sessions_to_setup.end()) {
      return false;
    }
  }

  return true;
}

byte_buffer srsran::test_helpers::get_rrc_container(const srs_cu_cp::ngap_message& msg)
{
  if (msg.pdu.init_msg().proc_code == ASN1_NGAP_ID_HO_PREP) {
    asn1::cbit_ref bref(msg.pdu.init_msg().value.ho_required()->source_to_target_transparent_container);
    asn1::ngap::source_ngran_node_to_target_ngran_node_transparent_container_s asn1_transparent_container;
    if (asn1_transparent_container.unpack(bref) != asn1::SRSASN_SUCCESS) {
      srslog::fetch_basic_logger("NGAP").error("Couldn't unpack Source to Target Transparent Container.");
      return byte_buffer{};
    }

    return std::move(asn1_transparent_container.rrc_container);
  }

  return byte_buffer{};
}
