/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "helpers.h"
#include <random>

using namespace srsran;
using namespace unittests;

static std::mt19937 gen(0);

static unsigned generate_sfn()
{
  std::uniform_int_distribution<unsigned> dist(0, 1023);
  return dist(gen);
}

static unsigned generate_slot()
{
  std::uniform_int_distribution<unsigned> dist(0, 159);

  return dist(gen);
}

static unsigned generate_nid_pdsch()
{
  std::uniform_int_distribution<unsigned> dist(0, 1023);

  return dist(gen);
}

static pci_t generate_pci()
{
  std::uniform_int_distribution<unsigned> dist(0, 1007);

  return dist(gen);
}

static unsigned generate_binary()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);

  return dist(gen);
}

static unsigned generate_block_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 63);

  return dist(gen);
}

static unsigned generate_subcarrier_offset()
{
  std::uniform_int_distribution<unsigned> dist(0, 23);

  return dist(gen);
}

static unsigned generate_offset_point_A()
{
  std::uniform_int_distribution<unsigned> dist(0, 2199);

  return dist(gen);
}

static unsigned generate_byte()
{
  std::uniform_int_distribution<unsigned> dist(0, 255);

  return dist(gen);
}

static unsigned generate_case_pattern()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);

  return dist(gen);
}

static bool generate_bool()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return dist(gen);
}

dl_ssb_pdu unittests::build_valid_dl_ssb_pdu()
{
  dl_ssb_pdu pdu;

  pdu.pci                             = generate_pci();
  pdu.pss_to_sss_epre                 = static_cast<srsran::ssb_pss_to_sss_epre>(generate_binary());
  pdu.ssb_index                       = generate_block_index();
  pdu.subcarrier_offset               = generate_subcarrier_offset();
  pdu.offset_to_pointA                = generate_offset_point_A();
  pdu.ssb_case                        = static_cast<ssb_pattern_case>(generate_case_pattern());
  pdu.L_max                           = 4;
  pdu.scs                             = subcarrier_spacing::kHz240;
  pdu.mib_data.dmrs_typeA_pos         = generate_bool() ? dmrs_typeA_position::pos2 : dmrs_typeA_position::pos3;
  pdu.mib_data.pdcch_config_sib1      = generate_byte();
  pdu.mib_data.cell_barred            = generate_bool();
  pdu.mib_data.intra_freq_reselection = generate_bool();

  return pdu;
}

static dci_payload generate_dci_payload()
{
  dci_payload payload;
  for (unsigned i = 0, e = payload.max_size(); i != e; ++i) {
    payload.push_back(generate_bool());
  }

  return payload;
}

static crb_interval generate_crb_interval()
{
  unsigned init = generate_block_index();
  return {init, init + generate_subcarrier_offset()};
}

static bwp_configuration generate_bwp_configuration()
{
  bwp_configuration config;

  config.cp   = generate_bool() ? cyclic_prefix::NORMAL : cyclic_prefix::EXTENDED;
  config.scs  = subcarrier_spacing::kHz240;
  config.crbs = generate_crb_interval();

  return config;
}

static unsigned generate_duration()
{
  std::uniform_int_distribution<unsigned> dist(1, 3);
  return dist(gen);
}

static unsigned generate_reg_bundle_sz()
{
  static const std::vector<unsigned>      values = {2, 3, 6};
  std::uniform_int_distribution<unsigned> dist(0, 2);
  return values[dist(gen)];
}

static coreset_configuration generate_coreset_configuration()
{
  coreset_configuration config;

  config.id                   = to_coreset_id(0);
  config.duration             = generate_duration();
  config.precoder_granurality = static_cast<coreset_configuration::precoder_granularity_type>(generate_binary());
  if (generate_bool()) {
    config.pdcch_dmrs_scrambling_id = 10;
    config.interleaved.emplace(coreset_configuration::interleaved_mapping_type{6, generate_reg_bundle_sz(), 10});
  }

  if (config.id == to_coreset_id(0)) {
    config.set_coreset0_crbs({0, 48});
  } else {
    freq_resource_bitmap freq_res_bitmap;
    for (unsigned i = 0, e = freq_res_bitmap.max_size(); i != e; ++i) {
      freq_res_bitmap.push_back(generate_bool());
    }
    config.set_freq_domain_resources(freq_res_bitmap);
  }

  return config;
}

