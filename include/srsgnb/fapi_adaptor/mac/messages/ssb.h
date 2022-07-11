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
#include "srsgnb/mac/mac_cell_result.h"

namespace srsgnb {
namespace fapi_adaptor {

/// \brief Helper function that converts from a SSB MAC PDU to a SSB FAPI PDU.
///
/// \param[out] fapi_pdu SSB FAPI PDU that will store the converted data.
/// \param[in] mac_pdu  SSB MAC PDU to convert to SSB FAPI PDU.
void convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu& fapi_pdu, const srsgnb::dl_ssb_pdu& mac_pdu);

/// \brief Helper function that converts from a SSB MAC PDU to a SSB FAPI PDU.
///
/// \param[out] builder SSB FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu  SSB MAC PDU to convert to SSB FAPI PDU.
void convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu_builder& builder, const srsgnb::dl_ssb_pdu& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsgnb
