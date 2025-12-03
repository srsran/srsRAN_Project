/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/fapi/messages/dl_tti_request.h"

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
