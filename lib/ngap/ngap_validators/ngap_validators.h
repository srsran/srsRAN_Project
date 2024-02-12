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

#include "../ue_context/ngap_ue_logger.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

struct pdu_session_resource_setup_validation_outcome {
  cu_cp_pdu_session_resource_setup_request  request;
  cu_cp_pdu_session_resource_setup_response response;
};

/// \brief Verify a received PduSessionResourceSetupRequest message as per TS 38.413 section 8.2.1.4.
/// \param[in] request The common type PduSessionResourceSetupRequest message.
/// \param[in] asn1_request The received PduSessionResourceSetupRequest message.
/// \param[in] logger The NGAP UE logger.
/// \returns the verified PduSessionResourceSetupRequest and PduSessionResourceSetupResponse messages. The
/// verified request will only contain PDU sessions that passed the verification. The verified response will contain PDU
/// sessions that failed the verification.
pdu_session_resource_setup_validation_outcome
verify_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request&    request,
                                          const asn1::ngap::pdu_session_res_setup_request_s& asn1_request,
                                          const ngap_ue_logger&                              logger);

struct pdu_session_resource_modify_validation_outcome {
  cu_cp_pdu_session_resource_modify_request  request;
  cu_cp_pdu_session_resource_modify_response response;
};

/// \brief Verify a received PduSessionResourceModifyRequest message as per TS 38.413 section 8.2.3.4.
/// \param[in] request The common type PduSessionResourceModifyRequest message.
/// \param[in] asn1_request The received PduSessionResourceModifyRequest message.
/// \param[in] logger The NGAP UE logger.
/// \returnsthe verified PduSessionResourceModifyRequest and PduSessionResourceModifyResponse messages. The
/// verified request will only contain PDU sessions that passed the verification. The verified response will contain PDU
/// sessions that failed the verification.
pdu_session_resource_modify_validation_outcome
verify_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request&    request,
                                           const asn1::ngap::pdu_session_res_modify_request_s& asn1_request,
                                           const ngap_ue_logger&                               ue_logger);

} // namespace srs_cu_cp
} // namespace srsran
