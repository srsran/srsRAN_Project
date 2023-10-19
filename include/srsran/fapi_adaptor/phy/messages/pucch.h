/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/fapi/messages.h"
#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {
namespace fapi_adaptor {

/// Helper function that converts from a PUCCH FAPI PDU to a PUCCH uplink slot PDU using the system frame number, slot
/// and number of reception antennas.
void convert_pucch_fapi_to_phy(uplink_processor::pucch_pdu& pdu,
                               const fapi::ul_pucch_pdu&    fapi_pdu,
                               uint16_t                     sfn,
                               uint16_t                     slot,
                               uint16_t                     num_rx_ant);

} // namespace fapi_adaptor
} // namespace srsran
