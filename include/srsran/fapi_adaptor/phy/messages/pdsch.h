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

#include "srsran/fapi/messages.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"

namespace srsran {
namespace fapi_adaptor {

class precoding_matrix_repository;

/// Helper function that converts from a PDSCH FAPI PDU to a PDSCH processor PDU.
void convert_pdsch_fapi_to_phy(pdsch_processor::pdu_t&            proc_pdu,
                               const fapi::dl_pdsch_pdu&          fapi_pdu,
                               uint16_t                           sfn,
                               uint16_t                           slot,
                               span<const re_pattern_list>        csi_re_pattern_list,
                               const precoding_matrix_repository& pm_repo);

} // namespace fapi_adaptor
} // namespace srsran
