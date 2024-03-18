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

#include "../config/cell_configuration.h"
#include "../config/ue_configuration.h"
#include "../ue_scheduling/harq_process.h"
#include "../ue_scheduling/ue_channel_state_manager.h"
#include "mcs_tbs_calculator.h"
#include "srsran/scheduler/harq_id.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

struct pdsch_config_params {
  pdsch_mcs_table   mcs_table;
  ofdm_symbol_range symbols;
  unsigned          nof_oh_prb;
  unsigned          tb_scaling_field;
  unsigned          nof_layers;
  dmrs_information  dmrs;
  unsigned          max_nof_cws_scheduled_by_dci;
};

/// Contains some of the PUSCH parameters needed to compute the MCS, the number of PRBs, the TBS and to build the PUSCH
/// PDU.
struct pusch_config_params {
  pusch_mcs_table   mcs_table;
  ofdm_symbol_range symbols;
  unsigned          nof_oh_prb;
  unsigned          tb_scaling_field;
  unsigned          nof_layers;
  bool              tp_pi2bpsk_present;
  dmrs_information  dmrs;
  unsigned          nof_harq_ack_bits{0};
  unsigned          nof_csi_part1_bits{0};
  unsigned          max_nof_csi_part2_bits{0};
};

/// \brief Fetches the PDSCH parameters needed for PDSCH PDU for DCI format 1_0, scrambled by TC-RNTI.
///
/// Returns parameters needed to compute the number of PRBs, MCS and TBS.
pdsch_config_params get_pdsch_config_f1_0_tc_rnti(const cell_configuration&                    cell_cfg,
                                                  const pdsch_time_domain_resource_allocation& pdsch_td_cfg);

/// \brief Fetches the PDSCH parameters needed for PUSCH PDU for DCI format 1_0, scrambled by C-RNTI.
///
/// Returns parameters needed to compute the number of PRBs, MCS and TBS.
pdsch_config_params get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg,
                                                 const ue_cell_configuration*                 ue_cell_cfg,
                                                 const pdsch_time_domain_resource_allocation& pdsch_td_cfg);

/// \brief Fetches the PDSCH parameters needed for PUSCH PDU for DCI format 1_1, scrambled by C-RNTI.
///
/// Returns parameters needed to compute the number of PRBs, MCS and TBS.
pdsch_config_params get_pdsch_config_f1_1_c_rnti(const ue_cell_configuration&                 ue_cell_cfg,
                                                 const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                                 unsigned                                     nof_layers);

/// \brief Fetches the PUSCH parameters needed for PUSCH PDU for DCI format 0_0, scrambled by TC-RNTI.
///
/// The parameters returned by this function are needed to compute the number of PRBs, MCS and TBS.
pusch_config_params get_pusch_config_f0_0_tc_rnti(const cell_configuration&                    cell_cfg,
                                                  const pusch_time_domain_resource_allocation& pusch_td_cfg);

/// \brief Fetches the PUSCH parameters needed for PUSCH PDU for DCI format 0_0, scrambled by C-RNTI.
///
/// The parameters returned by this function are needed to compute the number of PRBs, MCS and TBS.
pusch_config_params get_pusch_config_f0_0_c_rnti(const ue_cell_configuration&                 ue_cell_cfg,
                                                 const bwp_uplink_common&                     ul_bwp,
                                                 const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                 const unsigned                               nof_harq_ack_bits,
                                                 bool                                         is_csi_report_slot);

/// \brief Fetches the PUSCH parameters needed for PUSCH PDU for DCI format 0_1, scrambled by C-RNTI.
///
/// The parameters returned by this function are needed to compute the number of PRBs, MCS and TBS.
pusch_config_params get_pusch_config_f0_1_c_rnti(const ue_cell_configuration&                 ue_cell_cfg,
                                                 const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                 unsigned                                     nof_layers,
                                                 const unsigned                               nof_harq_ack_bits,
                                                 bool                                         is_csi_report_slot);

/// \brief Builds PDSCH PDU for DCI format 1_0, scrambled by SI-RNTI.
void build_pdsch_f1_0_si_rnti(pdsch_information&                   pdsch,
                              const cell_configuration&            cell_cfg,
                              unsigned                             tbs_bytes,
                              const dci_1_0_si_rnti_configuration& dci_cfg,
                              const crb_interval&                  crbs,
                              const ofdm_symbol_range&             symbols,
                              const dmrs_information&              dmrs_info);

/// \brief Builds PDSCH PDU for DCI format 1_0, scrambled by P-RNTI.
void build_pdsch_f1_0_p_rnti(pdsch_information&                  pdsch,
                             const cell_configuration&           cell_cfg,
                             unsigned                            tbs_bytes,
                             const dci_1_0_p_rnti_configuration& dci_cfg,
                             const crb_interval&                 crbs,
                             const ofdm_symbol_range&            symbols,
                             const dmrs_information&             dmrs_info);

/// \brief Builds PDSCH PDU for DCI format 1_0, scrambled by RA-RNTI.
void build_pdsch_f1_0_ra_rnti(pdsch_information&                   pdsch,
                              unsigned                             tbs_bytes,
                              rnti_t                               rnti,
                              const cell_configuration&            cell_cfg,
                              const dci_1_0_ra_rnti_configuration& dci_cfg,
                              const crb_interval&                  crbs,
                              const dmrs_information&              dmrs_info);

/// \brief Builds PDSCH PDU for DCI format 1_0, scrambled by TC-RNTI.
void build_pdsch_f1_0_tc_rnti(pdsch_information&                   pdsch,
                              const pdsch_config_params&           pdsch_cfg,
                              unsigned                             tbs_bytes,
                              rnti_t                               rnti,
                              const cell_configuration&            cell_cfg,
                              const dci_1_0_tc_rnti_configuration& dci_cfg,
                              const crb_interval&                  crbs,
                              bool                                 is_new_data);

/// \brief Builds PDSCH PDU for DCI format 1_0, scrambled by C-RNTI.
void build_pdsch_f1_0_c_rnti(pdsch_information&                  pdsch,
                             const pdsch_config_params&          pdsch_cfg,
                             unsigned                            tbs_bytes,
                             rnti_t                              rnti,
                             const cell_configuration&           cell_cfg,
                             const search_space_info&            ss_info,
                             const dci_1_0_c_rnti_configuration& dci_cfg,
                             const crb_interval&                 crbs,
                             bool                                is_new_data);

/// \brief Builds PDSCH PDU for DCI format 1_1, scrambled by C-RNTI.
void build_pdsch_f1_1_c_rnti(pdsch_information&              pdsch,
                             const pdsch_config_params&      pdsch_cfg,
                             sch_mcs_tbs                     mcs_tbs_info,
                             rnti_t                          rnti,
                             const ue_cell_configuration&    ue_cell_cfg,
                             search_space_id                 ss_id,
                             const dci_1_1_configuration&    dci_cfg,
                             const crb_interval&             crbs,
                             const dl_harq_process&          h_dl,
                             const ue_channel_state_manager& cs_mgr);

/// \brief Builds PUSCH PDU for DCI format 0_0, scrambled by TC-RNTI.
void build_pusch_f0_0_tc_rnti(pusch_information&                   pusch,
                              const pusch_config_params&           pusch_cfg,
                              unsigned                             tbs_bytes,
                              rnti_t                               rnti,
                              const cell_configuration&            cell_cfg,
                              const dci_0_0_tc_rnti_configuration& dci_cfg,
                              const crb_interval&                  crbs,
                              bool                                 is_new_data);

/// \brief Builds PUSCH PDU for DCI format 0_0, scrambled by C-RNTI.
void build_pusch_f0_0_c_rnti(pusch_information&                  pusch,
                             rnti_t                              rnti,
                             const pusch_config_params&          pusch_cfg,
                             unsigned                            tbs_bytes,
                             const cell_configuration&           cell_cfg,
                             const bwp_uplink_common&            ul_bwp,
                             const dci_0_0_c_rnti_configuration& dci_cfg,
                             const crb_interval&                 crbs,
                             bool                                is_new_data);

/// \brief Builds PUSCH PDU for DCI format 0_1, scrambled by C-RNTI.
void build_pusch_f0_1_c_rnti(pusch_information&           pusch,
                             rnti_t                       rnti,
                             const pusch_config_params&   pusch_cfg,
                             sch_mcs_tbs                  mcs_tbs_info,
                             const ue_cell_configuration& ue_cell_cfg,
                             search_space_id              ss_id,
                             const dci_0_1_configuration& dci_cfg,
                             const crb_interval&          crbs,
                             const ul_harq_process&       h_ul);

} // namespace srsran
