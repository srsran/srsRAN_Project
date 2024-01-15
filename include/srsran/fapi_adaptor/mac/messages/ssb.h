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

#include "srsran/fapi/message_builders.h"
#include "srsran/mac/mac_cell_result.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief Helper function that converts from a SSB MAC PDU to a SSB FAPI PDU.
///
/// \param[out] fapi_pdu SSB FAPI PDU that will store the converted data.
/// \param[in] mac_pdu  SSB MAC PDU to convert to SSB FAPI PDU.
void convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu& fapi_pdu, const srsran::dl_ssb_pdu& mac_pdu);

/// \brief Helper function that converts from a SSB MAC PDU to a SSB FAPI PDU.
///
/// \param[out] builder SSB FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu  SSB MAC PDU to convert to SSB FAPI PDU.
void convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu_builder& builder, const srsran::dl_ssb_pdu& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsran
