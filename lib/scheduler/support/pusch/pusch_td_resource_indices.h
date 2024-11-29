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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include "srsran/ran/slot_point.h"
#include <vector>

namespace srsran {

class cell_configuration;
struct search_space_info;

/// \brief Returns the list of all applicable PUSCH Time Domain resource indexes based on cell, UE configuration and
/// nof. symbols in PUSCH slot for a PDCCH slot.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] pdcch_slot Slot at which the PDCCH is supposed to be scheduled.
/// \param[in] ss_info SearchSpace information.
/// \return List of PUSCH Time Domain resource indexes.
/// \remark If \c ss_info is nullptr, then minimum k1 is taken from \c cell_cfg.
static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
get_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                              slot_point                pdcch_slot,
                              const search_space_info*  ss_info = nullptr);

/// \brief Returns the list circularly indexed by slot containing the list of all applicable PUSCH Time Domain resource
/// indexes per slot.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] ss_info SearchSpace information.
/// \return A list circularly indexed by slot containing the list of all applicable PUSCH Time Domain resource indexes
/// per slot.
/// \remark The periodicity of the circularly indexed list is equal to nof. slots in a TDD period in case of TDD and 1
/// slot in case of FDD.
/// \remark If \c ss_info is nullptr, then minimum k1 is taken from \c cell_cfg.
/// \remark The list of applicable PUSCH Time Domain resources would be empty for UL slots as UL PDCCHs are scheduled
/// only over DL slots.
/// \remark In case of FDD, the list returned would contain applicable PUSCH Time Domain resource indexes for only one
/// slot.
std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>
get_pusch_td_resource_indices_per_slot(const cell_configuration& cell_cfg, const search_space_info* ss_info = nullptr);

/// Returns the list circularly indexed by slot containing the list of applicable PUSCH Time Domain resource indexes per
/// slot fairly distributed among all the PDCCH slots.
///
/// In case of UL heavy TDD pattern, this function ensures that UL PDCCH scheduling the PUSCH are fairly
/// distributed across DL slots in a TDD configuration. In all other scenarios (FDD or DL heavy TDD pattern), the
/// function behaves similar to \c get_pusch_td_resource_indices_per_slot().
///
/// \param[in] cell_cfg Cell configuration.
/// \param[in] ss_info SearchSpace information.
/// \return A list circularly indexed by slot containing the list of applicable PUSCH Time Domain resource indexes per
/// slot.
/// \remark The periodicity of the circularly indexed list is equal to nof. slots in a TDD period in case of TDD and 1
/// slot in case of FDD.
/// \remark If \c ss_info is nullptr, then minimum k1 is taken from \c cell_cfg.
/// \remark The list of applicable PUSCH Time Domain resources would be empty for UL slots as UL PDCCHs are scheduled
/// only over DL slots.
/// \remark In case of FDD, the list returned would contain applicable PUSCH Time Domain resource indexes for only one
/// slot.
std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>
get_fairly_distributed_pusch_td_resource_indices(const cell_configuration& cell_cfg,
                                                 const search_space_info*  ss_info = nullptr);

} // namespace srsran