static dci_1_0_si_rnti_configuration generate_si_f1_0()
{
  return {1, 2, 3, vrb_to_prb::mapping_type::non_interleaved, 0, 2, 1};
}

static dci_1_0_ra_rnti_configuration generate_ra_f1_0()
{
  return {1, 2, 3, vrb_to_prb::mapping_type::non_interleaved, 1, 2};
}

static dci_dl_info generate_dci_dl_info()
{
  dci_dl_info info;
  info.type = static_cast<dci_dl_rnti_config_type>(generate_binary());
  if (info.type == dci_dl_rnti_config_type::si_f1_0) {
    info.si_f1_0 = generate_si_f1_0();
  } else {
    info.ra_f1_0 = generate_ra_f1_0();
  }

  return info;
}

static rnti_t generate_rnti()
{
  std::uniform_int_distribution<unsigned> dist(1, 65535);

  return to_rnti(dist(gen));
}

static dci_context_information generate_dci_context(const bwp_configuration& bwp, const coreset_configuration& coreset)
{
  dci_context_information info{};
  info.bwp_cfg     = &bwp;
  info.coreset_cfg = &coreset;

  info.rnti              = generate_rnti();
  info.n_id_pdcch_data   = 2;
  info.n_rnti_pdcch_data = 4;
  info.cces.ncce         = 2;
  info.starting_symbol   = 2;
  info.n_id_pdcch_dmrs   = 8;
  info.cces.aggr_lvl     = static_cast<aggregation_level>(generate_case_pattern());

  // NOTE: decision context is not filled as all the members are private in FAPI.
  // NOTE: add the precoding information when the struct is defined, now is empty.

  return info;
}

static void add_dl_pdcch_pdus_to_result(mac_dl_sched_result_test_helper& helper)
{
  static constexpr unsigned nof_pdus = 4;

  for (unsigned i = 0; i != nof_pdus; ++i) {
    pdcch_dl_information info{
        generate_dci_context(helper.bwp_cfg[std::min<unsigned>(i, helper.bwp_cfg.size() - 1U)],
                             helper.coreset_cfg[std::min<unsigned>(i, helper.coreset_cfg.size() - 1U)]),
        generate_dci_dl_info()};
    helper.sched_result.dl_pdcchs.push_back(std::move(info));
  }

  // Add the DCIs payload.
  for (unsigned i = 0; i != nof_pdus; ++i) {
    helper.result.dl_pdcch_pdus.push_back(generate_dci_payload());
  }
}

static void add_all_dl_pdcch_pdus_to_result(mac_dl_sched_result_test_helper& helper)
{
  for (unsigned i = 0; i != MAX_DL_PDCCH_PDUS_PER_SLOT; ++i) {
    pdcch_dl_information info{
        generate_dci_context(helper.bwp_cfg_pdcch[std::min<unsigned>(i, helper.bwp_cfg_pdcch.size() - 1U)],
                             helper.coreset_cfg[std::min<unsigned>(i, helper.coreset_cfg.size() - 1U)]),
        generate_dci_dl_info()};
    helper.sched_result.dl_pdcchs.push_back(std::move(info));
  }

  // Add the DCIs payload.
  for (unsigned i = 0; i != MAX_DL_PDCCH_PDUS_PER_SLOT; ++i) {
    helper.result.dl_pdcch_pdus.push_back(generate_dci_payload());
  }
}

static void add_all_ul_pdcch_pdus_to_result(mac_dl_sched_result_test_helper& helper)
{
  for (unsigned i = 0; i != MAX_UL_PDCCH_PDUS_PER_SLOT; ++i) {
    pdcch_ul_information info{
        generate_dci_context(helper.bwp_cfg_pdcch[std::min<unsigned>(i, helper.bwp_cfg_pdcch.size() - 1U)],
                             helper.coreset_cfg[std::min<unsigned>(i, helper.coreset_cfg.size() - 1U)]),
        {}};
    helper.sched_result.ul_pdcchs.push_back(std::move(info));
  }

  // Add the DCIs payload.
  for (unsigned i = 0; i != MAX_UL_PDCCH_PDUS_PER_SLOT; ++i) {
    helper.result.ul_pdcch_pdus.push_back(generate_dci_payload());
  }
}

static pdsch_information fill_valid_pdsch_information(coreset_configuration& coreset_cfg,
                                                      bwp_configuration&     bwp_config,
                                                      pdsch_information&     info,
                                                      unsigned               nof_ports)
{
  bwp_config  = generate_bwp_configuration();
  coreset_cfg = generate_coreset_configuration();

  info.rnti            = to_rnti(0x4444);
  info.bwp_cfg         = &bwp_config;
  info.coreset_cfg     = &coreset_cfg;
  info.rbs             = vrb_interval{40, 60};
  info.symbols         = {3, 10};
  info.dmrs            = {dmrs_symbol_mask(14), dmrs_config_type::type1, 2, 3, false, 0, 2, bounded_bitset<12>(12)};
  info.n_id            = generate_nid_pdsch();
  info.nof_layers      = 1U;
  info.vrb_prb_mapping = vrb_to_prb::mapping_type::non_interleaved;
  info.harq_id         = to_harq_id(4);
  info.ss_set_type     = search_space_set_type::type0;
  info.dci_fmt         = dci_dl_format::f1_0;
  info.codewords.push_back(pdsch_codeword{{modulation_scheme::QAM16, 220.F}, 5, pdsch_mcs_table::qam64, 2, 128});

  if (nof_ports == 2) {
    pdsch_precoding_info& pm   = info.precoding.emplace();
    pm.nof_rbs_per_prg         = 273U;
    csi_report_pmi& csi_report = pm.prg_infos.emplace_back();
    auto&           pmi        = csi_report.type.emplace<csi_report_pmi::two_antenna_port>();
    pmi.pmi                    = 1;
  } else if (nof_ports == 4) {
    pdsch_precoding_info& pm   = info.precoding.emplace();
    pm.nof_rbs_per_prg         = 273U;
    csi_report_pmi& csi_report = pm.prg_infos.emplace_back();
    auto&           pmi        = csi_report.type.emplace<csi_report_pmi::typeI_single_panel_4ports_mode1>();
    pmi.i_2                    = 1;
    pmi.i_1_1                  = 1;
  }

  // By default fill 1 port precofing matrix, which means not configuring the precoding in the 'pdsch_information'

  return info;
}

static void add_ul_pdcch_pdus_to_result(mac_dl_sched_result_test_helper& helper)
{
  pdcch_ul_information pdcch;

  dci_context_information& context = pdcch.ctx;
  context.bwp_cfg                  = &helper.bwp_cfg[0];
  context.coreset_cfg              = &helper.coreset_cfg[0];
  context.rnti                     = to_rnti(0x4444);
  context.n_id_pdcch_data          = 1;
  context.n_rnti_pdcch_data        = 1;
  context.cces                     = {1, aggregation_level::n1};
  context.starting_symbol          = 0;
  context.n_id_pdcch_dmrs          = 1;

  helper.sched_result.ul_pdcchs.push_back(pdcch);
  helper.result.ul_pdcch_pdus.push_back(generate_dci_payload());
}

sib_information_test_helper unittests::build_valid_sib1_information_pdu()
{
  sib_information_test_helper helper;
  sib_information&            info = helper.pdu;
  info.si_indicator                = sib_information::si_indicator_type::sib1;
  info.nof_txs                     = 0;

  fill_valid_pdsch_information(helper.coreset_cfg, helper.bwp_cfg, info.pdsch_cfg, 1);

  return helper;
}

rar_information_test_helper unittests::build_valid_rar_information_pdu()
{
  rar_information_test_helper helper;
  rar_information&            result = helper.pdu;

  fill_valid_pdsch_information(helper.coreset_cfg, helper.bwp_cfg, result.pdsch_cfg, 2);

  rar_ul_grant grant;
  grant.rapid                    = 2;
  grant.ta                       = 0;
  grant.temp_crnti               = to_rnti(0x0003);
  grant.freq_hop_flag            = false;
  grant.time_resource_assignment = 0;
  grant.freq_resource_assignment = 0;
  grant.mcs                      = 25;
  grant.tpc                      = 3;
  grant.csi_req                  = false;

  result.grants.push_back(grant);

  return helper;
}

dl_paging_allocation_test_helper unittests::build_valid_dl_paging_pdu()
{
  dl_paging_allocation_test_helper helper;

  dl_paging_allocation& result = helper.pdu;
  fill_valid_pdsch_information(helper.coreset_cfg, helper.bwp_cfg, result.pdsch_cfg, 1);

  return helper;
}

dl_msg_allocation_test_helper unittests::build_valid_dl_msg_alloc_pdu()
{
  dl_msg_allocation_test_helper helper;
  dl_msg_alloc&                 result = helper.pdu;

  fill_valid_pdsch_information(helper.coreset_cfg, helper.bwp_cfg, result.pdsch_cfg, 4);

  result.context.ss_id       = to_search_space_id(1);
  result.context.nof_retxs   = 2;
  result.context.k1          = 3;
  result.context.olla_offset = 4;
  result.context.ue_index    = to_du_ue_index(5);

  return helper;
}

mac_dl_sched_result_test_helper unittests::build_valid_mac_dl_sched_result()
{
  mac_dl_sched_result_test_helper helper;
  mac_dl_sched_result&            result = helper.result;

  for (unsigned i = 0, e = helper.coreset_cfg.size(); i != e; ++i) {
    helper.coreset_cfg[i] = generate_coreset_configuration();
    helper.bwp_cfg[i]     = generate_bwp_configuration();
  }

  result.slot   = slot_point(4, generate_sfn(), generate_slot());
  result.dl_res = &helper.sched_result;

  // Add DL PDCCH PDUs.
  add_dl_pdcch_pdus_to_result(helper);

  // Add UL PDCCH PDU.
  add_ul_pdcch_pdus_to_result(helper);

  // Add PDSCH PDU.
  helper.sib1 = build_valid_sib1_information_pdu();
  // Fix the BWP and CORESET pointers.
  helper.sib1.pdu.pdsch_cfg.bwp_cfg     = &helper.sib1.bwp_cfg;
  helper.sib1.pdu.pdsch_cfg.coreset_cfg = &helper.sib1.coreset_cfg;
  helper.sched_result.bc.sibs.push_back(helper.sib1.pdu);

  // Add SSBs.
  result.ssb_pdus.push_back(build_valid_dl_ssb_pdu());
  result.ssb_pdus.push_back(build_valid_dl_ssb_pdu());

  return helper;
}

static csi_rs_info build_valid_csi_pdu(mac_dl_sched_result_test_helper& helper)
{
  csi_rs_info result;
  result.bwp_cfg              = &helper.bwp_cfg[0];
  result.crbs                 = {40, 120};
  result.type                 = csi_rs_type::CSI_RS_ZP;
  result.row                  = 1;
  result.freq_domain          = bounded_bitset<12, false>(12);
  result.symbol0              = 0;
  result.symbol1              = 4;
  result.cdm_type             = csi_rs_cdm_type::no_CDM;
  result.freq_density         = csi_rs_freq_density_type::one;
  result.scrambling_id        = 0;
  result.power_ctrl_offset    = 0;
  result.power_ctrl_offset_ss = 0;

  return result;
}

mac_dl_sched_result_test_helper unittests::build_valid_mac_dl_sched_result_with_all_supported_pdus()
{
  mac_dl_sched_result_test_helper helper;
  mac_dl_sched_result&            result = helper.result;

  for (unsigned i = 0, e = helper.coreset_cfg.size(); i != e; ++i) {
    helper.coreset_cfg[i] = generate_coreset_configuration();
    helper.bwp_cfg[i]     = generate_bwp_configuration();
  }

  for (unsigned i = 0; i != MAX_DL_PDCCH_PDUS_PER_SLOT; ++i) {
    helper.bwp_cfg_pdcch[i] = {cyclic_prefix::NORMAL, subcarrier_spacing::kHz240, {0 + i, 20 + i}};
  }

  result.slot   = slot_point(4, generate_sfn(), generate_slot());
  result.dl_res = &helper.sched_result;

  // Add DL PDCCH PDUs.
  add_all_dl_pdcch_pdus_to_result(helper);

  // Add UL PDCCH PDU.
  add_all_ul_pdcch_pdus_to_result(helper);

  // Add SIB1 PDU.
  helper.sib1 = build_valid_sib1_information_pdu();
  // Fix the BWP and CORESET pointers.
  helper.sib1.pdu.pdsch_cfg.bwp_cfg     = &helper.sib1.bwp_cfg;
  helper.sib1.pdu.pdsch_cfg.coreset_cfg = &helper.sib1.coreset_cfg;
  for (unsigned i = 0; i != MAX_SI_PDUS_PER_SLOT; ++i) {
    helper.sched_result.bc.sibs.push_back(helper.sib1.pdu);
  }

  // Add RAR PDU.
  helper.rar = build_valid_rar_information_pdu();
  // Fix the BWP and CORESET pointers.
  helper.rar.pdu.pdsch_cfg.bwp_cfg     = &helper.rar.bwp_cfg;
  helper.rar.pdu.pdsch_cfg.coreset_cfg = &helper.rar.coreset_cfg;
  for (unsigned i = 0; i != MAX_RAR_PDUS_PER_SLOT; ++i) {
    helper.sched_result.rar_grants.push_back(helper.rar.pdu);
  }

  // Add paging PDU.
  helper.dl_paging = build_valid_dl_paging_pdu();
  // Fix the BWP and CORESET pointers.
  helper.dl_paging.pdu.pdsch_cfg.bwp_cfg     = &helper.dl_paging.bwp_cfg;
  helper.dl_paging.pdu.pdsch_cfg.coreset_cfg = &helper.dl_paging.coreset_cfg;
  for (unsigned i = 0; i != MAX_PAGING_PDUS_PER_SLOT; ++i) {
    helper.sched_result.paging_grants.push_back(helper.dl_paging.pdu);
  }

  // Add downlink UE PDU.
  helper.dl_msg_alloc = build_valid_dl_msg_alloc_pdu();
  // Fix the BWP and CORESET pointers.
  helper.dl_msg_alloc.pdu.pdsch_cfg.bwp_cfg     = &helper.dl_msg_alloc.bwp_cfg;
  helper.dl_msg_alloc.pdu.pdsch_cfg.coreset_cfg = &helper.dl_msg_alloc.coreset_cfg;
  for (unsigned i = 0; i != MAX_UE_PDUS_PER_SLOT; ++i) {
    helper.sched_result.ue_grants.push_back(helper.dl_msg_alloc.pdu);
  }

  // Add CSI PDU.
  for (unsigned i = 0; i != MAX_CSI_RS_PDUS_PER_SLOT; ++i) {
    helper.sched_result.csi_rs.push_back(build_valid_csi_pdu(helper));
  }

  // Add SSBs.
  for (unsigned i = 0; i != MAX_SSB_PER_SLOT; ++i) {
    result.ssb_pdus.push_back(build_valid_dl_ssb_pdu());
  }

  return helper;
}

prach_occasion_info unittests::build_valid_prach_occassion()
{
  prach_occasion_info prach;

  // NOTE: Used random values from the property range, FAPI document SCF-222 v4.0 section 3.4.3.1 in table PRACH PDU.
  prach.nof_preamble_indexes = 12;
  prach.start_preamble_index = 11;
  prach.pci                  = generate_pci();
  prach.nof_cs               = 3;
  prach.start_symbol         = 2;
  prach.index_fd_ra          = 2;
  prach.nof_fd_ra            = 4;
  prach.nof_prach_occasions  = 5;
  prach.format               = prach_format_type::one;

  return prach;
}

ul_sched_info_test_helper unittests::build_valid_pusch_pdu()
{
  ul_sched_info_test_helper helper;
  ul_sched_info&            info  = helper.info;
  pusch_information&        pusch = info.pusch_cfg;

  helper.bwp_cfg = {cyclic_prefix::NORMAL, subcarrier_spacing::kHz15, {10, 20}};

  pusch.rnti                       = to_rnti(0x4444);
  pusch.bwp_cfg                    = &helper.bwp_cfg;
  pusch.rbs                        = vrb_interval(10, 20);
  pusch.symbols                    = {2, 12};
  pusch.mcs_descr.target_code_rate = 193;
  pusch.mcs_descr.modulation       = modulation_scheme::QAM256;
  pusch.mcs_index                  = 3;
  pusch.mcs_table                  = pusch_mcs_table::qam256;
  pusch.dmrs = {dmrs_symbol_mask(14), dmrs_config_type::type1, 2, 3, false, 0, 2, bounded_bitset<12>(12)};
  pusch.intra_slot_freq_hopping    = false;
  pusch.pusch_second_hop_prb       = 3;
  pusch.tx_direct_current_location = 1;
  pusch.ul_freq_shift_7p5khz       = false;
  pusch.transform_precoding        = false;
  pusch.n_id                       = 69;
  pusch.nof_layers                 = 1;
  pusch.pusch_dmrs_id              = 18;
  pusch.dmrs_hopping_mode          = pusch_information::dmrs_hopping_mode::no_hopping;
  pusch.rv_index                   = 1;
  pusch.harq_id                    = to_harq_id(2);
  pusch.new_data                   = true;
  pusch.tb_size_bytes              = 11;
  pusch.num_cb                     = 0;

  // UCI.
  uci_info& uci             = info.uci.emplace();
  uci.alpha                 = alpha_scaling_opt::f0p5;
  auto& harq                = uci.harq.emplace(uci_info::harq_info());
  harq.harq_ack_nof_bits    = 1;
  harq.beta_offset_harq_ack = 4;
  auto& csi                 = uci.csi.emplace(uci_info::csi_info());
  csi.csi_part1_nof_bits    = 2;
  csi.beta_offset_csi_1     = 13;
  csi.beta_offset_csi_2.emplace(6);
  csi.csi_rep_cfg = {1,
                     pmi_codebook_type::typeI_single_panel_4ports_mode1,
                     ri_restriction_type({true, true, true, true}),
                     csi_report_quantities::cri_ri_li_pmi_cqi};

  return helper;
}

srs_info_helper unittests::build_valid_srs_pdu()
{
  srs_info_helper helper;

  srs_info& pdu  = helper.pdu;
  helper.bwp_cfg = {cyclic_prefix::NORMAL, subcarrier_spacing::kHz15, {10, 20}};

  pdu.crnti                                  = to_rnti(23);
  pdu.bwp_cfg                                = &helper.bwp_cfg;
  pdu.nof_antenna_ports                      = 4;
  pdu.symbols                                = {0, 1};
  pdu.nof_repetitions                        = n1;
  pdu.config_index                           = 4;
  pdu.sequence_id                            = 4;
  pdu.bw_index                               = 2;
  pdu.tx_comb                                = tx_comb_size::n2;
  pdu.comb_offset                            = 1;
  pdu.cyclic_shift                           = 2;
  pdu.freq_position                          = 32;
  pdu.freq_shift                             = 21;
  pdu.freq_hopping                           = 3;
  pdu.group_or_seq_hopping                   = srs_group_or_sequence_hopping::neither;
  pdu.resource_type                          = srs_resource_type::aperiodic;
  pdu.t_srs_period                           = srs_periodicity::sl8;
  pdu.t_offset                               = 65;
  pdu.normalized_channel_iq_matrix_requested = true;
  pdu.positioning_report_requested           = true;

  return helper;
}

pucch_info_test_helper unittests::build_valid_pucch_format_1_pdu()
{
  pucch_info_test_helper helper;
  pucch_info&            pucch = helper.info;

  helper.bwp_cfg = {cyclic_prefix::NORMAL, subcarrier_spacing::kHz15, {2, 10}};

  pucch.crnti                      = to_rnti(0x4444);
  pucch.bwp_cfg                    = &helper.bwp_cfg;
  auto& format_1                   = pucch.format_params.emplace<pucch_format_1>();
  pucch.resources.prbs             = {1, 4};
  pucch.resources.symbols          = {0, 14};
  pucch.resources.second_hop_prbs  = {2, 12};
  pucch.uci_bits.harq_ack_nof_bits = 2;
  pucch.uci_bits.sr_bits           = sr_nof_bits::no_sr;
  format_1.time_domain_occ         = 3;
  format_1.initial_cyclic_shift    = 9;
  format_1.n_id_hopping            = 2;
  format_1.group_hopping           = srsran::pucch_group_hopping::NEITHER;
  format_1.slot_repetition         = srsran::pucch_repetition_tx_slot::no_multi_slot;

  return helper;
}

pucch_info_test_helper srsran::unittests::build_valid_pucch_format_2_pdu()
{
  pucch_info_test_helper helper;
  pucch_info&            pucch = helper.info;

  helper.bwp_cfg = {cyclic_prefix::NORMAL, subcarrier_spacing::kHz15, {2, 10}};

  pucch.crnti                       = to_rnti(0x4444);
  pucch.bwp_cfg                     = &helper.bwp_cfg;
  auto& format_2                    = pucch.format_params.emplace<pucch_format_2>();
  pucch.resources.prbs              = {1, 4};
  pucch.resources.symbols           = {0, 1};
  pucch.resources.second_hop_prbs   = {1, 11};
  format_2.max_code_rate            = max_pucch_code_rate::dot_08;
  pucch.uci_bits.csi_part1_nof_bits = 102;
  pucch.uci_bits.harq_ack_nof_bits  = 100;
  pucch.uci_bits.sr_bits            = sr_nof_bits::one;
  format_2.n_id_0_scrambling        = 256;
  format_2.n_id_scambling           = 382;

  return helper;
}

pucch_info_test_helper srsran::unittests::build_valid_pucch_format_3_pdu()
{
  pucch_info_test_helper helper;
  pucch_info&            pucch = helper.info;

  helper.bwp_cfg = {cyclic_prefix::NORMAL, subcarrier_spacing::kHz15, {2, 10}};

  pucch.crnti                       = to_rnti(0x4444);
  pucch.bwp_cfg                     = &helper.bwp_cfg;
  auto& format_3                    = pucch.format_params.emplace<pucch_format_3>();
  pucch.resources.prbs              = {1, 4};
  pucch.resources.symbols           = {0, 3};
  pucch.resources.second_hop_prbs   = {1, 11};
  format_3.max_code_rate            = max_pucch_code_rate::dot_08;
  pucch.uci_bits.csi_part1_nof_bits = 102;
  pucch.uci_bits.harq_ack_nof_bits  = 100;
  pucch.uci_bits.sr_bits            = sr_nof_bits::one;
  format_3.n_id_0_scrambling        = 256;
  format_3.n_id_scrambling          = 382;
  format_3.n_id_hopping             = 180;
  format_3.additional_dmrs          = false;
  format_3.pi_2_bpsk                = true;

  return helper;
}

pucch_info_test_helper srsran::unittests::build_valid_pucch_format_4_pdu()
{
  pucch_info_test_helper helper;
  pucch_info&            pucch = helper.info;

  helper.bwp_cfg = {cyclic_prefix::NORMAL, subcarrier_spacing::kHz15, {2, 10}};

  pucch.crnti                       = to_rnti(0x4444);
  pucch.bwp_cfg                     = &helper.bwp_cfg;
  auto& format_4                    = pucch.format_params.emplace<pucch_format_4>();
  pucch.resources.prbs              = {1, 2};
  pucch.resources.symbols           = {0, 3};
  pucch.resources.second_hop_prbs   = {10, 11};
  format_4.max_code_rate            = max_pucch_code_rate::dot_08;
  pucch.uci_bits.csi_part1_nof_bits = 102;
  pucch.uci_bits.harq_ack_nof_bits  = 100;
  pucch.uci_bits.sr_bits            = sr_nof_bits::one;
  format_4.n_id_0_scrambling        = 256;
  format_4.n_id_scrambling          = 382;
  format_4.n_id_hopping             = 180;
  format_4.additional_dmrs          = false;
  format_4.pi_2_bpsk                = true;
  format_4.n_sf_pucch_f4            = pucch_format_4_sf::sf2;
  format_4.orthog_seq_idx           = 1;

  return helper;
}

