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

#include "../../../lib/fapi_adaptor/mac/fapi_to_mac_data_msg_translator.h"
#include "srsran/fapi/messages.h"
#include "srsran/support/math_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;

namespace {
class mac_cell_rach_handler_dummy : public mac_cell_rach_handler
{
  mac_rach_indication indication;

public:
  void handle_rach_indication(const mac_rach_indication& rach_ind) override { indication = rach_ind; }

  const mac_rach_indication& get_indication() const { return indication; }
};
} // namespace

class mac_rach_indication_fixture : public testing::TestWithParam<float>
{
  mac_cell_rach_handler_dummy     rach_handler;
  fapi_to_mac_data_msg_translator translator;

protected:
  mac_rach_indication_fixture() : translator(subcarrier_spacing::kHz15)
  {
    translator.set_cell_rach_handler(rach_handler);
  }

  void test_pdu()
  {
    const fapi::rach_indication_message& msg = build_message();
    translator.on_rach_indication(msg);
    check_pdu();
  }

  void test_power_values(float value)
  {
    rssi  = clamp<float>(value, -140.F, 30.F);
    power = clamp<float>(value, -140.F, 30.F);
    snr   = clamp<float>(value, -64.F, 63.F);
    test_pdu();
  }

private:
  unsigned slot            = 0;
  unsigned sfn             = 1;
  float    rssi            = 14.F;
  unsigned slot_index      = 3;
  unsigned start_symbol    = 4;
  unsigned freq_index      = 5;
  float    snr             = 12.F;
  float    power           = 13.F;
  unsigned time_advance_ns = 8;

  fapi::rach_indication_message build_message()
  {
    fapi::rach_indication_message fapi_msg;
    fapi_msg.sfn  = sfn;
    fapi_msg.slot = slot;

    fapi_msg.pdus.emplace_back();
    fapi_msg.num_pdu = fapi_msg.pdus.size();

    fapi::rach_indication_pdu& pdu = fapi_msg.pdus.back();
    pdu.avg_rssi                   = (rssi + 140) * 1000;
    pdu.symbol_index               = start_symbol;
    pdu.slot_index                 = slot_index;
    pdu.ra_index                   = freq_index;
    pdu.preambles.emplace_back();
    fapi::rach_indication_pdu_preamble& preamble = pdu.preambles.back();
    preamble.preamble_snr                        = (snr + 64) * 2;
    preamble.preamble_pwr                        = (power + 140) * 1000;
    preamble.timing_advance_offset_ns            = time_advance_ns;

    return fapi_msg;
  }

  void check_pdu()
  {
    const mac_rach_indication&                msg = rach_handler.get_indication();
    const mac_rach_indication::rach_occasion& occ = msg.occasions.front();
    EXPECT_EQ(start_symbol, occ.start_symbol);
    EXPECT_EQ(slot_index, occ.slot_index);
    EXPECT_EQ(freq_index, occ.frequency_index);
    EXPECT_FLOAT_EQ(rssi, occ.rssi_dB);

    const mac_rach_indication::rach_preamble& pream = occ.preambles.front();
    EXPECT_FLOAT_EQ(snr, pream.snr_dB);
    EXPECT_FLOAT_EQ(power, pream.power_dB);
    EXPECT_EQ(time_advance_ns, std::roundf(pream.time_advance.to_seconds() * 1e9));
  }
};

TEST_P(mac_rach_indication_fixture, PowerRelatedValuesWorks)
{
  test_power_values(GetParam());
}

INSTANTIATE_TEST_SUITE_P(PowerValues, mac_rach_indication_fixture, testing::Values(-140.F, -20.F, 0.F, 30.F, 63.F));

TEST_F(mac_rach_indication_fixture, CorrectMessageConvertsCorrectly)
{
  test_pdu();
}
