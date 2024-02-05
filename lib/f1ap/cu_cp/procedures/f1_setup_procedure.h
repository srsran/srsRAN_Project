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

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/f1ap/cu_cp/du_setup_notifier.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Convert the F1 Setup Request from ASN.1 to a request to be sent to the CU-CP.
/// \param[in] asn1_request The ASN.1 type F1 Setup Request.
/// \return Request to setup DU.
du_setup_request create_du_setup_request(const asn1::f1ap::f1_setup_request_s& asn1_request);

/// \brief Handle the F1 Setup Request from the DU as per TS 38.473 section 8.2.
///
/// This function will ensure the input message is valid, request the CU-CP to setup the DU and send the F1 Setup
/// Response/Failure back to the DU via the F1-C interface.
/// \param[in] request The F1 Setup Request.
/// \param[in] pdu_notifier The notifier to send F1AP messages to the DU.
/// \param[in] du_setup_notif The notifier to send the DU setup request to the CU-CP.
/// \param[in] logger The logger.
void handle_f1_setup_procedure(const asn1::f1ap::f1_setup_request_s& request,
                               f1ap_message_notifier&                pdu_notifier,
                               du_setup_notifier&                    du_setup_notif,
                               srslog::basic_logger&                 logger);

} // namespace srs_cu_cp
} // namespace srsran