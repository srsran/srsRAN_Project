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

struct e2_subscribe_delete_request_message {
  asn1::e2ap::ri_crequest_id_s                 request_id;
  asn1::e2ap::ricsubscription_delete_request_s subscription;
};

struct e2_subscribe_delete_response_message {
  asn1::e2ap::ri_crequest_id_s              request_id;
  asn1::e2ap::ricsubscription_delete_resp_s response;
  asn1::e2ap::ricsubscription_delete_fail_s failure;
  bool                                      success;
};

struct e2_indication_message {
  asn1::e2ap::ri_crequest_id_s request_id;
  asn1::e2ap::ri_cind_s        indication;
};

struct e2_ric_control_request {
  asn1::e2ap::ri_cctrl_request_s request;
};

struct e2_ric_control_response {
  asn1::e2ap::ri_cctrl_ack_s  ack;
  asn1::e2ap::ri_cctrl_fail_s failure;
  bool                        success;
};

} // namespace srsran