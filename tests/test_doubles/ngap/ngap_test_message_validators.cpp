/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_test_message_validators.h"
#include "srsran/asn1/ngap/common.h"
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
