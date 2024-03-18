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

#include "dci_builder.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/pdcch/dci_packing.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/pdsch/pdsch_antenna_ports_mapping.h"
#include "srsran/ran/pusch/pusch_configuration.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include <algorithm>

using namespace srsran;

/// Get PDSCH-to-HARQ-timing-indicator field as per TS38.213, 9.2.3.
static unsigned get_dci_1_0_pdsch_to_harq_timing_indicator(unsigned k1)
{
  // PDSCH-to-HARQ-timing-indicator maps to {1,2,3,4,5,6,7,8} for DCI 1_0.
  srsran_assert(k1 <= 8, "Invalid k1 value");
  return k1 - 1;
}

/// Get PDSCH-to-HARQ-timing-indicator field as per TS38.213, 9.2.3.
static unsigned get_dci_1_1_pdsch_to_harq_timing_indicator(unsigned k1, span<const uint8_t> dl_data_to_ul_ack)
{
  // PDSCH-to-HARQ-timing-indicator maps to dl-DataToUL-ACK for DCI 1_1.
  const auto* it = std::find_if(
      dl_data_to_ul_ack.begin(), dl_data_to_ul_ack.end(), [k1](const auto& k1_cfg) { return k1 == k1_cfg; });
  srsran_assert(it != dl_data_to_ul_ack.end(), "Invalid k1 value");
  return it - dl_data_to_ul_ack.begin();
}

void srsran::build_dci_f1_0_si_rnti(dci_dl_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    crb_interval               crbs,
                                    unsigned                   time_resource,
                                    sch_mcs_index              mcs_index,
                                    uint8_t                    si_indicator)
{
  dci.type                              = dci_dl_rnti_config_type::si_f1_0;
  dci.si_f1_0                           = {};
  dci_1_0_si_rnti_configuration& si_dci = dci.si_f1_0;
  // as per TS38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB.
  si_dci.N_rb_dl_bwp = init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length();
  const vrb_interval si_vrbs =
      crb_to_vrb_f1_0_common_ss_non_interleaved(crbs, init_dl_bwp.pdcch_common.coreset0->get_coreset_start_crb());
  si_dci.frequency_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{si_dci.N_rb_dl_bwp, si_vrbs.start(), si_vrbs.length()});
  // As per Table 7.3.1.2.2-5, TS 38.212, 0 = non-interleaved, 1 = interleaved.
  // TODO: Verify if interleaved is suitable for SIB1.
  si_dci.vrb_to_prb_mapping       = 0;
  si_dci.time_resource            = time_resource;
  si_dci.modulation_coding_scheme = mcs_index.to_uint();
  // Redundancy version for first transmission.
  // TODO: Check what is the best RV for SIB1.
  si_dci.redundancy_version           = 0;
  si_dci.system_information_indicator = si_indicator;
}

void srsran::build_dci_f1_0_p_rnti(dci_dl_info&               dci,
                                   const bwp_downlink_common& init_dl_bwp,
                                   crb_interval               crbs,
                                   unsigned                   time_resource,
                                   sch_mcs_index              mcs_index)
{
  dci.type        = dci_dl_rnti_config_type::p_rnti_f1_0;
  dci.p_rnti_f1_0 = {};
  auto& p_dci     = dci.p_rnti_f1_0;
  // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB for P-RNTI.
  p_dci.N_rb_dl_bwp = init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length();
  const vrb_interval paging_vrbs =
      crb_to_vrb_f1_0_common_ss_non_interleaved(crbs, init_dl_bwp.pdcch_common.coreset0->get_coreset_start_crb());
  p_dci.frequency_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{p_dci.N_rb_dl_bwp, paging_vrbs.start(), paging_vrbs.length()});
  p_dci.time_resource = time_resource;
  // As per Table 7.3.1.2.2-5, TS 38.212, 0 = non-interleaved, 1 = interleaved.
  // TODO: Verify if interleaved is suitable for Paging.
  p_dci.vrb_to_prb_mapping       = 0;
  p_dci.short_messages_indicator = dci_1_0_p_rnti_configuration::payload_info::scheduling_information;
  p_dci.modulation_coding_scheme = mcs_index.to_uint();
}

