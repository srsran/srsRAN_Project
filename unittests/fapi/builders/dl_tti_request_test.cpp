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
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace fapi;

TEST(dl_ssb_pdu_builder, valid_basic_parameters_passes)
{
  unsigned slot     = 13;
  unsigned sfn      = 599;
  unsigned n_groups = 2;

  dl_tti_request_message         msg;
  dl_tti_request_message_builder builder(msg);

  builder.set_basic_parameters(sfn, slot, n_groups);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(n_groups, msg.num_groups);
  ASSERT_TRUE(msg.pdus.empty());
}

TEST(dl_ssb_pdu_builder, add_pdcch_pdu_passes)
{
  dl_tti_request_message         msg;
  dl_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::PDCCH)]);

  unsigned nof_dci_in_pdu = 3;

  builder.add_pdcch_pdu(nof_dci_in_pdu);

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::PDCCH)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(dl_pdu_type::PDCCH, msg.pdus.back().pdu_type);
  ASSERT_EQ(nof_dci_in_pdu, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_tti_request_message::DL_DCI_INDEX)]);
}

TEST(dl_ssb_pdu_builder, add_pdsch_pdu_passes)
{
  dl_tti_request_message         msg;
  dl_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::PDSCH)]);

  builder.add_pdsch_pdu();

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::PDSCH)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(dl_pdu_type::PDSCH, msg.pdus.back().pdu_type);
}

TEST(dl_ssb_pdu_builder, add_ssb_pdu_passes)
{
  dl_tti_request_message         msg;
  dl_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::SSB)]);

  builder.add_ssb_pdu();

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::SSB)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(dl_pdu_type::SSB, msg.pdus.back().pdu_type);
}

TEST(dl_ssb_pdu_builder, add_csi_pdu_passes)
{
  dl_tti_request_message         msg;
  dl_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::CSI_RS)]);

  uint16_t                 start_rb      = 3;
  uint16_t                 nof_rbs       = 4;
  csi_rs_type              type          = csi_rs_type::CSI_RS_NZP;
  uint8_t                  row           = 3;
  bounded_bitset<12>       freq_domain   = {1, 0, 0};
  uint8_t                  symb_l0       = 5;
  uint8_t                  symb_l1       = 2;
  csi_rs_cdm_type          cdm_type      = csi_rs_cdm_type::cdm8_FD2_TD4;
  csi_rs_freq_density_type freq_density  = csi_rs_freq_density_type::dot5_even_RB;
  uint16_t                 scrambling_id = 56;

  builder.add_csi_rs_pdu(
      start_rb, nof_rbs, type, row, freq_domain, symb_l0, symb_l1, cdm_type, freq_density, scrambling_id);

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::CSI_RS)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(dl_pdu_type::CSI_RS, msg.pdus.back().pdu_type);
}
