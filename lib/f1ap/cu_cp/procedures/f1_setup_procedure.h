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

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/cu_cp/du_setup_notifier.h"

namespace srsran {
namespace srs_cu_cp {

struct f1ap_du_context;

/// \brief Convert the F1 Setup Request from ASN.1 to a request to be sent to the CU-CP.
/// \param[in] asn1_request The ASN.1 type F1 Setup Request.
/// \return Request to setup DU.
du_setup_request create_du_setup_request(const asn1::f1ap::f1_setup_request_s& asn1_request);

/// \brief Handle the F1 Setup Request from the DU as per TS 38.473 section 8.2.
///
/// This function will ensure the input message is valid, request the CU-CP to setup the DU and send the F1 Setup
/// Response/Failure back to the DU via the F1-C interface.
/// \param[in] request The F1 Setup Request.
/// \param[out] du_ctxt The DU context to store the DU information.
/// \param[in] pdu_notifier The notifier to send F1AP messages to the DU.
/// \param[in] du_setup_notif The notifier to send the DU setup request to the CU-CP.
/// \param[in] logger The logger.
void handle_f1_setup_procedure(const asn1::f1ap::f1_setup_request_s& request,
                               f1ap_du_context&                      du_ctxt,
                               f1ap_message_notifier&                pdu_notifier,
                               du_setup_notifier&                    du_setup_notif,
                               srslog::basic_logger&                 logger);

} // namespace srs_cu_cp
} // namespace srsran