/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_rx_status_report_test.h"
#include "../../lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/support/bit_encoding.h"
#include "srsgnb/support/test_utils.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

/// Test correct consntruction of PDCP status report
/// All PDUs are received before the t-Reordering expires.
TEST_P(pdcp_rx_status_report_test, build_status_report)
{
  auto test_rx_in_order = [this](uint32_t count) {
    srsgnb::test_delimit_logger delimiter(
        "RX build status report test, no t-Reordering. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);
    init(GetParam(), pdcp_t_reordering::ms10);

    pdcp_rx->set_as_security_config(sec_cfg);
    pdcp_rx->enable_or_disable_security(pdcp_integrity_enabled::no, pdcp_ciphering_enabled::enabled);

    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);

    // Check status report in the initial state (no bitmap present)
    byte_buffer status_report = pdcp_rx->compile_status_report();
    EXPECT_EQ(status_report.length(), 5);
    {
      bit_decoder dec(status_report);
      uint8_t     hdr_first_byte;
      dec.unpack(hdr_first_byte, 8);
      EXPECT_EQ(hdr_first_byte, 0x00);
      uint32_t hdr_fmc;
      dec.unpack(hdr_fmc, 32);
      EXPECT_EQ(hdr_fmc, count);
    }

    for (uint32_t i = count + 5; i > count; i--) {
      byte_buffer test_pdu;
      get_test_pdu(i, test_pdu);
      pdcp_rx->handle_pdu(byte_buffer_slice_chain{std::move(test_pdu)});

      // Check status report while Rx'ing PDUs in reverse order (bitmap present)
      status_report = pdcp_rx->compile_status_report();
      EXPECT_EQ(status_report.length(), 6);
      {
        bit_decoder dec(status_report);
        uint8_t     hdr_first_byte;
        dec.unpack(hdr_first_byte, 8);
        EXPECT_EQ(hdr_first_byte, 0x00);
        uint32_t hdr_fmc;
        dec.unpack(hdr_fmc, 32);
        EXPECT_EQ(hdr_fmc, count);
        uint8_t bitmap;
        dec.unpack(bitmap, 8);
        EXPECT_EQ(bitmap, (0b11110000 << (count + 5 - i)) & 0xff);
      }
    }

    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    pdcp_rx->handle_pdu(byte_buffer_slice_chain{std::move(test_pdu)});

    // Check status report in the final state (no bitmap present)
    status_report = pdcp_rx->compile_status_report();
    EXPECT_EQ(status_report.length(), 5);
    {
      bit_decoder dec(status_report);
      uint8_t     hdr_first_byte;
      dec.unpack(hdr_first_byte, 8);
      EXPECT_EQ(hdr_first_byte, 0x00);
      uint32_t hdr_fmc;
      dec.unpack(hdr_fmc, 32);
      EXPECT_EQ(hdr_fmc, count + 6);
    }
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_rx_in_order(262143);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_rx_in_order(262143);
  } else {
    FAIL();
  }
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
std::string test_param_info_to_string(const ::testing::TestParamInfo<pdcp_sn_size>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}bit", to_number(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_rx_test_all_sn_sizes,
                         pdcp_rx_status_report_test,
                         ::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
