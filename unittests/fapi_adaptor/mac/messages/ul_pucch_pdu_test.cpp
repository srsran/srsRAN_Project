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
#include "srsgnb/fapi_adaptor/mac/messages/pucch.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace fapi_adaptor;
using namespace unittests;

TEST(ULPUCCHFormat1PDUTest, ValidPUCCHFormat1ShouldPass)
{
  const pucch_info&  mac_pdu = build_valid_pucch_format_1_pdu();
  fapi::ul_pucch_pdu fapi_pdu;

  convert_pucch_mac_to_fapi(fapi_pdu, mac_pdu);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp_extended ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL, fapi_pdu.cp);
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
  ASSERT_EQ(not resources.second_hop_prbs.empty() ? true : false, fapi_pdu.intra_slot_frequency_hopping);

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
