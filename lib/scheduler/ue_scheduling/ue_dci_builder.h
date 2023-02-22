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

#include "harq_process.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {

/// Get redundancy version.
inline unsigned get_redundancy_version(unsigned nof_retxs)
{
  static constexpr std::array<unsigned, 4> rv_idx = {0, 2, 3, 1};
  return rv_idx[nof_retxs % rv_idx.size()];
}

/// Builds DCI f1_0 for TC-RNTI, used for instance in Msg4.
void build_dci_f1_0_tc_rnti(dci_dl_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            prb_interval               prbs,
                            unsigned                   time_resource,
                            unsigned                   k1,
                            unsigned                   pucch_res_indicator,
                            sch_mcs_index              mcs_index,
                            const dl_harq_process&     h_dl);

/// Builds DCI f1_0 for C-RNTI.
void build_dci_f1_0_c_rnti(dci_dl_info&                       dci,
                           const bwp_downlink_common&         init_dl_bwp,
                           const bwp_configuration&           active_dl_bwp,
                           search_space_configuration::type_t ss_type,
                           prb_interval                       prbs,
                           unsigned                           time_resource,
                           unsigned                           k1,
                           unsigned                           pucch_res_indicator,
                           unsigned                           dai,
                           sch_mcs_index                      mcs_index,
                           const dl_harq_process&             h_dl);

/// Builds DCI f0_0 for TC-RNTI, used in Msg3 retxs.
void build_dci_f0_0_tc_rnti(dci_ul_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            const bwp_configuration&   ul_bwp,
                            const prb_interval&        prbs,
                            unsigned                   time_resource,
                            sch_mcs_index              mcs_index,
                            const ul_harq_process&     h_ul);

/// Builds DCI f0_0 for C-RNTI.
void build_dci_f0_0_c_rnti(dci_ul_info&                       dci,
                           const bwp_downlink_common&         init_dl_bwp,
                           const bwp_configuration&           active_dl_bwp,
                           const bwp_configuration&           init_ul_bwp,
                           const bwp_configuration&           active_ul_bwp,
                           search_space_configuration::type_t ss_type,
                           const prb_interval&                prbs,
                           unsigned                           time_resource,
                           sch_mcs_index                      mcs_index,
                           const ul_harq_process&             h_ul);

} // namespace srsran
