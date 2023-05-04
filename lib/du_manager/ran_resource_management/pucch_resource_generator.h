/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// Defines the number of different Initial Cyclic Shifts that can be used for PUCCH Format 1, as per \c PUCCH-format1,
/// in \c PUCCH-Config, TS 38.331. We assume the CS are evenly distributed, which means we can only have a divisor of 12
/// possible cyclic shifts.
enum class nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

inline unsigned format1_cp_step_to_uint(nof_cyclic_shifts opt)
{
  return static_cast<unsigned>(opt);
}

/// Collects the parameters for PUCCH Format 1 that can be configured.
struct pucch_f1_params {
  /// Number of possible Initial Cyclic Shifts, equally spaced within the range {0,...,11}, as per \c PUCCH-format1, in
  /// \c PUCCH-Config, TS 38.331.
  nof_cyclic_shifts nof_cyc_shifts{nof_cyclic_shifts::no_cyclic_shift};
  /// Indicates whether OCCs (as per \c PUCCH-format1, in \c PUCCH-Config, TS 38.331) are supported.
  bool                             occ_supported{false};
  bounded_integer<unsigned, 4, 14> nof_symbols{14};
  bool                             intraslot_freq_hopping{false};
};

/// Collects the parameters for PUCCH Format 2 that can be configured.
struct pucch_f2_params {
  bounded_integer<unsigned, 1, 2> nof_symbols{1};
  unsigned                        max_nof_rbs{1};
  /// Maximum payload in bits that can be carried by PUCCH Format 2. When this field is set, \c max_nof_rbs is ignored
  /// and the maximum number of RBs is computed according to \ref get_pucch_format2_max_nof_prbs.
  optional<unsigned>  max_payload_bits;
  max_pucch_code_rate max_code_rate{max_pucch_code_rate::dot_25};
  /// For intraslot-freq-hopping, \c nof_symbols must be set to 2.
  bool intraslot_freq_hopping{false};
};

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
std::vector<cell_pucch_resource> generate_pucch_res_list_given_rbs(unsigned max_pucch_f1_rbs,
                                                                   unsigned max_pucch_f2_rbs,

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
