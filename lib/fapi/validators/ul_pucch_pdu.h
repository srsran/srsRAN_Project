/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {
namespace fapi {

struct ul_pucch_pdu;
struct validator_report;

/// Validate the given UL PUCCH PDU and returns true on success, otherwise false.
bool validate_ul_pucch_pdu(const ul_pucch_pdu& pdu, validator_report& report);

} // namespace fapi
} // namespace srsgnb
