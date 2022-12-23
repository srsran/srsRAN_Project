/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {
namespace srs_cu_cp {

struct e1ap_bearer_context_setup_request {
  asn1::e1ap::sys_bearer_context_setup_request_c sys_bearer_context_setup_request;
  uint64_t                                       uedl_aggregate_maximum_bit_rate;
};

struct e1ap_bearer_context_setup_response {
  asn1::e1ap::bearer_context_setup_resp_s response;
  asn1::e1ap::bearer_context_setup_fail_s failure;
  bool                                    success;
};

struct e1ap_bearer_context_release_command {
  asn1::e1ap::bearer_context_release_cmd_s msg;
};

struct e1ap_bearer_context_release_complete {
  asn1::e1ap::bearer_context_release_complete_s msg;
};

struct e1ap_bearer_context_modification_request {
  asn1::e1ap::bearer_context_mod_request_s msg;
};

struct e1ap_bearer_context_modification_response {
  asn1::e1ap::bearer_context_mod_resp_s response;
  asn1::e1ap::bearer_context_mod_fail_s failure;
  bool                                  success;
};

} // namespace srs_cu_cp
} // namespace srsgnb