void srsran::build_dci_f1_0_ra_rnti(dci_dl_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    crb_interval               crbs,
                                    unsigned                   time_resource,
                                    sch_mcs_index              mcs_index)
{
  dci.type                              = dci_dl_rnti_config_type::ra_f1_0;
  dci.ra_f1_0                           = {};
  dci_1_0_ra_rnti_configuration& ra_dci = dci.ra_f1_0;
  // as per TS38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB.
  ra_dci.N_rb_dl_bwp = init_dl_bwp.pdcch_common.coreset0.has_value()
                           ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()
                           : init_dl_bwp.generic_params.crbs.length();
  const vrb_interval rar_vrbs =
      crb_to_vrb_f1_0_common_ss_non_interleaved(crbs, init_dl_bwp.pdcch_common.coreset0->get_coreset_start_crb());
  ra_dci.frequency_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{ra_dci.N_rb_dl_bwp, rar_vrbs.start(), rar_vrbs.length()});
  ra_dci.vrb_to_prb_mapping       = 0;
  ra_dci.time_resource            = time_resource;
  ra_dci.modulation_coding_scheme = mcs_index.to_uint();
  ra_dci.tb_scaling               = 0; // TODO.
}

void srsran::build_dci_f1_0_tc_rnti(dci_dl_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    crb_interval               crbs,
                                    unsigned                   time_resource,
                                    unsigned                   k1,
                                    unsigned                   pucch_res_indicator,
                                    sch_mcs_index              mcs_index,
                                    uint8_t                    rv,
                                    const dl_harq_process&     h_dl)
{
  static constexpr unsigned tb_idx = 0;

  dci.type                            = dci_dl_rnti_config_type::tc_rnti_f1_0;
  dci.tc_rnti_f1_0                    = {};
  dci_1_0_tc_rnti_configuration& f1_0 = dci.tc_rnti_f1_0;

  f1_0.tpc_command        = 1;
  f1_0.vrb_to_prb_mapping = 0;

  // PDSCH resources.
  // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB is the size of CORESET 0 for TC-RNTI.
  const crb_interval cs0_crbs = init_dl_bwp.pdcch_common.coreset0->coreset0_crbs();
  f1_0.N_rb_dl_bwp            = cs0_crbs.length();
  // See TS38.211 7.3.1.6 - Mapping from VRBs to PRBs.
  const vrb_interval vrbs = crb_to_vrb_f1_0_common_ss_non_interleaved(crbs, cs0_crbs.start());
  f1_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{f1_0.N_rb_dl_bwp, vrbs.start(), vrbs.length()});
  f1_0.time_resource = time_resource;

  // UCI resources.
  f1_0.pucch_resource_indicator       = pucch_res_indicator;
  f1_0.pdsch_harq_fb_timing_indicator = get_dci_1_0_pdsch_to_harq_timing_indicator(k1);

  f1_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f1_0.harq_process_number = h_dl.id;
  f1_0.new_data_indicator  = h_dl.tb(tb_idx).ndi;
  f1_0.redundancy_version  = rv;
}

void srsran::build_dci_f1_0_c_rnti(dci_dl_info&               dci,
                                   const search_space_info&   ss_info,
                                   const bwp_downlink_common& init_dl_bwp,
                                   crb_interval               crbs,
                                   unsigned                   time_resource,
                                   unsigned                   k1,
                                   unsigned                   pucch_res_indicator,
                                   unsigned                   dai,
                                   sch_mcs_index              mcs_index,
                                   uint8_t                    rv,
                                   const dl_harq_process&     h_dl)
{
  const coreset_configuration& cs_cfg            = *ss_info.coreset;
  const bwp_downlink_common&   active_dl_bwp_cmn = *ss_info.bwp->dl_common;
  const bwp_configuration&     active_dl_bwp     = active_dl_bwp_cmn.generic_params;

  dci.type                           = dci_dl_rnti_config_type::c_rnti_f1_0;
  dci.c_rnti_f1_0                    = {};
  dci_1_0_c_rnti_configuration& f1_0 = dci.c_rnti_f1_0;

  f1_0.tpc_command        = 1;
  f1_0.vrb_to_prb_mapping = 0;

  // PDSCH resources.
  // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB for C-RNTI.
  unsigned N_rb_dl_bwp = active_dl_bwp.crbs.length();
  if (ss_info.cfg->is_common_search_space() and init_dl_bwp.pdcch_common.coreset0.has_value()) {
    N_rb_dl_bwp = init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length();
  }
  // See TS38.211 7.3.1.6 - Mapping from VRBs to PRBs.
  const vrb_interval vrbs = rb_helper::crb_to_vrb_dl_non_interleaved(crbs,
                                                                     active_dl_bwp.crbs.start(),
                                                                     cs_cfg.get_coreset_start_crb(),
                                                                     dci_dl_format::f1_0,
                                                                     ss_info.cfg->is_common_search_space());
  f1_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{N_rb_dl_bwp, vrbs.start(), vrbs.length()});
  f1_0.time_resource = time_resource;

  const dci_sizes& dci_sz = ss_info.dci_sz;

  // Compute DCI size.
  f1_0.payload_size =
      not ss_info.cfg->is_common_search_space() ? dci_sz.format1_0_ue_size.value() : dci_sz.format1_0_common_size;

  // UCI resources.
  f1_0.pucch_resource_indicator       = pucch_res_indicator;
  f1_0.pdsch_harq_fb_timing_indicator = get_dci_1_0_pdsch_to_harq_timing_indicator(k1);
  f1_0.dl_assignment_index            = dai;

  f1_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f1_0.harq_process_number = h_dl.id;
  f1_0.new_data_indicator  = h_dl.tb(0).ndi;
  f1_0.redundancy_version  = rv;
}

void srsran::build_dci_f1_1_c_rnti(dci_dl_info&                 dci,
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
                                   unsigned                     nof_layers)
{
  const search_space_info& ss_info = ue_cell_cfg.search_space(ss_id);
  srsran_assert(not ss_info.cfg->is_common_search_space(), "SearchSpace must be of type UE-Specific SearchSpace");

  // TODO: Update the value based on nof. CWs enabled.
  static const bool are_both_cws_enabled = false;

  const bwp_downlink_common& active_dl_bwp_cmn = *ss_info.bwp->dl_common;
  const bwp_configuration&   active_dl_bwp     = active_dl_bwp_cmn.generic_params;
  const auto                 k1_candidates     = ss_info.get_k1_candidates();
  const auto&                opt_pdsch_cfg     = ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg;

  dci.type                    = dci_dl_rnti_config_type::c_rnti_f1_1;
  dci.c_rnti_f1_1             = {};
  dci_1_1_configuration& f1_1 = dci.c_rnti_f1_1;

  f1_1.tpc_command             = 1;
  f1_1.srs_request             = 0;
  f1_1.dmrs_seq_initialization = 0;
  srsran_assert(ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg->pdsch_mapping_type_a_dmrs.has_value(),
                "No DMRS configured in PDSCH configuration");
  const auto& dmrs_cfg = ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg->pdsch_mapping_type_a_dmrs.value();
  f1_1.antenna_ports   = get_pdsch_antenna_port_mapping_row_index(
      nof_layers,
      ue_cell_cfg.cell_cfg_common.dl_carrier.nof_ant,
      dmrs_cfg.is_dmrs_type2 ? dmrs_config_type::type2 : dmrs_config_type::type1,
      dmrs_cfg.is_max_length_len2 ? dmrs_max_length::len2 : dmrs_max_length::len1,
      are_both_cws_enabled);

  // See 38.212, clause 7.3.1.2.2 - N^{DL,BWP}_RB for C-RNTI.
  f1_1.frequency_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{active_dl_bwp.crbs.length(), prbs.start(), prbs.length()});
  f1_1.time_resource = time_resource;

  const dci_size_config& dci_sz_cfg = ss_info.dci_sz_cfg;
  const dci_sizes&       dci_sz     = ss_info.dci_sz;

  // Compute DCI size.
  f1_1.payload_size = dci_sz.format1_1_ue_size.value();

  if (dci_sz_cfg.interleaved_vrb_prb_mapping.has_value() and dci_sz_cfg.interleaved_vrb_prb_mapping.value()) {
    if (opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value()) {
      f1_1.vrb_prb_mapping = static_cast<unsigned>(opt_pdsch_cfg.value().vrb_to_prb_itlvr.value());
    }
  }

  // TODO:
  //  1. When Dynamic PRB bundling is used, set prb_bundling_size_indicator.
  //  2. When Maximum number of code-block-groups (CBGs) is configured, set cbg_transmission_info.
  //  3. When DMRS configuration is set in PDSCH-Config, set dmrs_seq_initialization based on whether scramblingID0 or
  //  scramblingID1 to be used.

  // UCI resources.
  f1_1.pucch_resource_indicator       = pucch_res_indicator;
  f1_1.pdsch_harq_fb_timing_indicator = get_dci_1_1_pdsch_to_harq_timing_indicator(k1, k1_candidates);
  f1_1.downlink_assignment_index      = dai;

  f1_1.tb1_modulation_coding_scheme = tb1_mcs_index.to_uint();

  // HARQ params.
  f1_1.harq_process_number    = h_dl.id;
  f1_1.tb1_new_data_indicator = h_dl.tb(0).ndi;
  f1_1.tb1_redundancy_version = rv;
}

