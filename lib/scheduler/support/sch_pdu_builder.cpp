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

#include "sch_pdu_builder.h"
#include "../ue_scheduling/ue_channel_state_manager.h"
#include "dmrs_helpers.h"
#include "pdsch/pdsch_default_time_allocation.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "srsran/ran/csi_report/csi_report_pusch_size.h"
#include "srsran/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/scheduler/config/serving_cell_config.h"

using namespace srsran;

// See TS 38.211, 7.3.1.1. - Scrambling.
static unsigned
get_pdsch_n_id(pci_t pci, const bwp_downlink_dedicated* bwp_dl_ded, dci_dl_format dci_fmt, bool is_common_ss)
{
  if (bwp_dl_ded != nullptr and bwp_dl_ded->pdsch_cfg.has_value() and
      bwp_dl_ded->pdsch_cfg->data_scrambling_id_pdsch.has_value() and
      (dci_fmt != dci_dl_format::f1_0 or not is_common_ss)) {
    return *bwp_dl_ded->pdsch_cfg->data_scrambling_id_pdsch;
  }
  return pci;
}

pdsch_config_params srsran::get_pdsch_config_f1_0_tc_rnti(const cell_configuration&                    cell_cfg,
                                                          const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  static constexpr pdsch_mcs_table mcs_table = pdsch_mcs_table::qam64;
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned tb_scaling_field = 0;
  // As per TS 38.214, Section 5.1.3.2, nof_oh_prb by \c xOverhead, defined in \c PDSCH-ServingCellConfig, TS 38.331; it
  // is in the dedicated resources, configured the after DCI Format 1-0 TC-RNTI is used. Hence, nof_oh_prb is here set
  // as 0.
  static constexpr unsigned nof_oh_prb = 0;
  static constexpr unsigned nof_layers = 1;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_common(pdsch_td_cfg, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  pdsch.nof_oh_prb                   = nof_oh_prb;
  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.nof_layers                   = nof_layers;
  pdsch.max_nof_cws_scheduled_by_dci = 1;

  return pdsch;
}

pdsch_config_params srsran::get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg,
                                                         const ue_cell_configuration*                 ue_cell_cfg,
                                                         const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned        tb_scaling_field = 0;
  static constexpr unsigned        nof_layers       = 1;
  static constexpr pdsch_mcs_table mcs_table        = pdsch_mcs_table::qam64;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_common(pdsch_td_cfg, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  pdsch.nof_oh_prb = ue_cell_cfg != nullptr and ue_cell_cfg->cfg_dedicated().pdsch_serv_cell_cfg.has_value()
                         ? static_cast<unsigned>(ue_cell_cfg->cfg_dedicated().pdsch_serv_cell_cfg.value().x_ov_head)
                         : static_cast<unsigned>(x_overhead::not_set);

  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.nof_layers                   = nof_layers;
  pdsch.max_nof_cws_scheduled_by_dci = 1;

  return pdsch;
}

pdsch_config_params srsran::get_pdsch_config_f1_1_c_rnti(const ue_cell_configuration&                 ue_cell_cfg,
                                                         const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                                         unsigned                                     nof_layers)
{
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned tb_scaling_field = 0;

  // TODO: Update the value based on nof. CWs enabled.
  static const bool are_both_cws_enabled = false;

  pdsch_config_params pdsch;

  srsran_assert(ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg->pdsch_mapping_type_a_dmrs.has_value(),
                "No DMRS configured in PDSCH configuration");
  pdsch.dmrs =
      make_dmrs_info_dedicated(pdsch_td_cfg,
                               ue_cell_cfg.cell_cfg_common.pci,
                               ue_cell_cfg.cell_cfg_common.dmrs_typeA_pos,
                               ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg->pdsch_mapping_type_a_dmrs.value(),
                               nof_layers,
                               ue_cell_cfg.cell_cfg_common.dl_carrier.nof_ant,
                               are_both_cws_enabled);

  // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  pdsch.nof_oh_prb = ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg.has_value()
                         ? static_cast<unsigned>(ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg.value().x_ov_head)
                         : static_cast<unsigned>(x_overhead::not_set);

  pdsch.symbols          = pdsch_td_cfg.symbols;
  pdsch.mcs_table        = ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg->mcs_table;
  pdsch.tb_scaling_field = tb_scaling_field;
  pdsch.nof_layers       = nof_layers;
  pdsch.max_nof_cws_scheduled_by_dci =
      ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg->is_max_cw_sched_by_dci_is_two ? 2 : 1;

  return pdsch;
}

pusch_config_params srsran::get_pusch_config_f0_0_tc_rnti(const cell_configuration&                    cell_cfg,
                                                          const pusch_time_domain_resource_allocation& pusch_td_cfg)
{
  constexpr pusch_mcs_table mcs_table  = pusch_mcs_table::qam64;
  constexpr unsigned        nof_layers = 1;
  // As per TS 38.214, Section 6.1.4.2, nof_oh_prb by \c xOverhead, defined in \c PUSCH-ServingCellConfig, TS 38.331; it
  // is in the dedicated resources, configured the after DCI Format 0-0 TC-RNTI is used. Hence, nof_oh_prb is here set
  // as 0.
  static constexpr unsigned nof_oh_prb = 0;
  // As per TS 38.214, Section 5.1.3.2 and 6.1.4.2, and TS 38.212, Section 7.3.1.1 and 7.3.1.2, TB scaling filed is only
  // used for DCI Format 1-0 (in the DL). Therefore, for the PUSCH this is set to 0.
  constexpr unsigned tb_scaling_field = 0;
  // Parameter \c tp-pi2BPSK enabled is not supported yet.
  constexpr bool tp_pi2bpsk_present = false;
  // As with 1 or 2 bits the corresponding beta-offset won't change, we can over-allocate the number of HARQ-ACK bits
  // to 2.
  constexpr unsigned nof_harq_ack_bits = 2;

  pusch_config_params pusch;

  pusch.dmrs = make_dmrs_info_common(pusch_td_cfg, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  pusch.symbols = pusch_td_cfg.symbols;

  pusch.mcs_table          = mcs_table;
  pusch.nof_layers         = nof_layers;
  pusch.tp_pi2bpsk_present = tp_pi2bpsk_present;
  pusch.tb_scaling_field   = tb_scaling_field;

  pusch.nof_oh_prb        = nof_oh_prb;
  pusch.nof_harq_ack_bits = nof_harq_ack_bits;

  return pusch;
}

pusch_config_params srsran::get_pusch_config_f0_0_c_rnti(const ue_cell_configuration&                 ue_cell_cfg,
                                                         const bwp_uplink_common&                     ul_bwp,
                                                         const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                         const unsigned                               nof_harq_ack_bits,
                                                         bool is_csi_report_slot)
{
  const pusch_mcs_table mcs_table  = pusch_mcs_table::qam64;
  constexpr unsigned    nof_layers = 1;
  // As per TS 38.214, Section 5.1.3.2 and 6.1.4.2, and TS 38.212, Section 7.3.1.1 and 7.3.1.2, TB scaling filed is only
  // used for DCI Format 1-0 (in the DL). Therefore, for the PUSCH this is set to 0.
  constexpr unsigned tb_scaling_field = 0;
  // Parameter \c tp-pi2BPSK enabled is not supported yet.
  constexpr bool tp_pi2bpsk_present = false;

  pusch_config_params pusch;

  pusch.dmrs =
      make_dmrs_info_common(pusch_td_cfg, ue_cell_cfg.cell_cfg_common.pci, ue_cell_cfg.cell_cfg_common.dmrs_typeA_pos);

  pusch.symbols = pusch_td_cfg.symbols;

  pusch.mcs_table          = mcs_table;
  pusch.nof_layers         = nof_layers;
  pusch.tp_pi2bpsk_present = tp_pi2bpsk_present;
  pusch.tb_scaling_field   = tb_scaling_field;

  // According to TS 38.214, Section 6.1.4.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  pusch.nof_oh_prb =
      ue_cell_cfg.cfg_dedicated().ul_config.has_value() and
              ue_cell_cfg.cfg_dedicated().ul_config.value().pusch_scell_cfg.has_value()
          ? static_cast<unsigned>(ue_cell_cfg.cfg_dedicated().ul_config.value().pusch_scell_cfg.value().x_ov_head)
          : static_cast<unsigned>(x_overhead::not_set);

  // TODO: verify if this needs to be set depending on some configuration.
  pusch.nof_harq_ack_bits = nof_harq_ack_bits;
  if (is_csi_report_slot) {
    csi_report_configuration csi_rep_cfg =
        create_csi_report_configuration(ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value());
    // NOTE: The CSI size depends on whether the CSI is configured on PUSCH or PUCCH, as per Section 5.2.3, TS 38.214:
    // "For both Type I and Type II reports configured for PUCCH but transmitted on PUSCH, the determination of the
    // payload for CSI part 1 and CSI part 2 follows that of PUCCH as described in clause 5.2.4."
    if (is_pusch_configured(ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value())) {
      csi_report_pusch_size csi_size = get_csi_report_pusch_size(csi_rep_cfg);
      pusch.nof_csi_part1_bits       = csi_size.part1_size.value();
      pusch.max_nof_csi_part2_bits   = csi_size.part2_max_size.value();
    } else {
      pusch.nof_csi_part1_bits = get_csi_report_pucch_size(csi_rep_cfg).value();
    }
  }

  return pusch;
}

pusch_config_params srsran::get_pusch_config_f0_1_c_rnti(const ue_cell_configuration&                 ue_cell_cfg,
                                                         const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                         unsigned                                     nof_layers,
                                                         const unsigned                               nof_harq_ack_bits,
                                                         bool is_csi_report_slot)
{
  const pusch_mcs_table mcs_table = ue_cell_cfg.cfg_dedicated().ul_config->init_ul_bwp.pusch_cfg->mcs_table;
  // As per TS 38.214, Section 5.1.3.2 and 6.1.4.2, and TS 38.212, Section 7.3.1.1 and 7.3.1.2, TB scaling filed is only
  // used for DCI Format 1-0 (in the DL). Therefore, for the PUSCH this is set to 0.
  constexpr unsigned tb_scaling_field = 0;
  // Parameter \c tp-pi2BPSK enabled is not supported yet.
  constexpr bool tp_pi2bpsk_present = false;
  // TODO: Update the value based on nof. CWs enabled.
  static const bool are_both_cws_enabled = false;

  pusch_config_params pusch;

  // TODO: Consider DMRS configured in PUSCH-Config. Need helpers from Phy.
  srsran_assert(ue_cell_cfg.cfg_dedicated().ul_config->init_ul_bwp.pusch_cfg->pusch_mapping_type_a_dmrs.has_value(),
                "No DMRS configured in PUSCH configuration");
  pusch.dmrs = make_dmrs_info_dedicated(
      pusch_td_cfg,
      ue_cell_cfg.cell_cfg_common.pci,
      ue_cell_cfg.cell_cfg_common.dmrs_typeA_pos,
      ue_cell_cfg.cfg_dedicated().ul_config->init_ul_bwp.pusch_cfg->pusch_mapping_type_a_dmrs.value(),
      nof_layers,
      ue_cell_cfg.cell_cfg_common.ul_carrier.nof_ant,
      are_both_cws_enabled);

  pusch.symbols = pusch_td_cfg.symbols;

  pusch.mcs_table          = mcs_table;
  pusch.nof_layers         = nof_layers;
  pusch.tp_pi2bpsk_present = tp_pi2bpsk_present;
  pusch.tb_scaling_field   = tb_scaling_field;

  // According to TS 38.214, Section 6.1.4.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  pusch.nof_oh_prb =
      ue_cell_cfg.cfg_dedicated().ul_config.has_value() and
              ue_cell_cfg.cfg_dedicated().ul_config.value().pusch_scell_cfg.has_value()
          ? static_cast<unsigned>(ue_cell_cfg.cfg_dedicated().ul_config.value().pusch_scell_cfg.value().x_ov_head)
          : static_cast<unsigned>(x_overhead::not_set);

  // TODO: verify if this needs to be set depending on some configuration.
  pusch.nof_harq_ack_bits = nof_harq_ack_bits;
  if (is_csi_report_slot) {
    csi_report_configuration csi_rep_cfg =
        create_csi_report_configuration(ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value());
    // NOTE: The CSI size depends on whether the CSI is configured on PUSCH or PUCCH, as per Section 5.2.3, TS 38.214:
    // "For both Type I and Type II reports configured for PUCCH but transmitted on PUSCH, the determination of the
    // payload for CSI part 1 and CSI part 2 follows that of PUCCH as described in clause 5.2.4."
    if (is_pusch_configured(ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value())) {
      csi_report_pusch_size csi_size = get_csi_report_pusch_size(csi_rep_cfg);
      pusch.nof_csi_part1_bits       = csi_size.part1_size.value();
      pusch.max_nof_csi_part2_bits   = csi_size.part2_max_size.value();
    } else {
      pusch.nof_csi_part1_bits = get_csi_report_pucch_size(csi_rep_cfg).value();
    }
  }

  return pusch;
}

void srsran::build_pdsch_f1_0_si_rnti(pdsch_information&                   pdsch,
                                      const cell_configuration&            cell_cfg,
                                      unsigned                             tbs_bytes,
                                      const dci_1_0_si_rnti_configuration& dci_cfg,
                                      const crb_interval&                  crbs,
                                      const ofdm_symbol_range&             symbols,
                                      const dmrs_information&              dmrs_info)
{
  const bwp_downlink_common& bwp_dl = cell_cfg.dl_cfg_common.init_dl_bwp;
  const vrb_interval         vrbs =
      crb_to_vrb_f1_0_common_ss_non_interleaved(crbs, bwp_dl.pdcch_common.coreset0->get_coreset_start_crb());

  pdsch.rnti        = rnti_t::SI_RNTI;
  pdsch.bwp_cfg     = &bwp_dl.generic_params;
  pdsch.coreset_cfg = &*bwp_dl.pdcch_common.coreset0;
  pdsch.symbols     = symbols;
  pdsch.rbs         = vrbs;
  // As per TS 38.211, Section 7.3.1.1, n_ID is set to Physical Cell ID for SIB1.
  pdsch.n_id       = cell_cfg.pci;
  pdsch.nof_layers = 1;

  pdsch_codeword& cw   = pdsch.codewords.emplace_back();
  cw.rv_index          = dci_cfg.redundancy_version;
  cw.mcs_index         = dci_cfg.modulation_coding_scheme;
  cw.mcs_table         = pdsch_mcs_table::qam64;
  cw.mcs_descr         = pdsch_mcs_get_config(cw.mcs_table, cw.mcs_index);
  cw.tb_size_bytes     = tbs_bytes;
  pdsch.dmrs           = dmrs_info;
  pdsch.is_interleaved = dci_cfg.vrb_to_prb_mapping > 0;
  pdsch.ss_set_type =
      dci_cfg.system_information_indicator == 0 ? search_space_set_type::type0 : search_space_set_type::type0A;
  pdsch.dci_fmt = dci_dl_format::f1_0;

  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdsch.tx_pwr_info.pwr_ctrl_offset = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset;
    if (cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
      pdsch.tx_pwr_info.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
    }
  }
}

void srsran::build_pdsch_f1_0_p_rnti(pdsch_information&                  pdsch,
                                     const cell_configuration&           cell_cfg,
                                     unsigned                            tbs_bytes,
                                     const dci_1_0_p_rnti_configuration& dci_cfg,
                                     const crb_interval&                 crbs,
                                     const ofdm_symbol_range&            symbols,
                                     const dmrs_information&             dmrs_info)
{
  const bwp_downlink_common&        bwp_dl = cell_cfg.dl_cfg_common.init_dl_bwp;
  const search_space_configuration& ss_cfg =
      bwp_dl.pdcch_common.search_spaces[*bwp_dl.pdcch_common.paging_search_space_id];
  const coreset_configuration& cs_cfg =
      ss_cfg.get_coreset_id() == to_coreset_id(0) ? *bwp_dl.pdcch_common.coreset0 : *bwp_dl.pdcch_common.common_coreset;
  const vrb_interval vrbs =
      crb_to_vrb_f1_0_common_ss_non_interleaved(crbs, bwp_dl.pdcch_common.coreset0->get_coreset_start_crb());

  pdsch.rnti        = rnti_t::P_RNTI;
  pdsch.bwp_cfg     = &bwp_dl.generic_params;
  pdsch.coreset_cfg = &cs_cfg;
  pdsch.symbols     = symbols;
  pdsch.rbs         = vrbs;
  // As per TS 38.211, Section 7.3.1.1, n_ID is set to Physical Cell ID.
  pdsch.n_id       = cell_cfg.pci;
  pdsch.nof_layers = 1;

  pdsch_codeword& cw   = pdsch.codewords.emplace_back();
  cw.mcs_index         = dci_cfg.modulation_coding_scheme;
  cw.mcs_table         = pdsch_mcs_table::qam64;
  cw.mcs_descr         = pdsch_mcs_get_config(cw.mcs_table, cw.mcs_index);
  cw.tb_size_bytes     = tbs_bytes;
  pdsch.dmrs           = dmrs_info;
  pdsch.is_interleaved = dci_cfg.vrb_to_prb_mapping > 0;
  pdsch.ss_set_type    = search_space_set_type::type2;
  pdsch.dci_fmt        = dci_dl_format::f1_0;

  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdsch.tx_pwr_info.pwr_ctrl_offset = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset;
    if (cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
      pdsch.tx_pwr_info.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
    }
  }
}

void srsran::build_pdsch_f1_0_ra_rnti(pdsch_information&                   pdsch,
                                      unsigned                             tbs_bytes,
                                      rnti_t                               rnti,
                                      const cell_configuration&            cell_cfg,
                                      const dci_1_0_ra_rnti_configuration& dci_cfg,
                                      const crb_interval&                  crbs,
                                      const dmrs_information&              dmrs_info)
{
  const bwp_downlink_common&        bwp_dl = cell_cfg.dl_cfg_common.init_dl_bwp;
  const search_space_configuration& ss_cfg = bwp_dl.pdcch_common.search_spaces[bwp_dl.pdcch_common.ra_search_space_id];
  const coreset_configuration&      cs_cfg =
      ss_cfg.get_coreset_id() == to_coreset_id(0) ? *bwp_dl.pdcch_common.coreset0 : *bwp_dl.pdcch_common.common_coreset;
  const vrb_interval vrbs = crb_to_vrb_f1_0_common_ss_non_interleaved(
      crbs, cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->get_coreset_start_crb());
  const auto& pdsch_td_res_alloc_list =
      get_ra_rnti_pdsch_time_domain_list(cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common,
                                         cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp,
                                         cell_cfg.dmrs_typeA_pos);

  pdsch.rnti        = rnti;
  pdsch.bwp_cfg     = &bwp_dl.generic_params;
  pdsch.coreset_cfg = &cs_cfg;
  pdsch.rbs         = vrbs;
  pdsch.symbols     = pdsch_td_res_alloc_list[dci_cfg.time_resource].symbols;

  pdsch_codeword& cw = pdsch.codewords.emplace_back();
  cw.mcs_table       = pdsch_mcs_table::qam64;
  cw.mcs_index       = dci_cfg.modulation_coding_scheme;
  cw.rv_index        = 0; // Implementation-defined.
  cw.mcs_descr       = pdsch_mcs_get_config(cw.mcs_table, cw.mcs_index);
  cw.tb_size_bytes   = tbs_bytes;

  pdsch.dmrs = dmrs_info;
  // As per TS 38.211, Section 7.3.1.1, n_ID is set to Physical Cell ID for RA-RNTI.
  pdsch.n_id           = cell_cfg.pci;
  pdsch.nof_layers     = 1;
  pdsch.is_interleaved = dci_cfg.vrb_to_prb_mapping > 0;
  pdsch.ss_set_type    = search_space_set_type::type1;
  pdsch.dci_fmt        = dci_dl_format::f1_0;

  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdsch.tx_pwr_info.pwr_ctrl_offset = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset;
    if (cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
      pdsch.tx_pwr_info.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
    }
  }
}

void srsran::build_pdsch_f1_0_tc_rnti(pdsch_information&                   pdsch,
                                      const pdsch_config_params&           pdsch_cfg,
                                      unsigned                             tbs_bytes,
                                      rnti_t                               rnti,
                                      const cell_configuration&            cell_cfg,
                                      const dci_1_0_tc_rnti_configuration& dci_cfg,
                                      const crb_interval&                  crbs,
                                      bool                                 is_new_data)
{
  const bwp_downlink_common& bwp_dl = cell_cfg.dl_cfg_common.init_dl_bwp;

  pdsch.rnti = rnti;

  // PDSCH resources.
  pdsch.bwp_cfg = &bwp_dl.generic_params;
  // See 3GPP TS 38.213, clause 10.1 - CSS set configured by ra-SearchSpace is used for CRC scrambled by TC-RNTI.
  const search_space_configuration& ss = bwp_dl.pdcch_common.search_spaces[bwp_dl.pdcch_common.ra_search_space_id];
  if (ss.get_coreset_id() == to_coreset_id(0)) {
    pdsch.coreset_cfg = &*bwp_dl.pdcch_common.coreset0;
  } else {
    pdsch.coreset_cfg = &*bwp_dl.pdcch_common.common_coreset;
  }
  // See 3GPP TS 38.211, clause 7.3.1.6 - VRBs are shifted from PRBs by an offset equal to the coreset start.
  const vrb_interval vrbs = {crbs.start() - pdsch.coreset_cfg->get_coreset_start_crb(),
                             crbs.stop() - pdsch.coreset_cfg->get_coreset_start_crb()};
  pdsch.rbs               = vrbs;
  pdsch.symbols           = pdsch_cfg.symbols;

  pdsch.dmrs = pdsch_cfg.dmrs;
  // See TS 38.211, 7.3.1.1. - Scrambling.
  pdsch.n_id           = cell_cfg.pci;
  pdsch.is_interleaved = dci_cfg.vrb_to_prb_mapping > 0;
  // See TS38.213, 10.1. - Type1-PDCCH CSS set for CRC scrambled by a TC-RNTI on the PCell.
  pdsch.ss_set_type = search_space_set_type::type1;
  pdsch.dci_fmt     = dci_dl_format::f1_0;
  pdsch.harq_id     = to_harq_id(dci_cfg.harq_process_number);
  pdsch.nof_layers  = 1U;

  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdsch.tx_pwr_info.pwr_ctrl_offset = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset;
    if (cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
      pdsch.tx_pwr_info.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
    }
  }

  // One Codeword.
  pdsch_codeword& cw = pdsch.codewords.emplace_back();
  cw.new_data        = is_new_data;
  cw.rv_index        = dci_cfg.redundancy_version;
  cw.mcs_index       = dci_cfg.modulation_coding_scheme;
  cw.mcs_table       = pdsch_cfg.mcs_table;
  cw.mcs_descr       = pdsch_mcs_get_config(pdsch_cfg.mcs_table, cw.mcs_index);
  cw.tb_size_bytes   = tbs_bytes;
}

