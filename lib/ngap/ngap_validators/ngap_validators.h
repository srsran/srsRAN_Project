/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

} // namespace srs_cu_cp
} // namespace srsran
