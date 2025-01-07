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
#include "srsran/phy/upper/channel_processors/pdcch/pdcch_processor.h"

namespace srsran {
namespace fapi_adaptor {

class precoding_matrix_repository;

/// Helper function that converts from a PDCCH FAPI PDU to a PDCCH processor PDU.
///
/// \param[out] proc_pdu PDCCH processor PDU.
/// \param[in] fapi_pdu FAPI PDU received that contains all the CSI-RS parameters.
/// \param[in] sfn SFN when the FAPI PDU was received.
/// \param[in] slot Slot when the FAPI PDU was received.
/// \param[in] i_dci DCI index.
/// \param[in] pm_repo Precoding matrix repository.
void convert_pdcch_fapi_to_phy(pdcch_processor::pdu_t&            proc_pdu,
                               const fapi::dl_pdcch_pdu&          fapi_pdu,
                               uint16_t                           sfn,
                               uint16_t                           slot,
                               uint16_t                           i_dci,
                               const precoding_matrix_repository& pm_repo);

} // namespace fapi_adaptor
} // namespace srsran
