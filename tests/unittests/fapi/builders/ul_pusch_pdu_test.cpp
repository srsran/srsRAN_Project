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

TEST(ul_pusch_pdu_builder, valid_basic_parameters_passes)
{
  rnti_t   rnti   = to_rnti(245);
  unsigned handle = 100021;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_basic_parameters(rnti, handle);

  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
}

TEST(ul_pusch_pdu_builder, valid_bwp_parameters_passes)
{
  unsigned           bwp_start = 100;
  unsigned           bwp_size  = 69;
  subcarrier_spacing scs       = subcarrier_spacing::kHz30;
  cyclic_prefix      cp        = cyclic_prefix::EXTENDED;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_bwp_parameters(bwp_size, bwp_start, scs, cp);

  ASSERT_EQ(bwp_size, pdu.bwp_size);
  ASSERT_EQ(bwp_start, pdu.bwp_start);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(ul_pusch_pdu_builder, valid_information_parameters_passes)
{
  float             target_code_rate    = 32.1f;
  modulation_scheme qam_mod_order       = modulation_scheme::QAM256;
  unsigned          mcs_index           = 4;
  pusch_mcs_table   mcs_table           = pusch_mcs_table::qam256;
  bool              transform_precoding = false;
  unsigned          nid_pusch           = 890;
  unsigned          num_layers          = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_information_parameters(
      target_code_rate, qam_mod_order, mcs_index, mcs_table, transform_precoding, nid_pusch, num_layers);

  ASSERT_EQ(static_cast<uint16_t>(target_code_rate * 10), pdu.target_code_rate);
  ASSERT_EQ(qam_mod_order, pdu.qam_mod_order);
  ASSERT_EQ(mcs_index, pdu.mcs_index);
  ASSERT_EQ(mcs_table, pdu.mcs_table);
  ASSERT_EQ(transform_precoding, pdu.transform_precoding);
  ASSERT_EQ(nid_pusch, pdu.nid_pusch);
  ASSERT_EQ(num_layers, pdu.num_layers);
}

TEST(ul_pusch_pdu_builder, valid_dmrs_parameters_passes)
{
  unsigned           ul_dmrs_sym_pos                     = 98;
  dmrs_config_type   dmrs_type                           = dmrs_config_type::type2;
  unsigned           pusch_dmrs_scrambling_id            = 54;
  unsigned           pusch_dmrs_scrambling_id_complement = 554;
  low_papr_dmrs_type low_papr                            = low_papr_dmrs_type::independent_cdm_group;
  unsigned           pusch_dmrs_identity                 = 567;
  unsigned           nscid                               = 0;
  unsigned           num_dmrs_cdm_no_data                = 2;
  unsigned           dmrs_ports                          = 213;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_dmrs_parameters(ul_dmrs_sym_pos,
                              dmrs_type,
                              pusch_dmrs_scrambling_id,
                              pusch_dmrs_scrambling_id_complement,
                              low_papr,
                              pusch_dmrs_identity,
                              nscid,
                              num_dmrs_cdm_no_data,
                              dmrs_ports);

  ASSERT_EQ(ul_dmrs_sym_pos, pdu.ul_dmrs_symb_pos);
  ASSERT_EQ(dmrs_type == dmrs_config_type::type1 ? dmrs_cfg_type::type_1 : dmrs_cfg_type::type_2, pdu.dmrs_type);
  ASSERT_EQ(pusch_dmrs_scrambling_id, pdu.pusch_dmrs_scrambling_id);
  ASSERT_EQ(pusch_dmrs_scrambling_id_complement, pdu.pusch_dmrs_scrambling_id_complement);
  ASSERT_EQ(low_papr, pdu.low_papr_dmrs);
  ASSERT_EQ(pusch_dmrs_identity, pdu.pusch_dmrs_identity);
  ASSERT_EQ(nscid, pdu.nscid);
  ASSERT_EQ(num_dmrs_cdm_no_data, pdu.num_dmrs_cdm_grps_no_data);
  ASSERT_EQ(dmrs_ports, pdu.dmrs_ports);
}

TEST(ul_pusch_pdu_builder, valid_frequency_allocation_type0_parameters_passes)
{
  std::array<uint8_t, 36> rb_bitmap         = {1, 2, 3, 4, 5};
  unsigned                tx_direct_current = 54;
  bool                    uplink_freq_shift = false;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_allocation_in_frequency_type_0_parameters({rb_bitmap}, tx_direct_current, uplink_freq_shift);

  ASSERT_EQ(vrb_to_prb_mapping_type::non_interleaved, pdu.vrb_to_prb_mapping);
  ASSERT_EQ(resource_allocation_type::type_0, pdu.resource_alloc);
  ASSERT_EQ(tx_direct_current, pdu.tx_direct_current_location);
  ASSERT_EQ(uplink_freq_shift, pdu.uplink_frequency_shift_7p5kHz);
  ASSERT_TRUE(rb_bitmap == pdu.rb_bitmap);
}

TEST(ul_pusch_pdu_builder, valid_frequency_allocation_type1_parameters_passes)
{
  unsigned rb_start          = 127;
  unsigned rb_size           = 13;
  bool     intra_slot_fh     = false;
  unsigned tx_direct_current = 54;
  bool     uplink_freq_shift = false;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_allocation_in_frequency_type_1_parameters(
      rb_start, rb_size, intra_slot_fh, tx_direct_current, uplink_freq_shift);

  ASSERT_EQ(vrb_to_prb_mapping_type::non_interleaved, pdu.vrb_to_prb_mapping);
  ASSERT_EQ(resource_allocation_type::type_1, pdu.resource_alloc);
  ASSERT_EQ(tx_direct_current, pdu.tx_direct_current_location);
  ASSERT_EQ(uplink_freq_shift, pdu.uplink_frequency_shift_7p5kHz);
  ASSERT_EQ(rb_size, pdu.rb_size);
  ASSERT_EQ(rb_start, pdu.rb_start);
  ASSERT_EQ(intra_slot_fh, pdu.intra_slot_frequency_hopping);
}

TEST(ul_pusch_pdu_builder, valid_time_allocation_parameters_passes)
{
  unsigned start_symbol_idx = 4;
  unsigned num_symbols      = 6;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_allocation_in_time_parameters(start_symbol_idx, num_symbols);

  ASSERT_EQ(start_symbol_idx, pdu.start_symbol_index);
  ASSERT_EQ(num_symbols, pdu.nr_of_symbols);
}

TEST(ul_pusch_pdu_builder, valid_data_parameters_passes)
{
  unsigned                                          rv_index        = 2;
  unsigned                                          harq_process_id = 9;
  bool                                              new_data        = false;
  units::bytes                                      tb_size{89898989};
  unsigned                                          num_cb     = 16;
  static_vector<uint8_t, ul_pusch_data::MAX_NUM_CB> cb_present = {1, 5};

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_pusch_data(rv_index, harq_process_id, new_data, tb_size, num_cb, {cb_present});

  ASSERT_TRUE(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_DATA_BIT]);
  const auto& data = pdu.pusch_data;
  ASSERT_EQ(rv_index, data.rv_index);
  ASSERT_EQ(harq_process_id, data.harq_process_id);
  ASSERT_EQ(new_data, data.new_data);
  ASSERT_EQ(tb_size, data.tb_size);
  ASSERT_EQ(num_cb, data.num_cb);
  ASSERT_EQ(cb_present, data.cb_present_and_position);
}

TEST(ul_pusch_pdu_builder, valid_uci_alpha_parameters_passes)
{
  std::vector<alpha_scaling_opt> alpha_scaling_vector = {
      alpha_scaling_opt::f0p5, alpha_scaling_opt::f0p65, alpha_scaling_opt::f0p8, alpha_scaling_opt::f1};

  for (auto alpha_scaling : alpha_scaling_vector) {
    ul_pusch_pdu         pdu;
    ul_pusch_pdu_builder builder(pdu);

    builder.add_optional_pusch_uci_alpha(alpha_scaling);

    ASSERT_TRUE(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_UCI_BIT]);
    const auto& uci = pdu.pusch_uci;
    ASSERT_EQ(0, uci.beta_offset_harq_ack);
    ASSERT_EQ(0, uci.beta_offset_csi1);
    ASSERT_EQ(0, uci.beta_offset_csi2);
    ASSERT_EQ(0, uci.harq_ack_bit_length);
    ASSERT_EQ(0, uci.csi_part1_bit_length);
    ASSERT_EQ(0, uci.flags_csi_part2);
    ASSERT_EQ(alpha_scaling, uci.alpha_scaling);
  }
}

TEST(ul_pusch_pdu_builder, valid_uci_harq_parameters_passes)
{
  unsigned harq_ack_bit_len     = 3;
  unsigned beta_offset_harq_ack = 12;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_pusch_uci_harq(harq_ack_bit_len, beta_offset_harq_ack);

  ASSERT_TRUE(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_UCI_BIT]);
  const auto& uci = pdu.pusch_uci;
  ASSERT_EQ(beta_offset_harq_ack, uci.beta_offset_harq_ack);
  ASSERT_EQ(0, uci.beta_offset_csi1);
  ASSERT_EQ(0, uci.beta_offset_csi2);
  ASSERT_EQ(harq_ack_bit_len, uci.harq_ack_bit_length);
  ASSERT_EQ(0, uci.csi_part1_bit_length);
  ASSERT_EQ(0, uci.flags_csi_part2);
}

