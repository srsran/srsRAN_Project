/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

namespace srsran {

struct prach_buffer_context;

namespace fapi_adaptor {

/// \brief Helper function that converts from a UL PRACH FAPI PDU to a PRACH buffer context.
///
/// \param[out] context Context for the PRACH buffer.
/// \param[in] fapi_pdu FAPI PRACH PDU.
/// \param[in] prach_cfg PRACH configuration as per SCF-222 v4.0 Section 3.3.2.4 TLV 0x1031.
/// \param[in] carrier_cfg Carrier configuration as per SCF-222 v4.0 Section 3.3.2.4 TLV 0x102d.
/// \param[in] ports Port selection.
/// \param[in] sfn System frame number.
/// \param[in] slot Slot number.
/// \param[in] sector_id Base station sector identifier.
void convert_prach_fapi_to_phy(prach_buffer_context&       context,
                               const fapi::ul_prach_pdu&   fapi_pdu,
                               const fapi::prach_config&   prach_cfg,
                               const fapi::carrier_config& carrier_cfg,
                               span<const uint8_t>         ports,
                               unsigned                    sfn,
                               unsigned                    slot,
                               unsigned                    sector_id);

} // namespace fapi_adaptor
} // namespace srsran
