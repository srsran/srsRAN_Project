/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_sch_pdu_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/tbs_calculator.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"

using namespace srsgnb;

static prb_interval get_prb_interval(unsigned N_rb_bwp, unsigned frequency_resource)
{
  ra_frequency_type1_configuration ra_freq_cfg = ra_frequency_type1_from_riv(N_rb_bwp, frequency_resource);
  return prb_interval{ra_freq_cfg.start_vrb, ra_freq_cfg.start_vrb + ra_freq_cfg.length_vrb};
}

// See TS 38.211, 7.3.1.1. - Scrambling.
static unsigned get_pdsch_n_id(pci_t                              pci,
                               const bwp_downlink_dedicated*      bwp_dl_ded,
                               dci_dl_format                      dci_fmt,
                               search_space_configuration::type_t ss_type)
{
  if (bwp_dl_ded != nullptr and bwp_dl_ded->pdsch_cfg.has_value() and
      bwp_dl_ded->pdsch_cfg->data_scrambling_id_pdsch.has_value() and
      (dci_fmt != dci_dl_format::f1_0 or ss_type != search_space_configuration::type_t::common)) {
    return *bwp_dl_ded->pdsch_cfg->data_scrambling_id_pdsch;
  }
  return pci;
}

pdsch_config_params srsgnb::get_pdsch_config_f1_0_tc_rnti(const cell_configuration& cell_cfg, unsigned time_resource)
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

  pdsch.dmrs = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  pdsch.nof_oh_prb       = nof_oh_prb;
  pdsch.symbols          = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].symbols;
  pdsch.mcs_table        = mcs_table;
  pdsch.tb_scaling_field = tb_scaling_field;
  pdsch.nof_layers       = nof_layers;

  return pdsch;
}

pdsch_config_params srsgnb::get_pdsch_config_f1_0_c_rnti(const cell_configuration&    cell_cfg,
                                                         const ue_cell_configuration& ue_cell_cfg,
                                                         unsigned                     time_resource)
{
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned tb_scaling_field = 0;
  static constexpr unsigned nof_layers       = 1;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  pdsch.nof_oh_prb = ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg.has_value()
                         ? static_cast<unsigned>(ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg.value().x_ov_head)
                         : static_cast<unsigned>(x_overhead::not_set);

  pdsch.symbols          = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].symbols;
  pdsch.mcs_table        = ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg->mcs_table;
  pdsch.tb_scaling_field = tb_scaling_field;
  pdsch.nof_layers       = nof_layers;

  return pdsch;
}

pusch_config_params srsgnb::get_pusch_config_f0_0_tc_rnti(const cell_configuration& cell_cfg, unsigned time_resource)
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
  // TODO verify if this is the correct value.
  constexpr unsigned nof_harq_ack_bits = 2;
  // There is no need for CSI reporting for TC-RNTI.
  constexpr unsigned nof_csi_part1_bits = 0;
  constexpr unsigned nof_csi_part2_bits = 0;

  pusch_config_params pusch;

  // TODO: Use UE-dedicated DMRS info if needed.
  pusch.dmrs = make_dmrs_info_common(
      *cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  pusch.symbols = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[time_resource].symbols;

  pusch.mcs_table          = mcs_table;
  pusch.nof_layers         = nof_layers;
  pusch.tp_pi2bpsk_present = tp_pi2bpsk_present;
  pusch.tb_scaling_field   = tb_scaling_field;

  pusch.nof_oh_prb = nof_oh_prb;

  // TODO: verify if this needs to be set depending on some configuration.
  pusch.nof_harq_ack_bits  = nof_harq_ack_bits;
  pusch.nof_csi_part1_bits = nof_csi_part1_bits;
  pusch.nof_csi_part2_bits = nof_csi_part2_bits;

  return pusch;
}

