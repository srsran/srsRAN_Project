/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1ap/common/f1ap_common.h"
#include "srsgnb/ran/lcid.h"

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