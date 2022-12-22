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

#include "srsgnb/cu_cp/cu_cp_types.h"
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Request from CU to F1AP-CU to modify existing UE configuration.
struct f1ap_ue_context_modification_request {
  ue_index_t                           ue_index;
  asn1::f1ap::ue_context_mod_request_s msg;
};

/// \brief Response from F1AP-CU to CU once UE CONTEXT MODIFICATION is complete.
struct f1ap_ue_context_modification_response {
  asn1::f1ap::ue_context_mod_resp_s response;
  asn1::f1ap::ue_context_mod_fail_s failure;
  bool                              success;
};

} // namespace srs_cu_cp
} // namespace srsgnb
