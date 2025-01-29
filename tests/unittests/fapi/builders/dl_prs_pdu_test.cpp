/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

TEST(dl_prs_pdu_builder, valid_basic_parameters_passes)
{
  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  subcarrier_spacing scs = subcarrier_spacing::kHz30;
  cyclic_prefix      cp  = cyclic_prefix::NORMAL;
  builder.set_basic_parameters(scs, cp);

  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(dl_prs_pdu_builder, valid_n_id_parameter_passes)
{
  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  unsigned nid = 10;
  builder.set_n_id(nid);

  ASSERT_EQ(nid, pdu.nid_prs);
}

TEST(dl_prs_pdu_builder, valid_symbol_parameters_passes)
{
  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  prs_num_symbols nof_symbols  = prs_num_symbols::six;
  unsigned        first_symbol = 2;
  builder.set_symbol_parameters(nof_symbols, first_symbol);

  ASSERT_EQ(nof_symbols, pdu.num_symbols);
  ASSERT_EQ(first_symbol, pdu.first_symbol);
}

TEST(dl_prs_pdu_builder, valid_rb_parameters_passes)
{
  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  unsigned nof_rb   = 10;
  unsigned start_rb = 2;
  builder.set_rb_parameters(nof_rb, start_rb);

  ASSERT_EQ(nof_rb, pdu.num_rbs);
  ASSERT_EQ(start_rb, pdu.start_rb);
}

TEST(dl_prs_pdu_builder, valid_power_offset_parameter_passes)
{
  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  std::optional<float> power = -10;
  builder.set_power_offset(power);

  ASSERT_EQ(power, pdu.prs_power_offset);
}

TEST(dl_prs_pdu_builder, valid_comb_parameters_passes)
{
  dl_prs_pdu         pdu;
  dl_prs_pdu_builder builder(pdu);

  prs_comb_size comb_size   = prs_comb_size::twelve;
  unsigned      comb_offset = 2;
  builder.set_comb_parameters(comb_size, comb_offset);

  ASSERT_EQ(comb_size, pdu.comb_size);
  ASSERT_EQ(comb_offset, pdu.comb_offset);
}
