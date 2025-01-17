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

#include "srsran/ran/cyclic_prefix.h"
#include "srsran/scheduler/config/dmrs.h"
#include "srsran/scheduler/config/pxsch_time_domain_resource.h"

namespace srsran {

struct search_space_configuration;
struct bwp_uplink_common;
struct bwp_uplink_dedicated;

/// \brief Determines the default PUSCH time-domain resource allocation A.
///
/// The time domain allocation configuration is described in TS38.214 Table 5.1.2.1.1-2 for normal cyclic prefix and
/// TS38.214 Table 5.1.2.1.1-3 for extended cyclic prefix.
///
/// \param[in] cp        Cyclic prefix.
/// \param[in] row_index Row index.
/// \param[in] scs       PUSCH subcarrier spacing.
/// \return A valid PUSCH time-domain allocation configuration is the provided parameters are valid. Otherwise,
/// invalid resource.
pusch_time_domain_resource_allocation
pusch_default_time_allocation_default_A_get(cyclic_prefix cp, unsigned row_index, subcarrier_spacing scs);

/// \brief Determines the table of default PUSCH time-domain resource allocation A.
///
/// \param[in] cp  Cyclic prefix.
/// \param[in] scs PUSCH subcarrier spacing.
/// \return A list of valid PUSCH time-domain allocation configurations to choose from.
span<const pusch_time_domain_resource_allocation>
pusch_default_time_allocations_default_A_table(cyclic_prefix cp, subcarrier_spacing scs);

/// \brief Determines the time domain resource allocation table to be used for PUSCH as per TS 38.214, clause 6.1.2.1.1.
///
/// \param is_common_ss Whether the active searchSpace is a common searchSpace.
/// \param cs_id Coreset ID associated with the chosen searchSpace.
/// \param active_bwp_ul_common Active BWP UL common configuration.
/// \param active_bwp_ul_ded Active BWP UL UE-dedicated configuration.
/// \return A list of valid PUSCH time-domain allocation configurations to choose from.
span<const pusch_time_domain_resource_allocation>
get_c_rnti_pusch_time_domain_list(bool                        is_common_ss,
                                  coreset_id                  cs_id,
                                  const bwp_uplink_common&    active_bwp_ul_common,
                                  const bwp_uplink_dedicated* active_bwp_ul_ded);

/// \brief Determines the time domain resource allocation table to be used for PUSCH as per TS 38.214, clause 6.1.2.1.1.
///
/// \param ss_cfg Search Space configuration.
/// \param active_bwp_ul_common Active BWP UL common configuration.
/// \param active_bwp_ul_ded Active BWP UL UE-dedicated configuration.
/// \return A list of valid PUSCH time-domain allocation configurations to choose from.
span<const pusch_time_domain_resource_allocation>
get_c_rnti_pusch_time_domain_list(const search_space_configuration& ss_cfg,
                                  const bwp_uplink_common&          active_bwp_ul_common,
                                  const bwp_uplink_dedicated*       active_bwp_ul_ded);

} // namespace srsran
