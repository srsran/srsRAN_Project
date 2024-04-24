/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

/// Helper function that converts from a SRS FAPI PDU to a SRS uplink slot PDU using the system frame number and slot.
void convert_srs_fapi_to_phy(uplink_processor::srs_pdu& pdu,
                             const fapi::ul_srs_pdu&    fapi_pdu,
                             uint16_t                   sfn,
                             uint16_t                   slot);

} // namespace fapi_adaptor
} // namespace srsran