void srsran::build_pdsch_f1_0_c_rnti(pdsch_information&                  pdsch,
                                     const pdsch_config_params&          pdsch_cfg,
                                     unsigned                            tbs_bytes,
                                     rnti_t                              rnti,
                                     const cell_configuration&           cell_cfg,
                                     const search_space_info&            ss_info,
                                     const dci_1_0_c_rnti_configuration& dci_cfg,
                                     const crb_interval&                 crbs,
                                     bool                                is_new_data)
{
  const coreset_configuration& cs_cfg     = *ss_info.coreset;
  const bwp_info&              active_bwp = *ss_info.bwp;
  const bwp_downlink_common&   bwp_dl     = *active_bwp.dl_common;

  pdsch.rnti        = rnti;
  pdsch.bwp_cfg     = &bwp_dl.generic_params;
  pdsch.coreset_cfg = &cs_cfg;

  // See 3GPP TS 38.211, clause 7.3.1.6 - VRBs are shifted from PRBs by an offset equal to the coreset start.
  if (ss_info.cfg->is_common_search_space()) {
    pdsch.rbs =
        vrb_interval{crbs.start() - cs_cfg.get_coreset_start_crb(), crbs.stop() - cs_cfg.get_coreset_start_crb()};
  } else {
    const prb_interval prbs = crb_to_prb(bwp_dl.generic_params.crbs, crbs);
    pdsch.rbs               = vrb_interval{prbs.start(), prbs.stop()};
  }
  pdsch.symbols        = pdsch_cfg.symbols;
  pdsch.dmrs           = pdsch_cfg.dmrs;
  pdsch.is_interleaved = dci_cfg.vrb_to_prb_mapping > 0;
  // See TS38.213, 10.1.
  pdsch.ss_set_type =
      not ss_info.cfg->is_common_search_space() ? search_space_set_type::ue_specific : search_space_set_type::type3;
  pdsch.dci_fmt = dci_dl_format::f1_0;
  pdsch.harq_id = to_harq_id(dci_cfg.harq_process_number);
  // See TS 38.211, 7.3.1.1. - Scrambling.
  const bwp_downlink_dedicated* bwp_dl_ded = active_bwp.dl_ded;
  pdsch.n_id = get_pdsch_n_id(cell_cfg.pci, bwp_dl_ded, dci_dl_format::f1_0, ss_info.cfg->is_common_search_space());
  pdsch.nof_layers = 1;

  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdsch.tx_pwr_info.pwr_ctrl_offset = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset;
    if (cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
      pdsch.tx_pwr_info.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
    }
  }

  // One Codeword.
  pdsch_codeword& cw = pdsch.codewords.emplace_back();
  cw.new_data        = is_new_data;
  cw.rv_index        = dci_cfg.redundancy_version;
  cw.mcs_index       = dci_cfg.modulation_coding_scheme;
  cw.mcs_table       = pdsch_cfg.mcs_table;
  cw.mcs_descr       = pdsch_mcs_get_config(pdsch_cfg.mcs_table, cw.mcs_index);
  cw.tb_size_bytes   = tbs_bytes;
}

void srsran::build_pdsch_f1_1_c_rnti(pdsch_information&              pdsch,
                                     const pdsch_config_params&      pdsch_cfg,
                                     sch_mcs_tbs                     mcs_tbs_info,
                                     rnti_t                          rnti,
                                     const ue_cell_configuration&    ue_cell_cfg,
                                     search_space_id                 ss_id,
                                     const dci_1_1_configuration&    dci_cfg,
                                     const crb_interval&             crbs,
                                     const dl_harq_process&          h_dl,
                                     const ue_channel_state_manager& cs_mgr)
{
  const cell_configuration&    cell_cfg       = ue_cell_cfg.cell_cfg_common;
  const search_space_info&     ss_info        = ue_cell_cfg.search_space(ss_id);
  const coreset_configuration& cs_cfg         = *ss_info.coreset;
  const bwp_info&              active_bwp     = *ss_info.bwp;
  const bwp_configuration&     active_bwp_cfg = active_bwp.dl_common->generic_params;
  const prb_interval           prbs           = crb_to_prb(active_bwp_cfg.crbs, crbs);

  pdsch.rnti        = rnti;
  pdsch.bwp_cfg     = &active_bwp_cfg;
  pdsch.coreset_cfg = &cs_cfg;

  pdsch.rbs            = vrb_interval{prbs.start(), prbs.stop()};
  pdsch.symbols        = pdsch_cfg.symbols;
  pdsch.dmrs           = pdsch_cfg.dmrs;
  pdsch.is_interleaved = dci_cfg.vrb_prb_mapping.has_value();
  // See TS38.213, 10.1.
  pdsch.ss_set_type = search_space_set_type::ue_specific;
  pdsch.dci_fmt     = dci_dl_format::f1_1;
  pdsch.harq_id     = to_harq_id(dci_cfg.harq_process_number);
  // See TS 38.211, 7.3.1.1. - Scrambling.
  pdsch.n_id =
      get_pdsch_n_id(cell_cfg.pci, active_bwp.dl_ded, dci_dl_format::f1_1, ss_info.cfg->is_common_search_space());
  pdsch.nof_layers = pdsch_cfg.nof_layers;

  // TODO: Add second Codeword when supported.
  // One Codeword.
  pdsch_codeword& cw = pdsch.codewords.emplace_back();
  cw.new_data        = h_dl.tb(0).nof_retxs == 0;
  cw.rv_index        = dci_cfg.tb1_redundancy_version;
  cw.mcs_index       = dci_cfg.tb1_modulation_coding_scheme;
  cw.mcs_table       = pdsch_cfg.mcs_table;
  cw.mcs_descr       = pdsch_mcs_get_config(pdsch_cfg.mcs_table, cw.mcs_index);
  cw.tb_size_bytes   = mcs_tbs_info.tbs;

  // Beamforming and precoding.
  pdsch.precoding = cs_mgr.get_precoding(pdsch_cfg.nof_layers, prbs);

  // Populate power offsets.
  if (not cell_cfg.nzp_csi_rs_list.empty()) {
    // [Implementation-defined] It is assumed that same powerControlOffset and powerControlOffsetSS is configured in
    // NZP-CSI-RS-Resource across all resources.
    pdsch.tx_pwr_info.pwr_ctrl_offset = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset;
    if (cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.has_value()) {
      pdsch.tx_pwr_info.pwr_ctrl_offset_ss = cell_cfg.nzp_csi_rs_list.front().pwr_ctrl_offset_ss_db.value();
    }
  }
}

