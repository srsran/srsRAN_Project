/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class ue_repository;
class cell_configuration;
class ue_cell_configuration;

/// \brief Fetches a list of PUSCH Time Domain resource indexes based on cell, UE configuration and nof. symbols in
/// PUSCH slot.
/// \param[in] ue_cfg User's dedicated configuration.
/// \param[in] pdcch_slot Slot at which the PDCCH is supposed to be scheduled.
/// \return List of PUSCH Time Domain resource indexes.
static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
get_pusch_td_resource_indices(const ue_cell_configuration& ue_cfg, slot_point pdcch_slot);

/// \brief Fetches a list of PUSCH Time Domain resource indexes based on cell, UE configuration and nof. symbols in
/// PUSCH slot.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] pdcch_slot Slot at which the PDCCH is supposed to be scheduled.
/// \return List of PUSCH Time Domain resource indexes.
static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
get_pusch_td_res_indices_common(const cell_configuration& cell_cfg, slot_point pdcch_slot);

} // namespace srsran
