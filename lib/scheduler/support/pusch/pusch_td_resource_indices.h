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

#include "../../config/ue_configuration.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class ue_repository;
class cell_configuration;
class ue_cell_configuration;

/// \brief Fetches a list of PUSCH Time Domain resource indexes based on cell, UE configuration and nof. symbols in
/// PUSCH slot.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] ss_info SearchSpace information.
/// \param[in] pdcch_slot Slot at which the PDCCH is supposed to be scheduled.
/// \return List of PUSCH Time Domain resource indexes.
static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
get_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                              const search_space_info*  ss_info,
                              slot_point                pdcch_slot);

/// \brief Fetches a list of PUSCH Time Domain resource indexes based on cell, UE configuration and nof. symbols in
/// PUSCH slot.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] pdcch_slot Slot at which the PDCCH is supposed to be scheduled.
/// \return List of PUSCH Time Domain resource indexes.
static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
get_pusch_td_res_indices_common(const cell_configuration& cell_cfg, slot_point pdcch_slot);

} // namespace srsran