void srsran::build_pusch_f0_0_tc_rnti(pusch_information&                   pusch,
                                      const pusch_config_params&           pusch_cfg,
                                      unsigned                             tbs_bytes,
                                      rnti_t                               rnti,
                                      const cell_configuration&            cell_cfg,
                                      const dci_0_0_tc_rnti_configuration& dci_cfg,
                                      const crb_interval&                  crbs,
                                      bool                                 is_new_data)
{ // TODO.
  pusch.intra_slot_freq_hopping = false;
  pusch.pusch_second_hop_prb    = 0;
  pusch.tx_direct_current_location =
      dc_offset_helper::pack(cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs);
  pusch.ul_freq_shift_7p5khz = false;
  pusch.dmrs_hopping_mode    = pusch_information::dmrs_hopping_mode::no_hopping;

  pusch.rnti = rnti;

  // PUSCH resources.
  pusch.bwp_cfg           = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  const prb_interval prbs = crb_to_prb(pusch.bwp_cfg->crbs, crbs);
  pusch.rbs               = vrb_interval{prbs.start(), prbs.stop()};
  pusch.symbols           = pusch_cfg.symbols;

  // MCS.
  pusch.mcs_table = pusch_cfg.mcs_table;
  pusch.mcs_index = dci_cfg.modulation_coding_scheme;
  pusch.mcs_descr = pusch_mcs_get_config(pusch.mcs_table, pusch.mcs_index, false);

  // TS 38.214, 6.1.3. - "transform precoding either 'enabled' or 'disabled' according to the higher layer configured
  // parameter msg3-transformPrecoder".
  pusch.transform_precoding = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->msg3_transform_precoder;
  // As per TS 38.211, Section 6.3.1.1, n_ID is set to Physical Cell ID for TC-RNTI.
  pusch.n_id          = cell_cfg.pci;
  pusch.nof_layers    = pusch_cfg.nof_layers;
  pusch.dmrs          = pusch_cfg.dmrs;
  pusch.pusch_dmrs_id = 0;
  pusch.rv_index      = dci_cfg.redundancy_version;
  // TS 38.321, 5.4.2.1 - "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is used".
  pusch.harq_id  = 0;
  pusch.new_data = is_new_data;

  pusch.tb_size_bytes = tbs_bytes;
  // Set number of CB to zero if no CBs are being used.
  pusch.num_cb = 0;
}

