/**
 *
 * \section COPYRIGHT
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

static void test_crc_indication_builder_ok()
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
                    {cb_crc_status},
                    ul_sinr_dB,
                    timing_advance_offset,
                    timing_advance_offset_in_ns,
                    rssi_dB,
                    rsrp_dB,
                    use_dB);

    TESTASSERT_EQ(sfn, msg.sfn);
    TESTASSERT_EQ(slot, msg.slot);

    const crc_ind_pdu& pdu = msg.pdus.back();
    TESTASSERT_EQ(handle, pdu.handle);
    TESTASSERT_EQ(harq_id, pdu.harq_id);
    TESTASSERT_EQ(rnti, pdu.rnti);
    TESTASSERT_EQ(rapid ? rapid.value() : 255, pdu.rapid);
    TESTASSERT_EQ(tb_crc_status, pdu.tb_crc_status);
    TESTASSERT(cb_crc_status == pdu.cb_crc_status);
    TESTASSERT_EQ(static_cast<int16_t>(ul_sinr_dB ? ul_sinr_dB.value() * 500.F : -32768), pdu.ul_sinr_metric);
    TESTASSERT_EQ(static_cast<uint16_t>(timing_advance_offset ? timing_advance_offset.value() : 65535),
                  pdu.timing_advance_offset);
    TESTASSERT_EQ(static_cast<int16_t>(timing_advance_offset_in_ns ? timing_advance_offset_in_ns.value() : -32768),
                  pdu.timing_advance_offset_ns);
    TESTASSERT_EQ(static_cast<uint16_t>(rssi_dB ? (rssi_dB.value() + 128.F) * 10.F : 65535), pdu.rssi);
    TESTASSERT_EQ(static_cast<uint16_t>(rsrp_dB ? (rsrp_dB.value() + (use_dB ? 140.F : 128.F)) * 10.F : 65535),
                  pdu.rsrp);
  }
}

static void test_crc_indication_builder_ok_no_metrics()
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
                  {cb_crc_status},
                  ul_sinr_dB,
                  timing_advance_offset,
                  timing_advance_offset_in_ns,
                  rssi_dB,
                  rsrp_dB);

  TESTASSERT_EQ(sfn, msg.sfn);
  TESTASSERT_EQ(slot, msg.slot);

  const crc_ind_pdu& pdu = msg.pdus.back();
  TESTASSERT_EQ(handle, pdu.handle);
  TESTASSERT_EQ(harq_id, pdu.harq_id);
  TESTASSERT_EQ(rnti, pdu.rnti);
  TESTASSERT_EQ(rapid ? rapid.value() : 255, pdu.rapid);
  TESTASSERT_EQ(tb_crc_status, pdu.tb_crc_status);
  TESTASSERT(cb_crc_status == pdu.cb_crc_status);
  TESTASSERT_EQ(static_cast<int16_t>(ul_sinr_dB ? ul_sinr_dB.value() * 500.F : -32768), pdu.ul_sinr_metric);
  TESTASSERT_EQ(static_cast<uint16_t>(timing_advance_offset ? timing_advance_offset.value() : 65535),
                pdu.timing_advance_offset);
  TESTASSERT_EQ(static_cast<int16_t>(timing_advance_offset_in_ns ? timing_advance_offset_in_ns.value() : -32768),
                pdu.timing_advance_offset_ns);
  TESTASSERT_EQ(static_cast<uint16_t>(rssi_dB ? (rssi_dB.value() + 128) * 10.F : 65535), pdu.rssi);
  TESTASSERT_EQ(static_cast<uint16_t>(ul_sinr_dB ? ul_sinr_dB.value() * 500.F : 65535), pdu.rsrp);
}

int main()
{
  test_crc_indication_builder_ok();
  test_crc_indication_builder_ok_no_metrics();
  fmt::print("CRC.indication message builder -> OK\n");
}
