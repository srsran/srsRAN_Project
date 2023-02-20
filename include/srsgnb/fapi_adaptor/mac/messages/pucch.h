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

#include "srsgnb/fapi/message_builders.h"

namespace srsgnb {

struct pucch_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a PUCCH MAC PDU to a PUCCH FAPI PDU.
///
/// \param[out] fapi_pdu PUCCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu MAC PDU that contains the PUCCH parameters.
void convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu& fapi_pdu, const pucch_info& mac_pdu);

/// \brief Helper function that converts from a PUCCH MAC PDU to a PUCCH FAPI PDU.
///
/// \param[out] builder PUCCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu MAC PDU that contains the PUCCH parameters.
void convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsgnb
