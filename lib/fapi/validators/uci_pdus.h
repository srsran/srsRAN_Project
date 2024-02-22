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

namespace srsran {
namespace fapi {

struct uci_pucch_pdu_format_0_1;
struct uci_pucch_pdu_format_2_3_4;
struct uci_pusch_pdu;
struct validator_report;

/// Validates the given UCI PUSCH PDU and returns true on success, otherwise false.
bool validate_uci_pusch_pdu(const uci_pusch_pdu& pdu, validator_report& report);

/// Validates the given UCI PUCCH PDU format 0/1 and returns true on success, otherwise false.
bool validate_uci_pucch_format01_pdu(const uci_pucch_pdu_format_0_1& pdu, validator_report& report);

/// Validates the given UCI PUCCH PDU format 2/3/4 and returns true on success, otherwise false.
bool validate_uci_pucch_format234_pdu(const uci_pucch_pdu_format_2_3_4& pdu, validator_report& report);

} // namespace fapi
} // namespace srsran
