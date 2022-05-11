#ifndef SRSGNB_SRSGNB_FAPI_ADAPTOR_PHY_PDSCH_H
#define SRSGNB_SRSGNB_FAPI_ADAPTOR_PHY_PDSCH_H

#include "srsgnb/fapi/messages.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"

namespace srsgnb {
namespace fapi_adaptor {

/// Helper function that converts from a PDSCH fapi PDU to a PDSCH processor PDU.
void convert_pdsch_fapi_to_phy(pdsch_processor::pdu_t&   proc_pdu,
                               const fapi::dl_pdsch_pdu& fapi_pdu,
                               uint16_t                  sfn,
                               uint16_t                  slot);

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_SRSGNB_FAPI_ADAPTOR_PHY_PDSCH_H
