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

#include "srsran/fapi/messages.h"

namespace srsran {

struct prs_generator_configuration;

namespace fapi_adaptor {

class precoding_matrix_repository;

/// \brief Helper function that converts from a PRS FAPI PDU to a PRS generator configuration.
///
/// \param[out] generator_config PRS generator configuration.
/// \param[in] fapi_pdu FAPI PDU received that contains all the PRS parameters.
/// \param[in] sfn SFN when the FAPI PDU was received.
/// \param[in] slot Slot when the FAPI PDU was received.
/// \param[in] pm_repo Precoding weight matrix repository.
void convert_prs_fapi_to_phy(prs_generator_configuration&       generator_config,
                             const fapi::dl_prs_pdu&            fapi_pdu,
                             uint16_t                           sfn,
                             uint16_t                           slot,
                             const precoding_matrix_repository& pm_repo);

} // namespace fapi_adaptor
} // namespace srsran
