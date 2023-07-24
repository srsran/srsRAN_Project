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

#include "gtpu_test_shared.h"
#include "lib/gtpu/gtpu_pdu.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsran;

/// Fixture class for GTP-U PDU tests
class gtpu_test : public ::testing::Test
{
public:
  gtpu_test() :
    logger(srslog::fetch_basic_logger("TEST", false)), gtpu_logger(srslog::fetch_basic_logger("GTPU", false))
  {
  }

protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init GTPU logger
    gtpu_logger.set_level(srslog::basic_levels::debug);
    gtpu_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  // Test logger
  srslog::basic_logger& logger;

  // GTP-U logger
  srslog::basic_logger& gtpu_logger;
  gtpu_tunnel_logger    gtpu_rx_logger{"GTPU", {0, gtpu_teid_t{1}, "DL"}};
  gtpu_tunnel_logger    gtpu_tx_logger{"GTPU", {0, gtpu_teid_t{1}, "UL"}};
};

/// \brief Test correct read TEID helper function
TEST_F(gtpu_test, read_teid)
{
  srsran::test_delimit_logger delimiter("GTP-U read TEID test");
  byte_buffer                 orig_vec{gtpu_ping_vec_teid_1};
  byte_buffer                 test_vec{gtpu_ping_vec_teid_1};
  uint32_t                    teid = {};

  // Unpack SDU
  logger.info(orig_vec.begin(), orig_vec.end(), "Original SDU");
  bool read_ok = gtpu_read_teid(teid, test_vec, gtpu_logger);
  ASSERT_EQ(read_ok, true);

  // Check TEID
  ASSERT_EQ(teid, 1);
  ASSERT_EQ(orig_vec, test_vec);
}

/// \brief Test correct packing/unpacking of GTPU PDU
TEST_F(gtpu_test, pack_unpack)
{
  srsran::test_delimit_logger delimiter("GTP-U unpack/pack test");
  byte_buffer                 orig_vec{gtpu_ping_vec_teid_1};
  byte_buffer                 tst_vec{gtpu_ping_vec_teid_1};
  byte_buffer                 tst_vec_no_header{tst_vec.begin() + 8, tst_vec.end()};
  gtpu_dissected_pdu          dissected_pdu;

  // Unpack SDU
  logger.info(orig_vec.begin(), orig_vec.end(), "Original SDU");
  bool read_ok = gtpu_dissect_pdu(dissected_pdu, tst_vec.deep_copy(), gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);

  // Check flags
  ASSERT_EQ(dissected_pdu.hdr.flags.version, GTPU_FLAGS_VERSION_V1);
  ASSERT_EQ(dissected_pdu.hdr.flags.protocol_type, GTPU_FLAGS_GTP_PROTOCOL);
  ASSERT_EQ(dissected_pdu.hdr.flags.ext_hdr, false);
  ASSERT_EQ(dissected_pdu.hdr.flags.seq_number, false);
  ASSERT_EQ(dissected_pdu.hdr.flags.n_pdu, false);

  // Check message type
  ASSERT_EQ(dissected_pdu.hdr.message_type, GTPU_MSG_DATA_PDU);

  // Check length
  ASSERT_EQ(dissected_pdu.hdr.length, tst_vec_no_header.length());

  // Check TEID
  ASSERT_EQ(dissected_pdu.hdr.teid, 1);

  logger.info("Unpacked GTP-U header: {}", dissected_pdu.hdr);
  gtpu_header hdr = dissected_pdu.hdr;

  // Check SDU after striping header
  byte_buffer sdu = gtpu_extract_t_pdu(std::move(dissected_pdu));
  ASSERT_EQ(sdu.length(), tst_vec_no_header.length());
  ASSERT_EQ(sdu, tst_vec_no_header);

  byte_buffer repack_buf = tst_vec_no_header.deep_copy();

  gtpu_write_header(repack_buf, hdr, gtpu_tx_logger);
  logger.info(repack_buf.begin(), repack_buf.end(), "Repackaged GTP-U packet");
  ASSERT_EQ(repack_buf.length(), orig_vec.length());
  ASSERT_EQ(repack_buf, orig_vec);
}

