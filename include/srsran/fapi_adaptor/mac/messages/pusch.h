/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/message_builders.h"

namespace srsran {

struct ul_sched_info;

namespace fapi_adaptor {

class uci_part2_correspondence_mapper;

/// \brief Helper function that converts from a PUSCH MAC PDU to a PUSCH FAPI PDU.
///
/// \param[out] fapi_pdu     PUSCH FAPI PDU that will store the converted data.
/// \param[in]  mac_pdu      MAC PDU that contains the PUSCH parameters.
/// \param[in]  part2_mapper UCI Part2 mapper.
void convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu&              fapi_pdu,
                               const ul_sched_info&             mac_pdu,
                               uci_part2_correspondence_mapper& part2_mapper);

/// \brief Helper function that converts from a PUSCH MAC PDU to a PUSCH FAPI PDU.
///
/// \param[out] builder      PUSCH FAPI builder that helps to fill the PDU.
/// \param[in]  mac_pdu      MAC PDU that contains the PUSCH parameters.
/// \param[in]  part2_mapper UCI Part2 mapper.
void convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu_builder&      builder,
                               const ul_sched_info&             mac_pdu,
                               uci_part2_correspondence_mapper& part2_mapper);

} // namespace fapi_adaptor
} // namespace srsran
