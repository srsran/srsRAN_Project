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

#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/ran/lcid.h"

namespace srsran {

/// \brief Generate dummy F1AP SETUP RESPONSE message (CU -> DU).
f1ap_message generate_f1_setup_response_message(unsigned transaction_id);

/// \brief Generate dummy F1AP SETUP Failure message (CU -> DU).
/// \param[in] transaction_id Transaction ID of the message.
/// \param[in] time_to_wait Time that DU needs to wait. Nulltype if time_to_wait not present in the message.
f1ap_message
generate_f1_setup_failure_message(unsigned                   transaction_id,
                                  asn1::f1ap::time_to_wait_e time_to_wait = asn1::f1ap::time_to_wait_opts::nulltype);

/// \brief Generate dummy F1AP DL RRC Message Transfer message (CU -> DU).
f1ap_message generate_f1ap_dl_rrc_message_transfer(srb_id_t srb_id, const byte_buffer& rrc_container);

} // namespace srsran