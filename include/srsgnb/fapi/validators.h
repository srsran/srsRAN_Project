#ifndef SRSGNB_FAPI_VALIDATORS_H
#define SRSGNB_FAPI_VALIDATORS_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/fapi/messages.h"
#include "srsgnb/fapi/validator_report.h"

namespace srsgnb {
namespace fapi {

/// Validate the given DL TTI request message and returns a report about the result of the validation. The validation
/// checks every property of the message, as per SCF-222 v4.0 Section 3.4.2.
error_type<validator_report> validate_dl_tti_request(const dl_tti_request_message& msg);

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_VALIDATORS_H