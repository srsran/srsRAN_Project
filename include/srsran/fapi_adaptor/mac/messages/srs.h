/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
