#ifndef SRSGNB_LIB_FAPI_VALIDATORS_DL_SSB_PDU_H
#define SRSGNB_LIB_FAPI_VALIDATORS_DL_SSB_PDU_H

#include "srsgnb/fapi/messages.h"

namespace srsgnb {
namespace fapi {

struct validator_report;

/// Validate the given DL SSB pdu and returns true on success, otherwise false.
bool validate_dl_ssb_pdu(const dl_ssb_pdu& pdu, validator_report& report);

} // namespace fapi

} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_DL_SSB_PDU_H