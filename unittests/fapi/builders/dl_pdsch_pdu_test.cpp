/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;
using namespace fapi;

static std::mt19937 gen(0);

static void test_basic_parameters()
{
  std::uniform_int_distribution<unsigned> rnti_dist(1, 65535);

  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  rnti_t rnti = to_rnti(rnti_dist(gen));
  builder.set_basic_parameters(rnti);

  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(0, pdu.pdu_bitmap[dl_pdsch_pdu::PDU_BITMAP_CBG_RETX_CTRL_BIT]);
  TESTASSERT_EQ(0, pdu.pdu_bitmap[dl_pdsch_pdu::PDU_BITMAP_PTRS_BIT]);
}

static void test_bwp_parameters()
{
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  for (auto scs : {subcarrier_spacing::kHz15,
                   subcarrier_spacing::kHz30,
                   subcarrier_spacing::kHz60,
                   subcarrier_spacing::kHz120,
                   subcarrier_spacing::kHz240}) {
    for (auto cprefix : {cyclic_prefix_type::normal, cyclic_prefix_type::extended}) {
      dl_pdsch_pdu         pdu;
      dl_pdsch_pdu_builder builder(pdu);

      unsigned bwp_size  = bwp_size_dist(gen);
      unsigned bwp_start = bwp_start_dist(gen);

      builder.set_bwp_parameters(bwp_size, bwp_start, scs, cprefix);

      TESTASSERT_EQ(bwp_size, pdu.bwp_size);
      TESTASSERT_EQ(bwp_start, pdu.bwp_start);
      TESTASSERT_EQ(scs, pdu.scs);
      TESTASSERT_EQ(cprefix, pdu.cyclic_prefix);
    }
  }
}

static void test_codewords_parameters()
{
  std::uniform_int_distribution<unsigned> n_id_pdsch_dist(0, 1023);

  for (unsigned num_layers = 1; num_layers != 9; ++num_layers) {
    for (auto ref_point : {pdsch_ref_point_type::point_a, pdsch_ref_point_type::subcarrier_0}) {
      dl_pdsch_pdu         pdu;
      dl_pdsch_pdu_builder builder(pdu);

      unsigned transmission_scheme = 0;
      unsigned n_id_pdsch          = n_id_pdsch_dist(gen);
      builder.set_codeword_information_parameters(n_id_pdsch, num_layers, transmission_scheme, ref_point);

      TESTASSERT_EQ(n_id_pdsch, pdu.nid_pdsch);
      TESTASSERT_EQ(num_layers, pdu.num_layers);
      TESTASSERT_EQ(transmission_scheme, pdu.transmission_scheme);
      TESTASSERT_EQ(ref_point, pdu.ref_point);
    }
  }
}

static void test_add_codeword()
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  TESTASSERT(pdu.cws.empty());
  TESTASSERT(pdu.pdsch_maintenance_v3.cbg_tx_information.empty());

  builder.add_codeword();
  TESTASSERT_EQ(1, pdu.cws.size());
  TESTASSERT_EQ(1, pdu.pdsch_maintenance_v3.cbg_tx_information.size());
}

static void test_codeword_basic_parameters()
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  auto builder_cw = builder.add_codeword();

  uint16_t target_code_rate = 6;
  uint8_t  qam_mod          = 2;
  uint8_t  mcs_index        = 2;
  uint8_t  mcs_table        = 1;
  uint8_t  rv_index         = 2;
  uint32_t tb_size          = 128;

  builder_cw.set_basic_parameters(target_code_rate, qam_mod, mcs_index, mcs_table, rv_index, tb_size);

  TESTASSERT_EQ(mcs_table, pdu.cws[0].mcs_table);
  TESTASSERT_EQ(tb_size, pdu.cws[0].tb_size);
  TESTASSERT_EQ(mcs_index, pdu.cws[0].mcs_index);
  TESTASSERT_EQ(rv_index, pdu.cws[0].rv_index);
  TESTASSERT_EQ(qam_mod, pdu.cws[0].qam_mod_order);
  TESTASSERT_EQ(target_code_rate * 10, pdu.cws[0].target_code_rate);
}

static void test_dmrs_parameters()
{
  uint16_t dmrs_symbol_pos = 3;
  uint16_t dmrs_ports      = 4;

  std::uniform_int_distribution<unsigned> dmrs_scrambling_id_dist(0, 65535);
  std::uniform_int_distribution<unsigned> dmrs_scrambling_id_complement_dist(0, 65535);

  for (auto config_type : {dmrs_config_type::type1, dmrs_config_type::type2}) {
    for (auto low_papr : {low_papr_dmrs_type::dependent_cdm_group, low_papr_dmrs_type::independent_cdm_group}) {
      for (auto nscid : {0U, 1U}) {
        for (unsigned num_dmrs_cdm_grp_no_data = 0; num_dmrs_cdm_grp_no_data != 4; ++num_dmrs_cdm_grp_no_data) {
          dl_pdsch_pdu         pdu;
          dl_pdsch_pdu_builder builder(pdu);

          unsigned dmrs_scrambling_id            = dmrs_scrambling_id_dist(gen);
          unsigned dmrs_scrambling_id_complement = dmrs_scrambling_id_complement_dist(gen);

          builder.set_dmrs_parameters(dmrs_symbol_pos,
                                      config_type,
                                      dmrs_scrambling_id,
                                      dmrs_scrambling_id_complement,
                                      low_papr,
                                      nscid,
                                      num_dmrs_cdm_grp_no_data,
                                      dmrs_ports);

          TESTASSERT_EQ(dmrs_symbol_pos, pdu.dl_dmrs_symb_pos);
          TESTASSERT_EQ(config_type == dmrs_config_type::type1 ? dmrs_cfg_type::type_1 : dmrs_cfg_type::type_2,
                        pdu.dmrs_type);
          TESTASSERT_EQ(dmrs_scrambling_id, pdu.pdsch_dmrs_scrambling_id);
          TESTASSERT_EQ(dmrs_scrambling_id_complement, pdu.pdsch_dmrs_scrambling_id_compl);
          TESTASSERT_EQ(low_papr, pdu.low_papr_dmrs);
          TESTASSERT_EQ(nscid, pdu.nscid);
          TESTASSERT_EQ(num_dmrs_cdm_grp_no_data, pdu.num_dmrs_cdm_grps_no_data);
          TESTASSERT_EQ(dmrs_ports, pdu.dmrs_ports);
        }
      }
    }
  }
}

static void test_allocation_in_freq_parameters()
{
  // Type 0.
  for (auto vrb_to_prb : {vrb_to_prb_mapping_type::non_interleaved,
                          vrb_to_prb_mapping_type::interleaved_rb_size2,
                          vrb_to_prb_mapping_type::interleaved_rb_size4}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    // :TODO: Check the RB bitmap in the future.
    std::vector<uint8_t> bitmap(3, 1);

    builder.set_pdsch_allocation_in_frequency_type_0(span<const uint8_t>(bitmap), vrb_to_prb);

    TESTASSERT_EQ(vrb_to_prb, pdu.vrb_to_prb_mapping);
  }

  // Type 1.
  std::uniform_int_distribution<unsigned> rb_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> rb_size_dist(1, 275);

  for (auto vrb_to_prb : {vrb_to_prb_mapping_type::non_interleaved,
                          vrb_to_prb_mapping_type::interleaved_rb_size2,
                          vrb_to_prb_mapping_type::interleaved_rb_size4}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    uint16_t rb_start = rb_start_dist(gen);
    uint16_t rb_size  = rb_size_dist(gen);

    builder.set_pdsch_allocation_in_frequency_type_1(rb_start, rb_size, vrb_to_prb);

    TESTASSERT_EQ(vrb_to_prb, pdu.vrb_to_prb_mapping);
    TESTASSERT_EQ(rb_start, pdu.rb_start);
    TESTASSERT_EQ(rb_size, pdu.rb_size);
  }
}

static void test_allocation_in_time_parameters()
{
  std::uniform_int_distribution<unsigned> start_sym_dist(0, 13);
  std::uniform_int_distribution<unsigned> nof_symbols_dist(1, 14);

  for (unsigned i = 0; i != 10; ++i) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    uint8_t start_symb = start_sym_dist(gen);
    uint8_t nof_symb   = nof_symbols_dist(gen);

    builder.set_pdsch_allocation_in_time_parameters(start_symb, nof_symb);

    TESTASSERT_EQ(start_symb, pdu.start_symbol_index);
    TESTASSERT_EQ(nof_symb, pdu.nr_of_symbols);
  }
}

static void test_tx_power_info_parameters()
{
  for (int power = -8; power != 17; ++power) {
    for (auto ss_profile : {nzp_csi_rs_epre_to_ssb::dB0,
                            nzp_csi_rs_epre_to_ssb::dB6,
                            nzp_csi_rs_epre_to_ssb::L1_use_profile_sss,
                            nzp_csi_rs_epre_to_ssb::dB3,
                            nzp_csi_rs_epre_to_ssb::dB_minus_3}) {
      dl_pdsch_pdu         pdu;
      dl_pdsch_pdu_builder builder(pdu);

      optional<int> p;
      if (power != 16) {
        p.emplace(power);
      }

      builder.set_tx_power_info_parameters(p, ss_profile);

      TESTASSERT_EQ(p ? static_cast<unsigned>(p.value() + 8) : 255, pdu.power_control_offset_profile_nr);
      TESTASSERT_EQ(ss_profile, pdu.power_control_offset_ss_profile_nr);
    }
  }
}

static void test_cbg_retx_ctrl_parameters()
{
  for (unsigned last_cb = 0; last_cb != 4; ++last_cb) {
    for (auto inline_tb_crc : {inline_tb_crc_type::control_message, inline_tb_crc_type::data_payload}) {
      dl_pdsch_pdu         pdu;
      dl_pdsch_pdu_builder builder(pdu);

      // :TODO: check the vector in the future when the structure is defined.
      std::vector<uint32_t> dl_tb_crc(2, 1);

      builder.set_cbg_re_tx_ctrl_parameters(
          last_cb & 1U, (last_cb >> 1) & 1U, inline_tb_crc, span<const uint32_t>(dl_tb_crc));

      TESTASSERT_EQ(last_cb, pdu.is_last_cb_present);
      TESTASSERT_EQ(inline_tb_crc, pdu.is_inline_tb_crc);
      TESTASSERT_EQ(1, pdu.pdu_bitmap[dl_pdsch_pdu::PDU_BITMAP_CBG_RETX_CTRL_BIT]);
    }
  }
}

static void test_maintenance_v3_bwp_parameters()
{
  std::uniform_int_distribution<unsigned> dist(0, 274);

  for (auto trans_type : {pdsch_trans_type::interleaved_common_any_coreset0_not_present,
                          pdsch_trans_type::interleaved_common_type0_coreset0,
                          pdsch_trans_type::interleaved_common_any_coreset0_present,
                          pdsch_trans_type::interleaved_other,
                          pdsch_trans_type::non_interleaved_common_ss,
                          pdsch_trans_type::non_interleaved_other}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    uint16_t initial_dl_bwp_size = dist(gen);
    uint16_t start_point         = dist(gen);

    builder.set_maintenance_v3_bwp_parameters(trans_type, start_point, initial_dl_bwp_size);

    TESTASSERT_EQ(trans_type, pdu.pdsch_maintenance_v3.trans_type);
    TESTASSERT_EQ(start_point, pdu.pdsch_maintenance_v3.coreset_start_point);
    TESTASSERT_EQ(initial_dl_bwp_size, pdu.pdsch_maintenance_v3.initial_dl_bwp_size);
  }
}

static void test_maintenance_v3_codeword_info_parameters()
{
  std::uniform_int_distribution<unsigned> tb_size_dist(1, 274);

  for (unsigned tb_crc = 0; tb_crc != 4; ++tb_crc) {
    for (auto graph_type : {ldpc_base_graph_type::BG2, ldpc_base_graph_type::BG1}) {
      dl_pdsch_pdu         pdu;
      dl_pdsch_pdu_builder builder(pdu);

      uint32_t tb_size = tb_size_dist(gen);

      builder.set_maintenance_v3_codeword_parameters(graph_type, tb_size, tb_crc & 1U, (tb_crc >> 1) & 1U);

      TESTASSERT_EQ(tb_crc, pdu.pdsch_maintenance_v3.tb_crc_required);
      TESTASSERT_EQ(tb_size, pdu.pdsch_maintenance_v3.tb_size_lbrm_bytes);
      TESTASSERT_EQ(graph_type, pdu.pdsch_maintenance_v3.ldpc_base_graph);
    }
  }
}

static void test_maintenance_v3_rm_references_parameters()
{
  std::uniform_int_distribution<unsigned> pdu_index_dist(0, 65535);

  for (unsigned tb_crc = 0; tb_crc != 4; ++tb_crc) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    // :TODO: Check the arrays/vectors when the structure is defined.
    std::vector<uint16_t> ssb_pdus(3, 1);
    std::vector<uint8_t>  prb_pdus(3, 1);
    std::vector<uint8_t>  lte_pdus(3, 1);
    std::vector<uint16_t> csi(3, 1);

    uint16_t ssb_config = pdu_index_dist(gen);
    uint16_t pdcch      = pdu_index_dist(gen);
    uint16_t dci        = pdu_index_dist(gen);
    unsigned prb_size   = 54;
    unsigned lte_size   = 3;

    builder.set_maintenance_v3_rm_references_parameters(
        {ssb_pdus}, ssb_config, prb_size, {prb_pdus}, pdcch, dci, lte_size, {lte_pdus}, {csi});

    TESTASSERT_EQ(ssb_config, pdu.pdsch_maintenance_v3.ssb_config_for_rate_matching);
    TESTASSERT_EQ(pdcch, pdu.pdsch_maintenance_v3.pdcch_pdu_index);
    TESTASSERT_EQ(dci, pdu.pdsch_maintenance_v3.dci_index);
    TESTASSERT_EQ(prb_size, pdu.pdsch_maintenance_v3.prb_sym_rm_pattern_bitmap_size_byref);
    TESTASSERT_EQ(lte_size, pdu.pdsch_maintenance_v3.lte_crs_rm_pattern_bitmap_size);
  }
}

static void test_maintenance_v3_tx_power_info_parameters()
{
  for (int power = -33; power != 1; ++power) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    optional<float> dmrs;
    if (power != -33) {
      dmrs.emplace(power);
    }
    optional<float> data;
    if (power + 33 != 33) {
      data.emplace(power + 33);
    }

    builder.set_maintenance_v3_tx_power_info_parameters(dmrs, data);

    TESTASSERT_EQ(dmrs ? static_cast<int>(dmrs.value() * 1000) : -32768,
                  pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss);
    TESTASSERT_EQ(data ? static_cast<int>(data.value() * 1000) : -32768,
                  pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss);
  }
}

static void test_maintenance_v3_cbg_tx_crtl_parameters()
{
  for (auto cbg_per_tb : {2U, 4U, 6U, 8U}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    builder.set_maintenance_v3_cbg_tx_crtl_parameters(cbg_per_tb);

    TESTASSERT_EQ(cbg_per_tb, pdu.pdsch_maintenance_v3.max_num_cbg_per_tb);
  }
}

static void test_maintenance_v3_codeword_parameters()
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  TESTASSERT(pdu.cws.empty());
  TESTASSERT(pdu.pdsch_maintenance_v3.cbg_tx_information.empty());

  auto builder_cw = builder.add_codeword();

  uint8_t cbg = 2;

  builder_cw.set_maintenance_v3_parameters(cbg);

  TESTASSERT_EQ(cbg, pdu.pdsch_maintenance_v3.cbg_tx_information[0]);
  TESTASSERT_EQ(1, pdu.cws.size());
  TESTASSERT_EQ(1, pdu.pdsch_maintenance_v3.cbg_tx_information.size());
}

static void test_ptrs_maintenance_v3_tx_power_parameters()
{
  for (int power = -33; power != 1; ++power) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    optional<float> ptrs;
    if (power != -33) {
      ptrs.emplace(power);
    }

    builder.set_ptrs_maintenance_v3_tx_power_info_parameters(ptrs);

    TESTASSERT_EQ(ptrs ? static_cast<int>(ptrs.value() * 1000) : -32768,
                  pdu.ptrs_maintenance_v3.pdsch_ptrs_power_offset_profile_sss);
  }
}

static void test_maintenance_v4_basic_parameters()
{
  for (auto derivation_method : {0, 1}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    // :TODO: Modify the test to check the vector structures when they're defined.
    static_vector<uint8_t, 16> coreset_rm   = {1, 2, 3};
    static_vector<uint8_t, 16> lte_crs      = {1, 2, 3, 4};
    unsigned                   coreset_size = 43;

    builder.set_maintenance_v4_basic_parameters(coreset_size, {coreset_rm}, derivation_method, {lte_crs});

    TESTASSERT_EQ(coreset_size, pdu.pdsch_parameters_v4.coreset_rm_pattern_bitmap_size_by_ref);
    TESTASSERT_EQ(derivation_method, pdu.pdsch_parameters_v4.lte_crs_mbsfn_derivation_method);
    TESTASSERT(coreset_rm == pdu.pdsch_parameters_v4.coreset_rm_pattern_bmp_by_ref);
    TESTASSERT(lte_crs == pdu.pdsch_parameters_v4.lte_crs_mbsfn_pattern);
  }
}

static void test_pdsch_builder_ok()
{
  test_basic_parameters();
  fmt::print("[PDSCH Builder] - Basic parameters -> OK\n");
  test_bwp_parameters();
  fmt::print("[PDSCH Builder] - BWP parameters -> OK\n");
  test_codewords_parameters();
  fmt::print("[PDSCH Builder] - Codewords parameters -> OK\n");
  test_add_codeword();
  fmt::print("[PDSCH Builder] - Added codeword -> OK\n");
  test_codeword_basic_parameters();
  fmt::print("[PDSCH Builder] - Codeword basic parameters -> OK\n");
  test_dmrs_parameters();
  fmt::print("[PDSCH Builder] - DMRS parameters -> OK\n");
  test_allocation_in_freq_parameters();
  fmt::print("[PDSCH Builder] - Allocation in frequency parameters -> OK\n");
  test_allocation_in_time_parameters();
  fmt::print("[PDSCH Builder] - Allocation in time parameters -> OK\n");
  test_tx_power_info_parameters();
  fmt::print("[PDSCH Builder] - Tx power info parameters -> OK\n");
  test_cbg_retx_ctrl_parameters();
  fmt::print("[PDSCH Builder] - CBG ReTx Control parameters -> OK\n");
  test_maintenance_v3_bwp_parameters();
  fmt::print("[PDSCH Builder] - Maintenance v3 BWP parameters -> OK\n");
  test_maintenance_v3_codeword_info_parameters();
  fmt::print("[PDSCH Builder] - Maintenance v3 codeword information parameters -> OK\n");
  test_maintenance_v3_rm_references_parameters();
  fmt::print("[PDSCH Builder] - Maintenance v3 rate matching references parameters -> OK\n");
  test_maintenance_v3_tx_power_info_parameters();
  fmt::print("[PDSCH Builder] - Maintenance v3 Tx power info parameters -> OK\n");
  test_maintenance_v3_cbg_tx_crtl_parameters();
  fmt::print("[PDSCH Builder] - Maintenance v3 CBG tx control parameters -> OK\n");
  test_maintenance_v3_codeword_parameters();
  fmt::print("[PDSCH Builder] - Maintenance v3 codeword parameters -> OK\n");
  test_ptrs_maintenance_v3_tx_power_parameters();
  fmt::print("[PDSCH Builder] - PTRS Maintenance v3 tx power info parameters -> OK\n");
  test_maintenance_v4_basic_parameters();
  fmt::print("[PDSCH Builder] - Maintenance v4 basic parameters -> OK\n");
}

int main()
{
  test_pdsch_builder_ok();
}
