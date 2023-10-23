/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../../../fapi/validators/helpers.h"
#include "srsran/fapi_adaptor/phy/messages/pucch.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittest;

static void check_context_parameters(const ul_pucch_context&   context,
                                     const fapi::ul_pucch_pdu& fapi_pdu,
                                     unsigned                  sfn,
                                     unsigned                  slot,
                                     pucch_format              format)
{
  ASSERT_EQ(slot_point(to_numerology_value(fapi_pdu.scs), sfn, slot), context.slot);
  ASSERT_EQ(fapi_pdu.rnti, context.rnti);
  ASSERT_EQ(fapi_pdu.format_type, context.format);
}

static void check_context_f0_or_f1_parameters(const ul_pucch_context& context, const fapi::ul_pucch_pdu& fapi_pdu)
{
  ASSERT_TRUE(context.context_f0_f1.has_value());
  const ul_pucch_f0_f1_context& ctx = context.context_f0_f1.value();
  ASSERT_EQ(fapi_pdu.sr_bit_len == 1, ctx.is_sr_opportunity);
}

TEST(FAPIPPHYULPUCCHAdaptorTest, ValidFormat1PDUPass)
{
  fapi::ul_pucch_pdu fapi_pdu = build_valid_ul_pucch_f1_pdu();

  unsigned sfn             = 1U;
  unsigned slot            = 2U;
  unsigned nof_rx_antennas = 1U;

  uplink_processor::pucch_pdu pdu;
  convert_pucch_fapi_to_phy(pdu, fapi_pdu, sfn, slot, nof_rx_antennas);

  // Format 1 custom parameters.
  const pucch_processor::format1_configuration& phy_pdu = pdu.format1;
  ASSERT_EQ(fapi_pdu.start_symbol_index, phy_pdu.start_symbol_index);
  ASSERT_EQ(fapi_pdu.nr_of_symbols, phy_pdu.nof_symbols);
  ASSERT_EQ(fapi_pdu.time_domain_occ_index, phy_pdu.time_domain_occ);
  ASSERT_EQ(fapi_pdu.initial_cyclic_shift, phy_pdu.initial_cyclic_shift);

  // Common parameters.
  ASSERT_EQ(slot_point(to_numerology_value(fapi_pdu.scs), sfn, slot), phy_pdu.slot);
  ASSERT_EQ(fapi_pdu.bwp_start, phy_pdu.bwp_start_rb);
  ASSERT_EQ(fapi_pdu.bwp_size, phy_pdu.bwp_size_rb);
  ASSERT_EQ(fapi_pdu.cp, phy_pdu.cp);

  ASSERT_EQ(fapi_pdu.prb_start, phy_pdu.starting_prb);
  if (phy_pdu.second_hop_prb.has_value()) {
    ASSERT_EQ(fapi_pdu.second_hop_prb, phy_pdu.second_hop_prb.value());
  }
  ASSERT_EQ(fapi_pdu.bit_len_harq, phy_pdu.nof_harq_ack);
  ASSERT_EQ(fapi_pdu.nid_pucch_hopping, phy_pdu.n_id);

  // Ports.
  ASSERT_EQ(nof_rx_antennas, phy_pdu.ports.size());
  for (unsigned i = 0; i != nof_rx_antennas; ++i) {
    ASSERT_EQ(i, phy_pdu.ports[i]);
  }

  // Context parameters.
  check_context_parameters(pdu.context, fapi_pdu, sfn, slot, pucch_format::FORMAT_1);

  // Context Format 0 or Format 1 parameters.
  check_context_f0_or_f1_parameters(pdu.context, fapi_pdu);
}