mac_ul_sched_result_test_helper srsran::unittests::build_valid_mac_ul_sched_result()
{
  mac_ul_sched_result_test_helper helper{{}, {}, {}, build_valid_srs_pdu(), {}, {}};
  mac_ul_sched_result&            result = helper.result;
  result.slot                            = slot_point(4, generate_sfn(), generate_slot());
  result.ul_res                          = &helper.sched_result;

  // Add PUCCH PDUs.
  helper.pucch_f1              = build_valid_pucch_format_1_pdu();
  helper.pucch_f1.info.bwp_cfg = &helper.pucch_f1.bwp_cfg;
  helper.sched_result.pucchs.push_back(helper.pucch_f1.info);
  helper.pucch_f2              = build_valid_pucch_format_2_pdu();
  helper.pucch_f2.info.bwp_cfg = &helper.pucch_f2.bwp_cfg;
  helper.sched_result.pucchs.push_back(helper.pucch_f2.info);

  // Add PUSCH PDU.
  helper.pusch                        = build_valid_pusch_pdu();
  helper.pusch.info.pusch_cfg.bwp_cfg = &helper.pusch.bwp_cfg;
  helper.sched_result.puschs.push_back(helper.pusch.info);

  // Add PRACH.
  helper.sched_result.prachs.push_back(build_valid_prach_occassion());

  return helper;
}

mac_ul_sched_result_test_helper unittests::build_valid_mac_ul_sched_result_with_all_supported_pdus()
{
  mac_ul_sched_result_test_helper helper{{}, {}, {}, build_valid_srs_pdu(), {}, {}};
  mac_ul_sched_result&            result = helper.result;
  result.slot                            = slot_point(4, generate_sfn(), generate_slot());
  result.ul_res                          = &helper.sched_result;

  // Add PUCCH PDUs.
  for (unsigned i = 0; i != MAX_PUCCH_PDUS_PER_SLOT; ++i) {
    helper.pucch_f1              = build_valid_pucch_format_1_pdu();
    helper.pucch_f1.info.bwp_cfg = &helper.pucch_f1.bwp_cfg;
    helper.sched_result.pucchs.push_back(helper.pucch_f1.info);
  }

  // Add PUSCH PDU.
  for (unsigned i = 0; i != MAX_PUSCH_PDUS_PER_SLOT; ++i) {
    helper.pusch                        = build_valid_pusch_pdu();
    helper.pusch.info.pusch_cfg.bwp_cfg = &helper.pusch.bwp_cfg;
    helper.sched_result.puschs.push_back(helper.pusch.info);
  }

  // Add PRACH.
  for (unsigned i = 0; i != MAX_PRACH_OCCASIONS_PER_SLOT; ++i) {
    helper.sched_result.prachs.push_back(build_valid_prach_occassion());
  }

  // Add SRS.
  for (unsigned i = 0; i != MAX_SRS_PDUS_PER_SLOT; ++i) {
    helper.sched_result.srss.push_back(helper.srs.pdu);
  }

  return helper;
}

mac_dl_data_result_test_helper srsran::unittests::build_valid_mac_data_result()
{
  mac_dl_data_result_test_helper result;

  result.result.si_pdus.push_back({1, shared_transport_block(result.data)});

  return result;
}

mac_dl_data_result_test_helper unittests::build_valid_mac_data_result_with_all_supported_pdu()
{
  mac_dl_data_result_test_helper result;

  for (unsigned i = 0; i != MAX_SI_PDUS_PER_SLOT; ++i) {
    result.result.si_pdus.push_back({1, shared_transport_block(result.data)});
  }

  for (unsigned i = 0; i != MAX_RAR_PDUS_PER_SLOT; ++i) {
    result.result.rar_pdus.push_back({1, shared_transport_block(result.data)});
  }

  for (unsigned i = 0; i != MAX_UE_PDUS_PER_SLOT; ++i) {
    result.result.ue_pdus.push_back({1, shared_transport_block(result.data)});
  }

  for (unsigned i = 0; i != MAX_PAGING_PDUS_PER_SLOT; ++i) {
    result.result.paging_pdus.push_back({1, shared_transport_block(result.data)});
  }

  return result;
}
