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

#include "helpers.h"
#include "srsran/fapi_adaptor/mac/messages/pucch.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_fapi_ul_pucch_format1_pdu_conversor_test, ul_pucch_format1_pdu_valid_should_pass)
{
  const pucch_info_test_helper& pdu_test = build_valid_pucch_format_1_pdu();
  const pucch_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pucch_pdu            fapi_pdu;

  convert_pucch_mac_to_fapi(fapi_pdu, mac_pdu);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.start(), fapi_pdu.bwp_start);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.length(), fapi_pdu.bwp_size);

  ASSERT_EQ(mac_pdu.format, fapi_pdu.format_type);
  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // Resources.
  const pucch_resources& resources = mac_pdu.resources;
  ASSERT_EQ(resources.prbs.start(), fapi_pdu.prb_start);
  ASSERT_EQ(resources.prbs.length(), fapi_pdu.prb_size);
  ASSERT_EQ(resources.symbols.start(), fapi_pdu.start_symbol_index);
  ASSERT_EQ(resources.symbols.length(), fapi_pdu.nr_of_symbols);
  ASSERT_EQ(resources.second_hop_prbs.start(), fapi_pdu.second_hop_prb);
  ASSERT_EQ(!resources.second_hop_prbs.empty(), fapi_pdu.intra_slot_frequency_hopping);

  // Format 1.
  const pucch_format_1& f1 = mac_pdu.format_1;
  ASSERT_EQ(f1.slot_repetition, fapi_pdu.multi_slot_tx_indicator);
  ASSERT_EQ(f1.group_hopping, fapi_pdu.pucch_grp_hopping);
  ASSERT_EQ(f1.n_id_hopping, fapi_pdu.nid_pucch_hopping);
  ASSERT_EQ(f1.initial_cyclic_shift, fapi_pdu.initial_cyclic_shift);
  ASSERT_EQ(static_cast<unsigned>(f1.sr_bits), fapi_pdu.sr_bit_len);
  ASSERT_EQ(f1.harq_ack_nof_bits, fapi_pdu.bit_len_harq);
  ASSERT_EQ(f1.time_domain_occ, fapi_pdu.time_domain_occ_index);
}

TEST(mac_fapi_ul_pucch_format2_pdu_conversor_test, ul_pucch_format2_pdu_valid_should_pass)
{
  const pucch_info_test_helper& pdu_test = build_valid_pucch_format_2_pdu();
  const pucch_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pucch_pdu            fapi_pdu;

  convert_pucch_mac_to_fapi(fapi_pdu, mac_pdu);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.start(), fapi_pdu.bwp_start);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.length(), fapi_pdu.bwp_size);

  ASSERT_EQ(mac_pdu.format, fapi_pdu.format_type);
  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // Resources.
  const pucch_resources& resources = mac_pdu.resources;
  ASSERT_EQ(resources.prbs.start(), fapi_pdu.prb_start);
  ASSERT_EQ(resources.prbs.length(), fapi_pdu.prb_size);
  ASSERT_EQ(resources.symbols.start(), fapi_pdu.start_symbol_index);
  ASSERT_EQ(resources.symbols.length(), fapi_pdu.nr_of_symbols);
  ASSERT_EQ(resources.second_hop_prbs.start(), fapi_pdu.second_hop_prb);
  ASSERT_EQ(!resources.second_hop_prbs.empty(), fapi_pdu.intra_slot_frequency_hopping);

  // Format 2.
  const pucch_format_2& f2 = mac_pdu.format_2;
  ASSERT_EQ(static_cast<unsigned>(f2.max_code_rate), fapi_pdu.pucch_maintenance_v3.max_code_rate);
  ASSERT_EQ(f2.n_id_0_scrambling, fapi_pdu.nid0_pucch_dmrs_scrambling);
  ASSERT_EQ(f2.n_id_scambling, fapi_pdu.nid_pucch_scrambling);
  ASSERT_EQ(static_cast<unsigned>(f2.sr_bits), fapi_pdu.sr_bit_len);
  ASSERT_EQ(f2.harq_ack_nof_bits, fapi_pdu.bit_len_harq);
  ASSERT_EQ(f2.csi_part1_bits, fapi_pdu.csi_part1_bit_length);
}
