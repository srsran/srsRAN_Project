/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
  asn1::e2ap::e2ap_pdu_c pdu;
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
  asn1::e2ap::ric_request_id_s  request_id;
  asn1::e2ap::ric_sub_details_s subscription;
};

struct e2_subscribe_reponse_message {
  asn1::e2ap::ric_request_id_s               request_id;
  uint16_t                                   ran_function_id;
  asn1::e2ap::cause_c                        cause;
  asn1::e2ap::ric_action_admitted_list_l     admitted_list;
  asn1::e2ap::ric_action_not_admitted_list_l not_admitted_list;
  bool                                       success;
};

struct e2_subscribe_delete_request_message {
  asn1::e2ap::ric_request_id_s         request_id;
  asn1::e2ap::ric_sub_delete_request_s subscription;
};

struct e2_subscribe_delete_response_message {
  asn1::e2ap::ric_request_id_s      request_id;
  asn1::e2ap::ric_sub_delete_resp_s response;
  asn1::e2ap::ric_sub_delete_fail_s failure;
  bool                              success;
};

struct e2_indication_message {
  asn1::e2ap::ric_request_id_s request_id;
  asn1::e2ap::ric_ind_s        indication;
};

struct e2_ric_control_request {
  asn1::e2ap::ric_ctrl_request_s request;
};

struct e2_ric_control_response {
  asn1::e2ap::ric_ctrl_ack_s  ack;
  asn1::e2ap::ric_ctrl_fail_s failure;
  bool                        success;
};

} // namespace srsran
