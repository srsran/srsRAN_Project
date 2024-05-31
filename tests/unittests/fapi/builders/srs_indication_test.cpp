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

TEST(srs_indication_builder, valid_srs_indication_passes)
{
  srs_indication_message         msg;
  srs_indication_message_builder builder(msg);

  unsigned sfn  = 13;
  unsigned slot = 12;
  builder.set_basic_parameters(sfn, slot);

  unsigned handle      = 14;
  rnti_t   rnti        = to_rnti(3);
  auto     pdu_builder = builder.add_srs_pdu(handle, rnti);

  std::optional<unsigned> timing    = 0;
  std::optional<int32_t>  timing_ns = 0;
  srs_usage_mode          srs_usage = srs_usage_mode::codebook;

  pdu_builder.set_metrics_parameters(timing, timing_ns);

  std::array<cf_t, 4> values = {0, 0, 0, 0};
  srs_channel_matrix  matrix(values, 2, 2);
  pdu_builder.set_codebook_report_matrix(matrix);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(1, msg.pdus.size());

  const auto& pdu = msg.pdus.back();
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(rnti, pdu.rnti);

  ASSERT_EQ(timing ? timing.value() : std::numeric_limits<uint16_t>::max(), pdu.timing_advance_offset);
  ASSERT_EQ(timing_ns ? timing_ns.value() : std::numeric_limits<uint32_t>::max(), pdu.timing_advance_offset_ns);
  ASSERT_EQ(srs_usage, pdu.srs_usage);
  ASSERT_EQ(matrix.get_nof_rx_ports(), pdu.matrix.get_nof_rx_ports());
  ASSERT_EQ(matrix.get_nof_tx_ports(), pdu.matrix.get_nof_tx_ports());
}
