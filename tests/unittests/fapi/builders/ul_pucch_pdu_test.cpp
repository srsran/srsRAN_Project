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

TEST(ul_pucch_pdu_builder, valid_basic_parameters_passes)
{
  rnti_t   rnti   = to_rnti(14);
  uint32_t handle = 192;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_basic_parameters(rnti, handle);

  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
}

TEST(ul_pucch_pdu_builder, valid_format_common_parameters_passes)
{
  pucch_format             format_type = pucch_format::FORMAT_1;
  pucch_repetition_tx_slot multi_slot  = srsran::pucch_repetition_tx_slot::no_multi_slot;
  bool                     pi2         = false;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_common_parameters(format_type, multi_slot, pi2);

  ASSERT_EQ(format_type, pdu.format_type);
  ASSERT_EQ(multi_slot, pdu.multi_slot_tx_indicator);
  ASSERT_EQ(pi2, pdu.pi2_bpsk);
}

TEST(ul_pucch_pdu_builder, valid_bwp_parameters_passes)
{
  unsigned           start = 34;
  unsigned           size  = 35;
  subcarrier_spacing scs   = subcarrier_spacing::kHz120;
  cyclic_prefix      cp    = cyclic_prefix::NORMAL;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_bwp_parameters(size, start, scs, cp);

  ASSERT_EQ(start, pdu.bwp_start);
  ASSERT_EQ(size, pdu.bwp_size);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(ul_pucch_pdu_builder, valid_frequency_allocation_parameters_passes)
{
  unsigned start = 10;
  unsigned size  = 8;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_allocation_in_frequency_parameters(start, size);

  ASSERT_EQ(start, pdu.prb_start);
  ASSERT_EQ(size, pdu.prb_size);
}

TEST(ul_pucch_pdu_builder, valid_time_allocation_parameters_passes)
{
  unsigned start_symb  = 7;
  unsigned nof_symbols = 4;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_allocation_in_time_parameters(start_symb, nof_symbols);

  ASSERT_EQ(start_symb, pdu.start_symbol_index);
  ASSERT_EQ(nof_symbols, pdu.nr_of_symbols);
}

TEST(ul_pucch_pdu_builder, valid_hopping_information_parameters_passes)
{
  bool                intra_slot_freq      = true;
  unsigned            second_hop_prb       = 100;
  pucch_group_hopping pucch_grp_hopping    = pucch_group_hopping::NEITHER;
  unsigned            nid_pucch_hopping    = 200;
  unsigned            inicial_cyclic_shift = 7;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_hopping_information_parameters(
      intra_slot_freq, second_hop_prb, pucch_grp_hopping, nid_pucch_hopping, inicial_cyclic_shift);

  ASSERT_EQ(intra_slot_freq, pdu.intra_slot_frequency_hopping);
  ASSERT_EQ(second_hop_prb, pdu.second_hop_prb);
  ASSERT_EQ(pucch_grp_hopping, pdu.pucch_grp_hopping);
  ASSERT_EQ(nid_pucch_hopping, pdu.nid_pucch_hopping);
  ASSERT_EQ(inicial_cyclic_shift, pdu.initial_cyclic_shift);
}

TEST(ul_pucch_pdu_builder, valid_scrambling_parameters_passes)
{
  unsigned nid_scrambling = 500;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);
  builder.set_scrambling_parameters(nid_scrambling);

  ASSERT_EQ(nid_scrambling, pdu.nid_pucch_scrambling);
}

TEST(ul_pucch_pdu_builder, valid_pucch_format1_parameters_passes)
{
  unsigned time_domain_occ = 3;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);
  builder.set_format1_parameters(time_domain_occ);

  ASSERT_EQ(time_domain_occ, pdu.time_domain_occ_index);
}

TEST(ul_pucch_pdu_builder, valid_pucch_format4_parameters_passes)
{
  unsigned pre_dft_idx = 1;
  unsigned pre_dft_len = 2;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_format4_parameters(pre_dft_idx, pre_dft_len);

  ASSERT_EQ(pre_dft_idx, pdu.pre_dft_occ_idx);
  ASSERT_EQ(pre_dft_len, pdu.pre_dft_occ_len);
}

TEST(ul_pucch_pdu_builder, valid_pucch_format2_parameters_passes)
{
  unsigned nid0_dmrs_scrambling = 34567;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_format2_parameters(nid0_dmrs_scrambling);

  ASSERT_EQ(nid0_dmrs_scrambling, pdu.nid0_pucch_dmrs_scrambling);
}

TEST(ul_pucch_pdu_builder, valid_dmrs_parameters_passes)
{
  bool     dmrs_flag       = false;
  unsigned m0_cyclic_shift = 8;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_dmrs_parameters(dmrs_flag, m0_cyclic_shift);

  ASSERT_EQ(dmrs_flag, pdu.add_dmrs_flag);
  ASSERT_EQ(m0_cyclic_shift, pdu.m0_pucch_dmrs_cyclic_shift);
}

TEST(ul_pucch_pdu_builder, valid_bit_length_parameters_passes)
{
  unsigned sr_bit_len        = 3;
  unsigned harq_bit_len      = 190;
  unsigned csi_part1_bit_len = 1024;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_bit_length_parameters(sr_bit_len, harq_bit_len, csi_part1_bit_len);

  ASSERT_EQ(sr_bit_len, pdu.sr_bit_len);
  ASSERT_EQ(harq_bit_len, pdu.bit_len_harq);
  ASSERT_EQ(csi_part1_bit_len, pdu.csi_part1_bit_length);
}

TEST(ul_pucch_pdu_builder, valid_maintenance_v3_basic_parameters_passes)
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

    ASSERT_EQ((max_code_rate) ? static_cast<uint8_t>(max_code_rate.value()) : 255U,
              pdu.pucch_maintenance_v3.max_code_rate);
    ASSERT_EQ((ul_bwp_id) ? static_cast<uint8_t>(ul_bwp_id.value()) : 255U, pdu.pucch_maintenance_v3.ul_bwp_id);
  }
}

TEST(ul_pucch_pdu_builder, valid_uci_part1_part2_parameters_passes)
{
  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

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
    ASSERT_TRUE(offset == correspondence.param_offsets);
    ASSERT_TRUE(size == correspondence.param_sizes);
  }

  ASSERT_EQ(nof_part2, pdu.uci_correspondence.part2.size());
}
