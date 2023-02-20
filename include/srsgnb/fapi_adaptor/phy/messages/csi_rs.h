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
#include "srsgnb/phy/upper/signal_processors/nzp_csi_rs_generator.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief Helper function that converts from a CSI-RS FAPI PDU to a NZP-CSI-RS processor PDU.
///
/// \param[out] proc_pdu NZP-CSI-RS processor PDU.
/// \param[in] fapi_pdu FAPI PDU received that contains all the CSI-RS parameters.
/// \param[in] sfn SFN when the FAPI PDU was received.
/// \param[in] slot Slot when the FAPI PDU was received.
void convert_csi_rs_fapi_to_phy(nzp_csi_rs_generator::config_t& proc_pdu,
                                const fapi::dl_csi_rs_pdu&      fapi_pdu,
                                uint16_t                        sfn,
                                uint16_t                        slot);

} // namespace fapi_adaptor
} // namespace srsgnb
