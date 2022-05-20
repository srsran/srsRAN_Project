/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_MESSAGE_VALIDATORS_H
#define SRSGNB_FAPI_MESSAGE_VALIDATORS_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/fapi/validator_report.h"

namespace srsgnb {
namespace fapi {

/// Validates the given CRC.indication message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.8.
error_type<validator_report> validate_crc_indication(const crc_indication_message& msg);

/// Validates the given DL_TTI.request message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.2.
error_type<validator_report> validate_dl_tti_request(const dl_tti_request_message& msg);

/// Validates the given UL_DCI.request message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.4.
error_type<validator_report> validate_ul_dci_request(const ul_dci_request_message& msg);

/// Validates the given Tx_Data.request message and returns a report for the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.6.
error_type<validator_report> validate_tx_data_request(const tx_data_request_message& msg);

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_MESSAGE_VALIDATORS_H
