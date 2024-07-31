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

#include "srsran/nru/nru_packing.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture class for NR-U packing tests
class nru_packing_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init GTP-U logger
    srslog::fetch_basic_logger("GTP-U", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("GTP-U", false).set_hex_dump_max_size(-1);

    logger.info("Creating NR-U packing object.");

    // Create packer object
    packer = std::make_unique<nru_packing>(srslog::fetch_basic_logger("GTP-U", false));
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&        logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<nru_packing> packer;
};

TEST_F(nru_packing_test, create_new_entity)
{
  EXPECT_NE(packer, nullptr);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_smallest)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn = 0x112233;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_dl_flush)
{
  const uint8_t packed_vec[] = {
      0x02, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      0x99, // DL discard NR PDCP PDU SN (upper byte)
      0x88, // DL discard NR PDCP PDU SN (mid byte)
      0x77, // DL discard NR PDCP PDU SN (lower byte)
      // No further fields
      0x00, // Padding
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn             = 0x112233;
  exp_data.dl_discard_pdcp_sn = 0x998877;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_report_polling)
{
  const uint8_t packed_vec[] = {
      0x01, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn         = 0x112233;
  exp_data.report_polling = true;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_request_out_of_sequence_report)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x10, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn                    = 0x112233;
  exp_data.request_out_of_seq_report = true;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_report_delivered)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x08, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      0xee, // DL report NR PDCP PDU SN (upper byte)
      0xdd, // DL report NR PDCP PDU SN (mid byte)
      0xcc, // DL report NR PDCP PDU SN (lower byte)
      // No further fields
      0x00, // Padding
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn            = 0x112233;
  exp_data.report_delivered  = true;
  exp_data.dl_report_pdcp_sn = 0xeeddcc;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_user_data_existence_flag)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x04, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn                   = 0x112233;
  exp_data.user_data_existence_flag = true;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_assist_info_flag)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x02, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn                          = 0x112233;
  exp_data.assist_info_report_polling_flag = true;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_retransmission_flag)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x01, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn              = 0x112233;
  exp_data.retransmission_flag = true;
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_discard_blocks)
{
  const uint8_t packed_vec[] = {
      0x04, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      0x02, // DL discard Number of blocks
      0x44, // DL discard NR PDCP PDU SN start (upper byte)
      0x55, // DL discard NR PDCP PDU SN start (mid byte)
      0x66, // DL discard NR PDCP PDU SN start (lower byte)
      0x77, // Discarded Block size
      0xaa, // DL discard NR PDCP PDU SN start (upper byte)
      0xbb, // DL discard NR PDCP PDU SN start (mid byte)
      0xcc, // DL discard NR PDCP PDU SN start (lower byte)
      0xdd, // Discarded Block size
            // No further fields
            // No Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_TRUE(packer->unpack(out_data, packed_buf));
  nru_dl_user_data exp_data{};
  exp_data.nru_sn         = 0x112233;
  exp_data.discard_blocks = nru_pdcp_sn_discard_blocks{{0x445566, 0x77}, {0xaabbcc, 0xdd}};
  EXPECT_EQ(out_data, exp_data);

  // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_data));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_empty)
{
  byte_buffer      packed_buf = {};
  nru_dl_user_data out_data;
  EXPECT_FALSE(packer->unpack(out_data, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_invalid_pdu_type)
{
  const uint8_t packed_vec[] = {
      0xe0, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_FALSE(packer->unpack(out_data, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_missing_padding)
{
  const uint8_t packed_vec[] = {
      0xe0, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
            // No further fields
            // Missing Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_FALSE(packer->unpack(out_data, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_excessive_padding)
{
  const uint8_t packed_vec[] = {
      0xe0, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      // No further fields
      0x00, // Padding
      0x00, // Excessive Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_FALSE(packer->unpack(out_data, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_user_data_with_too_large_nof_discard_blocks)
{
  const uint8_t packed_vec[] = {
      0x04, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 3Spare | Req OoS Rep | Rep Deliv | User Data Ex | Assist Info Rep Poll Flag | Retransmission flag
      0x11, // NR-U Sequence Number (upper byte)
      0x22, // NR-U Sequence Number (mid byte)
      0x33, // NR-U Sequence Number (lower byte)
      0x03, // DL discard Number of blocks - PUT TOO LARGE VALUE
      0x44, // DL discard NR PDCP PDU SN start (upper byte)
      0x55, // DL discard NR PDCP PDU SN start (mid byte)
      0x66, // DL discard NR PDCP PDU SN start (lower byte)
      0x77, // Discarded Block size
      0xaa, // DL discard NR PDCP PDU SN start (upper byte)
      0xbb, // DL discard NR PDCP PDU SN start (mid byte)
      0xcc, // DL discard NR PDCP PDU SN start (lower byte)
      0xdd, // Discarded Block size
            // No further fields
            // No Padding
  };

  byte_buffer      packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_user_data out_data;
  EXPECT_FALSE(packer->unpack(out_data, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_smallest)
{
  const uint8_t packed_vec[] = {
      0x10, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
            // No further fields
            // No Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb = 0x11223344;
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_with_data_rate)
{
  const uint8_t packed_vec[] = {
      0x10, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x08, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0xaa, // Desired data rate (upper byte)
      0xbb, // Desired data rate (upper mid byte)
      0xcc, // Desired data rate (lower mid byte)
      0xdd, // Desired data rate (lower byte)
            // No further fields
            // No Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb = 0x11223344;
  exp_status.desired_data_rate           = 0xaabbccdd;
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_with_lost_sn_range)
{
  const uint8_t packed_vec[] = {
      0x11, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0x01, // Number of lost SN ranges
      0x01, // Start of lost SN range (upper byte)
      0x02, // Start of lost SN range (mid byte)
      0x03, // Start of lost SN range (lower byte)
      0x11, // End of lost SN range (upper byte)
      0x12, // End of lost SN range (mid byte)
      0x13, // End of lost SN range (lower byte)
            // No further fields
      0x00, // Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb = 0x11223344;
  exp_status.lost_nru_sn_ranges          = nru_lost_nru_sn_ranges{{0x010203, 0x111213}};
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_with_hi_dlv_sn)
{
  const uint8_t packed_vec[] = {
      0x14, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0xca, // Highest successfully delivered NR PDCP SN (upper byte)
      0xff, // Highest successfully delivered NR PDCP SN (mid byte)
      0xee, // Highest successfully delivered NR PDCP SN (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb = 0x11223344;
  exp_status.highest_delivered_pdcp_sn   = 0xcaffee;
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_with_hi_tx_sn)
{
  const uint8_t packed_vec[] = {
      0x18, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0xca, // Highest transmitted NR PDCP NR PDCP SN (upper byte)
      0xff, // Highest transmitted NR PDCP NR PDCP SN (mid byte)
      0xee, // Highest transmitted NR PDCP NR PDCP SN (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb = 0x11223344;
  exp_status.highest_transmitted_pdcp_sn = 0xcaffee;
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_with_cause)
{
  const uint8_t packed_vec[] = {
      0x10, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x01, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0x99, // Cause Value
      // No further fields
      0x00, // Padding
      0x00, // Padding
      0x00, // Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb = 0x11223344;
  exp_status.cause_value                 = 0x99;
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_with_hi_dlv_retx_sn)
{
  const uint8_t packed_vec[] = {
      0x10, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x02, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0xca, // Highest successfully delivered retransmitted NR PDCP NR PDCP SN (upper byte)
      0xff, // Highest successfully delivered retransmitted NR PDCP NR PDCP SN (mid byte)
      0xee, // Highest successfully delivered retransmitted NR PDCP NR PDCP SN (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb             = 0x11223344;
  exp_status.highest_delivered_retransmitted_pdcp_sn = 0xcaffee;
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_with_hi_retx_sn)
{
  const uint8_t packed_vec[] = {
      0x10, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x04, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0xca, // Highest retransmitted NR PDCP NR PDCP SN (upper byte)
      0xff, // Highest retransmitted NR PDCP NR PDCP SN (mid byte)
      0xee, // Highest retransmitted NR PDCP NR PDCP SN (lower byte)
      // No further fields
      0x00, // Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_TRUE(packer->unpack(out_status, packed_buf));
  nru_dl_data_delivery_status exp_status{};
  exp_status.desired_buffer_size_for_drb   = 0x11223344;
  exp_status.highest_retransmitted_pdcp_sn = 0xcaffee;
  EXPECT_EQ(out_status, exp_status);

  // // Test packing
  byte_buffer out_buf;
  EXPECT_TRUE(packer->pack(out_buf, exp_status));
  EXPECT_EQ(out_buf, packed_buf);
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_empty)
{
  byte_buffer                 packed_buf = {};
  nru_dl_data_delivery_status out_status;
  EXPECT_FALSE(packer->unpack(out_status, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_invalid_pdu_type)
{
  const uint8_t packed_vec[] = {
      0xaa, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
            // No further fields
            // No Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_FALSE(packer->unpack(out_status, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_missing_padding)
{
  const uint8_t packed_vec[] = {
      0x11, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0x01, // Number of lost SN ranges
      0x01, // Start of lost SN range (upper byte)
      0x02, // Start of lost SN range (mid byte)
      0x03, // Start of lost SN range (lower byte)
      0x11, // End of lost SN range (upper byte)
      0x12, // End of lost SN range (mid byte)
      0x13, // End of lost SN range (lower byte)
            // No further fields
            // Missing Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_FALSE(packer->unpack(out_status, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_excessive_padding)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      // No further fields
      0x00, // Excessive Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_FALSE(packer->unpack(out_status, packed_buf));
}

TEST_F(nru_packing_test, unpack_nru_dl_data_deliv_status_too_large_nof_lost_sn_ranges)
{
  const uint8_t packed_vec[] = {
      0x11, // 4PDU Type | Hi Tx SN ind | Hi Dlv SN ind | Final frame ind | Lost pkg report
      0x00, // 4Spare | Data rate ind | Hi ReTx SN ind | Hi Dlv ReTx SN ind | Cause report
      0x11, // Desired buffer size (upper byte)
      0x22, // Desired buffer size (upper mid byte)
      0x33, // Desired buffer size (lower mid byte)
      0x44, // Desired buffer size (lower byte)
      0x01, // Number of lost SN ranges - PUT TOO LARGE VALUE
            // No further fields
      0x00, // Padding
      0x00, // Padding
      0x00, // Padding
  };

  byte_buffer                 packed_buf = byte_buffer::create(packed_vec).value();
  nru_dl_data_delivery_status out_status;
  EXPECT_FALSE(packer->unpack(out_status, packed_buf));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
