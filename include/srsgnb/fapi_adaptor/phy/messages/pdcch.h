#ifndef SRSGNB_FAPI_ADAPTOR_MESSAGES_PDCCH_H
#define SRSGNB_FAPI_ADAPTOR_MESSAGES_PDCCH_H

#include "srsgnb/fapi/messages.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"

namespace srsgnb {
namespace fapi_adaptor {

/// Helper function that converts from a PDCCH FAPI PDU to a PDCCH processor PDU.
void convert_pdcch_fapi_to_phy(pdcch_processor::pdu_t&   proc_pdu,
                               const fapi::dl_pdcch_pdu& fapi_pdu,
                               uint16_t                  sfn,
                               uint16_t                  slot);

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_FAPI_ADAPTOR_MESSAGES_PDCCH_H
