/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gtpu_pdu_test.h"
#include "../../lib/gtpu/gtpu.h"
#include "srsgnb/support/test_utils.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

using namespace srsgnb;

/// \brief Test correct creation of PDCP TX  entity
TEST_F(gtpu_test, pack_unpack)
{
  srsgnb::test_delimit_logger delimiter("GTP-U unpack/pack test");
  byte_buffer                 tst_vec{gtpu_ping_vec};
  byte_buffer                 tst_vec_no_header{tst_vec.begin() + 8, tst_vec.end()};
  gtpu_header                 hdr;
  bool                        read_ok = gtpu_read_and_strip_header(hdr, tst_vec, logger);
  ASSERT_EQ(read_ok, true);

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
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
