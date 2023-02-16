/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gtpu_test.h"
#include "srsgnb/gtpu/gtpu_tunnel_factory.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

/// \brief Test correct read TEID helper function
TEST_F(gtpu_test, read_teid)
{
  srsgnb::test_delimit_logger delimiter("GTP-U read TEID test");
  byte_buffer                 orig_vec{gtpu_ping_vec};
  byte_buffer                 test_vec{gtpu_ping_vec};
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
  srsgnb::test_delimit_logger delimiter("GTP-U unpack/pack test");
  byte_buffer                 orig_vec{gtpu_ping_vec};
  byte_buffer                 tst_vec{gtpu_ping_vec};
  byte_buffer                 tst_vec_no_header{tst_vec.begin() + 8, tst_vec.end()};
  gtpu_header                 hdr;

  // Unpack SDU
  logger.info(orig_vec.begin(), orig_vec.end(), "Original SDU");
  bool read_ok = gtpu_read_and_strip_header(hdr, tst_vec, gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);
  logger.info(tst_vec.begin(), tst_vec.end(), "Unpacked PDU");

  // Check flags
  ASSERT_EQ(hdr.flags.version, GTPU_FLAGS_VERSION_V1);
  ASSERT_EQ(hdr.flags.protocol_type, GTPU_FLAGS_GTP_PROTOCOL);
  ASSERT_EQ(hdr.flags.ext_hdr, false);
  ASSERT_EQ(hdr.flags.seq_number, false);
  ASSERT_EQ(hdr.flags.n_pdu, false);

  // Check message type
  ASSERT_EQ(hdr.message_type, GTPU_MSG_DATA_PDU);

  // Check length
  ASSERT_EQ(hdr.length, tst_vec_no_header.length());

  // Check TEID
  ASSERT_EQ(hdr.teid, 1);

  // Check PDU after striping header
  ASSERT_EQ(tst_vec.length(), tst_vec_no_header.length());
  ASSERT_EQ(tst_vec, tst_vec_no_header);

  logger.info("Unpacked GTP-U header: {}", hdr);

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
  srsgnb::test_delimit_logger delimiter("GTP-U header extension unpack/pack test");
  byte_buffer                 orig_vec{gtpu_ping_two_ext_vec};
  byte_buffer                 tst_vec{gtpu_ping_two_ext_vec};
  uint16_t                    ext_size = 4;
  byte_buffer tst_vec_no_header{tst_vec.begin() + GTPU_EXTENDED_HEADER_LEN + 2 * ext_size, tst_vec.end()};
  gtpu_header hdr;

  // Unpack SDU
  logger.info(orig_vec.begin(), orig_vec.end(), "Original SDU");
  bool read_ok = gtpu_read_and_strip_header(hdr, tst_vec, gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);
  logger.info(tst_vec.begin(), tst_vec.end(), "Unpacked PDU");

  // Check flags
  ASSERT_EQ(hdr.flags.version, GTPU_FLAGS_VERSION_V1);
  ASSERT_EQ(hdr.flags.protocol_type, GTPU_FLAGS_GTP_PROTOCOL);
  ASSERT_EQ(hdr.flags.ext_hdr, true);
  ASSERT_EQ(hdr.flags.seq_number, false);
  ASSERT_EQ(hdr.flags.n_pdu, false);

  // Check message type
  ASSERT_EQ(hdr.message_type, GTPU_MSG_DATA_PDU);

  // Check length
  ASSERT_EQ(hdr.length, tst_vec_no_header.length() + ext_size + GTPU_NON_MANDATORY_HEADER_LEN);

  // Check TEID
  ASSERT_EQ(hdr.teid, 1);

  // Check PDU after striping header
  ASSERT_EQ(tst_vec.length(), tst_vec_no_header.length());
  ASSERT_EQ(tst_vec, tst_vec_no_header);

  // Check extension header is correct
  ASSERT_EQ(hdr.ext_list.size(), 2);
  ASSERT_EQ(hdr.ext_list[0].extension_header_type, gtpu_extension_header_type::pdcp_pdu_number);
  {
    const gtpu_extension_header& ext = hdr.ext_list[0];
    ASSERT_EQ(ext.container.size(), 2);
    ASSERT_EQ(ext.container[0], 0x00);
    ASSERT_EQ(ext.container[1], 0x01);
  }
  ASSERT_EQ(hdr.ext_list[1].extension_header_type, gtpu_extension_header_type::pdu_session_container);
  {
    const gtpu_extension_header& ext = hdr.ext_list[1];
    ASSERT_EQ(ext.container.size(), 2);
    ASSERT_EQ(ext.container[0], 0x00);
    ASSERT_EQ(ext.container[1], 0x01);
  }
  logger.info("Unpacked GTP-U header: {}", hdr);

  byte_buffer repack_buf = tst_vec_no_header.deep_copy();

  gtpu_write_header(repack_buf, hdr, gtpu_tx_logger);
  logger.info(repack_buf.begin(), repack_buf.end(), "Repackaged GTP-U packet");
  ASSERT_EQ(repack_buf.length(), orig_vec.length());
  ASSERT_EQ(repack_buf, orig_vec);
}

