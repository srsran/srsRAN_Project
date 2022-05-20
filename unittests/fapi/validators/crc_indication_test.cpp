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
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;

static void test_crc_indication_validator_ok()
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

    optional<float>    ul_sinr_dB;
    optional<unsigned> timing_advance_offset;
    optional<int>      timing_advance_offset_in_ns;
    optional<float>    rssi_dB;
    optional<float>    rsrp_dB;

    if (i) {
      ul_sinr_dB.emplace(-65);
      timing_advance_offset.emplace(30);
      timing_advance_offset_in_ns.emplace(-12345);
      rssi_dB.emplace(-64);
      rsrp_dB.emplace(-100);
    }

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
                    i);

    validator_report report(0, 0);
    const auto&      result = validate_crc_indication(msg);

    TESTASSERT(result);
  }
}

static void test_crc_indication_validator_error()
{
  crc_indication_message         msg;
  crc_indication_message_builder builder(msg);

  unsigned slot = 15;
  unsigned sfn  = 100;

  builder.set_basic_parameters(sfn, slot);

  uint32_t                                                      handle = 34U;
  rnti_t                                                        rnti   = to_rnti(10);
  optional<uint8_t>                                             rapid;
  uint8_t                                                       harq_id       = 128;
  uint8_t                                                       tb_crc_status = 180;
  static_vector<uint8_t, crc_ind_pdu::MAX_NUM_CB_PER_TTI_BYTES> cb_crc_status;

  optional<float>    ul_sinr_dB;
  optional<unsigned> timing_advance_offset;
  optional<int>      timing_advance_offset_in_ns;
  optional<float>    rssi_dB;
  rssi_dB.emplace(10);
  optional<float> rsrp_dB;

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

  const auto& result = validate_crc_indication(msg);

  TESTASSERT(!result);

  const auto& report = result.error();
  // Assert 3 reports were generated.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_crc_indication_validator_ok();
  test_crc_indication_validator_error();
  fmt::print("CRC.indication message validator -> OK\n");
}
