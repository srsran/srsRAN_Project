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

struct ul_sched_info;

namespace fapi_adaptor {

/// \brief Helper function that converts from a PUSCH MAC PDU to a PUSCH FAPI PDU.
///
/// \param[out] fapi_pdu PUSCH FAPI PDU that will store the converted data.
/// \param[in] mac_pdu MAC PDU that contains the PUSCH parameters.
void convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu& fapi_pdu, const ul_sched_info& mac_pdu);

/// \brief Helper function that converts from a PUSCH MAC PDU to a PUSCH FAPI PDU.
///
/// \param[out] builder PUSCH FAPI builder that helps to fill the PDU.
/// \param[in] mac_pdu MAC PDU that contains the PUSCH parameters.
void convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu_builder& builder, const ul_sched_info& mac_pdu);

} // namespace fapi_adaptor
} // namespace srsgnb
