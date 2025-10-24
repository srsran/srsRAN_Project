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

bool srsran::test_helpers::is_valid_e1_reset(const e1ap_message& msg)
{
  TRUE_OR_RETURN(msg.pdu.type() == asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  TRUE_OR_RETURN(msg.pdu.init_msg().proc_code == ASN1_E1AP_ID_RESET);
  TRUE_OR_RETURN(is_packable(msg));

  return true;
}
