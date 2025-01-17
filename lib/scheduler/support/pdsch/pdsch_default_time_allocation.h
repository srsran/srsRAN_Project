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

#include "srsran/adt/span.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/dmrs.h"
#include "srsran/scheduler/config/pxsch_time_domain_resource.h"

namespace srsran {

struct search_space_configuration;
struct pdsch_config_common;
struct bwp_downlink_common;
struct bwp_downlink_dedicated;

/// \brief Determines the default PDSCH time-domain resource allocation A.
///
/// The time domain allocation configuration is described in TS38.214 Table 5.1.2.1.1-2 for normal cyclic prefix and
/// TS38.214 Table 5.1.2.1.1-3 for extended cyclic prefix.
///
/// \param[in] cp        Cyclic prefix.
/// \param[in] row_index Row index.
/// \param[in] dmrs_pos  DM-RS for mapping Type A position.
/// \return A valid PDSCH time-domain allocation configuration is the provided parameters are valid. Otherwise,
/// invalid config.
pdsch_time_domain_resource_allocation
pdsch_default_time_allocation_default_A_get(cyclic_prefix cp, unsigned row_index, dmrs_typeA_position dmrs_pos);

/// \brief Determines the table of default PDSCH time-domain resource allocation A.
///
/// \param[in] cp        Cyclic prefix.
/// \param[in] dmrs_pos  DM-RS for mapping Type A position.
/// \return A list of valid PDSCH time-domain allocation configurations to choose from.
span<const pdsch_time_domain_resource_allocation>
pdsch_default_time_allocations_default_A_table(cyclic_prefix cp, dmrs_typeA_position dmrs_pos);

/// \brief Determines the time domain resource allocation table to be used for PDSCH as per TS 38.214, clause 5.1.2.1.1.
///
/// \param[in] ss_cfg Search Space configuration.
/// \param[in] active_bwp_dl_common Active DL BWP common configuration.
/// \param[in] active_bwp_dl_ded Active DL BWP UE-dedicated configuration.
/// \param[in] dmrs_typeA_pos DM-RS for mapping Type A position.
/// \return A list of valid PDSCH time-domain allocation configurations to choose from.
span<const pdsch_time_domain_resource_allocation>
get_c_rnti_pdsch_time_domain_list(const search_space_configuration& ss_cfg,
                                  const bwp_downlink_common&        active_bwp_dl_common,
                                  const bwp_downlink_dedicated*     active_bwp_dl_ded,
                                  dmrs_typeA_position               dmrs_typeA_pos);

/// \brief Determines the time domain resource allocation table to be used for PDSCH scheduled with SI-RNTI Type0 common
/// as per TS 38.214, clause 5.1.2.1.1-1.
///
/// \param[in] cp        Cyclic prefix.
/// \param[in] dmrs_pos  DM-RS for mapping Type A position.
/// \return A list of valid PDSCH time-domain allocation configurations to choose from.
span<const pdsch_time_domain_resource_allocation>
get_si_rnti_pdsch_time_domain_list(cyclic_prefix cp, dmrs_typeA_position dmrs_typeA_pos);

/// \brief Determines the time domain resource allocation table to be used for PDSCH scheduled with SI-RNTI Type0A
/// common as per TS 38.214, clause 5.1.2.1.1-1.
///
/// \param[in] cp        Cyclic prefix.
/// \param[in] dmrs_pos  DM-RS for mapping Type A position.
/// \return A list of valid PDSCH time-domain allocation configurations to choose from.
span<const pdsch_time_domain_resource_allocation>
get_si_rnti_type0A_common_pdsch_time_domain_list(const pdsch_config_common& pdsch_common,
                                                 cyclic_prefix              cp,
                                                 dmrs_typeA_position        dmrs_typeA_pos);

/// \brief Determines the time domain resource allocation table to be used for PDSCH scheduled with RA-RNTI as per
/// TS 38.214, clause 5.1.2.1.1-1.
///
/// \param[in] pdsch_common PDSCH common configuration.
/// \param[in] cp        Cyclic prefix.
/// \param[in] dmrs_pos  DM-RS for mapping Type A position.
/// \return A list of valid PDSCH time-domain allocation configurations to choose from.
span<const pdsch_time_domain_resource_allocation>
get_ra_rnti_pdsch_time_domain_list(const pdsch_config_common& pdsch_common,
                                   cyclic_prefix              cp,
                                   dmrs_typeA_position        dmrs_typeA_pos);

} // namespace srsran
