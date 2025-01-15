/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

class uci_part2_correspondence_repository;

/// Helper function that converts from a PUSCH FAPI PDU to a PUSCH uplink slot PDU.
void convert_pusch_fapi_to_phy(uplink_processor::pusch_pdu&         pdu,
                               const fapi::ul_pusch_pdu&            fapi_pdu,
                               uint16_t                             sfn,
                               uint16_t                             slot,
                               uint16_t                             num_rx_ant,
                               uci_part2_correspondence_repository& part2_repo);

} // namespace fapi_adaptor
} // namespace srsran
