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

#include "pdcp_rx_status_report_test.h"
#include "lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/bit_encoding.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Test correct construction of PDCP status report
/// All PDUs are received before the t-Reordering expires.
TEST_P(pdcp_rx_status_report_test, build_status_report)
{
  uint32_t count = 262143;

  srsran::test_delimit_logger delimiter(
      "RX build status report test, no t-Reordering. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);
  init(GetParam());

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
    pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu)});

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
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu)});

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
}

/// Test correct construction of a truncated PDCP status report (9000 Bytes)
TEST_P(pdcp_rx_status_report_test, build_truncated_status_report)
{
  // this test only applies to 18-bit SNs.
  if (sn_size == pdcp_sn_size::size12bits) {
    return;
  }

  uint32_t count = 262143;

  srsran::test_delimit_logger delimiter(
      "RX build status report test, no t-Reordering. SN_SIZE={} COUNT=[{}, {}]", sn_size, count + 1, count);
  init(GetParam());

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

  byte_buffer test_pdu1;
  get_test_pdu(count + (9000 - 5) * 8, test_pdu1); // Rx PDU with a COUNT value at max capacity of the report
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu1)});

  byte_buffer test_pdu2;
  get_test_pdu(count + 1 + (9000 - 5) * 8, test_pdu2); // Rx PDU with a COUNT value beyond max capacity of the report
  pdcp_rx->handle_pdu(byte_buffer_chain{std::move(test_pdu2)});

  // Check status report in the final state (truncated bitmap present)
  status_report = pdcp_rx->compile_status_report();
  EXPECT_EQ(status_report.length(), 9000);
  {
    bit_decoder dec(status_report);
    uint8_t     hdr_first_byte;
    dec.unpack(hdr_first_byte, 8);
    EXPECT_EQ(hdr_first_byte, 0x00);
    uint32_t hdr_fmc;
    dec.unpack(hdr_fmc, 32);
    EXPECT_EQ(hdr_fmc, count);
    uint8_t bitmap;
    for (uint32_t i = 0; i < (9000 - 5); i++) {
      ASSERT_TRUE(dec.unpack(bitmap, 8));
      if (i < (9000 - 5) - 1) {
        EXPECT_EQ(bitmap, 0xff); // whole bitmap shall be ones (all missing)
      } else {
        EXPECT_EQ(bitmap, 0xfe); // only the last one is received
      }
    }
  }
}

/// Test reception and forwarding of PDCP status report
TEST_P(pdcp_rx_status_report_test, rx_status_report)
{
  init(GetParam());

  pdcp_rx->configure_security(sec_cfg);

  ASSERT_TRUE(test_frame->status_report_queue.empty());

  // Build status report dummy to be forwarded to the TX entity (i.e. the test_frame)
  byte_buffer buf = {};
  bit_encoder enc(buf);

  // Pack PDU header
  enc.pack(to_number(pdcp_dc_field::control), 1);
  enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
  enc.pack(0b0000, 4);

  // Pack something into FMC field
  enc.pack(0xc0cac01a, 32);

  // Pack some bitmap
  enc.pack(0xcafe, 16);

  // Put into PDCP Rx entity
  pdcp_rx->handle_pdu(byte_buffer_chain{buf.deep_copy()});

  // Check the status report was forwared to the Tx entity
  ASSERT_FALSE(test_frame->status_report_queue.empty());
  ASSERT_EQ(test_frame->status_report_queue.front(), buf);
  test_frame->status_report_queue.pop();
  ASSERT_TRUE(test_frame->status_report_queue.empty());
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
std::string test_param_info_to_string(const ::testing::TestParamInfo<pdcp_sn_size>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "{}bit", pdcp_sn_size_to_uint(info.param));
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
