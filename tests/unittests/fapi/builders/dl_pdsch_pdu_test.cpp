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

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(dl_pdsch_pdu_builder, valid_basic_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  rnti_t rnti = to_rnti(29);
  builder.set_basic_parameters(rnti);

  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(0, pdu.pdu_bitmap[dl_pdsch_pdu::PDU_BITMAP_CBG_RETX_CTRL_BIT]);
  ASSERT_EQ(0, pdu.pdu_bitmap[dl_pdsch_pdu::PDU_BITMAP_PTRS_BIT]);
}

TEST(dl_pdsch_pdu_builder, valid_bwp_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  unsigned           bwp_size  = 128;
  unsigned           bwp_start = 192;
  cyclic_prefix      cprefix   = cyclic_prefix::NORMAL;
  subcarrier_spacing scs       = subcarrier_spacing::kHz15;

  builder.set_bwp_parameters(bwp_size, bwp_start, scs, cprefix);

  ASSERT_EQ(bwp_size, pdu.bwp_size);
  ASSERT_EQ(bwp_start, pdu.bwp_start);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cprefix, pdu.cp);
}

TEST(dl_pdsch_pdu_builder, valid_codewords_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  unsigned             num_layers          = 1;
  unsigned             transmission_scheme = 0;
  pdsch_ref_point_type ref_point           = pdsch_ref_point_type::point_a;
  unsigned             n_id_pdsch          = 469;
  builder.set_codeword_information_parameters(n_id_pdsch, num_layers, transmission_scheme, ref_point);

  ASSERT_EQ(n_id_pdsch, pdu.nid_pdsch);
  ASSERT_EQ(num_layers, pdu.num_layers);
  ASSERT_EQ(transmission_scheme, pdu.transmission_scheme);
  ASSERT_EQ(ref_point, pdu.ref_point);
}

TEST(dl_pdsch_pdu_builder, add_codeword_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  ASSERT_TRUE(pdu.cws.empty());
  ASSERT_TRUE(pdu.pdsch_maintenance_v3.cbg_tx_information.empty());

  builder.add_codeword();
  ASSERT_EQ(1, pdu.cws.size());
  ASSERT_EQ(1, pdu.pdsch_maintenance_v3.cbg_tx_information.size());
}

TEST(dl_pdsch_pdu_builder, valid_codeword_basic_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  auto builder_cw = builder.add_codeword();

  uint16_t     target_code_rate = 6;
  uint8_t      qam_mod          = 2;
  uint8_t      mcs_index        = 2;
  uint8_t      mcs_table        = 1;
  uint8_t      rv_index         = 2;
  units::bytes tb_size{128};

  builder_cw.set_basic_parameters(target_code_rate, qam_mod, mcs_index, mcs_table, rv_index, tb_size);

  ASSERT_EQ(mcs_table, pdu.cws[0].mcs_table);
  ASSERT_EQ(tb_size, pdu.cws[0].tb_size);
  ASSERT_EQ(mcs_index, pdu.cws[0].mcs_index);
  ASSERT_EQ(rv_index, pdu.cws[0].rv_index);
  ASSERT_EQ(qam_mod, pdu.cws[0].qam_mod_order);
  ASSERT_EQ(target_code_rate * 10, pdu.cws[0].target_code_rate);
}

TEST(dl_pdsch_pdu_builder, valid_dmrs_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  unsigned           dmrs_scrambling_id            = 20;
  unsigned           dmrs_scrambling_id_complement = 30;
  uint16_t           dmrs_symbol_pos               = 3;
  uint16_t           dmrs_ports                    = 4;
  unsigned           num_dmrs_cdm_grp_no_data      = 2;
  unsigned           nscid                         = 1;
  low_papr_dmrs_type low_papr                      = low_papr_dmrs_type::dependent_cdm_group;
  dmrs_config_type   config_type                   = dmrs_config_type::type1;

  builder.set_dmrs_parameters(dmrs_symbol_pos,
                              config_type,
                              dmrs_scrambling_id,
                              dmrs_scrambling_id_complement,
                              low_papr,
                              nscid,
                              num_dmrs_cdm_grp_no_data,
                              dmrs_ports);

  ASSERT_EQ(dmrs_symbol_pos, pdu.dl_dmrs_symb_pos);
  ASSERT_EQ(config_type == dmrs_config_type::type1 ? dmrs_cfg_type::type_1 : dmrs_cfg_type::type_2, pdu.dmrs_type);
  ASSERT_EQ(dmrs_scrambling_id, pdu.pdsch_dmrs_scrambling_id);
  ASSERT_EQ(dmrs_scrambling_id_complement, pdu.pdsch_dmrs_scrambling_id_compl);
  ASSERT_EQ(low_papr, pdu.low_papr_dmrs);
  ASSERT_EQ(nscid, pdu.nscid);
  ASSERT_EQ(num_dmrs_cdm_grp_no_data, pdu.num_dmrs_cdm_grps_no_data);
  ASSERT_EQ(dmrs_ports, pdu.dmrs_ports);
}

TEST(dl_pdsch_pdu_builder, valid_freq_allocation_type_0_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  std::vector<uint8_t>    bitmap(3, 1);
  vrb_to_prb_mapping_type vrb_to_prb = vrb_to_prb_mapping_type::non_interleaved;
  builder.set_pdsch_allocation_in_frequency_type_0(span<const uint8_t>(bitmap), vrb_to_prb);

  ASSERT_EQ(vrb_to_prb, pdu.vrb_to_prb_mapping);
}

TEST(dl_pdsch_pdu_builder, valid_freq_allocation_type_1_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  vrb_to_prb_mapping_type vrb_to_prb = vrb_to_prb_mapping_type::interleaved_rb_size2;
  uint16_t                rb_start   = 15;
  uint16_t                rb_size    = 20;

  builder.set_pdsch_allocation_in_frequency_type_1(rb_start, rb_size, vrb_to_prb);

  ASSERT_EQ(vrb_to_prb, pdu.vrb_to_prb_mapping);
  ASSERT_EQ(rb_start, pdu.rb_start);
  ASSERT_EQ(rb_size, pdu.rb_size);
}

TEST(dl_pdsch_pdu_builder, valid_time_allocation_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  uint8_t start_symb = 4;
  uint8_t nof_symb   = 8;

  builder.set_pdsch_allocation_in_time_parameters(start_symb, nof_symb);

  ASSERT_EQ(start_symb, pdu.start_symbol_index);
  ASSERT_EQ(nof_symb, pdu.nr_of_symbols);
}

TEST(dl_pdsch_pdu_builder, valid_tx_power_info_parameters_passes)
{
  for (int power : {-8, 16}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    nzp_csi_rs_epre_to_ssb ss_profile = nzp_csi_rs_epre_to_ssb::dB0;
    optional<int>          p;
    if (power != 16) {
      p.emplace(power);
    }

    builder.set_tx_power_info_parameters(p, ss_profile);

    ASSERT_EQ(p ? static_cast<unsigned>(p.value() + 8) : 255, pdu.power_control_offset_profile_nr);
    ASSERT_EQ(ss_profile, pdu.power_control_offset_ss_profile_nr);
  }
}

TEST(dl_pdsch_pdu_builder, valid_cdb_retx_ctrl_parameters_passes)
{
  for (unsigned last_cb = 0; last_cb != 4; ++last_cb) {
    for (auto inline_tb_crc : {inline_tb_crc_type::control_message, inline_tb_crc_type::data_payload}) {
      dl_pdsch_pdu         pdu;
      dl_pdsch_pdu_builder builder(pdu);

      std::vector<uint32_t> dl_tb_crc(2, 1);

      builder.set_cbg_re_tx_ctrl_parameters(
          last_cb & 1U, (last_cb >> 1) & 1U, inline_tb_crc, span<const uint32_t>(dl_tb_crc));

      ASSERT_EQ(last_cb, pdu.is_last_cb_present);
      ASSERT_EQ(inline_tb_crc, pdu.is_inline_tb_crc);
      ASSERT_EQ(1, pdu.pdu_bitmap[dl_pdsch_pdu::PDU_BITMAP_CBG_RETX_CTRL_BIT]);
    }
  }
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  pdsch_trans_type trans_type          = pdsch_trans_type::interleaved_common_any_coreset0_not_present;
  uint16_t         initial_dl_bwp_size = 16;
  uint16_t         start_point         = 20;

  builder.set_maintenance_v3_bwp_parameters(trans_type, start_point, initial_dl_bwp_size);

  ASSERT_EQ(trans_type, pdu.pdsch_maintenance_v3.trans_type);
  ASSERT_EQ(start_point, pdu.pdsch_maintenance_v3.coreset_start_point);
  ASSERT_EQ(initial_dl_bwp_size, pdu.pdsch_maintenance_v3.initial_dl_bwp_size);
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_codeword_info_parameters_passes)
{
  for (unsigned tb_crc = 0; tb_crc != 4; ++tb_crc) {
    for (auto graph_type : {ldpc_base_graph_type::BG2, ldpc_base_graph_type::BG1}) {
      dl_pdsch_pdu         pdu;
      dl_pdsch_pdu_builder builder(pdu);

      units::bytes tb_size_lbrm_bytes{15};
      builder.set_maintenance_v3_codeword_parameters(graph_type, tb_size_lbrm_bytes, tb_crc & 1U, (tb_crc >> 1) & 1U);

      ASSERT_EQ(tb_crc, pdu.pdsch_maintenance_v3.tb_crc_required);
      ASSERT_EQ(tb_size_lbrm_bytes, pdu.pdsch_maintenance_v3.tb_size_lbrm_bytes);
      ASSERT_EQ(graph_type, pdu.pdsch_maintenance_v3.ldpc_base_graph);
    }
  }
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_rm_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  std::vector<uint16_t> ssb_pdus(3, 1);
  std::vector<uint8_t>  prb_pdus(3, 1);
  std::vector<uint8_t>  lte_pdus(3, 1);

  uint16_t ssb_config = 10;
  uint16_t pdcch      = 12;
  uint16_t dci        = 15;
  unsigned prb_size   = 54;
  unsigned lte_size   = 3;

  builder.set_maintenance_v3_rm_references_parameters(
      {ssb_pdus}, ssb_config, prb_size, {prb_pdus}, pdcch, dci, lte_size, {lte_pdus});

  ASSERT_EQ(ssb_config, pdu.pdsch_maintenance_v3.ssb_config_for_rate_matching);
  ASSERT_EQ(pdcch, pdu.pdsch_maintenance_v3.pdcch_pdu_index);
  ASSERT_EQ(dci, pdu.pdsch_maintenance_v3.dci_index);
  ASSERT_EQ(prb_size, pdu.pdsch_maintenance_v3.prb_sym_rm_pattern_bitmap_size_byref);
  ASSERT_EQ(lte_size, pdu.pdsch_maintenance_v3.lte_crs_rm_pattern_bitmap_size);
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_csi_rm_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  static_vector<uint16_t, 16> csi = {0, 1, 2};

  builder.set_maintenance_v3_csi_rm_references({csi});

  ASSERT_EQ(csi, pdu.pdsch_maintenance_v3.csi_for_rm);
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_tx_power_info_parameters_passes)
{
  for (int power : {-33, -10, 0}) {
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

    ASSERT_EQ(dmrs ? static_cast<int>(dmrs.value() * 1000) : -32768,
              pdu.pdsch_maintenance_v3.pdsch_dmrs_power_offset_profile_sss);
    ASSERT_EQ(data ? static_cast<int>(data.value() * 1000) : -32768,
              pdu.pdsch_maintenance_v3.pdsch_data_power_offset_profile_sss);
  }
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_cbg_tx_crtl_parameters_passes)
{
  for (auto cbg_per_tb : {2U, 4U, 6U, 8U}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    builder.set_maintenance_v3_cbg_tx_crtl_parameters(cbg_per_tb);

    ASSERT_EQ(cbg_per_tb, pdu.pdsch_maintenance_v3.max_num_cbg_per_tb);
  }
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_codeword_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  ASSERT_TRUE(pdu.cws.empty());
  ASSERT_TRUE(pdu.pdsch_maintenance_v3.cbg_tx_information.empty());

  auto builder_cw = builder.add_codeword();

  uint8_t cbg = 2;

  builder_cw.set_maintenance_v3_parameters(cbg);

  ASSERT_EQ(cbg, pdu.pdsch_maintenance_v3.cbg_tx_information[0]);
  ASSERT_EQ(1, pdu.cws.size());
  ASSERT_EQ(1, pdu.pdsch_maintenance_v3.cbg_tx_information.size());
}

TEST(dl_pdsch_pdu_builder, valid_ptrs_maintenance_v3_tx_power_parameters_passes)
{
  for (int power : {-33, 0}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    optional<float> ptrs;
    if (power != -33) {
      ptrs.emplace(power);
    }

    builder.set_ptrs_maintenance_v3_tx_power_info_parameters(ptrs);

    ASSERT_EQ(ptrs ? static_cast<int>(ptrs.value() * 1000) : -32768,
              pdu.ptrs_maintenance_v3.pdsch_ptrs_power_offset_profile_sss);
  }
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v4_basica_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  static_vector<uint8_t, 16> coreset_rm        = {1, 2, 3};
  static_vector<uint8_t, 16> lte_crs           = {1, 2, 3, 4};
  unsigned                   coreset_size      = 43;
  unsigned                   derivation_method = 0;

  builder.set_maintenance_v4_basic_parameters(coreset_size, {coreset_rm}, derivation_method, {lte_crs});

  ASSERT_EQ(coreset_size, pdu.pdsch_parameters_v4.coreset_rm_pattern_bitmap_size_by_ref);
  ASSERT_EQ(derivation_method, pdu.pdsch_parameters_v4.lte_crs_mbsfn_derivation_method);
  ASSERT_EQ(coreset_rm, pdu.pdsch_parameters_v4.coreset_rm_pattern_bmp_by_ref);
  ASSERT_EQ(lte_crs, pdu.pdsch_parameters_v4.lte_crs_mbsfn_pattern);
}
