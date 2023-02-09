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
#include "srsgnb/pcap/mac_pcap.h"
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
  srsgnb::mac_pcap      mac_pcap_writer;
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

/* radioType */
#define FDD_RADIO 1
#define TDD_RADIO 2

/* Direction */
#define DIRECTION_UPLINK 0
#define DIRECTION_DOWNLINK 1

/* rntiType */
#define NO_RNTI 0 /* Used for BCH-BCH */
#define P_RNTI 1
#define RA_RNTI 2
#define C_RNTI 3
#define SI_RNTI 4
#define SPS_RNTI 5
#define M_RNTI 6
#define SL_BCH_RNTI 7
#define SL_RNTI 8
#define SC_RNTI 9
#define G_RNTI 10

TEST_F(pcap_mac_test, write_pdu)
{
  // std::array<uint8_t, 11>     tv     = {0x42, 0x00, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  // std::array<uint8_t, 13>     tv     = {0x42, 0x00, 0x0a, 0x0d, 0x72, 0x80, 0xd3, 0x96, 0x02, 0x7b, 0x01, 0xbd,
  // 0x26};
  std::array<uint8_t, 17> tv = {
      0x04, 0x0a, 0x0d, 0x72, 0x80, 0xd3, 0x96, 0x02, 0x7b, 0x01, 0xbd, 0x26, 0x3f, 0x00, 0x00, 0x00, 0x00};
  int                         crnti  = 0x01011;
  int                         ue_id  = 2;
  int                         harqid = 0;
  int                         tti    = 10;
  srsgnb::mac_nr_context_info context;
  context.radioType           = FDD_RADIO;
  context.direction           = DIRECTION_DOWNLINK;
  context.rntiType            = C_RNTI;
  context.rnti                = crnti;
  context.ueid                = ue_id;
  context.harqid              = harqid;
  context.system_frame_number = tti / 10;
  context.sub_frame_number    = tti % 10;
  mac_pcap_writer.write_pdu(context, tv);
  // ngap_pcap_writer.write_pdu(tv);
}