/// \brief Test correct creation of GTP-U entity
TEST_F(gtpu_test, entity_creation)
{
  srsgnb::test_delimit_logger  delimiter("GTP-U entity creation test");
  gtpu_tunnel_creation_message msg  = {};
  msg.cfg.tx.peer_addr              = "127.0.0.1";
  std::unique_ptr<gtpu_tunnel> gtpu = create_gtpu_tunnel(msg);

  ASSERT_NE(gtpu, nullptr);
};

/// \brief Test correct reception of GTP-U packet
TEST_F(gtpu_test, rx_sdu)
{
  srsgnb::test_delimit_logger delimiter("GTP-U entity creation test");

  gtpu_test_rx_lower           gtpu_rx = {};
  gtpu_test_tx_upper           gtpu_tx = {};
  gtpu_tunnel_creation_message msg     = {};
  msg.cfg.rx.local_teid                = 0x1;
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  std::unique_ptr<gtpu_tunnel> gtpu    = create_gtpu_tunnel(msg);

  byte_buffer orig_vec{gtpu_ping_vec};
  byte_buffer strip_vec{gtpu_ping_vec};
  gtpu_header tmp;
  bool        read_ok = gtpu_read_and_strip_header(tmp, strip_vec, gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);

  gtpu_tunnel_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(orig_vec));
  ASSERT_EQ(strip_vec, gtpu_rx.last_rx);
};

/// \brief Test correct transmission of GTP-U packet
TEST_F(gtpu_test, tx_pdu)
{
  srsgnb::test_delimit_logger delimiter("GTP-U entity creation test");

  gtpu_test_tx_upper           gtpu_tx = {};
  gtpu_test_rx_lower           gtpu_rx = {};
  gtpu_tunnel_creation_message msg     = {};
  msg.cfg.tx.peer_teid                 = 0x1;
  msg.cfg.tx.peer_addr                 = "127.0.0.1";
  msg.rx_lower                         = &gtpu_rx;
  msg.tx_upper                         = &gtpu_tx;
  std::unique_ptr<gtpu_tunnel> gtpu    = create_gtpu_tunnel(msg);

  byte_buffer orig_vec{gtpu_ping_vec};
  byte_buffer strip_vec{gtpu_ping_vec};
  gtpu_header tmp;
  bool        read_ok = gtpu_read_and_strip_header(tmp, strip_vec, gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);

  gtpu_tunnel_tx_lower_layer_interface* tx = gtpu->get_tx_lower_layer_interface();
  tx->handle_sdu(std::move(strip_vec));
  ASSERT_EQ(orig_vec, gtpu_tx.last_tx);
};

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
