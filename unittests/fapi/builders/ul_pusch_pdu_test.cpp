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

using namespace srsgnb;
using namespace fapi;

static void test_basic_params()
{
  rnti_t   rnti   = to_rnti(245);
  unsigned handle = 100021;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_basic_parameters(rnti, handle);

  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
}

static void test_bwp_params()
{
  unsigned           bwp_start = 100;
  unsigned           bwp_size  = 69;
  subcarrier_spacing scs       = subcarrier_spacing::kHz30;
  cyclic_prefix_type cp        = cyclic_prefix_type::extended;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_bwp_parameters(bwp_size, bwp_start, scs, cp);

  TESTASSERT_EQ(bwp_size, pdu.bwp_size);
  TESTASSERT_EQ(bwp_start, pdu.bwp_start);
  TESTASSERT_EQ(scs, pdu.scs);
  TESTASSERT_EQ(cp, pdu.cyclic_prefix);
}

static void test_information_params()
{
  float                target_code_rate    = 32.1f;
  unsigned             qam_mod_order       = 2;
  unsigned             mcs_index           = 4;
  pusch_mcs_table_type mcs_table           = pusch_mcs_table_type::not_qam256;
  bool                 transform_precoding = false;
  unsigned             nid_pusch           = 890;
  unsigned             num_layers          = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_information_parameters(
      target_code_rate, qam_mod_order, mcs_index, mcs_table, transform_precoding, nid_pusch, num_layers);

  TESTASSERT_EQ(static_cast<uint16_t>(target_code_rate * 10), pdu.target_code_rate);
  TESTASSERT_EQ(qam_mod_order, pdu.qam_mod_order);
  TESTASSERT_EQ(mcs_index, pdu.mcs_index);
  TESTASSERT_EQ(mcs_table, pdu.mcs_table);
  TESTASSERT_EQ(transform_precoding, pdu.transform_precoding);
  TESTASSERT_EQ(nid_pusch, pdu.nid_pusch);
  TESTASSERT_EQ(num_layers, pdu.num_layers);
}

static void test_dmrs_params()
{
  unsigned           ul_dmrs_sym_pos                     = 98;
  dmrs_config_type   dmrs_type                           = dmrs_config_type::type_2;
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

  TESTASSERT_EQ(ul_dmrs_sym_pos, pdu.ul_dmrs_symb_pos);
  TESTASSERT_EQ(dmrs_type, pdu.dmrs_type);
  TESTASSERT_EQ(pusch_dmrs_scrambling_id, pdu.pusch_dmrs_scrambling_id);
  TESTASSERT_EQ(pusch_dmrs_scrambling_id_complement, pdu.pusch_dmrs_scrambling_id_complement);
  TESTASSERT_EQ(low_papr, pdu.low_papr_dmrs);
  TESTASSERT_EQ(pusch_dmrs_identity, pdu.pusch_dmrs_identity);
  TESTASSERT_EQ(nscid, pdu.nscid);
  TESTASSERT_EQ(num_dmrs_cdm_no_data, pdu.num_dmrs_cdm_grps_no_data);
  TESTASSERT_EQ(dmrs_ports, pdu.dmrs_ports);
}

static void test_allocation_in_frequency_type0_params()
{
  std::array<uint8_t, 36> rb_bitmap         = {1, 2, 3, 4, 5};
  unsigned                tx_direct_current = 54;
  bool                    uplink_freq_shift = false;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_allocation_in_frequency_type_0_parameters({rb_bitmap}, tx_direct_current, uplink_freq_shift);

  TESTASSERT_EQ(vrb_to_prb_mapping_type::non_interleaved, pdu.vrb_to_prb_mapping);
  TESTASSERT_EQ(resource_allocation_type::type_0, pdu.resource_alloc);
  TESTASSERT_EQ(tx_direct_current, pdu.tx_direct_current_location);
  TESTASSERT_EQ(uplink_freq_shift, pdu.uplink_frequency_shift_7p5kHz);
  TESTASSERT(rb_bitmap == pdu.rb_bitmap);
}

static void test_allocation_in_frequency_type1_params()
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

  TESTASSERT_EQ(vrb_to_prb_mapping_type::non_interleaved, pdu.vrb_to_prb_mapping);
  TESTASSERT_EQ(resource_allocation_type::type_1, pdu.resource_alloc);
  TESTASSERT_EQ(tx_direct_current, pdu.tx_direct_current_location);
  TESTASSERT_EQ(uplink_freq_shift, pdu.uplink_frequency_shift_7p5kHz);
  TESTASSERT_EQ(rb_size, pdu.rb_size);
  TESTASSERT_EQ(rb_start, pdu.rb_start);
  TESTASSERT_EQ(intra_slot_fh, pdu.intra_slot_frequency_hopping);
}

static void test_allocation_in_time_params()
{
  unsigned start_symbol_idx = 4;
  unsigned num_symbols      = 6;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_allocation_in_time_parameters(start_symbol_idx, num_symbols);

  TESTASSERT_EQ(start_symbol_idx, pdu.start_symbol_index);
  TESTASSERT_EQ(num_symbols, pdu.nr_of_symbols);
}

static void test_optional_pusch_data_params()
{
  unsigned                                          rv_index        = 2;
  unsigned                                          harq_process_id = 9;
  unsigned                                          new_data        = 1;
  unsigned                                          tb_size         = 89898989;
  unsigned                                          num_cb          = 16;
  static_vector<uint8_t, ul_pusch_data::MAX_NUM_CB> cb_present      = {1, 5};

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_pusch_data(rv_index, harq_process_id, new_data, tb_size, num_cb, {cb_present});

  TESTASSERT(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_DATA_BIT]);
  const auto& data = pdu.pusch_data;
  TESTASSERT_EQ(rv_index, data.rv_index);
  TESTASSERT_EQ(harq_process_id, data.harq_process_id);
  TESTASSERT_EQ(new_data, data.new_data);
  TESTASSERT_EQ(tb_size, data.tb_size);
  TESTASSERT_EQ(num_cb, data.num_cb);
  TESTASSERT(cb_present == data.cb_present_and_position);
}

static unsigned get_alpha_scaling_value(float alpha_scaling)
{
  if (std::fabs(alpha_scaling - 0.5F) < 0.001)
    return 0U;

  if (std::fabs(alpha_scaling - 0.65F) < 0.001)
    return 1U;

  if (std::fabs(alpha_scaling - 0.8F) < 0.001)
    return 2U;

  if (std::fabs(alpha_scaling - 1.F) < 0.001)
    return 3U;

  srsgnb_assert(0, "Invalid alpha scaling value ({})", alpha_scaling);
  return 0U;
}

static void test_optional_pusch_uci_params()
{
  unsigned           harq_ack_bit_len     = 3;
  unsigned           csi_part1_bit_len    = 5;
  unsigned           flag_csi_part2       = 54;
  std::vector<float> alpha_scaling_vector = {0.5F, 0.65F, 0.8F, 1.F};
  unsigned           beta_offset_harq_ack = 12;
  unsigned           beta_offset_csi1     = 2;
  unsigned           beta_offset_csi2     = 3;

  for (auto alpha_scaling : alpha_scaling_vector) {
    ul_pusch_pdu         pdu;
    ul_pusch_pdu_builder builder(pdu);

    builder.add_optional_pusch_uci(harq_ack_bit_len,
                                   csi_part1_bit_len,
                                   flag_csi_part2,
                                   alpha_scaling,
                                   beta_offset_harq_ack,
                                   beta_offset_csi1,
                                   beta_offset_csi2);

    TESTASSERT(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_UCI_BIT]);
    const auto& uci = pdu.pusch_uci;
    TESTASSERT_EQ(beta_offset_harq_ack, uci.beta_offset_harq_ack);
    TESTASSERT_EQ(beta_offset_csi1, uci.beta_offset_csi1);
    TESTASSERT_EQ(beta_offset_csi2, uci.beta_offset_csi2);
    TESTASSERT_EQ(harq_ack_bit_len, uci.harq_ack_bit_length);
    TESTASSERT_EQ(csi_part1_bit_len, uci.csi_part1_bit_length);
    TESTASSERT_EQ(flag_csi_part2, uci.flags_csi_part2);
    TESTASSERT_EQ(get_alpha_scaling_value(alpha_scaling), uci.alpha_scaling);
  }
}

static void test_optional_pusch_ptrs_params()
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

        TESTASSERT(pdu.pdu_bitmap[ul_pusch_pdu::PUSCH_PTRS_BIT]);
        const auto& ptrs = pdu.pusch_ptrs;
        TESTASSERT_EQ(ptrs_power, ptrs.ul_ptrs_power);
        TESTASSERT_EQ(ptrs_freq / 4, ptrs.ptrs_freq_density);
        TESTASSERT_EQ(ptrs_time / 2, ptrs.ptrs_time_density);
        const auto& info = ptrs.port_info.back();
        TESTASSERT_EQ(1U, ptrs.port_info.size());
        TESTASSERT_EQ(4, info.ptrs_re_offset);
        TESTASSERT_EQ(1, info.ptrs_port_index);
        TESTASSERT_EQ(3, info.ptrs_dmrs_port);
      }
    }
}

