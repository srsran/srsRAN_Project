/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_LIB_FAPI_VALIDATORS_UL_PRACH_PDU_H
#define SRSGNB_LIB_FAPI_VALIDATORS_UL_PRACH_PDU_H

namespace srsgnb {
namespace fapi {

struct ul_prach_pdu;
struct validator_report;

/// Validate the given UL PRACH PDU and returns true on success, otherwise false.
bool validate_ul_prach_pdu(const ul_prach_pdu& pdu, validator_report& report);

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_UL_PRACH_PDU_H
