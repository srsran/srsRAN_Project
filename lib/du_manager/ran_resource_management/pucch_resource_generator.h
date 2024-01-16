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

#include "cell_group_config.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/pucch/pucch_configuration.h"

namespace srsran {
namespace srs_du {

/// The following values have to be set according to the \ref pucch_resource_manager capabilities.
/// Maximum number of PUCCH F1 resources per UE for HARQ-ACK reporting.
constexpr unsigned max_ue_f1_res_harq = 8;
/// Maximum number of PUCCH F2 resources per UE for HARQ-ACK reporting.
constexpr unsigned max_ue_f2_res_harq = 8;

/// \brief Validates the user-defined parameters for building the PUCCH resource list.
/// \param[in] nof_res_f1 number of PUCCH F1 resources to be generated.
/// \param[in] nof_res_f2 number of PUCCH F2 resources to be generated.
/// \param[in] f1_params PUCCH F1 resource parameters.
/// \param[in] f1_params PUCCH F2 resource parameters.
/// \param[in] bwp_size_rbs Size of the BWP in RBs.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string> pucch_parameters_validator(unsigned        nof_res_f1,
                                                   unsigned        nof_res_f2,
                                                   pucch_f1_params f1_params,
                                                   pucch_f2_params f2_params,
                                                   unsigned        bwp_size_rbs);

/// \brief Generates the list of cell PUCCH resources (Format 1 and 2) given the available PRBs.
///
/// The function attempts to allocate the maximum number of resources given the number of PRBs. PUCCH resources F1 and
/// F2 are allocated on different RBs. The function attempts to spread the resources on both
///// sides of the BWP.
///
/// \param[in] max_pucch_f1_rbs number of available RBs where to allocate PUCCH F1 resources.
/// \param[in] max_pucch_f2_rbs number of available RBs where to allocate PUCCH F2 resources.
/// \param[in] f1_params PUCCH F1 resource parameters.
/// \param[in] f1_params PUCCH F2 resource parameters.
/// \param[in] bwp_size_rbs Size of the BWP in RBs.
/// \return The list of PUCCH resources for a cell. The list has the PUCCH Format 1 resources in front of the list, and
/// the PUCCH Format 2 in the back of the list.
/// \remark The function returns an empty list in the following cases: (i) If the given number of RBs is not enough to
/// allocate at least 1 resource. (ii) If the RBs occupancy is larger than the BWP size. (iii) If F2 intra-slot
/// frequency hopping is enabled with only 1 symbol.
std::vector<pucch_resource> generate_cell_pucch_res_list_given_rbs(unsigned        max_pucch_f1_rbs,
                                                                   unsigned        max_pucch_f2_rbs,
                                                                   pucch_f1_params f1_params,
                                                                   pucch_f2_params f2_params,
                                                                   unsigned        bwp_size_rbs);

/// \brief Generates the list of cell PUCCH resources (Format 1 and 2) given the number of requested resources.
///
/// PUCCH resources F1 and F2 are allocated on different RBs. The function attempts to spread the resources on both
/// sides of the BWP.
///
/// \param[in] nof_res_f1 number of PUCCH F1 resources to be generated.
/// \param[in] nof_res_f2 number of PUCCH F2 resources to be generated.
/// \param[in] f1_params PUCCH F1 resource parameters.
/// \param[in] f1_params PUCCH F2 resource parameters.
/// \param[in] bwp_size_rbs Size of the BWP in RBs.
/// \return The list of PUCCH resources for a cell. The list has the PUCCH Format 1 resources in front of the list, and
/// the PUCCH Format 2 in the back of the list.
/// \remark The function returns an empty list in the following cases: (i) If overall the RBs occupancy is larger than
/// the BWP size. (ii) If F2 intra-slot frequency hopping is enabled with only 1 symbol.
std::vector<pucch_resource> generate_cell_pucch_res_list(unsigned        nof_res_f1,
                                                         unsigned        nof_res_f2,
                                                         pucch_f1_params f1_params,
                                                         pucch_f2_params f2_params,
                                                         unsigned        bwp_size_rbs);

/// \brief Generates the list of PUCCH resources for a given UE.
///
/// This function generates the list of PUCCH F1 and F2 resources for a given UE, including the resources for HARQ-ACK
/// reporting, SR and CSI. It also updates the PUCCH resource sets accordingly, as well as the pointers to the PUCCH F1
/// resource for SR and to the PUCCH F2 resource for CSI.
/// This function overwrites the default \c ServingCellConfig passed as a function input.
///
/// The UE's PUCCH resource list composed of:
/// - \ref nof_ue_pucch_f1_res_harq PUCCH Format 1 resources for HARQ-ACK reporting, chosen from
///   \ref nof_harq_pucch_sets possible sets of PUCCH Format 1 cell resources.
/// - 1 PUCCH Format 1 resource for SR chosen from \ref nof_cell_pucch_f1_res_sr possible sets of PUCCH Format 1
///   cell resources.
/// - \ref nof_ue_pucch_f1_res_harq PUCCH Format 2 resources for HARQ-ACK reporting, chosen from
///   \ref nof_harq_pucch_sets possible sets of PUCCH Format 2 cell resources.
/// - 1 PUCCH Format 2 resource for CSI chosen from \ref nof_cell_pucch_f2_res_csi possible sets of PUCCH Format 1
///   cell resources.
///
/// The returned UE PUCCH resource list \ref pucch_res_list contains the following resources, sorted as follows:
///       [ F1-HARQ_0 ... F1-HARQ_N-1 F1-SR F2-HARQ_0 ... F2-HARQ_M-1 F2-CSI ]
/// where N = nof_ue_pucch_f1_res_harq and M = nof_ue_pucch_f2_res_harq,
/// and with the following indices \ref res_id:
/// - The first \ref nof_ue_pucch_f1_res_harq are the PUCCH F1 resources for HARQ-ACK and have index
///   [ (du_harq_set_idx % nof_harq_pucch_sets) * nof_ue_pucch_f1_res_harq,
///     (du_harq_set_idx % nof_harq_pucch_sets) * nof_ue_pucch_f1_res_harq + nof_ue_pucch_f1_res_harq ).
/// - The next resource in the list is the PUCCH F1 resource for SR, which have index:
///      nof_harq_pucch_sets * nof_ue_pucch_f1_res_harq + du_sr_res_idx % nof_cell_pucch_f1_res_sr.
/// - The next \ref nof_ue_pucch_f2_res_harq are the PUCCH F2 resources for HARQ-ACK and have index
///   [  nof_harq_pucch_sets * nof_ue_pucch_f1_res_harq + nof_cell_pucch_f1_res_sr +
///                     (du_harq_set_idx % nof_harq_pucch_sets) * nof_ue_pucch_f2_res_harq,
///      nof_harq_pucch_sets * nof_ue_pucch_f1_res_harq + nof_cell_pucch_f1_res_sr +
///                     (du_harq_set_idx % nof_harq_pucch_sets) * nof_ue_pucch_f2_res_harq + nof_ue_pucch_f2_res_harq).
/// - The last resource in the list is the PUCCH F2 resource for CSI, which has index:
////     nof_harq_pucch_sets * nof_ue_pucch_f1_res_harq + nof_cell_pucch_f1_res_sr +
///                     nof_ue_pucch_f2_res_harq * nof_harq_pucch_sets + du_csi_res_idx % nof_cell_pucch_f2_res_csi.
///
/// \param[in,out] serv_cell_cfg default \c ServingCellConfig that will be overwritten by this function.
/// \param[in] res_list cell PUCCH resource list from which the function picks the UE PUCCH resources.
/// \param[in] du_harq_set_idx defines which PUCCH resource set for HARQ to be assigned to this UE among
///            \ref nof_harq_pucch_sets possible ones; the chosen set for this UE has index
///            du_harq_set_idx % nof_harq_pucch_sets.
/// \param[in] du_sr_res_idx defines which PUCCH resource for SR to be assigned to this UE among
///            \ref nof_cell_pucch_f1_res_sr possible ones.  Values: {0, ..., nof_cell_pucch_f1_res_sr-1}.
/// \param[in] du_csi_res_idx defines which PUCCH resource for CSI to be assigned to this UE among
///            \ref nof_cell_pucch_f2_res_csi possible ones.  Values: {0, ..., nof_cell_pucch_f2_res_csi-1}.
/// \param[in] nof_ue_pucch_f1_res_harq desired number of UE PUCCH F1 resources (HARQ-ACK) in UE configuration.
/// \param[in] nof_ue_pucch_f2_res_harq desired number of UE PUCCH F2 resources (HARQ-ACK) in UE configuration.
/// \param[in] nof_cell_pucch_f1_res_sr number of PUCCH F1 resources for SR available in the cell.
/// \param[in] nof_cell_pucch_f2_res_csi number of PUCCH F2 resources for CSI available in the cell.
/// \return true if the building is successful, false otherwise.
bool ue_pucch_config_builder(serving_cell_config&                             serv_cell_cfg,
                             const std::vector<pucch_resource>&               res_list,
                             unsigned                                         du_harq_set_idx,
                             unsigned                                         du_sr_res_idx,
                             unsigned                                         du_csi_res_idx,
                             bounded_integer<unsigned, 1, max_ue_f1_res_harq> nof_ue_pucch_f1_res_harq,
                             bounded_integer<unsigned, 1, max_ue_f2_res_harq> nof_ue_pucch_f2_res_harq,
                             unsigned                                         nof_harq_pucch_sets,
                             unsigned                                         nof_cell_pucch_f1_res_sr,
                             unsigned                                         nof_cell_pucch_f2_res_csi = 1);
} // namespace srs_du
} // namespace srsran
