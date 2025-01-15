/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_test_message_validators.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/e1ap/common/e1ap_message.h"

using namespace srsran;

#define TRUE_OR_RETURN(cond)                                                                                           \
  if (not(cond))                                                                                                       \
    return false;

static bool is_packable(const e1ap_message& msg)
{
  byte_buffer   temp_pdu;
  asn1::bit_ref bref{temp_pdu};
  return msg.pdu.pack(bref) == asn1::SRSASN_SUCCESS;
}

bool srsran::test_helpers::is_valid_bearer_context_setup_request(const e1ap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  TRUE_OR_RETURN(is_packable(msg));

  return true;
}

bool srsran::test_helpers::is_valid_bearer_context_modification_request(const e1ap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
  TRUE_OR_RETURN(is_packable(msg));

  return true;
}

bool srsran::test_helpers::is_valid_bearer_context_modification_request_with_ue_security_info(const e1ap_message& msg)
{
  TRUE_OR_RETURN(is_valid_bearer_context_modification_request(msg));
  TRUE_OR_RETURN(msg.pdu.init_msg().value.bearer_context_mod_request()->security_info_present);

  return true;
}

bool srsran::test_helpers::is_valid_bearer_context_release_command(const e1ap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  TRUE_OR_RETURN(is_packable(msg));

  return true;
}
