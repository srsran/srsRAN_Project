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

using namespace srsran;
using namespace fapi;

TEST(ul_dci_request_builder, valid_basic_parameters_passes)
{
  ul_dci_request_message         msg;
  ul_dci_request_message_builder builder(msg);

  unsigned sfn      = 16;
  unsigned slot     = 18;
  unsigned nof_dcis = 0;

  builder.set_basic_parameters(sfn, slot);

  for (unsigned i = 0, e = 10; i != e; ++i) {
    unsigned dci = i;
    nof_dcis += dci;

    builder.add_pdcch_pdu(dci);

    ASSERT_EQ(i + 1, msg.pdus.size());
    ASSERT_EQ(i + 1, msg.num_pdus_of_each_type[static_cast<unsigned>(ul_dci_pdu_type::PDCCH)]);
    ASSERT_EQ(nof_dcis, msg.num_pdus_of_each_type[ul_dci_request_message::DCI_INDEX]);
  }

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
}