void srsran::build_dci_f0_0_tc_rnti(dci_ul_info&               dci,
                                    const bwp_downlink_common& init_dl_bwp,
                                    const bwp_configuration&   ul_bwp,
                                    const crb_interval&        crbs,
                                    unsigned                   time_resource,
                                    sch_mcs_index              mcs_index,
                                    uint8_t                    rv,
                                    const ul_harq_process&     h_ul)
{
  // See TS38.321, 5.4.2.1 - "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is used."
  srsran_assert(h_ul.id == 0, "UL HARQ process used for Msg3 must have id=0");

  dci.type                            = dci_ul_rnti_config_type::tc_rnti_f0_0;
  dci.tc_rnti_f0_0                    = {};
  dci_0_0_tc_rnti_configuration& f0_0 = dci.tc_rnti_f0_0;

  // TODO.
  f0_0.N_ul_hop               = 0;
  f0_0.hopping_offset         = 0;
  f0_0.frequency_hopping_flag = 0;
  f0_0.tpc_command            = 1;

  // PUSCH params.
  const auto dci_sz_cfg = dci_size_config{
      false,
      false,
      init_dl_bwp.generic_params.crbs.length(),
      init_dl_bwp.generic_params.crbs.length(),
      ul_bwp.crbs.length(),
      ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0,
      false};
  srsran_assert(validate_dci_size_config(dci_sz_cfg), "Invalid DCI size configuration for DCI Format 0_0 (TC-RNTI)");
  dci_sizes dci_sz        = get_dci_sizes(dci_sz_cfg);
  f0_0.payload_size       = dci_sz.format0_0_common_size;
  const vrb_interval vrbs = rb_helper::crb_to_vrb_ul_non_interleaved(crbs, ul_bwp.crbs.start());
  f0_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{ul_bwp.crbs.length(), vrbs.start(), vrbs.length()});
  f0_0.time_resource = time_resource;

  f0_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_0.redundancy_version = rv;
}

