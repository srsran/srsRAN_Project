/*
 *
 * \section COPYRIGHT
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

namespace srsgnb {
namespace fapi {

struct dl_pdcch_pdu;
struct validator_report;

/// Validate the given DL PDCCH PDU and returns true on success, otherwise false.
bool validate_dl_pdcch_pdu(const dl_pdcch_pdu& pdu, validator_report& report);

} // namespace fapi

} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_DL_PDCCH_PDU_H
