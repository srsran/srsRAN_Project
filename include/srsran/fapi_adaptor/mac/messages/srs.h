/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

namespace srsran {

struct srs_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a SRS MAC PDU to a SRS FAPI PDU.
///
/// \param[out] fapi_pdu     SRS FAPI PDU that will store the converted data.
/// \param[in]  mac_pdu      MAC PDU that contains the SRS parameters.
void convert_srs_mac_to_fapi(fapi::ul_srs_pdu& fapi_pdu, const srs_info& mac_pdu);

/// \brief Helper function that converts from a SRS MAC PDU to a SRS FAPI PDU.
///
/// \param[out] fapi_pdu     SRS FAPI builder that helps to fill the PDU.
/// \param[in]  mac_pdu      MAC PDU that contains the SRS parameters.
void convert_srs_mac_to_fapi(fapi::ul_srs_pdu_builder& builder, const srs_info& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsran
