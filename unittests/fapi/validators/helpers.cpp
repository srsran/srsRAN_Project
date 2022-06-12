/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "helpers.h"
#include "srsgnb/adt/bitmap_utils.h"
#include "srsgnb/fapi/message_builders.h"
#include <random>

static std::mt19937 gen(0);
using namespace srsgnb;
using namespace fapi;
using namespace unittest;

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

static rnti_t generate_rnti()
{
  std::uniform_int_distribution<unsigned> dist(1, 65535);

  return to_rnti(dist(gen));
}

static unsigned generate_rapid()
{
  std::uniform_int_distribution<unsigned> dist(0, 128);
  unsigned                                value = dist(gen);

  return (value < 64U) ? value : 255U;
}

static unsigned generate_harq()
{
  std::uniform_int_distribution<unsigned> dist(0, 15);

  return dist(gen);
}

static unsigned generate_handle()
{
  std::uniform_int_distribution<unsigned> dist(0, std::numeric_limits<uint32_t>::max());

  return dist(gen);
}

static unsigned generate_bwp_size()
{
  std::uniform_int_distribution<unsigned> dist(1, 275);

  return dist(gen);
}

static unsigned generate_bwp_start()
{
  std::uniform_int_distribution<unsigned> dist(0, 274);

  return dist(gen);
}

static subcarrier_spacing generate_scs()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);

  return static_cast<subcarrier_spacing>(dist(gen));
}

static cyclic_prefix_type generate_cyclic_prefix()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);

  return static_cast<cyclic_prefix_type>(dist(gen));
}

/// Returns random values for the maintenance v3 basic parameters. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.4.2.4.
static std::tuple<ssb_pattern_case, subcarrier_spacing, unsigned> generate_random_maintenance_v3_basic_params()
{
  std::uniform_int_distribution<unsigned> enum_dist(0, 4);

  static constexpr std::array<unsigned, 3> lmax{4U, 8U, 64U};
  std::uniform_int_distribution<unsigned>  lmax_dist(0, 2);

  return {static_cast<ssb_pattern_case>(enum_dist(gen)),
          static_cast<subcarrier_spacing>(enum_dist(gen)),
          lmax[lmax_dist(gen)]};
}

static pci_t generate_pci()
{
  std::uniform_int_distribution<unsigned> dist(0, 1007);

  return dist(gen);
}

dl_ssb_pdu unittest::build_valid_dl_ssb_pdu()
{
  dl_ssb_pdu                              pdu;
  dl_ssb_pdu_builder                      builder(pdu);
  std::uniform_int_distribution<unsigned> pci_dist(0, 1007);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> block_index_dist(0, 63);
  std::uniform_int_distribution<unsigned> subcarrier_offset_dist(0, 31);
  std::uniform_int_distribution<unsigned> offset_pointA_dist(0, 2199);
  std::uniform_int_distribution<unsigned> sib1_dist(0, 255);
  std::uniform_real_distribution<>        power_dist(-30.8, 69.5);
  auto                                    pss_profile = static_cast<beta_pss_profile_type>(binary_dist(gen));
  builder.set_basic_parameters(
      pci_dist(gen), pss_profile, block_index_dist(gen), subcarrier_offset_dist(gen), offset_pointA_dist(gen));
  builder.set_bch_payload_phy_full(binary_dist(gen), sib1_dist(gen), binary_dist(gen), binary_dist(gen));
  const auto& v3 = generate_random_maintenance_v3_basic_params();
  builder.set_maintenance_v3_basic_parameters(std::get<0>(v3), std::get<1>(v3), std::get<2>(v3));
  // When the PSS to SSS NR profile is configured to use the field beta PSS Profile SSS then generate a random number,
  // otherwise leave it unset, as per SCF-222 Section 3.4.2.4.
  optional<float> beta_pss;
  if (pss_profile == beta_pss_profile_type::beta_pss_profile_sss) {
    beta_pss = power_dist(gen);
  }
  builder.set_maintenance_v3_tx_power_info(power_dist(gen), beta_pss);

  return pdu;
}

dl_pdcch_pdu unittest::build_valid_dl_pdcch_pdu()
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);
  std::uniform_int_distribution<unsigned> duration_symbol_dist(1, 3);
  std::uniform_int_distribution<unsigned> shift_index_dist(0, 275);
  std::uniform_int_distribution<unsigned> n_rnti_dist(0, 65535);
  std::uniform_int_distribution<unsigned> cce_dist(0, 135);
  std::uniform_int_distribution<unsigned> aggregation_dist(0, 4);
  std::uniform_int_distribution<unsigned> nid_dmrs_dist(0, 65535);
  std::uniform_int_distribution<unsigned> nid_data_dist(0, 65535);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<int>      power_dist(-8, 8);
  std::uniform_int_distribution<unsigned> custom_dist(2, 3);

  // :TODO: generate  better frequency domain resource bitmap.
  static_vector<uint8_t, 6> freq_domain = {3, 2, 1, 4, 5, 1};

  // Always work with the biggest numerology.
  builder.set_bwp_parameters(bwp_size_dist(gen),
                             bwp_start_dist(gen),
                             subcarrier_spacing::kHz240,
                             static_cast<cyclic_prefix_type>(binary_dist(gen)));

  builder.set_coreset_parameters(start_symbol_index_dist(gen),
                                 duration_symbol_dist(gen),
                                 {freq_domain},
                                 static_cast<cce_to_reg_mapping_type>(binary_dist(gen)),
                                 custom_dist(gen),
                                 custom_dist(gen),
                                 static_cast<pdcch_coreset_type>(binary_dist(gen)),
                                 shift_index_dist(gen),
                                 static_cast<precoder_granularity_type>(binary_dist(gen)));

  // Add DCI.
  auto builder_dci = builder.add_dl_dci();

  builder_dci.set_basic_parameters(to_rnti(n_rnti_dist(gen) + 1),
                                   nid_data_dist(gen),
                                   n_rnti_dist(gen),
                                   cce_dist(gen),
                                   std::pow(2, aggregation_dist(gen)));
  optional<float> profile_nr;
  profile_nr.emplace(power_dist(gen));
  builder_dci.set_tx_power_info_parameter(profile_nr);

  // Payload.
  static_vector<uint8_t, 128> payload(32, 1);

  builder_dci.set_payload(2, {payload});

  optional<float> profile_data;
  optional<float> profile_dmrs;
  builder_dci.set_maintenance_v3_dci_parameters(true, profile_dmrs, profile_data);
  builder_dci.set_parameters_v4_dci(nid_dmrs_dist(gen));

  return pdu;
}

dl_pdsch_pdu unittest::build_valid_dl_pdsch_pdu()
{
  static constexpr unsigned iterations = 10000;
  std::vector<unsigned>     results;
  results.reserve(iterations);

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> rnti_dist(1, 65535);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> nid_pdsch_dist(0, 1023);
  std::uniform_int_distribution<unsigned> nof_layers_dist(1, 8);
  std::uniform_int_distribution<unsigned> dmrs_scrambling_dist(0, 65535);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> dmrs_cdm_grps_no_data_dist(1, 3);
  std::uniform_int_distribution<unsigned> nr_of_symbols_dist(1, 14);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);
  std::uniform_real_distribution<float>   power_dist(-32, 32.0);

  unsigned                 pdu_bitmap          = 0;
  cyclic_prefix_type       cyclic_p            = cyclic_prefix_type::normal;
  pdsch_ref_point_type     ref_point           = pdsch_ref_point_type::point_a;
  dmrs_config_type         config_type         = dmrs_config_type::type_1;
  low_papr_dmrs_type       low_papr            = low_papr_dmrs_type::independent_cdm_group;
  resource_allocation_type resource_alloc      = resource_allocation_type::type_0;
  vrb_to_prb_mapping_type  vrb_prb_mapping     = vrb_to_prb_mapping_type::non_interleaved;
  nzp_csi_rs_epre_to_ssb   power_ss_profile_nr = nzp_csi_rs_epre_to_ssb::dB_minus_3;
  int                      power_nr            = -7;
  int                      power               = -30;
  pdsch_trans_type         trasn_type          = pdsch_trans_type::non_interleaved_other;
  ldpc_base_graph_type     ldpc_graph          = ldpc_base_graph_type::bg_1;

  rnti_t   rnti                     = to_rnti(rnti_dist(gen));
  unsigned bwp_size                 = bwp_size_dist(gen);
  unsigned bwp_start                = bwp_start_dist(gen);
  unsigned nid_pdsch                = nid_pdsch_dist(gen);
  unsigned scrambling_id            = dmrs_scrambling_dist(gen);
  unsigned scrambling_id_complement = dmrs_scrambling_dist(gen);
  bool     n_scid                   = binary_dist(gen);
  unsigned dmrs_cdm_grps_no_data    = dmrs_cdm_grps_no_data_dist(gen);
  unsigned start_symbol_index       = start_symbol_index_dist(gen);
  unsigned nr_of_symbols            = nr_of_symbols_dist(gen);
  unsigned coreset_start            = bwp_size_dist(gen);
  unsigned initial_bwp_size         = bwp_size_dist(gen);
  unsigned tb_size_lbrm_bytes       = bwp_size_dist(gen);
  unsigned dl_dmrs_symbol           = rnti_dist(gen);
  unsigned rb_size                  = nr_of_symbols_dist(gen);
  unsigned rb_start                 = start_symbol_index_dist(gen);
  float    profile_sss              = power_dist(gen);

  std::array<uint8_t, 36> rb_bitmap = {};

  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder_pdsch(pdu);

  builder_pdsch.set_basic_parameters(pdu_bitmap & 1, (pdu_bitmap >> 1) & 1, rnti);

  // Always work with the biggest numerology.
  builder_pdsch.set_bwp_parameters(bwp_size, bwp_start, subcarrier_spacing::kHz240, cyclic_p);
  builder_pdsch.set_codeword_information_parameters(nid_pdsch, 1, 0, ref_point);
  builder_pdsch.set_dmrs_parameters(
      dl_dmrs_symbol, config_type, scrambling_id, scrambling_id_complement, low_papr, n_scid, dmrs_cdm_grps_no_data, 0);

  auto builder_cw = builder_pdsch.add_codeword();

  unsigned target_code = 2;
  unsigned qam_mod     = 2;
  unsigned mcs         = 20;
  unsigned mcs_table   = 1;
  unsigned rv_index    = 0;
  unsigned tb_size     = 42;

  builder_cw.set_basic_parameters(target_code, qam_mod, mcs, mcs_table, rv_index, tb_size);

  if (resource_alloc == resource_allocation_type::type_0) {
    builder_pdsch.set_pdsch_allocation_in_frequency_type_0({rb_bitmap}, vrb_prb_mapping);
  } else {
    builder_pdsch.set_pdsch_allocation_in_frequency_type_1(rb_start, rb_size, vrb_prb_mapping);
  }

  builder_pdsch.set_pdsch_allocation_in_time_parameters(start_symbol_index, nr_of_symbols);

  optional<int>   profile_nr;
  optional<float> dmrs_profile;
  if (power_nr != -9) {
    profile_nr.emplace(power_nr);
  } else {
    dmrs_profile.emplace(profile_sss);
  }

  builder_pdsch.set_tx_power_info_parameters(profile_nr, power_ss_profile_nr);

  // :TODO: not filling CBG to retx control parameters.

  builder_pdsch.set_maintenance_v3_bwp_parameters(trasn_type, coreset_start, initial_bwp_size);
  builder_pdsch.set_maintenance_v3_codeword_parameters(
      ldpc_graph, tb_size_lbrm_bytes, pdu_bitmap & 1U, (pdu_bitmap >> 1) & 1U);

  optional<float> data_profile;
  if (power != -33) {
    data_profile.emplace(power);
  }

  builder_pdsch.set_maintenance_v3_tx_power_info_parameters(dmrs_profile, data_profile);

  std::vector<uint8_t> rm_vector;

  builder_pdsch.set_maintenance_v4_basic_parameters(3, {rm_vector}, binary_dist(gen), {rm_vector});

  return pdu;
}

dl_csi_rs_pdu unittest::build_valid_dl_csi_pdu()
{
  dl_csi_rs_pdu         pdu;
  dl_csi_rs_pdu_builder builder(pdu);

  std::uniform_int_distribution<unsigned> nof_rb_dist(24, 276);
  std::uniform_int_distribution<unsigned> start_rb_dist(0, 274);
  std::uniform_int_distribution<unsigned> csi_type_dist(0, 2);
  std::uniform_int_distribution<unsigned> row_dist(1, 18);
  std::uniform_int_distribution<unsigned> symb_l0_dist(0, 13);
  std::uniform_int_distribution<unsigned> symb_l1_dist(2, 12);
  std::uniform_int_distribution<unsigned> cdm_type_dist(0, 3);
  std::uniform_int_distribution<unsigned> freq_density_dist(0, 3);
  std::uniform_int_distribution<unsigned> scram_id_dist(0, 1023);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<int>      power_dist(-8, 8);
  std::uniform_int_distribution<unsigned> ss_power_dist(0, 10);

  builder.set_basic_parameters(start_rb_dist(gen),
                               nof_rb_dist(gen),
                               static_cast<csi_type>(csi_type_dist(gen)),
                               row_dist(gen),
                               freq_density_dist(gen),
                               symb_l0_dist(gen),
                               symb_l1_dist(gen),
                               static_cast<csi_cdm_type>(cdm_type_dist(gen)),
                               static_cast<csi_freq_density_type>(freq_density_dist(gen)),
                               scram_id_dist(gen));

  // Always work with the biggest numerology.
  builder.set_bwp_parameters(subcarrier_spacing::kHz240, static_cast<cyclic_prefix_type>(binary_dist(gen)));

  optional<float> profile_nr;
  profile_nr.emplace(power_dist(gen));
  unsigned ss_power = ss_power_dist(gen);
  ss_power          = (ss_power > 3) ? 255 : ss_power;
  builder.set_tx_power_info_parameters(profile_nr, static_cast<nzp_csi_rs_epre_to_ssb>(ss_power));

  optional<float> profile_sss;
  builder.set_maintenance_v3_tx_power_info_parameters(profile_sss);

  return pdu;
}

dl_tti_request_message unittest::build_valid_dl_tti_request()
{
  dl_tti_request_message         msg;
  dl_tti_request_message_builder builder(msg);

  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);

  // :TODO: at this moment code does not support groups, the number of groups is set to 0.
  builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen), 0);

  // Manually add the SSB PDU to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type = dl_pdu_type::SSB;
  msg.pdus.back().ssb_pdu  = build_valid_dl_ssb_pdu();

  // Manually add the PDCCH PDU to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type  = dl_pdu_type::PDCCH;
  msg.pdus.back().pdcch_pdu = build_valid_dl_pdcch_pdu();

  // Manually add the PDSCH PDU to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type  = dl_pdu_type::PDSCH;
  msg.pdus.back().pdsch_pdu = build_valid_dl_pdsch_pdu();

  // Manually add the CSI PDU to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type   = dl_pdu_type::CSI_RS;
  msg.pdus.back().csi_rs_pdu = build_valid_dl_csi_pdu();

  return msg;
}

ul_dci_request_message unittest::build_valid_ul_dci_request()
{
  ul_dci_request_message msg;

  msg.slot = generate_slot();
  msg.sfn  = generate_sfn();

  // Manually add the PDCCH PDU to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type = ul_dci_pdu_type::PDCCH;
  msg.pdus.back().pdu      = build_valid_dl_pdcch_pdu();

  return msg;
}

slot_indication_message unittest::build_valid_slot_indication()
{
  slot_indication_message msg;
  msg.slot = generate_slot();
  msg.sfn  = generate_sfn();

  return msg;
}

error_indication_message unittest::build_valid_error_indication()
{
  error_indication_message msg;

  msg.slot          = generate_slot();
  msg.sfn           = generate_sfn();
  msg.message_id    = message_type_id::tx_data_request;
  msg.error_code    = error_code_id::msg_invalid_config;
  msg.expected_sfn  = std::numeric_limits<uint16_t>::max();
  msg.expected_slot = std::numeric_limits<uint16_t>::max();

  return msg;
}

error_indication_message unittest::build_valid_out_of_sync_error_indication()
{
  error_indication_message msg;

  msg.slot          = generate_slot();
  msg.sfn           = generate_sfn();
  msg.message_id    = message_type_id::tx_data_request;
  msg.error_code    = error_code_id::out_of_sync;
  msg.expected_sfn  = generate_sfn();
  msg.expected_slot = generate_slot();

  return msg;
}

rx_data_indication_message unittest::build_valid_rx_data_indication()
{
  rx_data_indication_message msg;

  msg.sfn            = generate_sfn();
  msg.slot           = generate_slot();
  msg.control_length = 0;

  msg.pdus.emplace_back();
  auto& pdu   = msg.pdus.back();
  pdu.handle  = generate_handle();
  pdu.rnti    = generate_rnti();
  pdu.rapid   = generate_rapid();
  pdu.harq_id = generate_harq();

  return msg;
}

static uint8_t generate_num_prach_ocas()
{
  std::uniform_int_distribution<unsigned> dist(1, 7);

  return dist(gen);
}

static uint8_t generate_index_fd_ra()
{
  std::uniform_int_distribution<unsigned> dist(0, 7);

  return dist(gen);
}

static uint8_t generate_prach_start_symbol()
{
  std::uniform_int_distribution<unsigned> dist(0, 13);

  return dist(gen);
}

static uint8_t generate_num_cs()
{
  std::uniform_int_distribution<unsigned> dist(0, 419);

  return dist(gen);
}

static uint8_t generate_is_msga_prach()
{
  std::uniform_int_distribution<unsigned> dist(0, 2);

  return dist(gen);
}

static prach_format_type generate_prach_format()
{
  std::uniform_int_distribution<unsigned> dist(0, 13);

  return static_cast<prach_format_type>(dist(gen));
}

static prach_config_scope_type generate_prach_config_scope()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);

  return static_cast<prach_config_scope_type>(dist(gen));
}

static uint8_t generate_start_preamble()
{
  unsigned value = generate_num_cs();

  return (value < 64U) ? value : 255U;
}

static uint8_t generate_num_preambles_indices()
{
  std::uniform_int_distribution<unsigned> dist(1, 64);

  return dist(gen);
}

ul_prach_pdu unittest::build_valid_ul_prach_pdu()
{
  ul_prach_pdu pdu;

  pdu.phys_cell_id                = generate_pci();
  pdu.num_prach_ocas              = generate_num_prach_ocas();
  pdu.prach_format                = generate_prach_format();
  pdu.index_fd_ra                 = generate_index_fd_ra();
  pdu.prach_start_symbol          = generate_prach_start_symbol();
  pdu.num_cs                      = generate_num_cs();
  pdu.is_msg_a_prach              = generate_is_msga_prach();
  pdu.has_msg_a_pusch_beamforming = generate_is_msga_prach();

  // Maintenance v3 parameters.
  auto& v3                = pdu.maintenance_v3;
  v3.prach_config_scope   = generate_prach_config_scope();
  v3.num_fd_ra            = generate_index_fd_ra() + 1U;
  v3.start_preamble_index = generate_start_preamble();
  v3.num_preamble_indices = generate_num_preambles_indices();

  return pdu;
}

static int generate_ul_sinr_metric()
{
  std::uniform_int_distribution<int> dist(-32768, 32767);
  return dist(gen);
}

static unsigned generate_timing_advance_offset()
{
  std::uniform_int_distribution<unsigned> dist(0, 100);
  unsigned                                value = dist(gen);

  return (value < 64U) ? value : 65535U;
}

static int generate_timing_advance_offset_in_ns()
{
  std::uniform_int_distribution<int> dist(-32768, 32767);
  int                                value = dist(gen);

  return (-16800 <= value && value <= 16800) ? value : -32768;
}

static unsigned generate_rssi_or_rsrp()
{
  std::uniform_int_distribution<unsigned> dist(0, 2500);
  unsigned                                value = dist(gen);

  return (value <= 1280U) ? value : 65535;
}

static uci_detection_status generate_detection_status()
{
  std::uniform_int_distribution<unsigned> dist(1, 5);
  return static_cast<uci_detection_status>(dist(gen));
}

static unsigned generate_bit_length()
{
  std::uniform_int_distribution<unsigned> dist(1, 1706);
  return dist(gen);
}

static uci_harq_pdu generate_harq_pdu()
{
  uci_harq_pdu pdu;

  pdu.detection_status = generate_detection_status();
  pdu.bit_length       = generate_bit_length();
  if (pdu.detection_status == uci_detection_status::crc_pass || pdu.detection_status == uci_detection_status::no_dtx ||
      pdu.detection_status == uci_detection_status::dtx_not_checked) {
    pdu.payload.resize(std::ceil(static_cast<float>(pdu.bit_length) / 8.F));
  }

  return pdu;
}

static uci_csi_part1 generate_csi_part1_pdu()
{
  uci_csi_part1 pdu;

  pdu.detection_status = generate_detection_status();
  pdu.bit_length       = generate_bit_length();
  if (pdu.detection_status == uci_detection_status::crc_pass || pdu.detection_status == uci_detection_status::no_dtx ||
      pdu.detection_status == uci_detection_status::dtx_not_checked) {
    pdu.payload.resize(std::ceil(static_cast<float>(pdu.bit_length) / 8.F));
  }

  return pdu;
}

static uci_csi_part2 generate_csi_part2_pdu()
{
  uci_csi_part2 pdu;

  pdu.detection_status = generate_detection_status();
  pdu.bit_length       = generate_bit_length();
  if (pdu.detection_status == uci_detection_status::crc_pass || pdu.detection_status == uci_detection_status::no_dtx ||
      pdu.detection_status == uci_detection_status::dtx_not_checked) {
    pdu.payload.resize(std::ceil(static_cast<float>(pdu.bit_length) / 8.F));
  }

  return pdu;
}

uci_pusch_pdu unittest::build_valid_uci_pusch_pdu()
{
  uci_pusch_pdu pdu;

  pdu.handle                   = generate_handle();
  pdu.rnti                     = generate_rnti();
  pdu.ul_sinr_metric           = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset    = generate_timing_advance_offset();
  pdu.timing_advance_offset_ns = static_cast<int16_t>(generate_timing_advance_offset_in_ns());
  pdu.rssi                     = generate_rssi_or_rsrp();
  pdu.rsrp                     = generate_rssi_or_rsrp();

  // Enable HARQ, CSI part 1 and CSI Part 2.
  pdu.pdu_bitmap = 14U;
  pdu.harq       = generate_harq_pdu();
  pdu.csi_part1  = generate_csi_part1_pdu();
  pdu.csi_part2  = generate_csi_part2_pdu();

  return pdu;
}

static sr_pdu_format_0_1 generate_sr_format01_pdu()
{
  sr_pdu_format_0_1 pdu;

  std::uniform_int_distribution<unsigned> dist(0, 3);
  pdu.sr_indication = std::min(1U, dist(gen));

  unsigned value          = dist(gen);
  pdu.sr_confidence_level = (value < 2U) ? value : 255U;

  return pdu;
}

static uci_harq_format_0_1 generate_harq_format01_pdu()
{
  uci_harq_format_0_1 pdu;

  std::uniform_int_distribution<unsigned> dist(0, 2);
  unsigned                                value = dist(gen);
  pdu.harq_confidence_level                     = (value < 2U) ? value : 255U;

  std::uniform_int_distribution<unsigned> nof_dist(1, 2);
  for (unsigned i = 0, e = nof_dist(gen); i != e; ++i) {
    pdu.harq_values.emplace_back(dist(gen));
  }

  return pdu;
}

uci_pucch_pdu_format_0_1 unittest::build_valid_uci_pucch_format01_pdu()
{
  uci_pucch_pdu_format_0_1 pdu;

  pdu.handle                   = generate_handle();
  pdu.rnti                     = generate_rnti();
  pdu.ul_sinr_metric           = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset    = generate_timing_advance_offset();
  pdu.timing_advance_offset_ns = static_cast<int16_t>(generate_timing_advance_offset_in_ns());
  pdu.rssi                     = generate_rssi_or_rsrp();
  pdu.rsrp                     = generate_rssi_or_rsrp();

  std::uniform_int_distribution<unsigned> dist(0, 1);
  pdu.pucch_format = static_cast<uci_pucch_pdu_format_0_1::format_type>(dist(gen));

  // Enable SR and HARQ for PUCCH format 0/1.
  pdu.pdu_bitmap = 3U;
  pdu.sr         = generate_sr_format01_pdu();
  pdu.harq       = generate_harq_format01_pdu();

  return pdu;
}

static sr_pdu_format_2_3_4 generate_sr_format234_pdu()
{
  sr_pdu_format_2_3_4 pdu;

  std::uniform_int_distribution<unsigned> dist(1, 4);
  pdu.sr_bitlen = dist(gen);
  pdu.sr_payload.resize(std::ceil(static_cast<float>(pdu.sr_bitlen) / 8.F));

  return pdu;
}

static uci_payload_pusch_pucch generate_uci_payload()
{
  uci_payload_pusch_pucch payload;

  payload.detection_status          = generate_detection_status();
  payload.expected_uci_payload_size = generate_bit_length();
  if (payload.detection_status == uci_detection_status::crc_pass ||
      payload.detection_status == uci_detection_status::no_dtx ||
      payload.detection_status == uci_detection_status::dtx_not_checked) {
    payload.payload.resize(std::ceil(static_cast<float>(payload.expected_uci_payload_size) / 8.F));
  }

  return payload;
}

uci_pucch_pdu_format_2_3_4 unittest::build_valid_uci_pucch_format234_pdu()
{
  uci_pucch_pdu_format_2_3_4 pdu;

  pdu.handle                   = generate_handle();
  pdu.rnti                     = generate_rnti();
  pdu.ul_sinr_metric           = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset    = generate_timing_advance_offset();
  pdu.timing_advance_offset_ns = static_cast<int16_t>(generate_timing_advance_offset_in_ns());
  pdu.rssi                     = generate_rssi_or_rsrp();
  pdu.rsrp                     = generate_rssi_or_rsrp();

  std::uniform_int_distribution<unsigned> dist(0, 2);
  pdu.pucch_format = static_cast<uci_pucch_pdu_format_2_3_4::format_type>(dist(gen));

  // Enable SR, HARQ, CSI part 1. CSI part 2, UCI payload part 1 and UCI payload part 2.
  pdu.pdu_bitmap = 15U;
  pdu.sr         = generate_sr_format234_pdu();
  pdu.harq       = generate_harq_pdu();
  pdu.csi_part1  = generate_csi_part1_pdu();
  pdu.csi_part2  = generate_csi_part2_pdu();
  pdu.uci_part1  = generate_uci_payload();
  pdu.uci_part2  = generate_uci_payload();

  return pdu;
}

uci_indication_message unittest::build_valid_uci_indication()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);

  uci_indication_message msg;

  msg.sfn  = sfn_dist(gen);
  msg.slot = slot_dist(gen);

  // Add one PDU of each type.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type  = uci_pdu_type::PUSCH;
  msg.pdus.back().pusch_pdu = build_valid_uci_pusch_pdu();

  // Add one PDU of each type.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type      = uci_pdu_type::PUCCH_format_0_1;
  msg.pdus.back().pucch_pdu_f01 = build_valid_uci_pucch_format01_pdu();

  // Add one PDU of each type.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type       = uci_pdu_type::PUCCH_format_2_3_4;
  msg.pdus.back().pucch_pdu_f234 = build_valid_uci_pucch_format234_pdu();

  return msg;
}

static multi_slot_tx_indicator_type generate_multi_slot_tx_indicator()
{
  std::uniform_int_distribution<unsigned> dist(0, 3);
  return static_cast<multi_slot_tx_indicator_type>(dist(gen));
}

static unsigned generate_prb_size()
{
  std::uniform_int_distribution<unsigned> dist(1, 16);
  return dist(gen);
}

static unsigned generate_start_symbol_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 13);
  return dist(gen);
}

static unsigned generate_nof_symbols(pucch_format_type type)
{
  if (type == pucch_format_type::f0 || type == pucch_format_type::f2) {
    std::uniform_int_distribution<unsigned> dist(1, 2);
    return dist(gen);
  }

  std::uniform_int_distribution<unsigned> dist(4, 14);
  return dist(gen);
}

static unsigned generate_sr_bit_len(pucch_format_type type)
{
  if (type == pucch_format_type::f0 || type == pucch_format_type::f1) {
    std::uniform_int_distribution<unsigned> dist(0, 1);
    return dist(gen);
  }

  std::uniform_int_distribution<unsigned> dist(0, 4);
  return dist(gen);
}

static unsigned generate_harq_bit_len(pucch_format_type type)
{
  if (type == pucch_format_type::f0 || type == pucch_format_type::f1) {
    std::uniform_int_distribution<unsigned> dist(1, 2);
    return dist(gen);
  }

  std::uniform_int_distribution<unsigned> dist(2, 1706);
  return dist(gen);
}

static unsigned generate_max_code_rate(pucch_format_type type)
{
  if (type == pucch_format_type::f0 || type == pucch_format_type::f1) {
    return 255;
  }

  std::uniform_int_distribution<unsigned> dist(0, 7);
  return dist(gen);
}

static pucch_group_hopping_type generate_pucch_group_hopping()
{
  std::uniform_int_distribution<unsigned> dist(0, 2);
  return static_cast<pucch_group_hopping_type>(dist(gen));
}

static unsigned generate_nid_pucch_hopping()
{
  std::uniform_int_distribution<unsigned> dist(0, 1023);
  return dist(gen);
}

static unsigned generate_initial_cyclic_shift()
{
  std::uniform_int_distribution<unsigned> dist(0, 11);
  return dist(gen);
}

static unsigned generate_csi_bit_len()
{
  std::uniform_int_distribution<unsigned> dist(0, 1706);
  return dist(gen);
}

static ul_pucch_pdu generate_generic_ul_pucch_pdu()
{
  ul_pucch_pdu pdu;

  pdu.rnti                           = generate_rnti();
  pdu.handle                         = generate_handle();
  pdu.bwp_size                       = generate_bwp_size();
  pdu.bwp_start                      = generate_bwp_start();
  pdu.scs                            = generate_scs();
  pdu.cyclic_prefix                  = generate_cyclic_prefix();
  pdu.multi_slot_tx_indicator        = generate_multi_slot_tx_indicator();
  pdu.pi2_bpsk                       = true;
  pdu.prb_start                      = generate_bwp_start();
  pdu.prb_size                       = generate_prb_size();
  pdu.start_symbol_index             = generate_start_symbol_index();
  pdu.intra_slot_frequency_hopping   = true;
  pdu.second_hop_prb                 = generate_bwp_start();
  pdu.pucch_group_hopping            = generate_pucch_group_hopping();
  pdu.nid_pucch_hopping              = generate_nid_pucch_hopping();
  pdu.initial_cyclic_shift           = generate_initial_cyclic_shift();
  pdu.nid_pucch_scrambling           = generate_nid_pucch_hopping();
  pdu.time_domain_occ_index          = 5;
  pdu.pre_dft_occ_len                = 2;
  pdu.pre_dft_occ_idx                = 2;
  pdu.add_dmrs_flag                  = true;
  pdu.nid0_pucch_dmrs_scrambling     = 10200;
  pdu.m0_pucch_dmrs_cyclic_shift     = 8;
  pdu.csi_part1_bit_length           = generate_csi_bit_len();
  pdu.pucch_maintenance_v3.ul_bwp_id = 43;

  // Add 1 part1 to part2 correspondence.
  pdu.uci_correspondence.part2.emplace_back();
  auto& corr                = pdu.uci_correspondence.part2.back();
  corr.priority             = 3;
  corr.param_offsets        = {1, 2, 3};
  corr.param_sizes          = {1, 2, 3};
  corr.part2_size_map_index = 43;
  corr.part2_size_map_scope = uci_part1_to_part2_correspondence_v3::map_scope_type::common_context;

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f0_pdu()
{
  auto                               pdu    = generate_generic_ul_pucch_pdu();
  static constexpr pucch_format_type format = pucch_format_type::f0;
  pdu.format_type                           = format;
  pdu.nr_of_symbols                         = generate_nof_symbols(format);
  pdu.sr_bit_len                            = generate_sr_bit_len(format);
  pdu.bit_len_harq                          = generate_harq_bit_len(format);
  pdu.pucch_maintenance_v3.max_code_rate    = generate_max_code_rate(format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f1_pdu()
{
  auto                               pdu    = generate_generic_ul_pucch_pdu();
  static constexpr pucch_format_type format = pucch_format_type::f1;
  pdu.format_type                           = format;
  pdu.nr_of_symbols                         = generate_nof_symbols(format);
  pdu.sr_bit_len                            = generate_sr_bit_len(format);
  pdu.bit_len_harq                          = generate_harq_bit_len(format);
  pdu.pucch_maintenance_v3.max_code_rate    = generate_max_code_rate(format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f2_pdu()
{
  auto                               pdu    = generate_generic_ul_pucch_pdu();
  static constexpr pucch_format_type format = pucch_format_type::f2;
  pdu.format_type                           = format;
  pdu.nr_of_symbols                         = generate_nof_symbols(format);
  pdu.sr_bit_len                            = generate_sr_bit_len(format);
  pdu.bit_len_harq                          = generate_harq_bit_len(format);
  pdu.pucch_maintenance_v3.max_code_rate    = generate_max_code_rate(format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f3_pdu()
{
  auto                               pdu    = generate_generic_ul_pucch_pdu();
  static constexpr pucch_format_type format = pucch_format_type::f3;
  pdu.format_type                           = format;
  pdu.nr_of_symbols                         = generate_nof_symbols(format);
  pdu.sr_bit_len                            = generate_sr_bit_len(format);
  pdu.bit_len_harq                          = generate_harq_bit_len(format);
  pdu.pucch_maintenance_v3.max_code_rate    = generate_max_code_rate(format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f4_pdu()
{
  auto                               pdu    = generate_generic_ul_pucch_pdu();
  static constexpr pucch_format_type format = pucch_format_type::f4;
  pdu.format_type                           = format;
  pdu.nr_of_symbols                         = generate_nof_symbols(format);
  pdu.sr_bit_len                            = generate_sr_bit_len(format);
  pdu.bit_len_harq                          = generate_harq_bit_len(format);
  pdu.pucch_maintenance_v3.max_code_rate    = generate_max_code_rate(format);

  return pdu;
}

static bool generate_bool()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return dist(gen);
}

static unsigned generate_qam_mod_order(bool transform_precoding)
{
  static constexpr std::array<unsigned, 5> values = {1, 2, 4, 6, 8};

  std::uniform_int_distribution<unsigned> dist(0, 4);
  unsigned                                index = dist(gen);

  if (!transform_precoding && index == 0) {
    ++index;
  }

  return values[index];
}

static unsigned generate_mcs_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 31);
  return dist(gen);
}

static pusch_mcs_table_type generate_mcs_table()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);
  return static_cast<pusch_mcs_table_type>(dist(gen));
}

static unsigned generate_num_layers()
{
  std::uniform_int_distribution<unsigned> dist(1, 4);
  return dist(gen);
}

static dmrs_config_type generate_dmrs_type()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return static_cast<dmrs_config_type>(dist(gen));
}

static low_papr_dmrs_type generate_low_papr_dmrs()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return static_cast<low_papr_dmrs_type>(dist(gen));
}

static unsigned generate_pusch_dmrs_identity()
{
  std::uniform_int_distribution<unsigned> dist(0, 1007);
  return dist(gen);
}

static unsigned generate_num_dmrs_cdm_no_data()
{
  std::uniform_int_distribution<unsigned> dist(1, 3);
  return dist(gen);
}

static resource_allocation_type generate_resource_allocation()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return static_cast<resource_allocation_type>(dist(gen));
}

static unsigned generate_tx_direct_current_location()
{
  std::uniform_int_distribution<unsigned> dist(0, 4095);
  return dist(gen);
}

static ldpc_base_graph_type generate_ldpc_graph_type()
{
  std::uniform_int_distribution<unsigned> dist(1, 2);
  return static_cast<ldpc_base_graph_type>(dist(gen));
}

ul_pusch_pdu unittest::build_valid_ul_pusch_pdu()
{
  ul_pusch_pdu pdu;

  pdu.rnti                                = generate_rnti();
  pdu.handle                              = generate_handle();
  pdu.bwp_size                            = generate_bwp_size();
  pdu.bwp_start                           = generate_bwp_start();
  pdu.scs                                 = generate_scs();
  pdu.cyclic_prefix                       = generate_cyclic_prefix();
  pdu.target_code_rate                    = 1982U;
  pdu.transform_precoding                 = generate_bool();
  pdu.qam_mod_order                       = generate_qam_mod_order(pdu.transform_precoding);
  pdu.mcs_index                           = generate_mcs_index();
  pdu.mcs_table                           = generate_mcs_table();
  pdu.nid_pusch                           = generate_nid_pucch_hopping();
  pdu.num_layers                          = generate_num_layers();
  pdu.ul_dmrs_symb_pos                    = 3U;
  pdu.dmrs_type                           = generate_dmrs_type();
  pdu.pusch_dmrs_scrambling_id            = 32421;
  pdu.pusch_dmrs_scrambling_id_complement = 3213;
  pdu.low_papr_dmrs                       = generate_low_papr_dmrs();
  pdu.pusch_dmrs_identity                 = generate_pusch_dmrs_identity();
  pdu.nscid                               = generate_bool();
  pdu.num_dmrs_cdm_grps_no_data           = generate_num_dmrs_cdm_no_data();
  pdu.dmrs_ports                          = 4;
  pdu.resource_alloc                      = generate_resource_allocation();
  if (pdu.resource_alloc == resource_allocation_type::type_1) {
    pdu.rb_start = generate_bwp_start();
    pdu.rb_size  = generate_bwp_size();
  }
  pdu.vrb_to_prb_mapping            = vrb_to_prb_mapping_type::non_interleaved;
  pdu.intra_slot_frequency_hopping  = generate_bool();
  pdu.tx_direct_current_location    = generate_tx_direct_current_location();
  pdu.uplink_frequency_shift_7p5kHz = generate_bool();
  pdu.start_symbol_index            = generate_start_symbol_index();
  pdu.nr_of_symbols                 = 3U;

  auto& data = pdu.pusch_data;
  pdu.pdu_bitmap.set(ul_pusch_pdu::PUSCH_DATA_BIT);
  data.rv_index        = 2;
  data.harq_process_id = 2;
  data.new_data        = 0;
  data.tb_size         = 213131;
  data.num_cb          = 3414;

  auto& uci = pdu.pusch_uci;
  pdu.pdu_bitmap.set(ul_pusch_pdu::PUSCH_UCI_BIT);
  uci.harq_ack_bit_length  = 3;
  uci.csi_part1_bit_length = 4;
  uci.flags_csi_part2      = 65535;
  uci.alpha_scaling        = 3;
  uci.beta_offset_harq_ack = 12;
  uci.beta_offset_csi1     = 16;
  uci.beta_offset_csi2     = 17;

  // Add 1 part1 to part2 correspondence.
  pdu.uci_correspondence.part2.emplace_back();
  auto& corr                = pdu.uci_correspondence.part2.back();
  corr.priority             = 3;
  corr.param_offsets        = {1, 2, 3};
  corr.param_sizes          = {1, 2, 3};
  corr.part2_size_map_index = 43;
  corr.part2_size_map_scope = uci_part1_to_part2_correspondence_v3::map_scope_type::common_context;

  auto& ptrs = pdu.pusch_ptrs;
  pdu.pdu_bitmap.set(ul_pusch_pdu::PUSCH_PTRS_BIT);
  ptrs.ul_ptrs_power     = ul_ptrs_power_type::dB4_77;
  ptrs.ptrs_freq_density = 1;
  ptrs.ptrs_time_density = 2;
  ptrs.port_info.push_back({3, 4, 5});

  auto& ofdm = pdu.pusch_ofdm;
  pdu.pdu_bitmap.set(ul_pusch_pdu::DFTS_OFDM_BIT);
  ofdm.low_papr_group_number                    = 25;
  ofdm.low_papr_sequence_number                 = 3232;
  ofdm.ul_ptrs_sample_density                   = 3;
  ofdm.ul_ptrs_time_density_transform_precoding = 4;

  auto& v3                            = pdu.pusch_maintenance_v3;
  v3.pusch_trans_type                 = 2;
  v3.delta_bwp0_start_from_active_bwp = generate_bwp_start();
  v3.initial_ul_bwp_size              = generate_bwp_start();
  v3.group_or_sequence_hopping        = 2;
  v3.pusch_second_hop_prb             = generate_bwp_start();
  v3.ldpc_base_graph                  = generate_ldpc_graph_type();
  v3.tb_size_lbrm_bytes               = 32323242;

  pdu.pusch_params_v4.cb_crc_status_request = generate_bool();

  return pdu;
}
ul_tti_request_message unittest::build_valid_ul_tti_request()
{
  ul_tti_request_message msg;

  msg.slot       = generate_slot();
  msg.sfn        = generate_sfn();
  msg.num_groups = 2000;

  {
    ++msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PRACH)];
    msg.pdus.push_back({ul_pdu_type::PRACH, 0, build_valid_ul_prach_pdu()});
  }
  {
    ++msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)];
    ul_tti_request_pdu pdu;
    pdu.pdu_type  = ul_pdu_type::PUCCH;
    pdu.pucch_pdu = build_valid_ul_pucch_f0_pdu();
    msg.pdus.push_back(pdu);
  }
  {
    ++msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)];
    ul_tti_request_pdu pdu;
    pdu.pdu_type  = ul_pdu_type::PUCCH;
    pdu.pucch_pdu = build_valid_ul_pucch_f3_pdu();
    msg.pdus.push_back(pdu);
  }
  {
    ++msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUSCH)];
    ul_tti_request_pdu pdu;
    pdu.pdu_type  = ul_pdu_type::PUSCH;
    pdu.pusch_pdu = build_valid_ul_pusch_pdu();
    msg.pdus.push_back(pdu);
  }

  return msg;
}

static unsigned generate_slot_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 79);
  return dist(gen);
}

static unsigned generate_ra_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 7);
  return dist(gen);
}

static unsigned generate_preamble_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 63);
  return dist(gen);
}

rach_indication_message unittest::build_valid_rach_indication()
{
  rach_indication_message msg;

  msg.slot = generate_slot();
  msg.sfn  = generate_sfn();

  msg.pdus.emplace_back();
  rach_indication_pdu& pdu = msg.pdus.back();

  pdu.symbol_index = generate_start_symbol_index();
  pdu.slot_index   = generate_slot_index();
  pdu.ra_index     = generate_ra_index();
  pdu.avg_rssi     = std::numeric_limits<uint32_t>::max();
  pdu.rsrp         = std::numeric_limits<uint16_t>::max();
  pdu.avg_snr      = std::numeric_limits<uint8_t>::max();

  pdu.preambles.emplace_back();
  rach_indication_pdu_preamble& preamble = pdu.preambles.back();

  preamble.preamble_index           = generate_preamble_index();
  preamble.timing_advance_offset    = std::numeric_limits<uint16_t>::max();
  preamble.timing_advance_offset_ns = std::numeric_limits<uint32_t>::max();
  preamble.preamble_pwr             = std::numeric_limits<uint32_t>::max();
  preamble.preamble_snr             = std::numeric_limits<uint8_t>::max();

  return msg;
}