TEST(ul_pusch_pdu_builder, valid_uci_csi1_parameters_passes)
{
  unsigned csi_part1_bit_len = 5;
  unsigned beta_offset_csi1  = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_pusch_uci_csi1(csi_part1_bit_len, beta_offset_csi1);

  ASSERT_TRUE(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_UCI_BIT]);
  const auto& uci = pdu.pusch_uci;
  ASSERT_EQ(0, uci.beta_offset_harq_ack);
  ASSERT_EQ(beta_offset_csi1, uci.beta_offset_csi1);
  ASSERT_EQ(0, uci.beta_offset_csi2);
  ASSERT_EQ(0, uci.harq_ack_bit_length);
  ASSERT_EQ(csi_part1_bit_len, uci.csi_part1_bit_length);
  ASSERT_EQ(0, uci.flags_csi_part2);
}

TEST(ul_pusch_pdu_builder, valid_uci_csi2_parameters_passes)
{
  unsigned beta_offset_csi2 = 3;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_pusch_uci_csi2(beta_offset_csi2);

  const auto& uci = pdu.pusch_uci;
  ASSERT_EQ(0, uci.beta_offset_harq_ack);
  ASSERT_EQ(0, uci.beta_offset_csi1);
  ASSERT_EQ(beta_offset_csi2, uci.beta_offset_csi2);
  ASSERT_EQ(0, uci.harq_ack_bit_length);
  ASSERT_EQ(0, uci.csi_part1_bit_length);
  ASSERT_EQ(std::numeric_limits<decltype(ul_pusch_uci::flags_csi_part2)>::max(), uci.flags_csi_part2);
}

TEST(ul_pusch_pdu_builder, valid_pusch_ptrs_parameters_passes)
{
  static_vector<ul_pusch_ptrs::ptrs_port_info, 2> ptr_ports              = {{1, 3, 4}};
  std::vector<unsigned>                           ptrs_time_density_vect = {1, 2, 4};
  std::vector<unsigned>                           ptrs_freq_density_vect = {2, 4};
  ul_ptrs_power_type                              ptrs_power             = ul_ptrs_power_type::dB4_77;

  for (auto ptrs_time : ptrs_time_density_vect)
    for (auto ptrs_freq : ptrs_freq_density_vect) {
      {
        ul_pusch_pdu         pdu;
        ul_pusch_pdu_builder builder(pdu);

        builder.add_optional_pusch_ptrs({ptr_ports}, ptrs_time, ptrs_freq, ptrs_power);

        ASSERT_TRUE(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_PTRS_BIT]);
        const auto& ptrs = pdu.pusch_ptrs;
        ASSERT_EQ(ptrs_power, ptrs.ul_ptrs_power);
        ASSERT_EQ(ptrs_freq / 4, ptrs.ptrs_freq_density);
        ASSERT_EQ(ptrs_time / 2, ptrs.ptrs_time_density);
        const auto& info = ptrs.port_info.back();
        ASSERT_EQ(1U, ptrs.port_info.size());
        ASSERT_EQ(4, info.ptrs_re_offset);
        ASSERT_EQ(1, info.ptrs_port_index);
        ASSERT_EQ(3, info.ptrs_dmrs_port);
      }
    }
}

