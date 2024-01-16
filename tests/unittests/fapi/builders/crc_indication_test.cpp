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

TEST(crc_indication_builder, valid_indication_passes)
{
  for (unsigned i = 0; i != 2; ++i) {
    crc_indication_message         msg;
    crc_indication_message_builder builder(msg);

    unsigned slot = 15;
    unsigned sfn  = 100;

    builder.set_basic_parameters(sfn, slot);

    uint32_t                                                      handle = 34U;
    rnti_t                                                        rnti   = to_rnti(10);
    optional<uint8_t>                                             rapid;
    uint8_t                                                       harq_id       = 0;
    uint8_t                                                       tb_crc_status = 0;
    unsigned                                                      num_cb        = 32;
    static_vector<uint8_t, crc_ind_pdu::MAX_NUM_CB_PER_TTI_BYTES> cb_crc_status;

    optional<float> ul_sinr_dB;
    ul_sinr_dB.emplace(-65);

    optional<unsigned> timing_advance_offset;
    timing_advance_offset.emplace(30);

    optional<int> timing_advance_offset_in_ns;
    timing_advance_offset_in_ns.emplace(-12345);

    optional<float> rssi_dB;
    rssi_dB.emplace(-64);

    optional<float> rsrp_dB;
    rsrp_dB.emplace(-100);
    bool use_dB = i;

    builder.add_pdu(handle,
                    rnti,
                    rapid,
                    harq_id,
                    tb_crc_status,
                    num_cb,
                    {cb_crc_status},
                    ul_sinr_dB,
                    timing_advance_offset,
                    timing_advance_offset_in_ns,
                    rssi_dB,
                    rsrp_dB,
                    use_dB);

    ASSERT_EQ(sfn, msg.sfn);
    ASSERT_EQ(slot, msg.slot);

    const crc_ind_pdu& pdu = msg.pdus.back();
    ASSERT_EQ(handle, pdu.handle);
    ASSERT_EQ(num_cb, pdu.num_cb);
    ASSERT_EQ(harq_id, pdu.harq_id);
    ASSERT_EQ(rnti, pdu.rnti);
    ASSERT_EQ(rapid ? rapid.value() : 255, pdu.rapid);
    ASSERT_EQ(tb_crc_status, pdu.tb_crc_status_ok);
    ASSERT_EQ(cb_crc_status, pdu.cb_crc_status);
    ASSERT_EQ(static_cast<int16_t>(ul_sinr_dB ? ul_sinr_dB.value() * 500.F : -32768), pdu.ul_sinr_metric);
    ASSERT_EQ(static_cast<uint16_t>(timing_advance_offset ? timing_advance_offset.value() : 65535),
              pdu.timing_advance_offset);
    ASSERT_EQ(static_cast<int16_t>(timing_advance_offset_in_ns ? timing_advance_offset_in_ns.value() : -32768),
              pdu.timing_advance_offset_ns);
    ASSERT_EQ(static_cast<uint16_t>(rssi_dB ? (rssi_dB.value() + 128.F) * 10.F : 65535), pdu.rssi);
    ASSERT_EQ(static_cast<uint16_t>(rsrp_dB ? (rsrp_dB.value() + (use_dB ? 140.F : 128.F)) * 10.F : 65535), pdu.rsrp);
  }
}

TEST(crc_indication_builder, valid_indication_with_no_metrics_passes)
{
  crc_indication_message         msg;
  crc_indication_message_builder builder(msg);

  unsigned slot = 15;
  unsigned sfn  = 100;

  builder.set_basic_parameters(sfn, slot);

  uint32_t                                                      handle = 34U;
  rnti_t                                                        rnti   = to_rnti(10);
  optional<uint8_t>                                             rapid;
  uint8_t                                                       harq_id       = 0;
  uint8_t                                                       tb_crc_status = 0;
  uint16_t                                                      num_cb        = 21;
  static_vector<uint8_t, crc_ind_pdu::MAX_NUM_CB_PER_TTI_BYTES> cb_crc_status;

  optional<float>    ul_sinr_dB;
  optional<unsigned> timing_advance_offset;
  optional<int>      timing_advance_offset_in_ns;
  optional<float>    rssi_dB;
  optional<float>    rsrp_dB;

  builder.add_pdu(handle,
                  rnti,
                  rapid,
                  harq_id,
                  tb_crc_status,
                  num_cb,
                  {cb_crc_status},
                  ul_sinr_dB,
                  timing_advance_offset,
                  timing_advance_offset_in_ns,
                  rssi_dB,
                  rsrp_dB);

  ASSERT_EQ(sfn, msg.sfn);
  ASSERT_EQ(slot, msg.slot);

  const crc_ind_pdu& pdu = msg.pdus.back();
  ASSERT_EQ(handle, pdu.handle);
  ASSERT_EQ(harq_id, pdu.harq_id);
  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(rapid ? rapid.value() : 255, pdu.rapid);
  ASSERT_EQ(tb_crc_status, pdu.tb_crc_status_ok);
  ASSERT_EQ(cb_crc_status, pdu.cb_crc_status);
  ASSERT_EQ(num_cb, pdu.num_cb);
  ASSERT_EQ(static_cast<int16_t>(ul_sinr_dB ? ul_sinr_dB.value() * 500.F : -32768), pdu.ul_sinr_metric);
  ASSERT_EQ(static_cast<uint16_t>(timing_advance_offset ? timing_advance_offset.value() : 65535),
            pdu.timing_advance_offset);
  ASSERT_EQ(static_cast<int16_t>(timing_advance_offset_in_ns ? timing_advance_offset_in_ns.value() : -32768),
            pdu.timing_advance_offset_ns);
  ASSERT_EQ(static_cast<uint16_t>(rssi_dB ? (rssi_dB.value() + 128) * 10.F : 65535), pdu.rssi);
  ASSERT_EQ(static_cast<uint16_t>(ul_sinr_dB ? ul_sinr_dB.value() * 500.F : 65535), pdu.rsrp);
}
