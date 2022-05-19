/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_VALIDATORS_DL_PDCCH_PDU_H
#define SRSGNB_LIB_FAPI_VALIDATORS_DL_PDCCH_PDU_H

#include "srsgnb/fapi/messages.h"

namespace srsgnb {
namespace fapi {

struct validator_report;

/// Validate the given DL PDCCH PDU and returns true on success, otherwise false.
bool validate_dl_pdcch_pdu(message_type_id msg_type, const dl_pdcch_pdu& pdu, validator_report& report);

} // namespace fapi

} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_DL_PDCCH_PDU_H