TEST(ul_pusch_pdu_builder, valid_dfts_ofdm_parameters_passes)
{
  unsigned low_papr_group_num     = 11;
  unsigned low_papr_sequ_num      = 323;
  unsigned ul_ptrs_sample_density = 3;
  unsigned ul_ptrs_time_density   = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_dfts_ofdm(low_papr_group_num, low_papr_sequ_num, ul_ptrs_sample_density, ul_ptrs_time_density);

  ASSERT_TRUE(pdu.pdu_bitmap[ul_pusch_pdu::DFTS_OFDM_BIT]);
  const auto& ofdm = pdu.pusch_ofdm;
  ASSERT_EQ(ul_ptrs_sample_density, ofdm.ul_ptrs_sample_density);
  ASSERT_EQ(ul_ptrs_time_density, ofdm.ul_ptrs_time_density_transform_precoding);
  ASSERT_EQ(low_papr_sequ_num, ofdm.low_papr_sequence_number);
  ASSERT_EQ(low_papr_group_num, ofdm.low_papr_group_number);
}

TEST(ul_pusch_pdu_builder, valid_maintenance_v3_bwp_parameters_passes)
{
  unsigned trans_type     = 2;
  unsigned delta_bwp      = 113;
  unsigned initial_ul_bwp = 234;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_maintenance_v3_bwp_parameters(trans_type, delta_bwp, initial_ul_bwp);

  const auto& v3 = pdu.pusch_maintenance_v3;
  ASSERT_EQ(trans_type, v3.pusch_trans_type);
  ASSERT_EQ(initial_ul_bwp, v3.initial_ul_bwp_size);
  ASSERT_EQ(delta_bwp, v3.delta_bwp0_start_from_active_bwp);
}

TEST(ul_pusch_pdu_builder, valid_maintenance_v3_dmrs_parameters_passes)
{
  unsigned group_or_sequence = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_maintenance_v3_dmrs_parameters(group_or_sequence);

  const auto& v3 = pdu.pusch_maintenance_v3;
  ASSERT_EQ(group_or_sequence, v3.group_or_sequence_hopping);
}

TEST(ul_pusch_pdu_builder, valid_maintenance_v3_freq_allocation_parameters_passes)
{
  unsigned             pusch_second_hop = 2;
  ldpc_base_graph_type ldcp_graph       = ldpc_base_graph_type::BG2;
  units::bytes         tb_size{3232};

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_maintenance_v3_frequency_allocation_parameters(pusch_second_hop, ldcp_graph, tb_size);

  const auto& v3 = pdu.pusch_maintenance_v3;
  ASSERT_EQ(tb_size, v3.tb_size_lbrm_bytes);
  ASSERT_EQ(ldcp_graph, v3.ldpc_base_graph);
  ASSERT_EQ(pusch_second_hop, v3.pusch_second_hop_prb);
}

TEST(ul_pusch_pdu_builder, valid_maintenance_v3_basic_parameters_passes)
{
  bool cb_crc_status = true;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_parameters_v4_basic_parameters(cb_crc_status);

  const auto& v4 = pdu.pusch_params_v4;
  ASSERT_EQ(cb_crc_status, v4.cb_crc_status_request);
}

TEST(ul_pusch_pdu_builder, valid_uci_part1_part2_correspondence_parameters_passes)
{
  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  ASSERT_TRUE(pdu.uci_correspondence.part2.empty());

  unsigned nof_part2 = 2;
  for (unsigned i = 0; i != nof_part2; ++i) {
    unsigned                                             priority             = 3 * (i + 1);
    static_vector<uint16_t, 4>                           offset               = {1, 2, 3};
    static_vector<uint8_t, 4>                            size                 = {6, 3, 2};
    unsigned                                             part2_size_map_index = 31 * (i + 1);
    uci_part1_to_part2_correspondence_v3::map_scope_type part2_size_map_scope =
        (i) ? uci_part1_to_part2_correspondence_v3::map_scope_type::common_context
            : uci_part1_to_part2_correspondence_v3::map_scope_type::phy_context;

    builder.add_uci_part1_part2_corresnpondence_v3(
        priority, {offset}, {size}, part2_size_map_index, part2_size_map_scope);

    const auto& correspondence = pdu.uci_correspondence.part2.back();
    ASSERT_EQ(i + 1, pdu.uci_correspondence.part2.size());
    ASSERT_EQ(priority, correspondence.priority);
    ASSERT_EQ(part2_size_map_scope, correspondence.part2_size_map_scope);
    ASSERT_EQ(part2_size_map_index, correspondence.part2_size_map_index);
    ASSERT_EQ(priority, correspondence.priority);
    ASSERT_EQ(offset, correspondence.param_offsets);
    ASSERT_EQ(size, correspondence.param_sizes);
  }

  ASSERT_EQ(nof_part2, pdu.uci_correspondence.part2.size());
}
