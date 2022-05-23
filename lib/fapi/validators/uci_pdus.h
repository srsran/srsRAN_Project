/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_VALIDATORS_UCI_PDUS_H
#define SRSGNB_LIB_FAPI_VALIDATORS_UCI_PDUS_H

namespace srsgnb {
namespace fapi {

struct uci_pucch_pdu_format_0_1;
struct uci_pucch_pdu_format_2_3_4;
struct uci_pusch_pdu;
struct validator_report;

/// Validate the given UCI PUSCH PDU and returns true on success, otherwise false.
bool validate_uci_pusch_pdu(const uci_pusch_pdu& pdu, validator_report& report);

/// Validate the given UCI PUCCH PDU format 0/1 and returns true on success, otherwise false.
bool validate_uci_pucch_format01_pdu(const uci_pucch_pdu_format_0_1& pdu, validator_report& report);

/// Validate the given UCI PUCCH PDU format 2/3/4 and returns true on success, otherwise false.
bool validate_uci_pucch_format234_pdu(const uci_pucch_pdu_format_2_3_4& pdu, validator_report& report);

} // namespace fapi

} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_UCI_PDUS_H
