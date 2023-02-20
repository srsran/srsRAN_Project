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

#include "srsgnb/cu_cp/cu_cp_types.h"
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Request from CU to F1AP-CU to start an F1AP "UE Context Setup" procedure, as per TS38.473 8.3.1.
struct f1ap_ue_context_setup_request {
  // TODO: replace asn1 by necessary containers
  ue_index_t                             ue_index = ue_index_t::invalid;
  asn1::f1ap::ue_context_setup_request_s msg;
};

/// \brief Response from F1AP-CU to CU once "UE Context Setup" procedure is complete.
struct f1ap_ue_context_setup_response {
  asn1::f1ap::ue_context_setup_resp_s response;
  asn1::f1ap::ue_context_setup_fail_s failure;
  bool                                success = false;
};

/// \brief Request from CU to F1AP-CU to start an F1AP "UE Context Modification" procedure, as per TS38.473 8.3.4.
struct f1ap_ue_context_modification_request {
  ue_index_t                           ue_index = ue_index_t::invalid;
  asn1::f1ap::ue_context_mod_request_s msg;
};

/// \brief Response from F1AP-CU to CU once "UE Context Modification" procedure is complete.
struct f1ap_ue_context_modification_response {
  asn1::f1ap::ue_context_mod_resp_s response;
  asn1::f1ap::ue_context_mod_fail_s failure;
  bool                              success = false;
};

} // namespace srs_cu_cp
} // namespace srsgnb
