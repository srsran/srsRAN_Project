/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_ADAPTOR_PHY_SSB_H
#define SRSGNB_FAPI_ADAPTOR_PHY_SSB_H

#include "srsgnb/fapi/messages.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"

namespace srsgnb {
namespace fapi_adaptor {

/// Helper function that converts from a SSB fapi pdu to a SSB processor pdu.
void convert_ssb_fapi_to_phy(ssb_processor::pdu_t&   proc_pdu,
                             const fapi::dl_ssb_pdu& fapi_pdu,
                             uint16_t                sfn,
                             uint16_t                slot,
                             subcarrier_spacing      numerology);

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_FAPI_ADAPTOR_PHY_SSB_H
