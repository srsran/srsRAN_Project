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

#include "srsran/fapi/messages.h"
#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {
namespace fapi_adaptor {

/// Helper function that converts from a PUCCH FAPI PDU to a PUCCH uplink slot PDU.
void convert_pucch_fapi_to_phy(uplink_processor::pucch_pdu& pdu,
                               const fapi::ul_pucch_pdu&    fapi_pdu,
                               uint16_t                     sfn,
                               uint16_t                     slot);

} // namespace fapi_adaptor
} // namespace srsran
