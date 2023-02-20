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
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"

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
