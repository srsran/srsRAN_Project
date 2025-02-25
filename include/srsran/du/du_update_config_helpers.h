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

#include "srsran/adt/expected.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {

struct pucch_builder_params;
struct srs_builder_params;

namespace config_helpers {

/// \brief Compute the largest (internal) BWP PRB interval without PUCCH resources.
///
/// This function assumes that the PUCCH resources are located in 2 separate blocks, at both external sides of the BWP,
/// i.e., 1 block on the left side (where indices is 0, 1, 2, ...) and 1 block on the right side (where indices are ...,
/// N_BWP_RBs -3, N_BWP_RBs-2, N_BWP_RBs-1) of the BWP. The function computes the largest interval of PRBs that are not
/// used by the PUCCH resources. The starting PRB of the interval is the first PRB that is not used by the PUCCH
/// resources on the left side of the BWP, while and the ending PRB is the first PRB that is used by the PUCCH resources
/// on the right side of the BWP.
///
/// \param user_params parameters passed by the user for the generation the PUCCH resource list.
/// \param bwp_size size of the BWP in RBs.
/// \return The largest (internal) BWP PRB interval without PUCCH resources.
prb_interval find_largest_prb_interval_without_pucch(const pucch_builder_params& user_params, unsigned bwp_size);

/// \brief Compute the PRACH frequency start as a function of the PUCCH guardbands.
///
/// This function computes the PRACH frequency start so as it won't collide with the PUCCH resources. As per TS 38.331,
/// PRACH frequency start is \c msg1-FrequencyStart.
///
/// \param user_params parameters passed by the user for the generation the PUCCH resource list.
/// \param bwp_size size of the BWP in RBs.
/// \return PRACH frequency start.
unsigned compute_prach_frequency_start(const pucch_builder_params& user_params, unsigned bwp_size, bool is_long_prach);

/// \brief Compute the number of PUCCH resources that are used for SR and CSI.
///
/// If the user has defined a larger number of resources for SR and CSI than needed, this will returns the actual
/// estimated number of resources that will be used by the GNB, constrained by the maximum PUCCH grants per slot.
/// \param user_params parameters passed by the user for the generation the PUCCH resource list.
/// \param max_pucch_grants_per_slot maximum number of PUCCH grants that can be allocated per slot in the cell.
/// \param sr_period_msec SR period in milliseconds.
/// \param csi_period_msec CSI period in milliseconds.
void compute_nof_sr_csi_pucch_res(pucch_builder_params&   user_params,
                                  unsigned                max_pucch_grants_per_slot,
                                  float                   sr_period_msec,
                                  std::optional<unsigned> csi_period_msec);

bounded_integer<unsigned, 1, 14> compute_max_nof_pucch_symbols(const srs_builder_params& user_srs_params);

/// \brief Recompute or add new PUSCH time-domain resources to the list.
///
/// This function is used to enable the PUSCH to be scheduled in the SRS slots, where not all OFDM symbols are
/// available for PUSCH.
///
/// \param td_alloc_list List of PUSCH time-domain resources.
/// \param user_srs_params SRS configuration parameters.
/// \param tdd_cfg Optional TDD configuration.
void recompute_pusch_time_domain_resources(std::vector<pusch_time_domain_resource_allocation>& td_alloc_list,
                                           const srs_builder_params&                           user_srs_params,
                                           const std::optional<tdd_ul_dl_config_common>&       tdd_cfg);

} // namespace config_helpers
} // namespace srsran