/// \brief Test correct packing/unpacking of GTPU PDU
/// with header extension.
TEST_F(gtpu_test, pack_unpack_ext_hdr)
{
  srsran::test_delimit_logger delimiter("GTP-U header extension unpack/pack test");
  byte_buffer                 orig_vec{gtpu_ping_two_ext_vec};
  byte_buffer                 tst_vec{gtpu_ping_two_ext_vec};
  uint16_t                    ext_size = 4;
  byte_buffer        tst_vec_no_header{tst_vec.begin() + GTPU_EXTENDED_HEADER_LEN + 2 * ext_size, tst_vec.end()};
  gtpu_dissected_pdu dissected_pdu;

  // Unpack SDU
  logger.info(orig_vec.begin(), orig_vec.end(), "Original SDU");
  bool read_ok = gtpu_dissect_pdu(dissected_pdu, tst_vec.deep_copy(), gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);

  // Check flags
  ASSERT_EQ(dissected_pdu.hdr.flags.version, GTPU_FLAGS_VERSION_V1);
  ASSERT_EQ(dissected_pdu.hdr.flags.protocol_type, GTPU_FLAGS_GTP_PROTOCOL);
  ASSERT_EQ(dissected_pdu.hdr.flags.ext_hdr, true);
  ASSERT_EQ(dissected_pdu.hdr.flags.seq_number, false);
  ASSERT_EQ(dissected_pdu.hdr.flags.n_pdu, false);

  // Check message type
  ASSERT_EQ(dissected_pdu.hdr.message_type, GTPU_MSG_DATA_PDU);

  // Check length
  ASSERT_EQ(dissected_pdu.hdr.length, tst_vec_no_header.length() + ext_size + GTPU_NON_MANDATORY_HEADER_LEN);

  // Check TEID
  ASSERT_EQ(dissected_pdu.hdr.teid, 1);

  // Check extension header is correct
  ASSERT_EQ(dissected_pdu.hdr.ext_list.size(), 2);
  ASSERT_EQ(dissected_pdu.hdr.ext_list[0].extension_header_type, gtpu_extension_header_type::pdcp_pdu_number);
  {
    const gtpu_extension_header& ext = dissected_pdu.hdr.ext_list[0];
    ASSERT_EQ(ext.container.length(), 2);
    ASSERT_EQ(ext.container[0], 0x00);
    ASSERT_EQ(ext.container[1], 0x01);
  }
  ASSERT_EQ(dissected_pdu.hdr.ext_list[1].extension_header_type, gtpu_extension_header_type::pdu_session_container);
  {
    const gtpu_extension_header& ext = dissected_pdu.hdr.ext_list[1];
    ASSERT_EQ(ext.container.length(), 2);
    ASSERT_EQ(ext.container[0], 0x00);
    ASSERT_EQ(ext.container[1], 0x01);
  }

  logger.info("Unpacked GTP-U header: {}", dissected_pdu.hdr);

  gtpu_header hdr = dissected_pdu.hdr;

  byte_buffer container0                = {dissected_pdu.hdr.ext_list[0].container.begin(),
                                           dissected_pdu.hdr.ext_list[0].container.end()};
  hdr.ext_list[0].container             = container0;
  hdr.ext_list[0].extension_header_type = dissected_pdu.hdr.ext_list[0].extension_header_type;

  byte_buffer container1                = {dissected_pdu.hdr.ext_list[1].container.begin(),
                                           dissected_pdu.hdr.ext_list[1].container.end()};
  hdr.ext_list[1].container             = container1;
  hdr.ext_list[1].extension_header_type = dissected_pdu.hdr.ext_list[1].extension_header_type;

  // Check SDU after striping header
  byte_buffer sdu = gtpu_extract_t_pdu(std::move(dissected_pdu));
  ASSERT_EQ(sdu.length(), tst_vec_no_header.length());
  ASSERT_EQ(sdu, tst_vec_no_header);

  byte_buffer repack_buf = tst_vec_no_header.deep_copy();

  gtpu_write_header(repack_buf, hdr, gtpu_tx_logger);
  logger.info(repack_buf.begin(), repack_buf.end(), "Repackaged GTP-U packet");
  ASSERT_EQ(repack_buf.length(), orig_vec.length());
  ASSERT_EQ(repack_buf, orig_vec);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
