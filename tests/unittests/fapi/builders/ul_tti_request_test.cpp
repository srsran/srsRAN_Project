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

TEST(ul_tti_request_builder, valid_basic_parameters_passes)
{
  unsigned slot = 13;
  unsigned sfn  = 599;

  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  builder.set_basic_parameters(sfn, slot);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_TRUE(msg.pdus.empty());
}

TEST(ul_tti_request_builder, add_prach_pdu_passes)
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_pdu_type::PRACH)]);

  pci_t             pci                = 3;
  uint8_t           num_occasions      = 4;
  prach_format_type format_type        = prach_format_type::one;
  uint8_t           index_fd_ra        = 5;
  uint8_t           prach_start_symbol = 5;
  uint16_t          num_cs             = 15;

  builder.add_prach_pdu(pci, num_occasions, format_type, index_fd_ra, prach_start_symbol, num_cs);

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_pdu_type::PRACH)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(ul_pdu_type::PRACH, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().prach_pdu;
  ASSERT_EQ(pci, pdu.phys_cell_id);
  ASSERT_EQ(format_type, pdu.prach_format);
  ASSERT_EQ(num_occasions, pdu.num_prach_ocas);
  ASSERT_EQ(index_fd_ra, pdu.index_fd_ra);
  ASSERT_EQ(prach_start_symbol, pdu.prach_start_symbol);
  ASSERT_EQ(num_cs, pdu.num_cs);
  ASSERT_EQ(0, pdu.is_msg_a_prach);
  ASSERT_FALSE(pdu.has_msg_a_pusch_beamforming);
}

TEST(ul_tti_request_builder, add_pucch_f0_pdu_passes)
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);

  rnti_t       rnti   = to_rnti(3);
  uint32_t     handle = 3214;
  pucch_format format = pucch_format::FORMAT_0;

  builder.add_pucch_pdu(rnti, handle, format);

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(format, pdu.format_type);
}

TEST(ul_tti_request_builder, add_pucch_f1_pdu_passes)
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);

  rnti_t       rnti   = to_rnti(3);
  uint32_t     handle = 3214;
  pucch_format format = pucch_format::FORMAT_1;

  builder.add_pucch_pdu(rnti, handle, format);

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(format, pdu.format_type);
}

TEST(ul_tti_request_builder, add_pucch_f2_pdu_passes)
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);

  rnti_t       rnti   = to_rnti(3);
  uint32_t     handle = 3214;
  pucch_format format = pucch_format::FORMAT_2;

  builder.add_pucch_pdu(rnti, handle, format);

  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);
  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(format, pdu.format_type);
}

TEST(ul_tti_request_builder, add_pucch_f3_pdu_passes)
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);

  rnti_t       rnti   = to_rnti(3);
  uint32_t     handle = 3214;
  pucch_format format = pucch_format::FORMAT_3;

  builder.add_pucch_pdu(rnti, handle, format);

  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);
  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(format, pdu.format_type);
}

TEST(ul_tti_request_builder, add_pucch_f4_pdu_passes)
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);

  rnti_t       rnti   = to_rnti(3);
  uint32_t     handle = 3214;
  pucch_format format = pucch_format::FORMAT_4;

  builder.add_pucch_pdu(rnti, handle, format);

  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format01)]);
  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_tti_request_message::pdu_type::PUCCH_format234)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(format, pdu.format_type);
}

TEST(ul_tti_request_builder, add_pusch_pdu_passes)
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());
  ASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_pdu_type::PUSCH)]);

  rnti_t   rnti   = to_rnti(3);
  uint32_t handle = 3214;

  builder.add_pusch_pdu(rnti, handle);

  ASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_pdu_type::PUSCH)]);
  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_EQ(ul_pdu_type::PUSCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pusch_pdu;
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
}
