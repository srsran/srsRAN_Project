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

TEST(rach_indication_builder, valid_basic_parameters_passes)
{
  rach_indication_message         msg;
  rach_indication_message_builder builder(msg);

  unsigned        slot    = 12;
  unsigned        sfn     = 13;
  unsigned        handle  = 14;
  unsigned        symb_id = 3;
  unsigned        slot_id = 45;
  unsigned        ra_id_d = 4;
  unsigned        index   = 32;
  optional<float> rssi;
  optional<float> rsrp = 0;
  optional<float> snr  = 10;

  builder.set_basic_parameters(sfn, slot);
  auto pdu_builder = builder.add_pdu(handle, symb_id, slot_id, ra_id_d, rssi, rsrp, snr);

  optional<unsigned> timing         = 0;
  optional<uint32_t> timing_ns      = 0;
  optional<float>    preamble_power = 0;
  optional<float>    preamble_snr;

  pdu_builder.add_preamble(index, timing, timing_ns, preamble_power, preamble_snr);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(1, msg.pdus.size());

  const auto& pdu = msg.pdus.back();
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(symb_id, pdu.symbol_index);
  ASSERT_EQ(slot_id, pdu.slot_index);
  ASSERT_EQ(ra_id_d, pdu.ra_index);
  ASSERT_EQ(rssi ? static_cast<unsigned>((rssi.value() + 140.F) * 1000) : std::numeric_limits<uint32_t>::max(),
            pdu.avg_rssi);
  ASSERT_EQ(rsrp ? static_cast<unsigned>((rsrp.value() + 140.F) * 10.F) : std::numeric_limits<uint16_t>::max(),
            pdu.rsrp);
  ASSERT_EQ(snr ? (snr.value() + 64) * 2 : std::numeric_limits<uint8_t>::max(), pdu.avg_snr);
  ASSERT_EQ(1, pdu.preambles.size());

  const auto& preamble = pdu.preambles.back();
  ASSERT_EQ(index, preamble.preamble_index);
  ASSERT_EQ(timing ? timing.value() : std::numeric_limits<uint16_t>::max(), preamble.timing_advance_offset);
  ASSERT_EQ(timing_ns ? timing_ns.value() : std::numeric_limits<uint32_t>::max(), preamble.timing_advance_offset_ns);
  ASSERT_EQ(preamble_power ? static_cast<unsigned>((preamble_power.value() + 140.F) * 1000.F)
                           : std::numeric_limits<uint32_t>::max(),
            preamble.preamble_pwr);
  ASSERT_EQ(preamble_snr ? static_cast<unsigned>((preamble_snr.value() + 64.F) * 2.F)
                         : std::numeric_limits<uint8_t>::max(),
            preamble.preamble_snr);
}