pusch_config_params srsgnb::get_pusch_config_f0_0_c_rnti(const cell_configuration&    cell_cfg,
                                                         const ue_cell_configuration& ue_cell_cfg,
                                                         const bwp_uplink_common&     ul_bwp,
                                                         unsigned                     time_resource)
{
  const pusch_mcs_table mcs_table  = ue_cell_cfg.cfg_dedicated().ul_config->init_ul_bwp.pusch_cfg->mcs_table;
  constexpr unsigned    nof_layers = 1;
  // As per TS 38.214, Section 5.1.3.2 and 6.1.4.2, and TS 38.212, Section 7.3.1.1 and 7.3.1.2, TB scaling filed is only
  // used for DCI Format 1-0 (in the DL). Therefore, for the PUSCH this is set to 0.
  constexpr unsigned tb_scaling_field = 0;
  // Parameter \c tp-pi2BPSK enabled is not supported yet.
  constexpr bool tp_pi2bpsk_present = false;
  // We set 6 bits, assuming a maximum of 1 HARQ-ACK per slot and maximum number of slot corresponding to the number of
  // DL slots in TDD, currently 6.
  // TODO verify if this is the correct value.
  constexpr unsigned nof_harq_ack_bits = 6;
  // We assume only 4 bits for CSI part-1.
  // TODO set this to 4 when PHY supports it.
  constexpr unsigned nof_csi_part1_bits = 0;
  constexpr unsigned nof_csi_part2_bits = 0;

  pusch_config_params pusch;

  pusch.dmrs = make_dmrs_info_common(
      *cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  pusch.symbols = ul_bwp.pusch_cfg_common->pusch_td_alloc_list[time_resource].symbols;

  pusch.mcs_table          = mcs_table;
  pusch.nof_layers         = nof_layers;
  pusch.tp_pi2bpsk_present = tp_pi2bpsk_present;
  pusch.tb_scaling_field   = tb_scaling_field;

  // According to TS 38.214, Section 6.1.4.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  // NOTE: x_overhead::not_set is mapped to 0.
  pusch.nof_oh_prb = ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg.has_value()
                         ? static_cast<unsigned>(ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg.value().x_ov_head)
                         : static_cast<unsigned>(x_overhead::not_set);

  // TODO: verify if this needs to be set depending on some configuration.
  pusch.nof_harq_ack_bits  = nof_harq_ack_bits;
  pusch.nof_csi_part1_bits = nof_csi_part1_bits;
  pusch.nof_csi_part2_bits = nof_csi_part2_bits;

  return pusch;
}

void srsgnb::build_pdsch_f1_0_tc_rnti(pdsch_information&                   pdsch,
                                      const pdsch_config_params&           pdsch_cfg,
                                      unsigned                             tbs_bytes,
                                      rnti_t                               rnti,
                                      const cell_configuration&            cell_cfg,
                                      const dci_1_0_tc_rnti_configuration& dci_cfg,
                                      bool                                 is_new_data)
{
  pdsch.rnti = rnti;

  // PDSCH resources.
  pdsch.bwp_cfg = &cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  // See 3GPP TS 38.213, clause 10.1 - CSS set configured by ra-SearchSpace is used for CRC scrambled by TC-RNTI.
  const search_space_configuration& ss =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
          .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id];
  if (ss.cs_id == to_coreset_id(0)) {
    pdsch.coreset_cfg = &*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0;
  } else {
    pdsch.coreset_cfg = &*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset;
  }
  pdsch.prbs    = get_prb_interval(dci_cfg.N_rb_dl_bwp, dci_cfg.frequency_resource);
  pdsch.symbols = pdsch_cfg.symbols;

  // TODO: Use UE-dedicated DMRS info if needed.
  pdsch.dmrs = pdsch_cfg.dmrs;
  // See TS 38.211, 7.3.1.1. - Scrambling.
  pdsch.n_id           = cell_cfg.pci;
  pdsch.is_interleaved = dci_cfg.vrb_to_prb_mapping > 0;
  // See TS38.213, 10.1. - Type1-PDCCH CSS set for CRC scrambled by a TC-RNTI on the PCell.
  pdsch.ss_set_type = search_space_set_type::type1;
  pdsch.dci_fmt     = dci_dl_format::f1_0;
  pdsch.harq_id     = to_harq_id(dci_cfg.harq_process_number);

  // One Codeword.
  pdsch_codeword& cw = pdsch.codewords.emplace_back();
  cw.new_data        = is_new_data;
  cw.rv_index        = dci_cfg.redundancy_version;
  cw.mcs_index       = dci_cfg.modulation_coding_scheme;
  cw.mcs_table       = pdsch_cfg.mcs_table;
  cw.mcs_descr       = pdsch_mcs_get_config(pdsch_cfg.mcs_table, cw.mcs_index);
  cw.tb_size_bytes   = tbs_bytes;
}

