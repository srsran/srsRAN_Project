/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/messages.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"

namespace srsgnb {
namespace fapi_adaptor {

/// Helper function that converts from a PUSCH FAPI PDU to a PUSCH processor PDU.
void convert_pusch_fapi_to_phy(pusch_processor::pdu_t&   proc_pdu,
                               const fapi::ul_pusch_pdu& fapi_pdu,
                               uint16_t                  sfn,
                               uint16_t                  slot);

} // namespace fapi_adaptor
} // namespace srsgnb
