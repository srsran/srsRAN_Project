/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/e2ap/e2ap.h"

namespace srsran {

struct e2_message {
  asn1::e2ap::e2_ap_pdu_c pdu;
};

/// \brief CU initiated E2 setup request.
struct e2_setup_request_message {
  asn1::e2ap::e2setup_request_s request;
  unsigned                      max_setup_retries = 5;
};

/// \brief Response to CU initiated E2 setup request.
struct e2_setup_response_message {
  asn1::e2ap::e2setup_resp_s response;
  asn1::e2ap::e2setup_fail_s failure;
  bool                       success;
};

struct e2_subscribe_request_message {
  asn1::e2ap::ri_crequest_id_s          request_id;
  asn1::e2ap::ricsubscription_details_s subscription;
};

struct e2_subscribe_reponse_message {
  asn1::e2ap::ri_crequest_id_s               request_id;
  uint16_t                                   ran_function_id;
  asn1::e2ap::cause_c                        cause;
  asn1::e2ap::ri_caction_admitted_list_l     admitted_list;
  asn1::e2ap::ri_caction_not_admitted_list_l not_admitted_list;
  bool                                       success;
};

struct e2_indication_message {
  asn1::e2ap::ri_crequest_id_s request_id;
  asn1::e2ap::ri_cind_s        indication;
};

} // namespace srsran