void srsran::build_pusch_f0_0_c_rnti(pusch_information&                  pusch,
                                     rnti_t                              rnti,
                                     const pusch_config_params&          pusch_cfg,
                                     unsigned                            tbs_bytes,
                                     const cell_configuration&           cell_cfg,
                                     const bwp_uplink_common&            ul_bwp,
                                     const dci_0_0_c_rnti_configuration& dci_cfg,
                                     const crb_interval&                 crbs,
                                     bool                                is_new_data)
{
  // TODO.
  pusch.intra_slot_freq_hopping = false;
  pusch.pusch_second_hop_prb    = 0;
  pusch.tx_direct_current_location =
      dc_offset_helper::pack(cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs);
  pusch.ul_freq_shift_7p5khz = false;
  pusch.dmrs_hopping_mode    = pusch_information::dmrs_hopping_mode::no_hopping;

  pusch.rnti = rnti;

  // PUSCH resources.
  pusch.bwp_cfg           = &ul_bwp.generic_params;
  const prb_interval prbs = crb_to_prb(ul_bwp.generic_params.crbs, crbs);
  pusch.rbs               = vrb_interval{prbs.start(), prbs.stop()};
  pusch.symbols           = pusch_cfg.symbols;

  // MCS.
  pusch.mcs_table = pusch_cfg.mcs_table;
  pusch.mcs_index = dci_cfg.modulation_coding_scheme;
  pusch.mcs_descr = pusch_mcs_get_config(pusch.mcs_table, pusch.mcs_index, pusch_cfg.tp_pi2bpsk_present);

  pusch.transform_precoding = cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->msg3_transform_precoder;
  pusch.n_id                = cell_cfg.pci;
  pusch.dmrs                = pusch_cfg.dmrs;
  pusch.pusch_dmrs_id       = cell_cfg.pci;

  // TBS.
  pusch.nof_layers    = pusch_cfg.nof_layers;
  pusch.tb_size_bytes = tbs_bytes;
  pusch.num_cb        = 0;

  // HARQ.
  pusch.rv_index = dci_cfg.redundancy_version;
  pusch.harq_id  = dci_cfg.harq_process_number;
  pusch.new_data = is_new_data;
}

