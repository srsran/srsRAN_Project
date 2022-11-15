/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/cell_configuration.h"
#include "srsgnb/scheduler/harq_id.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include "ue_configuration.h"

namespace srsgnb {

/// \brief Builds PDSCH PDU for DCI format 1_0, scrambled by TC-RNTI.
void build_pdsch_f1_0_tc_rnti(pdsch_information&                   pdsch,
                              rnti_t                               rnti,
                              const cell_configuration&            cell_cfg,
                              const dci_1_0_tc_rnti_configuration& dci_cfg);

/// \brief Builds PDSCH PDU for DCI format 1_0, scrambled by C-RNTI.
void build_pdsch_f1_0_c_rnti(pdsch_information&                  pdsch,
                             rnti_t                              rnti,
                             const ue_cell_configuration&        ue_cell_cfg,
                             bwp_id_t                            active_bwp_id,
                             const search_space_configuration&   ss_cfg,
                             const dci_1_0_c_rnti_configuration& dci_cfg);

/// \brief Builds PUSCH PDU for DCI format 0_0, scrambled by TC-RNTI.
void build_pusch_f0_0_tc_rnti(pusch_information&                   pusch,
                              rnti_t                               rnti,
                              const cell_configuration&            cell_cfg,
                              const dci_0_0_tc_rnti_configuration& dci_cfg,
                              bool                                 is_new_data);

/// \brief Builds PUSCH PDU for DCI format 0_0, scrambled by C-RNTI.
void build_pusch_f0_0_c_rnti(pusch_information&                  pusch,
                             rnti_t                              rnti,
                             const cell_configuration&           cell_cfg,
                             const bwp_uplink_common&            ul_bwp,
                             const dci_0_0_c_rnti_configuration& dci_cfg,
                             bool                                is_new_data);

} // namespace srsgnb
