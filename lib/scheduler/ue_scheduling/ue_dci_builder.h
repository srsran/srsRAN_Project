/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "harq_process.h"
#include "srsgnb/ran/pdcch/search_space.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsgnb/scheduler/config/bwp_configuration.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

/// Builds DCI f1_0 for TC-RNTI, used for instance in Msg4.
void build_dci_f1_0_tc_rnti(dci_dl_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            prb_interval               prbs,
                            unsigned                   time_resource,
                            unsigned                   k1,
                            unsigned                   pucch_res_indicator,
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
                           const dl_harq_process&             h_dl,
                           unsigned                           tb_idx);

/// Builds DCI f0_0 for TC-RNTI, used in Msg3 retxs.
void build_dci_f0_0_tc_rnti(dci_ul_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            const bwp_configuration&   ul_bwp,
                            const prb_interval&        prbs,
                            unsigned                   time_resource,
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
                           const ul_harq_process&             h_ul);

} // namespace srsgnb