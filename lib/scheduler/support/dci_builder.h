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

#include "../config/ue_configuration.h"
#include "../ue_scheduling/harq_process.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {

/// Builds DCI f1_0 for SI-RNTI used in SIBs.
void build_dci_f1_0_si_rnti(dci_dl_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            crb_interval               crbs,
                            unsigned                   time_resource,
                            sch_mcs_index              mcs_index,
                            uint8_t                    si_indicator);

/// Builds DCI f1_0 for P-RNTI used in PCCHs.
void build_dci_f1_0_p_rnti(dci_dl_info&               dci,
                           const bwp_downlink_common& init_dl_bwp,
                           crb_interval               crbs,
                           unsigned                   time_resource,
                           sch_mcs_index              mcs_index);

/// Builds DCI f1_0 for RA-RNTI used in RAR.
void build_dci_f1_0_ra_rnti(dci_dl_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            crb_interval               crbs,
                            unsigned                   time_resource,
                            sch_mcs_index              mcs_index);

/// Builds DCI f1_0 for TC-RNTI, used for instance in Msg4.
void build_dci_f1_0_tc_rnti(dci_dl_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            crb_interval               crbs,
                            unsigned                   time_resource,
                            unsigned                   k1,
                            unsigned                   pucch_res_indicator,
                            sch_mcs_index              mcs_index,
                            uint8_t                    rv,
                            const dl_harq_process&     h_dl);

/// Builds DCI f1_0 for C-RNTI.
void build_dci_f1_0_c_rnti(dci_dl_info&                 dci,
                           const ue_cell_configuration& ue_cell_cfg,
                           search_space_id              ss_id,
                           crb_interval                 crbs,
                           unsigned                     time_resource,
                           unsigned                     k1,
                           unsigned                     pucch_res_indicator,
                           unsigned                     dai,
                           sch_mcs_index                mcs_index,
                           uint8_t                      rv,
                           const dl_harq_process&       h_dl);

/// Builds DCI f1_1 for C-RNTI.
void build_dci_f1_1_c_rnti(dci_dl_info&                 dci,
                           const ue_cell_configuration& ue_cell_cfg,
                           search_space_id              ss_id,
                           prb_interval                 prbs,
                           unsigned                     time_resource,
                           unsigned                     k1,
                           unsigned                     pucch_res_indicator,
                           unsigned                     dai,
                           sch_mcs_index                tb1_mcs_index,
                           uint8_t                      rv,
                           const dl_harq_process&       h_dl,
                           unsigned                     nof_layers);

/// Builds DCI f0_0 for TC-RNTI, used in Msg3 retxs.
void build_dci_f0_0_tc_rnti(dci_ul_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            const bwp_configuration&   ul_bwp,
                            const crb_interval&        crbs,
                            unsigned                   time_resource,
                            sch_mcs_index              mcs_index,
                            uint8_t                    rv,
                            const ul_harq_process&     h_ul);

/// Builds DCI f0_0 for C-RNTI.
void build_dci_f0_0_c_rnti(dci_ul_info&                 dci,
                           const ue_cell_configuration& ue_cell_cfg,
                           search_space_id              ss_id,
                           const crb_interval&          crbs,
                           unsigned                     time_resource,
                           sch_mcs_index                mcs_index,
                           uint8_t                      rv,
                           const ul_harq_process&       h_ul);

/// Builds DCI f0_1 for C-RNTI.
void build_dci_f0_1_c_rnti(dci_ul_info&                 dci,
                           const ue_cell_configuration& ue_cell_cfg,
                           search_space_id              ss_id,
                           const crb_interval&          crbs,
                           unsigned                     time_resource,
                           sch_mcs_index                mcs_index,
                           uint8_t                      rv,
                           const ul_harq_process&       h_ul,
                           unsigned                     total_dai,
                           unsigned                     nof_layers);

} // namespace srsran
