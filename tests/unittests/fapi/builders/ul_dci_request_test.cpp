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
