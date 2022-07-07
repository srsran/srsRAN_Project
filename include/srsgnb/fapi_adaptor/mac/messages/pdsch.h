/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_ADAPTOR_MAC_MESSAGES_PDSCH_H
#define SRSGNB_FAPI_ADAPTOR_MAC_MESSAGES_PDSCH_H

#include "srsgnb/fapi/message_builders.h"

namespace srsgnb {

struct sib_information;

namespace fapi_adaptor {

/// \brief Helper function that converts from a PDSCH MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] fapi_pdu PDSCH FAPI PDU that will store the converted data.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const sib_information& mac_pdu);

/// \brief Helper function that converts from a PDSCH MAC PDU to a PDSCH FAPI PDU.
///
/// \param[out] builder PDSCH FAPI builder that helps to fill the PDU.
void convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder, const sib_information& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_FAPI_ADAPTOR_MAC_MESSAGES_PDSCH_H
