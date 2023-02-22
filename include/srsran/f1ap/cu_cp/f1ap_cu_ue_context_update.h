/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_cp_types.h"
#include <vector>

namespace srsran {
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
} // namespace srsran
