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
using pdu_type = ul_tti_request_message::pdu_type;

static void test_basic_params()
{
  unsigned slot = 13;
  unsigned sfn  = 599;

  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  builder.set_basic_parameters(sfn, slot);

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);
  TESTASSERT(msg.pdus.empty());
}

static void test_add_prach_pdu()
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  TESTASSERT(msg.pdus.empty());
  TESTASSERT_EQ(0, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PRACH)]);

  pci_t             pci                = 3;
  uint8_t           num_occasions      = 4;
  prach_format_type format_type        = prach_format_type::one;
  uint8_t           index_fd_ra        = 5;
  uint8_t           prach_start_symbol = 5;
  uint16_t          num_cs             = 15;

  builder.add_prach_pdu(pci, num_occasions, format_type, index_fd_ra, prach_start_symbol, num_cs);

  TESTASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PRACH)]);
  TESTASSERT_EQ(1U, msg.pdus.size());
  TESTASSERT_EQ(ul_pdu_type::PRACH, msg.pdus.back().pdu_type);

  const auto& pdu = msg.pdus.back().prach_pdu;
  TESTASSERT_EQ(pci, pdu.phys_cell_id);
  TESTASSERT_EQ(format_type, pdu.prach_format);
  TESTASSERT_EQ(num_occasions, pdu.num_prach_ocas);
  TESTASSERT_EQ(index_fd_ra, pdu.index_fd_ra);
  TESTASSERT_EQ(prach_start_symbol, pdu.prach_start_symbol);
  TESTASSERT_EQ(num_cs, pdu.num_cs);
  TESTASSERT_EQ(0, pdu.is_msg_a_prach);
  TESTASSERT(!pdu.has_msg_a_pusch_beamforming);
}

static void test_add_pucch_f0_pdu()
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  TESTASSERT(msg.pdus.empty());
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);

  rnti_t                       rnti       = to_rnti(3);
  uint32_t                     handle     = 3214;
  pucch_format_type            format     = pucch_format_type::f0;
  multi_slot_tx_indicator_type slot_type  = multi_slot_tx_indicator_type::no_multi_slot_transmission;
  bool                         pi_to_bpsk = false;

  builder.add_pucch_pdu(rnti, handle, format, slot_type, pi_to_bpsk);

  TESTASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(1U, msg.pdus.size());
  TESTASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format, pdu.format_type);
  TESTASSERT_EQ(slot_type, pdu.multi_slot_tx_indicator);
  TESTASSERT_EQ(pi_to_bpsk, pdu.pi2_bpsk);
}

static void test_add_pucch_f1_pdu()
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  TESTASSERT(msg.pdus.empty());
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);

  rnti_t                       rnti       = to_rnti(3);
  uint32_t                     handle     = 3214;
  pucch_format_type            format     = pucch_format_type::f1;
  multi_slot_tx_indicator_type slot_type  = multi_slot_tx_indicator_type::no_multi_slot_transmission;
  bool                         pi_to_bpsk = false;

  builder.add_pucch_pdu(rnti, handle, format, slot_type, pi_to_bpsk);

  TESTASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(1U, msg.pdus.size());
  TESTASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format, pdu.format_type);
  TESTASSERT_EQ(slot_type, pdu.multi_slot_tx_indicator);
  TESTASSERT_EQ(pi_to_bpsk, pdu.pi2_bpsk);
}

static void test_add_pucch_f2_pdu()
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  TESTASSERT(msg.pdus.empty());
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);

  rnti_t                       rnti       = to_rnti(3);
  uint32_t                     handle     = 3214;
  pucch_format_type            format     = pucch_format_type::f2;
  multi_slot_tx_indicator_type slot_type  = multi_slot_tx_indicator_type::no_multi_slot_transmission;
  bool                         pi_to_bpsk = false;

  builder.add_pucch_pdu(rnti, handle, format, slot_type, pi_to_bpsk);

  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);
  TESTASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(1U, msg.pdus.size());
  TESTASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format, pdu.format_type);
  TESTASSERT_EQ(slot_type, pdu.multi_slot_tx_indicator);
  TESTASSERT_EQ(pi_to_bpsk, pdu.pi2_bpsk);
}

static void test_add_pucch_f3_pdu()
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  TESTASSERT(msg.pdus.empty());
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);

  rnti_t                       rnti       = to_rnti(3);
  uint32_t                     handle     = 3214;
  pucch_format_type            format     = pucch_format_type::f3;
  multi_slot_tx_indicator_type slot_type  = multi_slot_tx_indicator_type::no_multi_slot_transmission;
  bool                         pi_to_bpsk = false;

  builder.add_pucch_pdu(rnti, handle, format, slot_type, pi_to_bpsk);

  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);
  TESTASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(1U, msg.pdus.size());
  TESTASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format, pdu.format_type);
  TESTASSERT_EQ(slot_type, pdu.multi_slot_tx_indicator);
  TESTASSERT_EQ(pi_to_bpsk, pdu.pi2_bpsk);
}

static void test_add_pucch_f4_pdu()
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  TESTASSERT(msg.pdus.empty());
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);

  rnti_t                       rnti       = to_rnti(3);
  uint32_t                     handle     = 3214;
  pucch_format_type            format     = pucch_format_type::f4;
  multi_slot_tx_indicator_type slot_type  = multi_slot_tx_indicator_type::no_multi_slot_transmission;
  bool                         pi_to_bpsk = false;

  builder.add_pucch_pdu(rnti, handle, format, slot_type, pi_to_bpsk);

  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format01)]);
  TESTASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUCCH_format234)]);
  TESTASSERT_EQ(1U, msg.pdus.size());
  TESTASSERT_EQ(ul_pdu_type::PUCCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pucch_pdu;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(format, pdu.format_type);
  TESTASSERT_EQ(slot_type, pdu.multi_slot_tx_indicator);
  TESTASSERT_EQ(pi_to_bpsk, pdu.pi2_bpsk);
}

static void test_add_pusch_pdu()
{
  ul_tti_request_message         msg;
  ul_tti_request_message_builder builder(msg);

  TESTASSERT(msg.pdus.empty());
  TESTASSERT_EQ(0U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUSCH)]);

  rnti_t   rnti   = to_rnti(3);
  uint32_t handle = 3214;

  builder.add_pusch_pdu(rnti, handle);

  TESTASSERT_EQ(1U, msg.num_pdus_of_each_type[static_cast<unsigned>(pdu_type::PUSCH)]);
  TESTASSERT_EQ(1U, msg.pdus.size());
  TESTASSERT_EQ(ul_pdu_type::PUSCH, msg.pdus.back().pdu_type);
  const auto& pdu = msg.pdus.back().pusch_pdu;
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(handle, pdu.handle);
}

static void test_ul_tti_request_builder_ok()
{
  test_basic_params();
  test_add_prach_pdu();
  test_add_pucch_f0_pdu();
  test_add_pucch_f1_pdu();
  test_add_pucch_f2_pdu();
  test_add_pucch_f3_pdu();
  test_add_pucch_f4_pdu();
  test_add_pusch_pdu();
}

int main()
{
  test_ul_tti_request_builder_ok();
  fmt::print("UL_TTI.request builder -> OK\n");
}
