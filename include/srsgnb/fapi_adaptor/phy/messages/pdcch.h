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
#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"

namespace srsgnb {
namespace fapi_adaptor {

/// Helper function that converts from a PDCCH FAPI PDU to a PDCCH processor PDU.
void convert_pdcch_fapi_to_phy(pdcch_processor::pdu_t&   proc_pdu,
                               const fapi::dl_pdcch_pdu& fapi_pdu,
                               uint16_t                  sfn,
                               uint16_t                  slot,
                               uint16_t                  i_dci);

} // namespace fapi_adaptor
} // namespace srsgnb
