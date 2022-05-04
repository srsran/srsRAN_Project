#ifndef SRSGNB_LIB_FAPI_VALIDATORS_DL_SSB_PDU_H
#define SRSGNB_LIB_FAPI_VALIDATORS_DL_SSB_PDU_H

namespace srsgnb {
namespace fapi {

struct dl_ssb_pdu;
struct validator_report;

/// Validate the given DL SSB PDU and returns true on success, otherwise false.
bool validate_dl_ssb_pdu(const dl_ssb_pdu& pdu, validator_report& report);

} // namespace fapi

} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_DL_SSB_PDU_H
