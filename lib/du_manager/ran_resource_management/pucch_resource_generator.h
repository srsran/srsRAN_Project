/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cell_group_config.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/pucch/pucch_configuration.h"

namespace srsran {
namespace srs_du {

/// Parameters for the generated PUCCH resource.
struct cell_pucch_resource {
  /// Identifies the PUCCH resources within the cell's list.
  unsigned cell_res_id;
  unsigned starting_prb;
  /// Only set for intra-slot frequency hopping.
  optional<unsigned>                                                                        second_hop_prb;
  pucch_format                                                                              format;
  variant<pucch_format_0_cfg, pucch_format_1_cfg, pucch_format_2_3_cfg, pucch_format_4_cfg> format_params;
};

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
/// \return The list of PUCCH resources for a cell.
/// \remark The function returns an empty list in the following cases: (i) If the given number of RBs is not enough to
/// allocate at least 1 resource. (ii) If the RBs occupancy is larger than the BWP size. (iii) If F2 intra-slot
/// frequency hopping is enabled with only 1 symbol.
std::vector<cell_pucch_resource> generate_pucch_res_list_given_rbs(unsigned        max_pucch_f1_rbs,
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
/// \return The list of PUCCH resources for a cell.
/// \remark The function returns an empty list in the following cases: (i) If overall the RBs occupancy is larger than
/// the BWP size. (ii) If F2 intra-slot frequency hopping is enabled with only 1 symbol.
std::vector<cell_pucch_resource> generate_pucch_res_list_given_number(unsigned        nof_res_f1,
                                                                      unsigned        nof_res_f2,
                                                                      pucch_f1_params f1_params,
                                                                      pucch_f2_params f2_params,
                                                                      unsigned        bwp_size_rbs);
} // namespace srs_du
} // namespace srsran
