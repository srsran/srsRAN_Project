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