void srsran::build_dci_f0_0_c_rnti(dci_ul_info&                 dci,
                                   const ue_cell_configuration& ue_cell_cfg,
                                   search_space_id              ss_id,
                                   const crb_interval&          crbs,
                                   unsigned                     time_resource,
                                   sch_mcs_index                mcs_index,
                                   uint8_t                      rv,
                                   const ul_harq_process&       h_ul)
{
  const search_space_info& ss_info       = ue_cell_cfg.search_space(ss_id);
  const bwp_configuration& active_ul_bwp = ss_info.bwp->ul_common->generic_params;
  const bwp_uplink_common& init_ul_bwp   = *ue_cell_cfg.bwp(to_bwp_id(0)).ul_common;

  dci.type                           = dci_ul_rnti_config_type::c_rnti_f0_0;
  dci.c_rnti_f0_0                    = {};
  dci_0_0_c_rnti_configuration& f0_0 = dci.c_rnti_f0_0;

  // TODO.
  f0_0.N_ul_hop               = 0;
  f0_0.hopping_offset         = 0;
  f0_0.frequency_hopping_flag = 0;
  f0_0.tpc_command            = 1;
  f0_0.ul_sul_indicator       = {};

  // PDCCH params.
  const dci_sizes& dci_sz = ss_info.dci_sz;

  f0_0.payload_size =
      not ss_info.cfg->is_common_search_space() ? dci_sz.format0_0_ue_size.value() : dci_sz.format0_0_common_size;

  // PUSCH resources.
  // See 38.212, clause 7.3.1.1.1 - N^{UL,BWP}_RB for C-RNTI.
  const unsigned N_ul_bwp_rb =
      ss_info.cfg->is_common_search_space() ? init_ul_bwp.generic_params.crbs.length() : active_ul_bwp.crbs.length();
  const vrb_interval vrbs = rb_helper::crb_to_vrb_ul_non_interleaved(crbs, active_ul_bwp.crbs.start());
  f0_0.frequency_resource =
      ra_frequency_type1_get_riv(ra_frequency_type1_configuration{N_ul_bwp_rb, vrbs.start(), vrbs.length()});

  f0_0.time_resource = time_resource;

  f0_0.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_0.harq_process_number = h_ul.id;
  f0_0.new_data_indicator  = h_ul.tb().ndi;
  f0_0.redundancy_version  = rv;
}

void srsran::build_dci_f0_1_c_rnti(dci_ul_info&                 dci,
                                   const ue_cell_configuration& ue_cell_cfg,
                                   search_space_id              ss_id,
                                   const crb_interval&          crbs,
                                   unsigned                     time_resource,
                                   sch_mcs_index                mcs_index,
                                   uint8_t                      rv,
                                   const ul_harq_process&       h_ul,
                                   unsigned                     dai,
                                   unsigned                     nof_layers)
{
  const search_space_info& ss_info = ue_cell_cfg.search_space(ss_id);
  srsran_assert(not ss_info.cfg->is_common_search_space(), "SearchSpace must be of type UE-Specific SearchSpace");

  const bwp_configuration& active_ul_bwp = ss_info.bwp->ul_common->generic_params;

  dci.type                    = dci_ul_rnti_config_type::c_rnti_f0_1;
  dci.c_rnti_f0_1             = {};
  dci_0_1_configuration& f0_1 = dci.c_rnti_f0_1;

  const dci_sizes& dci_sz = ss_info.dci_sz;

  f0_1.payload_size = dci_sz.format0_1_ue_size.value();

  f0_1.ul_sul_indicator        = {};
  f0_1.tpc_command             = 1;
  f0_1.srs_request             = 0;
  f0_1.dmrs_seq_initialization = 0;
  // TODO: Set proper value based on nof. layers used. See TS 38.212, clause 7.3.1.1.2.
  // PHY does not support nof. DMRS CDM groups(s) without data other than 2, hence selected antenna port value from
  // Table 7.3.1.1.2-8 in TS 38.212 based on assumption of max. rank 1 and DMRS max. length 1.
  f0_1.antenna_ports = 2;

  // See 38.212, clause 7.3.1.1.2 - N^{UL,BWP}_RB for C-RNTI.
  const vrb_interval vrbs = rb_helper::crb_to_vrb_ul_non_interleaved(crbs, active_ul_bwp.crbs.start());
  f0_1.frequency_resource = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{active_ul_bwp.crbs.length(), vrbs.start(), vrbs.length()});
  f0_1.time_resource = time_resource;

  f0_1.modulation_coding_scheme = mcs_index.to_uint();

  // HARQ params.
  f0_1.harq_process_number = h_ul.id;
  f0_1.new_data_indicator  = h_ul.tb().ndi;
  f0_1.redundancy_version  = rv;

  // TODO: Set values for -
  //  - srs_resource_indicator
  //  - precoding_info_nof_layers
  //  - beta_offset_indicator
  //  - ptrs_dmrs_association
  //  - cbg_transmission_info

  // DAI is set based on TS 38.213, clause 9.1.3.2.
  f0_1.first_dl_assignment_index = dai;
  f0_1.ul_sch_indicator          = 1;
}
