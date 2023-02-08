/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/pcap/ngap_pcap.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

class pcap_mac_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("PCAP").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("PCAP").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    srslog::init();

    mac_pcap_writer.open("mac.pcap");
    ngap_pcap_writer.open("ngap.pcap");

    // Start the log backend.
    srslog::init();
  }

  void TearDown() override
  {
    ngap_pcap_writer.close();
    // flush logger after each test
    srslog::flush();
  }

  srsgnb::ngap_pcap     ngap_pcap_writer;
  srsgnb::ngap_pcap     mac_pcap_writer;
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

TEST_F(pcap_mac_test, write_pdu)
{
  std::array<uint8_t, 11> tv = {0x42, 0x00, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

  mac_pcap_writer.write_pdu(tv);
}
