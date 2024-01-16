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
#include "srsran/phy/upper/channel_processors/ssb_processor.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief Helper function that converts from a SSB FAPI PDU to a SSB processor PDU.
///
/// \param[out] proc_pdu SSB processor PDU.
/// \param[in] fapi_pdu FAPI PDU received that contains all the SSB parameters.
/// \param[in] sfn SFN when the FAPI PDU was received.
/// \param[in] slot Slot when the FAPI PDU was received.
/// \param[] scs_common Parameter subCarrierSpacingCommon as per TS38.331, Section 6.2.2 \c MIB message.
void convert_ssb_fapi_to_phy(ssb_processor::pdu_t&   proc_pdu,
                             const fapi::dl_ssb_pdu& fapi_pdu,
                             uint16_t                sfn,
                             uint16_t                slot,
                             subcarrier_spacing      scs_common);

} // namespace fapi_adaptor
} // namespace srsran
