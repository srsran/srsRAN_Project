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

#include "srsran/adt/expected.h"
#include "srsran/fapi/validator_report.h"

namespace srsran {
namespace fapi {

/// Validates the given CRC.indication message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.8.
error_type<validator_report> validate_crc_indication(const crc_indication_message& msg);

/// Validates the given DL_TTI.request message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.2.
error_type<validator_report> validate_dl_tti_request(const dl_tti_request_message& msg);

/// Validates the given ERROR.indication message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.3.6.1.
error_type<validator_report> validate_error_indication(const error_indication_message& msg);

/// Validates the given RACH.indication message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.11.
error_type<validator_report> validate_rach_indication(const rach_indication_message& msg);

/// Validates the given Rx_Data.indication message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.7.
error_type<validator_report> validate_rx_data_indication(const rx_data_indication_message& msg);

/// Validates the given SLOT.indication message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.1.
error_type<validator_report> validate_slot_indication(const slot_indication_message& msg);

/// Validates the given Tx_Data.request message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.6.
error_type<validator_report> validate_tx_data_request(const tx_data_request_message& msg);

/// Validates the given UCI.indication message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.9.
error_type<validator_report> validate_uci_indication(const uci_indication_message& msg);

/// Validates the given UL_DCI.request message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.4.
error_type<validator_report> validate_ul_dci_request(const ul_dci_request_message& msg);

/// Validates the given UL_TTI.request message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.3.
error_type<validator_report> validate_ul_tti_request(const ul_tti_request_message& msg);

/// Logs the given validator report.
void log_validator_report(const validator_report& report, srslog::basic_logger& logger);

} // namespace fapi
} // namespace srsran
