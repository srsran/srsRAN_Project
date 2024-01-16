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

  uint16_t                  start_rb      = 3;
  uint16_t                  nof_rbs       = 4;
  csi_rs_type               type          = csi_rs_type::CSI_RS_NZP;
  uint8_t                   row           = 3;
  bounded_bitset<12, false> freq_domain   = {0, 0, 1};
  uint8_t                   symb_l0       = 5;
  uint8_t                   symb_l1       = 2;
  csi_rs_cdm_type           cdm_type      = csi_rs_cdm_type::cdm8_FD2_TD4;
  csi_rs_freq_density_type  freq_density  = csi_rs_freq_density_type::dot5_even_RB;
  uint16_t                  scrambling_id = 56;

  builder.add_csi_rs_pdu(
      start_rb, nof_rbs, type, row, freq_domain, symb_l0, symb_l1, cdm_type, freq_density, scrambling_id);

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(dl_pdu_type::CSI_RS)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(dl_pdu_type::CSI_RS, msg.pdus.back().pdu_type);
}
