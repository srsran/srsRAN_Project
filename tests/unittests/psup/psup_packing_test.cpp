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

#include "srsran/psup/psup_packing.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture class for PSUP packing tests
class psup_packing_test : public ::testing::Test
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

    logger.info("Creating PSUP packing object.");

    // Create packer object
    packer = std::make_unique<psup_packing>(srslog::fetch_basic_logger("GTP-U", false));
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&         logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<psup_packing> packer;
};

TEST_F(psup_packing_test, create_new_entity)
{
  EXPECT_NE(packer, nullptr);
}

TEST_F(psup_packing_test, pack_psup_dl_pdu_session_information_smallest)
{
  psup_dl_pdu_session_information sess_info{};
  sess_info.qos_flow_id = uint_to_qos_flow_id(0x17);
  byte_buffer out_buf;
  packer->pack(out_buf, sess_info);
  const uint8_t expected_vec[] = {
      0x00, // 4PDU Type | QMP | SNP | 2Spare
      0x17, // PPP | RQI | 6QFI
            // No further fields
            // No padding
  };
  EXPECT_EQ(out_buf, byte_buffer::create(expected_vec).value());
}

TEST_F(psup_packing_test, unpack_psup_dl_pdu_session_information_smallest)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | QMP | SNP | 2Spare
      0x13, // PPP | RQI | 6QFI
            // No further fields
            // No padding
  };
  byte_buffer                     packed_buf = byte_buffer::create(packed_vec).value();
  psup_dl_pdu_session_information out_sess_info;
  packer->unpack(out_sess_info, packed_buf);
  psup_dl_pdu_session_information expected_sess_info{};
  expected_sess_info.qos_flow_id = uint_to_qos_flow_id(0x13);
  EXPECT_EQ(out_sess_info, expected_sess_info);
}

TEST_F(psup_packing_test, pack_psup_dl_pdu_session_information_largest)
{
  psup_dl_pdu_session_information sess_info{};
  sess_info.qos_flow_id           = uint_to_qos_flow_id(0x18);
  sess_info.ppi                   = uint_to_psup_ppi(0x1);
  sess_info.dl_sending_time_stamp = 0xffeeddccbbaa9988;
  sess_info.dl_qfi_sn             = 0x996601;
  byte_buffer out_buf;
  packer->pack(out_buf, sess_info);
  const uint8_t expected_vec[] = {
      0x0c, // 4PDU Type | QMP | SNP | 2Spare
      0x98, // PPP | RQI | 6QFI
      0x20, // 3PPI | 5Spare
      0xff, // 64DL Sending Time Stamp
      0xee,
      0xdd,
      0xcc,
      0xbb,
      0xaa,
      0x99,
      0x88,
      0x99, // 24DL QFI Sequence Number
      0x66,
      0x01,
      // No padding
  };
  EXPECT_EQ(out_buf, byte_buffer::create(expected_vec).value());
}

TEST_F(psup_packing_test, unpack_psup_dl_pdu_session_information_largest)
{
  const uint8_t packed_vec[] = {
      0x0c, // 4PDU Type | QMP | SNP | 2Spare
      0x94, // PPP | RQI | 6QFI
      0xa0, // 3PPI | 5Spare
      0x99, // 64DL Sending Time Stamp
      0xee,
      0xdd,
      0xcc,
      0xbb,
      0xaa,
      0x55,
      0x88,
      0xcc, // 24DL QFI Sequence Number
      0x66,
      0x01,
      // No padding
  };
  byte_buffer                     packed_buf = byte_buffer::create(packed_vec).value();
  psup_dl_pdu_session_information out_sess_info;
  packer->unpack(out_sess_info, packed_buf);
  psup_dl_pdu_session_information expected_sess_info{};
  expected_sess_info.qos_flow_id           = uint_to_qos_flow_id(0x14);
  expected_sess_info.ppi                   = uint_to_psup_ppi(0x5);
  expected_sess_info.dl_sending_time_stamp = 0x99eeddccbbaa5588;
  expected_sess_info.dl_qfi_sn             = 0xcc6601;
  EXPECT_EQ(out_sess_info, expected_sess_info);
}

TEST_F(psup_packing_test, pack_unpack_psup_dl_pdu_session_information_grow)
{
  psup_dl_pdu_session_information sess_info_in{};
  psup_dl_pdu_session_information sess_info_out{};
  byte_buffer                     buf_out;
  byte_buffer                     buf_in;

  sess_info_in.qos_flow_id = uint_to_qos_flow_id(0x11);
  buf_out.clear();
  packer->pack(buf_out, sess_info_in);
  EXPECT_TRUE((buf_out.length() + 2) % 4 == 0);
  buf_in = buf_out.deep_copy().value();
  packer->unpack(sess_info_out, buf_in);
  EXPECT_EQ(sess_info_out, sess_info_in);

  sess_info_in.ppi = uint_to_psup_ppi(0x6);
  buf_out.clear();
  packer->pack(buf_out, sess_info_in);
  EXPECT_EQ(buf_out.length(), 6);
  EXPECT_TRUE((buf_out.length() + 2) % 4 == 0);
  buf_in = buf_out.deep_copy().value();
  packer->unpack(sess_info_out, buf_in);
  EXPECT_EQ(sess_info_out, sess_info_in);

  sess_info_in.dl_sending_time_stamp = 0xfedcba9876543210;
  buf_out.clear();
  packer->pack(buf_out, sess_info_in);
  EXPECT_EQ(buf_out.length(), 14);
  EXPECT_TRUE((buf_out.length() + 2) % 4 == 0);
  buf_in = buf_out.deep_copy().value();
  packer->unpack(sess_info_out, buf_in);
  EXPECT_EQ(sess_info_out, sess_info_in);

  sess_info_in.dl_qfi_sn = 0x987654;
  buf_out.clear();
  packer->pack(buf_out, sess_info_in);
  EXPECT_EQ(buf_out.length(), 14);
  EXPECT_TRUE((buf_out.length() + 2) % 4 == 0);
  buf_in = buf_out.deep_copy().value();
  packer->unpack(sess_info_out, buf_in);
  EXPECT_EQ(sess_info_out, sess_info_in);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
