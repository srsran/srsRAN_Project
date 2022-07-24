/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/message_builders.h"

namespace srsgnb {

struct prach_occasion_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a PRACH MAC PDU to a PRACH FAPI PDU.
///
/// \param[out] fapi_pdu PRACH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu MAC PDU that contains the PRACH parameters.
void convert_prach_mac_to_fapi(fapi::ul_prach_pdu& fapi_pdu, const prach_occasion_info& mac_pdu);

/// \brief Helper function that converts from a PRACH MAC PDU to a PRACH FAPI PDU.
///
/// \param[out] builder PRACH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu MAC PDU that contains the PRACH parameters.
void convert_prach_mac_to_fapi(fapi::ul_prach_pdu_builder& builder, const prach_occasion_info& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsgnb
