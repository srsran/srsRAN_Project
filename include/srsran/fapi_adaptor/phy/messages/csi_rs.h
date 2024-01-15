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
#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"
#include "srsran/ran/csi_rs/csi_rs_pattern.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief Helper function that converts from a CSI-RS FAPI PDU to a NZP-CSI-RS processor PDU.
///
/// \param[out] proc_pdu NZP-CSI-RS processor PDU.
/// \param[in] fapi_pdu FAPI PDU received that contains all the CSI-RS parameters.
/// \param[in] sfn SFN when the FAPI PDU was received.
/// \param[in] slot Slot when the FAPI PDU was received.
/// \param[in] cell_bandwidth_prb Cell bandwidth in PRB.
void convert_csi_rs_fapi_to_phy(nzp_csi_rs_generator::config_t& proc_pdu,
                                const fapi::dl_csi_rs_pdu&      fapi_pdu,
                                uint16_t                        sfn,
                                uint16_t                        slot,
                                uint16_t                        cell_bandwidth_prb);

/// \brief Gets the CSI-RS pattern from the given FAPI PDU.
///
/// \param[out] pattern CSI-RS pattern.
/// \param[in] fapi_pdu FAPI PDU received that contains all the CSI-RS parameters.
/// \param[in] cell_bandwidth_prb Cell bandwidth in PRB.
void get_csi_rs_pattern_from_fapi_pdu(csi_rs_pattern&            pattern,
                                      const fapi::dl_csi_rs_pdu& fapi_pdu,
                                      uint16_t                   cell_bandwidth_prb);

} // namespace fapi_adaptor
} // namespace srsran