void srsgnb::build_pdsch_f1_0_c_rnti(pdsch_information&                  pdsch,
                                     const pdsch_config_params&          pdsch_cfg,
                                     unsigned                            tbs_bytes,
                                     rnti_t                              rnti,
                                     const ue_cell_configuration&        ue_cell_cfg,
                                     bwp_id_t                            active_bwp_id,
                                     const search_space_configuration&   ss_cfg,
                                     const dci_1_0_c_rnti_configuration& dci_cfg,
                                     bool                                is_new_data)
{
  const cell_configuration&    cell_cfg = ue_cell_cfg.cell_cfg_common;
  const coreset_configuration& cs_cfg   = ue_cell_cfg.coreset(ss_cfg.cs_id);

  pdsch.rnti        = rnti;
  pdsch.bwp_cfg     = &ue_cell_cfg.find_dl_bwp_common(active_bwp_id)->generic_params;
  pdsch.coreset_cfg = &cs_cfg;

  pdsch.prbs    = get_prb_interval(dci_cfg.N_rb_dl_bwp, dci_cfg.frequency_resource);
  pdsch.symbols = pdsch_cfg.symbols;
  // TODO: Use UE-dedicated DMRS info if needed.
  pdsch.dmrs           = pdsch_cfg.dmrs;
  pdsch.is_interleaved = dci_cfg.vrb_to_prb_mapping > 0;
  // See TS38.213, 10.1.
  pdsch.ss_set_type = ss_cfg.type == search_space_configuration::type_t::ue_dedicated
                          ? search_space_set_type::ue_specific
                          : search_space_set_type::type3;
  pdsch.dci_fmt     = dci_dl_format::f1_0;
  pdsch.harq_id     = to_harq_id(dci_cfg.harq_process_number);
  // See TS 38.211, 7.3.1.1. - Scrambling.
  const bwp_downlink_dedicated* bwp_dl_ded = ue_cell_cfg.find_dl_bwp_ded(active_bwp_id);
  pdsch.n_id                               = get_pdsch_n_id(cell_cfg.pci, bwp_dl_ded, dci_dl_format::f1_0, ss_cfg.type);

  // One Codeword.
  pdsch_codeword& cw = pdsch.codewords.emplace_back();
  cw.new_data        = is_new_data;
  cw.rv_index        = dci_cfg.redundancy_version;
  cw.mcs_index       = dci_cfg.modulation_coding_scheme;
  cw.mcs_table       = pdsch_cfg.mcs_table;
  cw.mcs_descr       = pdsch_mcs_get_config(pdsch_cfg.mcs_table, cw.mcs_index);
  cw.tb_size_bytes   = tbs_bytes;
}

void srsgnb::build_pusch_f0_0_tc_rnti(pusch_information&                   pusch,
                                      const pusch_config_params&           pusch_cfg,
                                      unsigned                             tbs_bytes,
                                      rnti_t                               rnti,
                                      const cell_configuration&            cell_cfg,
                                      const dci_0_0_tc_rnti_configuration& dci_cfg,
                                      bool                                 is_new_data)
{ // TODO.
  pusch.intra_slot_freq_hopping    = false;
  pusch.pusch_second_hop_prb       = 0;
  pusch.tx_direct_current_location = 0;
  pusch.ul_freq_shift_7p5khz       = false;
  pusch.dmrs_hopping_mode          = pusch_information::dmrs_hopping_mode::no_hopping;

  pusch.rnti = rnti;

  // PUSCH resources.
  pusch.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pusch.prbs    = get_prb_interval(dci_cfg.N_rb_ul_bwp, dci_cfg.frequency_resource);
  pusch.symbols =
      cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[dci_cfg.time_resource].symbols;

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

void srsgnb::build_pusch_f0_0_c_rnti(pusch_information&                  pusch,
                                     rnti_t                              rnti,
                                     const pusch_config_params&          pusch_cfg,
                                     unsigned                            tbs_bytes,
                                     const cell_configuration&           cell_cfg,
                                     const bwp_uplink_common&            ul_bwp,
                                     const dci_0_0_c_rnti_configuration& dci_cfg,
                                     bool                                is_new_data)
{
  // TODO.
  pusch.intra_slot_freq_hopping    = false;
  pusch.pusch_second_hop_prb       = 0;
  pusch.tx_direct_current_location = 0;
  pusch.ul_freq_shift_7p5khz       = false;
  pusch.dmrs_hopping_mode          = pusch_information::dmrs_hopping_mode::no_hopping;

  pusch.rnti = rnti;

  // PUSCH resources.
  pusch.bwp_cfg = &ul_bwp.generic_params;
  pusch.prbs    = get_prb_interval(dci_cfg.N_rb_ul_bwp, dci_cfg.frequency_resource);
  pusch.symbols = ul_bwp.pusch_cfg_common->pusch_td_alloc_list[dci_cfg.time_resource].symbols;

  // MCS.
  pusch.mcs_table                = pusch_cfg.mcs_table;
  pusch.mcs_index                = dci_cfg.modulation_coding_scheme;
  sch_mcs_description mcs_config = pusch_mcs_get_config(pusch.mcs_table, pusch.mcs_index, pusch_cfg.tp_pi2bpsk_present);
  pusch.mcs_descr                = mcs_config;

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
