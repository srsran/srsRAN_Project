/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/rlc/rlc_am_pdu.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

template <std::size_t N>
byte_buffer make_pdu_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer pdu;
  pdu.append(tv);
  // write_pdu_to_pcap(4, tv.data(), tv.size()); TODO
  return pdu;
}

// RLC AM PDU 12bit with complete SDU
void test_rlc_am_12bit_complete_sdu()
{
  test_delimit_logger delimiter{"AM PDU with 12 Bit SN and SI=full"};

  const int                                     header_len = 2, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {0x80, 0x00, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};
  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size12bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::full_sdu);
  }
  {
    // Pack
    byte_buffer buf = make_pdu_and_log(tv_sdu);
    TESTASSERT(rlc_am_write_data_pdu_header(hdr, buf));
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 12bit first segment of SDU with P flag and SN 511
void test_rlc_am_12bit_first_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 12 bit and SI=first");
  const int                                     header_len = 2, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {0xd1, 0xff, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size12bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::first_segment);
    TESTASSERT(hdr.sn == 511);
    TESTASSERT(hdr.p == 1);
    TESTASSERT(hdr.so == 0);
  }
  {
    // Pack
    byte_buffer buf = make_pdu_and_log(tv_sdu);
    TESTASSERT(rlc_am_write_data_pdu_header(hdr, buf));
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 12bit last segment of SDU without P flag and SN 0x0404 and SO 0x0404 (1028)
void test_rlc_am_12bit_last_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 12 bit and SI=last");
  const int                                     header_len = 4, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {0xa4, 0x04, 0x04, 0x04, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size12bits, &hdr));

    TESTASSERT(hdr.si == rlc_si_field::last_segment);
    TESTASSERT(hdr.sn == 1028);
    TESTASSERT(hdr.so == 1028);
  }
  {
    // Pack
    byte_buffer buf = make_pdu_and_log(tv_sdu);
    TESTASSERT(rlc_am_write_data_pdu_header(hdr, buf));
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 18bit full SDU with P flag and SN 0x100000001000000010 (131586)
void test_rlc_am_18bit_complete_sdu()
{
  test_delimit_logger                           delimiter("AM PDU with 18 bit and SI=full");
  const int                                     header_len = 3, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {0xc2, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size18bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::full_sdu);
    TESTASSERT(hdr.sn == 131586);
    TESTASSERT(hdr.so == 0);
  }
  {
    // Pack
    byte_buffer buf = make_pdu_and_log(tv_sdu);
    TESTASSERT(rlc_am_write_data_pdu_header(hdr, buf));
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 18bit middle part of SDU (SO 514) without P flag and SN 131327
void test_rlc_am_18bit_middle_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 18 bit and SI=middle");
  const int                                     header_len = 5, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {0xb2, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size18bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::neither_first_nor_last_segment);
    TESTASSERT(hdr.sn == 131327);
    TESTASSERT(hdr.so == 514);
  }
  {
    // Pack
    byte_buffer buf = make_pdu_and_log(tv_sdu);
    TESTASSERT(rlc_am_write_data_pdu_header(hdr, buf));
    TESTASSERT(buf == tv_pdu);
  }
}

// Malformed RLC AM PDU 18bit with reserved bits set
void test_rlc_am_18bit_malformed()
{
  test_delimit_logger                           delimiter("Malformed AM PDU with 18 bit");
  const int                                     header_len = 5, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv  = {0xb7, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  byte_buffer                                   pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size18bits, &header) == false);
  TESTASSERT(header.sn == 0);
}

} // namespace srsgnb

int main()
{
  auto& logger = srslog::fetch_basic_logger("TEST", false);
  logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  logger.info("Testing UM PDU packing/unpacking");
  srslog::flush();
  srsgnb::test_rlc_am_12bit_complete_sdu();
  srsgnb::test_rlc_am_12bit_first_segment();
  srsgnb::test_rlc_am_12bit_last_segment();
  srsgnb::test_rlc_am_18bit_complete_sdu();
  srsgnb::test_rlc_am_18bit_middle_segment();
  srsgnb::test_rlc_am_18bit_malformed();
}