void srsran::build_pusch_f0_1_c_rnti(pusch_information&           pusch,
                                     rnti_t                       rnti,
                                     const pusch_config_params&   pusch_cfg,
                                     sch_mcs_tbs                  mcs_tbs_info,
                                     const ue_cell_configuration& ue_cell_cfg,
                                     search_space_id              ss_id,
                                     const dci_0_1_configuration& dci_cfg,
                                     const crb_interval&          crbs,
                                     const ul_harq_process&       h_ul)
{
  const cell_configuration&           cell_cfg      = ue_cell_cfg.cell_cfg_common;
  const search_space_info&            ss_info       = ue_cell_cfg.search_space(ss_id);
  const bwp_info&                     bwp_info      = *ss_info.bwp;
  const bwp_uplink_dedicated*         bwp_ul_ded    = bwp_info.ul_ded;
  const bwp_uplink_common&            bwp_ul_cmn    = *bwp_info.ul_common;
  const optional<rach_config_common>& opt_rach_cfg  = bwp_ul_cmn.rach_cfg_common;
  const optional<pusch_config>&       pusch_cfg_ded = bwp_ul_ded->pusch_cfg;
  const prb_interval                  prbs          = crb_to_prb(bwp_ul_cmn.generic_params.crbs, crbs);

  // TODO: Populate based on config.
  pusch.intra_slot_freq_hopping = false;
  pusch.pusch_second_hop_prb    = 0;
  pusch.tx_direct_current_location =
      dc_offset_helper::pack(cell_cfg.expert_cfg.ue.initial_ul_dc_offset, cell_cfg.nof_ul_prbs);
  pusch.ul_freq_shift_7p5khz = false;
  pusch.dmrs_hopping_mode    = pusch_information::dmrs_hopping_mode::no_hopping;

  pusch.rnti = rnti;

  // PUSCH resources.
  pusch.bwp_cfg = &bwp_info.ul_common->generic_params;
  pusch.rbs     = vrb_interval{prbs.start(), prbs.stop()};
  pusch.symbols = pusch_cfg.symbols;

  // MCS.
  pusch.mcs_table = pusch_cfg.mcs_table;
  pusch.mcs_index = dci_cfg.modulation_coding_scheme;
  pusch.mcs_descr = pusch_mcs_get_config(pusch.mcs_table, pusch.mcs_index, pusch_cfg.tp_pi2bpsk_present);

  pusch.n_id = cell_cfg.pci;

  // Dedicated config overrides previously set value.
  if (pusch_cfg_ded.has_value()) {
    pusch.transform_precoding = false;
    if (pusch_cfg_ded.value().trans_precoder != pusch_config::transform_precoder::not_set) {
      pusch.transform_precoding = pusch_cfg_ded.value().trans_precoder == pusch_config::transform_precoder::enabled;
    } else if (opt_rach_cfg.has_value()) {
      pusch.transform_precoding = opt_rach_cfg.value().msg3_transform_precoder;
    }

    if (pusch_cfg_ded.value().data_scrambling_id_pusch.has_value()) {
      pusch.n_id = pusch_cfg_ded.value().data_scrambling_id_pusch.value();
    }
  }

  pusch.dmrs          = pusch_cfg.dmrs;
  pusch.pusch_dmrs_id = pusch_cfg.dmrs.dmrs_scrambling_id;

  // TBS.
  pusch.nof_layers    = pusch_cfg.nof_layers;
  pusch.tb_size_bytes = mcs_tbs_info.tbs;
  // TODO: Set based on CobeBook config.
  pusch.num_cb = 0;

  // HARQ.
  pusch.rv_index = dci_cfg.redundancy_version;
  pusch.harq_id  = dci_cfg.harq_process_number;
  pusch.new_data = h_ul.tb().nof_retxs == 0;
}
