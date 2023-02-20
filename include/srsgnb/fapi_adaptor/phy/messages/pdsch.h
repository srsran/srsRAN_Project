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

#include "srsgnb/fapi/messages.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"

namespace srsran {
namespace fapi_adaptor {

/// Helper function that converts from a PDSCH FAPI PDU to a PDSCH processor PDU.
void convert_pdsch_fapi_to_phy(pdsch_processor::pdu_t&   proc_pdu,
                               const fapi::dl_pdsch_pdu& fapi_pdu,
                               uint16_t                  sfn,
                               uint16_t                  slot);

} // namespace fapi_adaptor
} // namespace srsran
