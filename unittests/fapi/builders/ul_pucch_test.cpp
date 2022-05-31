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
  rnti_t                       rnti        = to_rnti(14);
  uint32_t                     handle      = 192;
  pucch_format_type            format_type = pucch_format_type::f1;
  multi_slot_tx_indicator_type multi_slot  = multi_slot_tx_indicator_type::no_multi_slot_transmission;
  bool                         pi2         = false;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_basic_parameters(rnti, handle, format_type, multi_slot, pi2);

  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format_type, pdu.format_type);
  TESTASSERT_EQ(multi_slot, pdu.multi_slot_tx_indicator);
  TESTASSERT_EQ(pi2, pdu.pi2_bpsk);
}

static void test_bwp_params()
{
  unsigned           start = 34;
  unsigned           size  = 35;
  subcarrier_spacing scs   = subcarrier_spacing::kHz120;
  cyclic_prefix_type cp    = cyclic_prefix_type::normal;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_bwp_parameters(size, start, scs, cp);

  TESTASSERT_EQ(start, pdu.bwp_start);
  TESTASSERT_EQ(size, pdu.bwp_size);
  TESTASSERT_EQ(scs, pdu.scs);
  TESTASSERT_EQ(cp, pdu.cyclic_prefix);
}

static void test_allocation_in_freq_params()
{
  unsigned start = 10;
  unsigned size  = 8;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_allocation_in_frequency_parameters(start, size);

  TESTASSERT_EQ(start, pdu.prb_start);
  TESTASSERT_EQ(size, pdu.prb_size);
}

static void test_allocation_in_time_params()
{
  unsigned start_symb  = 7;
  unsigned nof_symbols = 4;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_allocation_in_time_parameters(start_symb, nof_symbols);

  TESTASSERT_EQ(start_symb, pdu.start_symbol_index);
  TESTASSERT_EQ(nof_symbols, pdu.nr_of_symbols);
}

static void test_hopping_information_params()
{
  bool                     intra_slot_freq      = true;
  unsigned                 second_hop_prb       = 100;
  pucch_group_hopping_type pucch_group_hopping  = pucch_group_hopping_type::neither;
  unsigned                 nid_pucch_hopping    = 200;
  unsigned                 inicial_cyclic_shift = 7;
  unsigned                 nid_scrambling       = 500;
  unsigned                 time_domain_occ      = 3;
  unsigned                 pre_dft_idx          = 1;
  unsigned                 pre_dft_len          = 2;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_hopping_information_parameters(intra_slot_freq,
                                             second_hop_prb,
                                             pucch_group_hopping,
                                             nid_pucch_hopping,
                                             inicial_cyclic_shift,
                                             nid_scrambling,
                                             time_domain_occ,
                                             pre_dft_idx,
                                             pre_dft_len);

  TESTASSERT_EQ(intra_slot_freq, pdu.intra_slot_frequency_hopping);
  TESTASSERT_EQ(second_hop_prb, pdu.second_hop_prb);
  TESTASSERT_EQ(pucch_group_hopping, pdu.pucch_group_hopping);
  TESTASSERT_EQ(nid_pucch_hopping, pdu.nid_pucch_hopping);
  TESTASSERT_EQ(inicial_cyclic_shift, pdu.initial_cyclic_shift);
  TESTASSERT_EQ(nid_scrambling, pdu.nid_pucch_scrambling);
  TESTASSERT_EQ(time_domain_occ, pdu.time_domain_occ_index);
  TESTASSERT_EQ(pre_dft_idx, pdu.pre_dft_occ_idx);
  TESTASSERT_EQ(pre_dft_len, pdu.pre_dft_occ_len);
}

static void test_dmrs_params()
{
  bool     dmrs_flag            = false;
  unsigned nid0_dmrs_scrambling = 34567;
  unsigned m0_cyclic_shift      = 8;
  unsigned sr_bit_len           = 3;
  unsigned harq_bit_len         = 190;
  unsigned csi_part1_bit_len    = 1024;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_dmrs_parameters(
      dmrs_flag, nid0_dmrs_scrambling, m0_cyclic_shift, sr_bit_len, harq_bit_len, csi_part1_bit_len);

  TESTASSERT_EQ(dmrs_flag, pdu.add_dmrs_flag);
  TESTASSERT_EQ(nid0_dmrs_scrambling, pdu.nid0_pucch_dmrs_scrambling);
  TESTASSERT_EQ(m0_cyclic_shift, pdu.m0_pucch_dmrs_cyclic_shift);
  TESTASSERT_EQ(sr_bit_len, pdu.sr_bit_len);
  TESTASSERT_EQ(harq_bit_len, pdu.bit_len_harq);
  TESTASSERT_EQ(csi_part1_bit_len, pdu.csi_part1_bit_length);
}

static void test_maintenance_v3_basic_params()
{
  optional<unsigned> max_code_rate;
  optional<unsigned> ul_bwp_id;

  for (unsigned i = 0, e = 2; i != e; ++i) {
    if (i) {
      max_code_rate.emplace(5);
      ul_bwp_id.emplace(3);
    }

    ul_pucch_pdu         pdu;
    ul_pucch_pdu_builder builder(pdu);

    builder.set_maintenance_v3_basic_parameters(max_code_rate, ul_bwp_id);

    TESTASSERT_EQ((max_code_rate) ? static_cast<uint8_t>(max_code_rate.value()) : 255U,
                  pdu.pucch_maintenance_v3.max_code_rate);
    TESTASSERT_EQ((ul_bwp_id) ? static_cast<uint8_t>(ul_bwp_id.value()) : 255U, pdu.pucch_maintenance_v3.ul_bwp_id);
  }
}

static void test_uci_part1_part2_params()
{
  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

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
  test_allocation_in_freq_params();
  test_allocation_in_time_params();
  test_hopping_information_params();
  test_dmrs_params();
  test_maintenance_v3_basic_params();
  test_uci_part1_part2_params();
  fmt::print("UL PUCCH PDU builder -> OK\n");
}