static void test_optional_dfts_ofdm_params()
{
  unsigned low_papr_group_num     = 11;
  unsigned low_papr_sequ_num      = 323;
  unsigned ul_ptrs_sample_density = 3;
  unsigned ul_ptrs_time_density   = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_dfts_ofdm(low_papr_group_num, low_papr_sequ_num, ul_ptrs_sample_density, ul_ptrs_time_density);

  TESTASSERT(pdu.pdu_bitmap[ul_pusch_pdu::DFTS_OFDM_BIT]);
  const auto& ofdm = pdu.pusch_ofdm;
  TESTASSERT_EQ(ul_ptrs_sample_density, ofdm.ul_ptrs_sample_density);
  TESTASSERT_EQ(ul_ptrs_time_density, ofdm.ul_ptrs_time_density_transform_precoding);
  TESTASSERT_EQ(low_papr_sequ_num, ofdm.low_papr_sequence_number);
  TESTASSERT_EQ(low_papr_group_num, ofdm.low_papr_group_number);
}

static void test_maintenance_v3_bwp_params()
{
  unsigned trans_type     = 2;
  unsigned delta_bwp      = 113;
  unsigned initial_ul_bwp = 234;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_maintenance_v3_bwp_parameters(trans_type, delta_bwp, initial_ul_bwp);

  const auto& v3 = pdu.pusch_maintenance_v3;
  TESTASSERT_EQ(trans_type, v3.pusch_trans_type);
  TESTASSERT_EQ(initial_ul_bwp, v3.initial_ul_bwp_size);
  TESTASSERT_EQ(delta_bwp, v3.delta_bwp0_start_from_active_bwp);
}

static void test_maintenance_v3_dmrs_params()
{
  unsigned group_or_sequence = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_maintenance_v3_dmrs_parameters(group_or_sequence);

  const auto& v3 = pdu.pusch_maintenance_v3;
  TESTASSERT_EQ(group_or_sequence, v3.group_or_sequence_hopping);
}

static void test_maintenance_v3_freq_allocation_params()
{
  unsigned             pusch_second_hop = 2;
  ldpc_base_graph_type ldcp_graph       = ldpc_base_graph_type::BG2;
  unsigned             tb_size          = 3232;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_maintenance_v3_frequency_allocation_parameters(pusch_second_hop, ldcp_graph, tb_size);

  const auto& v3 = pdu.pusch_maintenance_v3;
  TESTASSERT_EQ(tb_size, v3.tb_size_lbrm_bytes);
  TESTASSERT_EQ(ldcp_graph, v3.ldpc_base_graph);
  TESTASSERT_EQ(pusch_second_hop, v3.pusch_second_hop_prb);
}

static void test_parameters_v3_basic_params()
{
  bool cb_crc_status = true;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_parameters_v4_basic_parameters(cb_crc_status);

  const auto& v4 = pdu.pusch_params_v4;
  TESTASSERT_EQ(cb_crc_status, v4.cb_crc_status_request);
}

static void test_optional_uci_part1_to_part2_correspondence_parameters()
{
  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  TESTASSERT(pdu.uci_correspondence.part2.empty());

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
    TESTASSERT_EQ(i + 1, pdu.uci_correspondence.part2.size());
    TESTASSERT_EQ(priority, correspondence.priority);
    TESTASSERT_EQ(part2_size_map_scope, correspondence.part2_size_map_scope);
    TESTASSERT_EQ(part2_size_map_index, correspondence.part2_size_map_index);
    TESTASSERT_EQ(priority, correspondence.priority);
    TESTASSERT(offset == correspondence.param_offsets);
    TESTASSERT(size == correspondence.param_sizes);
  }

  TESTASSERT_EQ(nof_part2, pdu.uci_correspondence.part2.size());
}

int main()
{
  test_basic_params();
  test_bwp_params();
  test_information_params();
  test_dmrs_params();
  test_allocation_in_frequency_type0_params();
  test_allocation_in_frequency_type1_params();
  test_allocation_in_time_params();
  test_optional_pusch_data_params();
  test_optional_pusch_uci_params();
  test_optional_pusch_ptrs_params();
  test_optional_dfts_ofdm_params();
  test_maintenance_v3_bwp_params();
  test_maintenance_v3_dmrs_params();
  test_maintenance_v3_freq_allocation_params();
  test_parameters_v3_basic_params();
  test_optional_uci_part1_to_part2_correspondence_parameters();
  fmt::print("UL PUSCH PDU builder -> OK\n");
}