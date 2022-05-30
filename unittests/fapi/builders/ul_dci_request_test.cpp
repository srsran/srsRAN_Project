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
#include <random>

using namespace srsgnb;
using namespace fapi;

static std::mt19937 gen(0);

static void test_ul_dci_builder_ok()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  ul_dci_request_message                  msg;
  ul_dci_request_message_builder          builder(msg);

  unsigned sfn      = sfn_dist(gen);
  unsigned slot     = slot_dist(gen);
  unsigned nof_dcis = 0;

  builder.set_basic_parameters(sfn, slot);

  for (unsigned i = 0, e = 10; i != e; ++i) {
    unsigned dci = sfn_dist(gen);
    nof_dcis += dci;

    builder.add_pdcch_pdu(dci);

    TESTASSERT_EQ(i + 1, msg.pdus.size());
    TESTASSERT_EQ(i + 1, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_dci_pdu_type::PDCCH)]);
    TESTASSERT_EQ(nof_dcis, msg.num_pdus_of_each_type[ul_dci_request_message::DCI_INDEX]);
  }

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);
}

int main()
{
  test_ul_dci_builder_ok();
  fmt::print("UL_DCI.request builder -> OK\n");
}
