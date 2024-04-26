/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/nrup/nrup_packing.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Fixture class for MRUP packing tests
class nrup_packing_test : public ::testing::Test
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

    logger.info("Creating NRUP packing object.");

    // Create packer object
    packer = std::make_unique<nrup_packing>(srslog::fetch_basic_logger("GTP-U", false));
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&         logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<nrup_packing> packer;
};

TEST_F(nrup_packing_test, create_new_entity)
{
  EXPECT_NE(packer, nullptr);
}

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_smallest)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_with_dl_flush)
{
  const uint8_t packed_vec[] = {
      0x02, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_with_report_polling)
{
  const uint8_t packed_vec[] = {
      0x01, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_with_assist_info_flag)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x02, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_with_retransmission_flag)
{
  const uint8_t packed_vec[] = {
      0x00, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x01, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_with_discard_blocks)
{
  const uint8_t packed_vec[] = {
      0x04, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_empty)
{
  byte_buffer      packed_buf = {};
  nru_dl_user_data out_data;
  EXPECT_FALSE(packer->unpack(out_data, packed_buf));
}

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_invalid_pdu_type)
{
  const uint8_t packed_vec[] = {
      0xe0, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_missing_padding)
{
  const uint8_t packed_vec[] = {
      0xe0, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_excessive_padding)
{
  const uint8_t packed_vec[] = {
      0xe0, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

TEST_F(nrup_packing_test, unpack_nru_dl_user_data_with_too_large_nof_discard_blocks)
{
  const uint8_t packed_vec[] = {
      0x04, // 4PDU Type | Spare | DL Discard Blocks | DL Flush | Report polling
      0x00, // 6Spare | Assist Info Rep Poll Flag | Retransmission flag
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

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
