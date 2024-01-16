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

#include "lib/rlc/rlc_am_pdu.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/test_utils.h"

namespace srsran {

template <std::size_t N>
byte_buffer make_pdu_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer pdu;
  TESTASSERT(pdu.append(tv));
  // write_pdu_to_pcap(4, tv.data(), tv.size()); TODO
  return pdu;
}

size_t copy_pdu_and_log(span<uint8_t> out_pdu, const span<uint8_t> in_pdu)
{
  TESTASSERT(in_pdu.size() <= out_pdu.size());
  std::copy(in_pdu.begin(), in_pdu.end(), out_pdu.begin());
  return in_pdu.size();
}

// RLC AM PDU 12bit with complete SDU
void test_rlc_am_12bit_complete_sdu()
{
  test_delimit_logger delimiter{"AM PDU with 12 Bit SN and SI=full"};

  const int                                     header_len  = 2;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0x80, 0x00, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
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
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 12bit first segment of SDU with P flag and SN 511
void test_rlc_am_12bit_first_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 12 bit and SI=first");
  const int                                     header_len  = 2;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0xd1, 0xff, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
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
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 12bit middle segment of SDU without P flag and SN 0x0404 and SO 0x0404 (1028)
void test_rlc_am_12bit_middle_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 12 bit and SI=middle");
  const int                                     header_len  = 4;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0xb4, 0x04, 0x04, 0x04, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size12bits, &hdr));

    TESTASSERT(hdr.si == rlc_si_field::middle_segment);
    TESTASSERT(hdr.sn == 1028);
    TESTASSERT(hdr.p == 0);
    TESTASSERT(hdr.so == 1028);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 12bit last segment of SDU without P flag and SN 0x0404 and SO 0x0404 (1028)
void test_rlc_am_12bit_last_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 12 bit and SI=last");
  const int                                     header_len  = 4;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0xa4, 0x04, 0x04, 0x04, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
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
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 18bit full SDU with P flag and SN 0x100000001000000010 (131586)
void test_rlc_am_18bit_complete_sdu()
{
  test_delimit_logger                           delimiter("AM PDU with 18 bit and SI=full");
  const int                                     header_len  = 3;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0xc2, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
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
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 18bit first segment of SDU without P flag and SN 131327
void test_rlc_am_18bit_first_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 18 bit and SI=first");
  const int                                     header_len  = 3;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0x92, 0x00, 0xff, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size18bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::first_segment);
    TESTASSERT(hdr.sn == 131327);
    TESTASSERT(hdr.p == 0);
    TESTASSERT(hdr.so == 0);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 18bit middle segment of SDU (SO 514) without P flag and SN 131327
void test_rlc_am_18bit_middle_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 18 bit and SI=middle");
  const int                                     header_len  = 5;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0xb2, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size18bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::middle_segment);
    TESTASSERT(hdr.sn == 131327);
    TESTASSERT(hdr.p == 0);
    TESTASSERT(hdr.so == 514);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// RLC AM PDU 18bit last segment of SDU (SO 514) without P flag and SN 131327
void test_rlc_am_18bit_last_segment()
{
  test_delimit_logger                           delimiter("AM PDU with 18 bit and SI=last");
  const int                                     header_len  = 5;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu      = {0xa2, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu      = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_am_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_am_read_data_pdu_header(buf, rlc_am_sn_size::size18bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::last_segment);
    TESTASSERT(hdr.sn == 131327);
    TESTASSERT(hdr.p == 0);
    TESTASSERT(hdr.so == 514);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_am_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Malformed RLC AM PDU 18bit with reserved bits set
void test_rlc_am_18bit_malformed()
{
  test_delimit_logger                           delimiter("Malformed AM PDU with 18 bit");
  const int                                     header_len  = 5;
  const int                                     payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv          = {0xb7, 0x00, 0xff, 0x02, 0x02, 0x11, 0x22, 0x33, 0x44};
  byte_buffer                                   pdu         = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size18bits, &header) == false);
  TESTASSERT(header.sn == 0);
}

// Malformed RLC AM PDU 12bit with short header (lower SN is missing)
void test_rlc_am_12bit_short_header_missing_lower_sn()
{
  test_delimit_logger    delimiter("Short AM PDU with 12 bit (lower SN is missing)");
  std::array<uint8_t, 1> tv  = {0b10110000};
  byte_buffer            pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size12bits, &header) == false);
}

// Malformed RLC AM PDU 12bit with short header (upper SO is missing)
void test_rlc_am_12bit_short_header_missing_upper_so()
{
  test_delimit_logger    delimiter("Short AM PDU with 12 bit (upper SO is missing)");
  std::array<uint8_t, 2> tv  = {0b10110000, 0x11};
  byte_buffer            pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size12bits, &header) == false);
}

// Malformed RLC AM PDU 12bit with short header (lower SO is missing)
void test_rlc_am_12bit_short_header_missing_lower_so()
{
  test_delimit_logger    delimiter("Short AM PDU with 12 bit (lower SO is missing)");
  std::array<uint8_t, 3> tv  = {0b10110000, 0x11, 0x22};
  byte_buffer            pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size12bits, &header) == false);
}

// Malformed RLC AM PDU 18bit with short header (center SN is missing)
void test_rlc_am_18bit_short_header_missing_center_sn()
{
  test_delimit_logger    delimiter("Short AM PDU with 18 bit (center SN is missing)");
  std::array<uint8_t, 1> tv  = {0b10110000};
  byte_buffer            pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size18bits, &header) == false);
}

// Malformed RLC AM PDU 18bit with short header (lower SN is missing)
void test_rlc_am_18bit_short_header_missing_lower_sn()
{
  test_delimit_logger    delimiter("Short AM PDU with 18 bit (lower SN is missing)");
  std::array<uint8_t, 2> tv  = {0b10110000, 0x11};
  byte_buffer            pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size18bits, &header) == false);
}

// Malformed RLC AM PDU 18bit with short header (upper SO is missing)
void test_rlc_am_18bit_short_header_missing_upper_so()
{
  test_delimit_logger    delimiter("Short AM PDU with 18 bit (upper SO is missing)");
  std::array<uint8_t, 3> tv  = {0b10110000, 0x11, 0x22};
  byte_buffer            pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size18bits, &header) == false);
}

// Malformed RLC AM PDU 18bit with short header (lower SO is missing)
void test_rlc_am_18bit_short_header_missing_lower_so()
{
  test_delimit_logger    delimiter("Short AM PDU with 18 bit (lower SO is missing)");
  std::array<uint8_t, 4> tv  = {0b10110000, 0x11, 0x22, 0x33};
  byte_buffer            pdu = make_pdu_and_log(tv);

  // unpack PDU
  rlc_am_pdu_header header = {};
  TESTASSERT(rlc_am_read_data_pdu_header(pdu, rlc_am_sn_size::size18bits, &header) == false);
}

///////////////////////////////////////////
// Control PDU tests (12 bit SN)
///////////////////////////////////////////

/// Status PDU for 12 bit SN with ACK_SN=2065 and no further NACK_SN (E1 bit not set)
void test_control_pdu_no_nack_12bit()
{
  test_delimit_logger      delimiter("Control PDU without NACK (12 bit)");
  const int                len = 3;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x00};
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.get_nacks().size() == 0);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

/// Status PDU for 12 bit SN with ACK_SN=2065 and NACK_SN=273 (E1 bit set)
void test_control_pdu_with_nack_12bit()
{
  test_delimit_logger      delimiter("Control PDU with one NACK (12 bit)");
  const int                len = 5;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x80, 0x11, 0x10};
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.get_nacks().size() == 1);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 273);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Status PDU for 12 bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275, SO_START=5, SO_END=0xffff
// E1 and E2 bit set on first NACK, only E2 on second.
void test_control_pdu_nacks_and_so_12bit()
{
  test_delimit_logger      delimiter("Control PDU with NACKs and SO (12 bit)");
  const int                len = 15;
  std::array<uint8_t, len> tv  = {
       0x08, 0x11, 0x80, 0x11, 0x1c, 0x00, 0x02, 0x00, 0x05, 0x11, 0x34, 0x00, 0x05, 0xff, 0xff};
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.get_nacks().size() == 2);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 273);
  TESTASSERT(status_pdu.get_nacks()[0].so_start == 2);
  TESTASSERT(status_pdu.get_nacks()[0].so_end == 5);
  TESTASSERT(status_pdu.get_nacks()[1].nack_sn == 275);
  TESTASSERT(status_pdu.get_nacks()[1].so_start == 5);
  TESTASSERT(status_pdu.get_nacks()[1].so_end == 0xffff);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Status PDU for 12 bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275
// E1 and E2 bit set on first NACK, neither E1 or E2 on the second.
void test_control_pdu_nacks_and_mixed_so_12bit()
{
  test_delimit_logger      delimiter("Control PDU with NACKs and mixed SO (12 bit)");
  const int                len = 11;
  std::array<uint8_t, len> tv  = {0x08, 0x11, 0x80, 0x11, 0x1c, 0x00, 0x02, 0x00, 0x05, 0x11, 0x30};
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.get_nacks().size() == 2);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 273);
  TESTASSERT(status_pdu.get_nacks()[0].has_so == true);
  TESTASSERT(status_pdu.get_nacks()[0].so_start == 2);
  TESTASSERT(status_pdu.get_nacks()[0].so_end == 5);
  TESTASSERT(status_pdu.get_nacks()[1].nack_sn == 275);
  TESTASSERT(status_pdu.get_nacks()[1].has_so == false);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Status PDU for 12 bit SN with ACK_SN=2065,
// NACK range0: 3 full SDUs, NACK_SN=273..275
// NACK range1: missing segment sequence across 4 SDUs
//              starting at NACK_SN=276, SO_START=2,
//              ending at NACK_SN=279, SO_END=5
// E1 and E3 bit set on first NACK, E2 and E3 bit set on the second.
void test_control_pdu_nacks_and_range_12bit()
{
  test_delimit_logger      delimiter("Control PDU with NACKs and range (12 bit)");
  const int                len = 13;
  std::array<uint8_t, len> tv  = {0x08,  // D/C | 3CPT | 4ACK_SN_upper
                                  0x11,  // 8ACK_SN_lower
                                  0x80,  // E1 | 7R
                                  0x11,  // 8NACK_SN_upper
                                  0x1a,  // 4NACK_SN_lower | E1 | E2 | E3 | R
                                  0x03,  // 8NACK_range
                                  0x11,  // 8NACK_SN_upper
                                  0x46,  // 4NACK_SN_lower | E1 | E2 | E3 | R
                                  0x00,  // 8SO_START_upper
                                  0x02,  // 8SO_START_lower
                                  0x00,  // 8SO_END_upper
                                  0x05,  // 8SO_END_lower
                                  0x04}; // 8NACK_range
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 2065);
  TESTASSERT(status_pdu.get_nacks().size() == 2);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 273);
  TESTASSERT(status_pdu.get_nacks()[0].has_so == false);
  TESTASSERT(status_pdu.get_nacks()[0].has_nack_range == true);
  TESTASSERT(status_pdu.get_nacks()[0].nack_range == 3);

  TESTASSERT(status_pdu.get_nacks()[1].nack_sn == 276);
  TESTASSERT(status_pdu.get_nacks()[1].has_so == true);
  TESTASSERT(status_pdu.get_nacks()[1].so_start == 2);
  TESTASSERT(status_pdu.get_nacks()[1].so_end == 5);
  TESTASSERT(status_pdu.get_nacks()[1].has_nack_range == true);
  TESTASSERT(status_pdu.get_nacks()[1].nack_range == 4);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Malformed Status PDU that is too short to be unpacked
void test_control_pdu_invalid_too_short_12bit()
{
  test_delimit_logger      delimiter("Control PDU too short for unpacking (12 bit)");
  const int                len = 2;
  std::array<uint8_t, len> tv  = {0x08,  // D/C | 3CPT | 4ACK_SN_upper
                                  0x11}; // 8ACK_SN_lower; Missing: E1 | 7R
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

// Malformed Status PDU, with E1 still set at the end of the PDU
// 12 bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275, SO_START=5, SO_END=0xffff, [missing NACK]
// E1 and E2 bit set on both NACKs, but not third NACK follows - end of the buffer is reached prematurely
void test_control_pdu_invalid_e1_extension_cross_boundary_12bit()
{
  test_delimit_logger      delimiter("Control PDU with invalid E1 extension cross boundary (12 bit)");
  const int                len = 15;
  std::array<uint8_t, len> tv  = {0x08,  // D/C | 3CPT | 4ACK_SN_upper
                                  0x11,  // 8ACK_SN_lower
                                  0x80,  // E1 | 7R
                                  0x11,  // 8NACK_SN_upper
                                  0x1c,  // 4NACK_SN_lower | E1 | E2 | E3 | R
                                  0x00,  // 8SO_START_upper
                                  0x02,  // 8SO_START_lower
                                  0x00,  // 8SO_END_upper
                                  0x05,  // 8SO_END_lower
                                  0x11,  // 8NACK_SN_upper
                                  0x3c,  // 4NACK_SN_lower | [!E1!] | E2 | E3 | R
                                  0x00,  // 8SO_START_upper
                                  0x05,  // 8SO_START_lower
                                  0xff,  // 8SO_END_upper
                                  0xff}; // 8SO_END_lower
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

// Malformed Status PDU, with E2 set beyond the boundaries of the PDU
// 12 bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275, [missing SO]
// E1 and E2 bit set on first NACK;
// E2 bit is set on second NACK, but SO_START and SO_END are missing - end of the buffer is reached prematurely
void test_control_pdu_invalid_e2_extension_cross_boundary_12bit()
{
  test_delimit_logger      delimiter("Control PDU with invalid E2 extension cross boundary (12 bit)");
  const int                len = 12;
  std::array<uint8_t, len> tv  = {0x08,  // D/C | 3CPT | 4ACK_SN_upper
                                  0x11,  // 8ACK_SN_lower
                                  0x80,  // E1 | 7R
                                  0x11,  // 8NACK_SN_upper
                                  0x1c,  // 4NACK_SN_lower | E1 | E2 | E3 | R
                                  0x00,  // 8SO_START_upper
                                  0x02,  // 8SO_START_lower
                                  0x00,  // 8SO_END_upper
                                  0x05,  // 8SO_END_lower
                                  0x11,  // 8NACK_SN_upper
                                  0x34,  // 4NACK_SN_lower | E1 | [!E2!] | E3 | R
                                  0x00}; // 8SO_START_upper; Missing: 8SO_START_lower, 8SO_END_upper, 8SO_END_lower
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

// Malformed Status PDU, with E3 set beyond the boundaries of the PDU
// 12 bit SN with ACK_SN=2065, NACK_SN=273, SO_START=2, SO_END=5, NACK_SN=275, [missing range]
// E1 and E2 bit set on first NACK;
// E3 bit is set on second NACK, but range is missing - end of the buffer is reached prematurely
void test_control_pdu_invalid_e3_extension_cross_boundary_12bit()
{
  test_delimit_logger      delimiter("Control PDU with invalid E3 extension cross boundary (12 bit)");
  const int                len = 11;
  std::array<uint8_t, len> tv  = {0x08,  // D/C | 3CPT | 4ACK_SN_upper
                                  0x11,  // 8ACK_SN_lower
                                  0x80,  // E1 | 7R
                                  0x11,  // 8NACK_SN_upper
                                  0x1c,  // 4NACK_SN_lower | E1 | E2 | E3 | R
                                  0x00,  // 8SO_START_upper
                                  0x02,  // 8SO_START_lower
                                  0x00,  // 8SO_END_upper
                                  0x05,  // 8SO_END_lower
                                  0x11,  // 8NACK_SN_upper
                                  0x32}; // 4NACK_SN_lower | E1 | E2 | [!E3!] | R; Missing: range
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size12bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

///////////////////////////////////////////
// Control PDU tests (18 bit SN)
///////////////////////////////////////////

/// Status PDU for 18 bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 and no further NACK_SN (E1 bit not set)
void test_control_pdu_no_nack_18bit()
{
  test_delimit_logger      delimiter("Control PDU without NACK (18 bit)");
  const int                len = 3;
  std::array<uint8_t, len> tv  = {0x0e, 0x66, 0x64};
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.get_nacks().size() == 0);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Status PDU for 18 bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set)
//                          and NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110
void test_control_pdu_with_nack_18bit()
{
  test_delimit_logger      delimiter("Control PDU with one NACK (18 bit)");
  const int                len = 6;
  std::array<uint8_t, len> tv  = {0x0e, 0x66, 0x66, 0xd9, 0x99, 0x80};
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.get_nacks().size() == 1);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 222822);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Status PDU for 18 bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                              NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                              SO_START=2, SO_END=5,
//                              NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 (E2 bit set),
//                              SO_START=5, SO_END=0xffff
void test_control_pdu_nacks_and_so_18bit()
{
  test_delimit_logger      delimiter("Control PDU with NACKs and SO (18 bit)");
  const int                len = 17;
  std::array<uint8_t, len> tv  = {0b00001110, // D/C | 3CPT | 4ACK_SN_upper
                                  0b01100110, // 8ACK_SN_center
                                  0b01100110, // 6ACK_SN_lower | E1 | R
                                  0b11011001, // 8NACK_SN_upper
                                  0b10011001, // 8NACK_SN_center
                                  0b10110000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x00,       // 8SO_START_upper
                                  0x02,       // 8SO_START_lower
                                  0x00,       // 8SO_END_upper
                                  0x05,       // 8SO_END_lower
                                  0b11011001, // 8NACK_SN_upper
                                  0b10111111, // 8NACK_SN_center
                                  0b11010000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x00,       // 8SO_START_upper
                                  0x05,       // 8SO_START_lower
                                  0xff,       // 8SO_END_upper
                                  0xff};      // 8SO_END_lower
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.get_nacks().size() == 2);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 222822);
  TESTASSERT(status_pdu.get_nacks()[0].has_so == true);
  TESTASSERT(status_pdu.get_nacks()[0].so_start == 2);
  TESTASSERT(status_pdu.get_nacks()[0].so_end == 5);
  TESTASSERT(status_pdu.get_nacks()[1].nack_sn == 222975);
  TESTASSERT(status_pdu.get_nacks()[1].has_so == true);
  TESTASSERT(status_pdu.get_nacks()[1].so_start == 5);
  TESTASSERT(status_pdu.get_nacks()[1].so_end == 0xffff);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Status PDU for 18 bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                              NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                              SO_START=2, SO_END=5,
//                              NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 (E1 and E2 bit not set),
void test_control_pdu_nacks_and_mixed_so_18bit()
{
  test_delimit_logger      delimiter("Control PDU with NACKs and mixed SO (18 bit)");
  const int                len = 13;
  std::array<uint8_t, len> tv  = {0b00001110,  // D/C | 3CPT | 4ACK_SN_upper
                                  0b01100110,  // 8ACK_SN_center
                                  0b01100110,  // 6ACK_SN_lower | E1 | R
                                  0b11011001,  // 8NACK_SN_upper
                                  0b10011001,  // 8NACK_SN_center
                                  0b10110000,  // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x00,        // 8SO_START_upper
                                  0x02,        // 8SO_START_lower
                                  0x00,        // 8SO_END_upper
                                  0x05,        // 8SO_END_lower
                                  0b11011001,  // 8NACK_SN_upper
                                  0b10111111,  // 8NACK_SN_center
                                  0b11000000}; // 2NACK_SN_lower | E1 | E2 | E3 | 3R
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 235929);
  TESTASSERT(status_pdu.get_nacks().size() == 2);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 222822);
  TESTASSERT(status_pdu.get_nacks()[0].has_so == true);
  TESTASSERT(status_pdu.get_nacks()[0].so_start == 2);
  TESTASSERT(status_pdu.get_nacks()[0].so_end == 5);
  TESTASSERT(status_pdu.get_nacks()[1].nack_sn == 222975);
  TESTASSERT(status_pdu.get_nacks()[1].has_so == false);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Status PDU for 18 bit SN with ACK_SN=200977=0x31111=0b11 0001 0001 0001 0001,
// NACK range0: 3 full SDUs,     NACK_SN=69905=0x11111=0b01 0001 0001 0001 0001
// NACK range1: missing segment sequence across 4 SDUs
//              starting at      NACK_SN=69913=0x11119=0b01 0001 0001 0001 1001, SO_START=2,
//              ending at        NACK_SN=69916, SO_END=5
// E1 and E3 bit set on first NACK, E2 and E3 bit set on the second.
void test_control_pdu_nacks_and_range_18bit()
{
  test_delimit_logger      delimiter("Control PDU with NACKs and range (18 bit)");
  const int                len = 15;
  std::array<uint8_t, len> tv  = {0b00001100, // D/C | 3CPT | 4ACK_SN_upper
                                  0b01000100, // 8ACK_SN_center
                                  0b01000110, // 6ACK_SN_lower | E1 | R
                                  0b01000100, // 8NACK_SN_upper
                                  0b01000100, // 8NACK_SN_center
                                  0b01101000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x03,       // 8NACK_range
                                  0b01000100, // 8NACK_SN_upper
                                  0b01000110, // 8NACK_SN_center
                                  0b01011000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x00,       // 8SO_START_upper
                                  0x02,       // 8SO_START_lower
                                  0x00,       // 8SO_END_upper
                                  0x05,       // 8SO_END_lower
                                  0x04};      // 8NACK_range
  std::array<uint8_t, len> res = {};
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == true);
  TESTASSERT(status_pdu.ack_sn == 200977);
  TESTASSERT(status_pdu.get_nacks().size() == 2);
  TESTASSERT(status_pdu.get_nacks()[0].nack_sn == 69905);
  TESTASSERT(status_pdu.get_nacks()[0].has_so == false);
  TESTASSERT(status_pdu.get_nacks()[0].has_nack_range == true);
  TESTASSERT(status_pdu.get_nacks()[0].nack_range == 3);

  TESTASSERT(status_pdu.get_nacks()[1].nack_sn == 69913);
  TESTASSERT(status_pdu.get_nacks()[1].has_so == true);
  TESTASSERT(status_pdu.get_nacks()[1].so_start == 2);
  TESTASSERT(status_pdu.get_nacks()[1].so_end == 5);
  TESTASSERT(status_pdu.get_nacks()[1].has_nack_range == true);
  TESTASSERT(status_pdu.get_nacks()[1].nack_range == 4);

  // reset status PDU
  pdu.clear();

  // pack again
  TESTASSERT(status_pdu.pack(res) == len);
  TESTASSERT(res == tv);
}

// Malformed Status PDU that is too short to be unpacked
void test_control_pdu_invalid_too_short_18bit()
{
  test_delimit_logger      delimiter("Control PDU too short for unpacking (18 bit)");
  const int                len = 2;
  std::array<uint8_t, len> tv  = {0b00001110,  // D/C | 3CPT | 4ACK_SN_upper
                                  0b01100110}; // 8ACK_SN_center; Missing: 6ACK_SN_lower | E1 | R
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

// Malformed Status PDU, similar to test3 but with E1 still set at the end of the PDU
// Status PDU for 18 bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                              NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                              SO_START=2, SO_END=5,
//                              NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 ([!E1!] and E2 bit set),
//                              SO_START=5, SO_END=0xffff
// E1 and E2 bit set on both NACKs, but not third NACK follows - end of the buffer is reached prematurely
void test_control_pdu_invalid_e1_extension_cross_boundary_18bit()
{
  test_delimit_logger      delimiter("Control PDU with invalid E1 extension cross boundary (18 bit)");
  const int                len = 17;
  std::array<uint8_t, len> tv  = {0b00001110, // D/C | 3CPT | 4ACK_SN_upper
                                  0b01100110, // 8ACK_SN_center
                                  0b01100110, // 6ACK_SN_lower | E1 | R
                                  0b11011001, // 8NACK_SN_upper
                                  0b10011001, // 8NACK_SN_center
                                  0b10110000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x00,       // 8SO_START_upper
                                  0x02,       // 8SO_START_lower
                                  0x00,       // 8SO_END_upper
                                  0x05,       // 8SO_END_lower
                                  0b11011001, // 8NACK_SN_upper
                                  0b10111111, // 8NACK_SN_center
                                  0b11110000, // 2NACK_SN_lower | [!E1!] | E2 | E3 | 3R
                                  0x00,       // 8SO_START_upper
                                  0x05,       // 8SO_START_lower
                                  0xff,       // 8SO_END_upper
                                  0xff};      // 8SO_END_lower
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

// Malformed Status PDU, with E2 set beyond the boundaries of the PDU
// Status PDU for 18 bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                              NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                              SO_START=2, SO_END=5,
//                              NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 ([!E2!] bit set),
// E2 bit is set on second NACK, but SO_START and SO_END are missing - end of the buffer is reached prematurely
void test_control_pdu_invalid_e2_extension_cross_boundary_18bit()
{
  test_delimit_logger      delimiter("Control PDU with invalid E2 extension cross boundary (18 bit)");
  const int                len = 14;
  std::array<uint8_t, len> tv  = {0b00001110, // D/C | 3CPT | 4ACK_SN_upper
                                  0b01100110, // 8ACK_SN_center
                                  0b01100110, // 6ACK_SN_lower | E1 | R
                                  0b11011001, // 8NACK_SN_upper
                                  0b10011001, // 8NACK_SN_center
                                  0b10110000, // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x00,       // 8SO_START_upper
                                  0x02,       // 8SO_START_lower
                                  0x00,       // 8SO_END_upper
                                  0x05,       // 8SO_END_lower
                                  0b11011001, // 8NACK_SN_upper
                                  0b10111111, // 8NACK_SN_center
                                  0b11010000, // 2NACK_SN_lower | E1 | [!E2!] | E3 | 3R
                                  0x00};      // 8SO_START_upper; Missing: 8SO_START_lower, 8SO_END_upper, 8SO_END_lower
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

// Malformed Status PDU, with E3 set beyond the boundaries of the PDU
// Status PDU for 18 bit SN with ACK_SN=235929=0x39999=0b11 1001 1001 1001 1001 (E1 bit set),
//                              NACK_SN=222822=0x36666=0b11 0110 0110 0110 0110 (E1 and E2 bit set),
//                              SO_START=2, SO_END=5,
//                              NACK_SN=222975=0x366ff=0b11 0110 0110 1111 1111 ([!E3!] bit set),
// E3 bit is set on second NACK, but range is missing - end of the buffer is reached prematurely
void test_control_pdu_invalid_e3_extension_cross_boundary_18bit()
{
  test_delimit_logger      delimiter("Control PDU with invalid E3 extension cross boundary (18 bit)");
  const int                len = 13;
  std::array<uint8_t, len> tv  = {0b00001110,  // D/C | 3CPT | 4ACK_SN_upper
                                  0b01100110,  // 8ACK_SN_center
                                  0b01100110,  // 6ACK_SN_lower | E1 | R
                                  0b11011001,  // 8NACK_SN_upper
                                  0b10011001,  // 8NACK_SN_center
                                  0b10110000,  // 2NACK_SN_lower | E1 | E2 | E3 | 3R
                                  0x00,        // 8SO_START_upper
                                  0x02,        // 8SO_START_lower
                                  0x00,        // 8SO_END_upper
                                  0x05,        // 8SO_END_lower
                                  0b11011001,  // 8NACK_SN_upper
                                  0b10111111,  // 8NACK_SN_center
                                  0b11001000}; // 2NACK_SN_lower | E1 | E2 | [!E3!] | 3R; Missing: range
  byte_buffer              pdu = make_pdu_and_log(tv);

  TESTASSERT(rlc_am_status_pdu::is_control_pdu(pdu) == true);

  // unpack PDU
  rlc_am_status_pdu status_pdu(rlc_am_sn_size::size18bits);
  TESTASSERT(status_pdu.unpack(pdu) == false);
}

///////////////////////////////////////////
// Status PDU function tests (any SN size)
///////////////////////////////////////////

// Test merge of NACKs upon status PDU creation -- previous NACK: non-range; next NACK: non-range
void rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(rlc_am_sn_size sn_size)
{
  test_delimit_logger delimiter("Control PDU ({}) test NACK merge: SDU + SDU", sn_size);

  const uint16_t so_end_of_sdu = rlc_am_status_nack::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(to_number(sn_size));
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK SDU] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK SDU] + [NACK SDU] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 0;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK SDU] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK SDU] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK SDU] + [NACK segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK SDU] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(2, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }
}

// Test merge of NACKs upon status PDU creation -- previous NACK: range; next NACK: non-range
void rlc_am_nr_control_pdu_test_nack_merge_range_sdu(rlc_am_sn_size sn_size)
{
  test_delimit_logger delimiter("Control PDU ({}) test NACK merge: range + SDU", sn_size);

  const uint16_t so_end_of_sdu = rlc_am_status_nack::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(to_number(sn_size));
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK range<255] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range=255] + [NACK SDU] (continuous, but nack range at maximum: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 255;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1255;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK range] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK range] + [NACK SDU] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 4;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range] + [NACK SDU] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 5;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK range] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range] + [NACK segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK SDU] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK SDU] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK SDU] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK SDU]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + 1, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = false;
    next_nack.nack_range     = 0;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }
}

// Test merge of NACKs upon status PDU creation -- previous NACK: non-range; next NACK: range
void rlc_am_nr_control_pdu_test_nack_merge_sdu_range(rlc_am_sn_size sn_size)
{
  test_delimit_logger delimiter("Control PDU ({}) test NACK merge: SDU + range", sn_size);

  const uint16_t so_end_of_sdu = rlc_am_status_nack::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(to_number(sn_size));
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK SDU] + [NACK range<255] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK range=255] (continuous, but nack range at maximum: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 255;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK SDU] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK SDU] + [NACK range] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 0;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK range] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK SDU] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK SDU] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK SDU] + [NACK range+segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK SDU]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(3, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1002;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK segm] + [NACK range+segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = false;
    prev_nack.nack_range     = 0;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1001;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }
}

// Test merge of NACKs upon status PDU creation -- previous NACK: range; next NACK: range
void rlc_am_nr_control_pdu_test_nack_merge_range_range(rlc_am_sn_size sn_size)
{
  test_delimit_logger delimiter("Control PDU ({}) test NACK merge: range + SDU", sn_size);

  const uint16_t so_end_of_sdu = rlc_am_status_nack::so_end_of_sdu;
  const uint32_t mod_nr        = cardinality(to_number(sn_size));
  const uint32_t min_size      = 3;
  const uint32_t nack_size     = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
  const uint32_t so_size       = 4;
  const uint32_t range_size    = 1;

  // Case: [...][NACK range] + [NACK range] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range=128] + [NACK range=128] (continuous, but nack ranges exceed maximum: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 128;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1128;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 128;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK range] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK range] + [NACK range] (continuous: merge with previous element) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 4;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(false, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range] + [NACK range] (non-continuous, SN gap: append as is) -- with SN overflow
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = mod_nr - 1;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 5;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT(prev_nack == status_pdu.get_nacks().front());
    TESTASSERT(next_nack == status_pdu.get_nacks().back());
  }

  // Case: [...][NACK range] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(0, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range] + [NACK range+segm] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = false;
    prev_nack.so_start       = 0;
    prev_nack.so_end         = 0;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 1;
    next_nack.so_end         = 50;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * range_size + so_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(so_end_of_sdu, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK range] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range] (non-continuous, SO gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = false;
    next_nack.so_start       = 0;
    next_nack.so_end         = 0;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + so_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (continuous: merge with previous element)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());
    TESTASSERT_EQ(prev_nack.nack_sn, status_pdu.get_nacks().back().nack_sn);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_so);
    TESTASSERT_EQ(prev_nack.so_start, status_pdu.get_nacks().back().so_start);
    TESTASSERT_EQ(next_nack.so_end, status_pdu.get_nacks().back().so_end);
    TESTASSERT_EQ(true, status_pdu.get_nacks().back().has_nack_range);
    TESTASSERT_EQ(prev_nack.nack_range + next_nack.nack_range, status_pdu.get_nacks().back().nack_range);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (non-continuous, SN gap: append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1006;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (non-continuous, SO gap (left): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = 99;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 0;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }

  // Case: [...][NACK range+segm] + [NACK range+segm] (non-continuous, SO gap (right): append as is)
  {
    rlc_am_status_pdu status_pdu(sn_size);
    status_pdu.ack_sn = 2000;
    TESTASSERT_EQ(0, status_pdu.get_nacks().size());

    // Prepare status_pdu.nacks: [...][NACK range+segm]
    rlc_am_status_nack prev_nack;
    prev_nack.nack_sn        = 1000;
    prev_nack.has_so         = true;
    prev_nack.so_start       = 7;
    prev_nack.so_end         = so_end_of_sdu;
    prev_nack.has_nack_range = true;
    prev_nack.nack_range     = 5;
    status_pdu.push_nack(prev_nack);
    TESTASSERT_EQ(1, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + nack_size + so_size + range_size, status_pdu.get_packed_size());

    // Add next NACK: [NACK range+segm]
    rlc_am_status_nack next_nack;
    next_nack.nack_sn        = 1005;
    next_nack.has_so         = true;
    next_nack.so_start       = 5;
    next_nack.so_end         = 22;
    next_nack.has_nack_range = true;
    next_nack.nack_range     = 2;
    status_pdu.push_nack(next_nack);
    TESTASSERT_EQ(2, status_pdu.get_nacks().size());
    TESTASSERT_EQ(min_size + 2 * nack_size + 2 * so_size + 2 * range_size, status_pdu.get_packed_size());
    TESTASSERT(status_pdu.get_nacks().front() == prev_nack);
    TESTASSERT(status_pdu.get_nacks().back() == next_nack);
  }
}

// Test status PDU for correct trimming and estimation of packed size
// 1) Test init, copy and reset
// 2) Test step-wise growth and trimming of status PDU while covering several corner cases
void rlc_am_nr_control_pdu_test_trimming(rlc_am_sn_size sn_size)
{
  test_delimit_logger delimiter("Control PDU ({}) test trimming", sn_size);

  // status PDU with no NACKs
  {
    constexpr uint32_t       min_size = 3;
    const int                len      = 100;
    std::array<uint8_t, len> pdu      = {};
    rlc_am_status_pdu        status_pdu(sn_size);

    status_pdu.ack_sn = 99;
    TESTASSERT_EQ(status_pdu.get_packed_size(), min_size); // minimum size
    TESTASSERT_EQ(status_pdu.pack(pdu), min_size);

    // test copy assignment is deep copy
    rlc_am_status_pdu status_pdu_copy = rlc_am_status_pdu{status_pdu};
    TESTASSERT_EQ(status_pdu_copy.ack_sn, 99);
    TESTASSERT_EQ(status_pdu_copy.get_packed_size(), min_size); // minimum size
    TESTASSERT_EQ(status_pdu_copy.pack(pdu), min_size);

    // modify original
    status_pdu.reset();
    status_pdu.ack_sn = 77;
    TESTASSERT_EQ(status_pdu.get_packed_size(), min_size); // should still have minimum size
    TESTASSERT_EQ(status_pdu.pack(pdu), min_size);

    // check the copy has not changed
    TESTASSERT_EQ(status_pdu_copy.ack_sn, 99);                  // shouldn't have changed
    TESTASSERT_EQ(status_pdu_copy.get_packed_size(), min_size); // minimum size
    TESTASSERT_EQ(status_pdu_copy.pack(pdu), min_size);
  }

  // status PDU with multiple NACKs
  // expect: ACK=77, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5][27 444:666 r3]
  {
    constexpr uint32_t       min_size      = 3;
    const uint32_t           nack_size     = sn_size == rlc_am_sn_size::size12bits ? 2 : 3;
    constexpr uint32_t       so_size       = 4;
    constexpr uint32_t       range_size    = 1;
    uint32_t                 expected_size = min_size;
    const int                len           = 100;
    std::array<uint8_t, len> pdu           = {};
    rlc_am_status_pdu        status_pdu(sn_size);

    status_pdu.ack_sn = 77;
    {
      rlc_am_status_nack nack;
      nack.nack_sn = 12;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size;
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);
    {
      rlc_am_status_nack nack;
      nack.nack_sn = 14;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size;
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);
    {
      rlc_am_status_nack nack;
      nack.nack_sn  = 17;
      nack.has_so   = true;
      nack.so_start = 50;
      nack.so_end   = 99;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size;
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);
    {
      rlc_am_status_nack nack;
      nack.nack_sn  = 17;
      nack.has_so   = true;
      nack.so_start = 150;
      nack.so_end   = 199;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size;
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);
    {
      rlc_am_status_nack nack;
      nack.nack_sn  = 17;
      nack.has_so   = true;
      nack.so_start = 250;
      nack.so_end   = 299;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size;
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);
    {
      rlc_am_status_nack nack;
      nack.nack_sn = 19;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size;
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);
    {
      rlc_am_status_nack nack;
      nack.nack_sn        = 21;
      nack.has_so         = true;
      nack.so_start       = 333;
      nack.so_end         = 111;
      nack.has_nack_range = true;
      nack.nack_range     = 5;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);
    {
      rlc_am_status_nack nack;
      nack.nack_sn        = 27;
      nack.has_so         = true;
      nack.so_start       = 444;
      nack.so_end         = 666;
      nack.has_nack_range = true;
      nack.nack_range     = 3;
      status_pdu.push_nack(nack);
    }
    expected_size += nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.ack_sn, 77);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // current state: ACK=77, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5][27 444:666 r3]

    // create a copy, check content
    rlc_am_status_pdu status_pdu_copy               = rlc_am_status_pdu{status_pdu};
    uint32_t          status_pdu_copy_expected_size = expected_size;
    TESTASSERT_EQ(status_pdu_copy.ack_sn, 77);
    TESTASSERT_EQ(status_pdu_copy.get_packed_size(), status_pdu_copy_expected_size);
    TESTASSERT_EQ(status_pdu_copy.pack(pdu), expected_size);

    // current state: ACK=77, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5][27 444:666 r3]

    // trim to much larger size: nothing should change
    TESTASSERT_EQ(status_pdu.trim(expected_size * 2), true);
    TESTASSERT_EQ(status_pdu.ack_sn, 77);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // trim to exact size: nothing should change
    TESTASSERT_EQ(status_pdu.trim(expected_size), true);
    TESTASSERT_EQ(status_pdu.ack_sn, 77);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // trim to (expected_size - 1): this should remove the last NACK and update ACK accordingly
    TESTASSERT_EQ(status_pdu.trim(expected_size - 1), true);
    expected_size -= nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.ack_sn, 27);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // current state: ACK=27, NACKs=[12][14][17 50:99][17 150:199][17 250:299][19][21 333:111 r5]

    // trim to (expected_size - last two NACKs): this should remove the last NACK and update ACK accordingly
    TESTASSERT_EQ(status_pdu.trim(expected_size - (2 * nack_size + so_size + range_size)), true);
    expected_size -= 2 * nack_size + so_size + range_size;
    TESTASSERT_EQ(status_pdu.ack_sn, 19);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // current state: ACK=19, NACKs=[12][14][17 50:99][17 150:199][17 250:299]

    // trim to (expected_size - 1): this should remove the last NACK and all other NACKs with the same SN
    TESTASSERT_EQ(status_pdu.trim(expected_size - 1), true);
    expected_size -= 3 * (nack_size + so_size);
    TESTASSERT_EQ(status_pdu.ack_sn, 17);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // current state: ACK=17, NACKs=[12][14]

    // trim to impossible size = 1: this should report a failure without changes of the PDU
    TESTASSERT_EQ(status_pdu.trim(1), false);
    TESTASSERT_EQ(status_pdu.ack_sn, 17);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // current state: ACK=17, NACKs=[12][14]

    // trim to minimum size: this should remove all NACKs and update ACK to the SN of the first NACK
    expected_size = min_size;
    TESTASSERT_EQ(status_pdu.trim(expected_size), true);
    TESTASSERT_EQ(status_pdu.ack_sn, 12);
    TESTASSERT_EQ(status_pdu.get_packed_size(), expected_size);
    TESTASSERT_EQ(status_pdu.pack(pdu), expected_size);

    // current state: ACK=12, NACKs empty

    // check the copy again - should be unchanged if not a shallow copy
    TESTASSERT_EQ(status_pdu_copy.ack_sn, 77);
    TESTASSERT_EQ(status_pdu_copy.get_packed_size(), status_pdu_copy_expected_size);
    TESTASSERT_EQ(status_pdu_copy.pack(pdu), status_pdu_copy_expected_size);
  }
}

} // namespace srsran

int main()
{
  auto& logger = srslog::fetch_basic_logger("TEST", false);
  logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  logger.info("Testing AM PDU packing/unpacking");
  srslog::flush();
  srsran::test_rlc_am_12bit_complete_sdu();
  srsran::test_rlc_am_12bit_first_segment();
  srsran::test_rlc_am_12bit_middle_segment();
  srsran::test_rlc_am_12bit_last_segment();
  srsran::test_rlc_am_18bit_complete_sdu();
  srsran::test_rlc_am_18bit_first_segment();
  srsran::test_rlc_am_18bit_middle_segment();
  srsran::test_rlc_am_18bit_last_segment();
  srsran::test_rlc_am_18bit_malformed();

  srsran::test_rlc_am_12bit_short_header_missing_lower_sn();
  srsran::test_rlc_am_12bit_short_header_missing_upper_so();
  srsran::test_rlc_am_12bit_short_header_missing_lower_so();
  srsran::test_rlc_am_18bit_short_header_missing_center_sn();
  srsran::test_rlc_am_18bit_short_header_missing_lower_sn();
  srsran::test_rlc_am_18bit_short_header_missing_upper_so();
  srsran::test_rlc_am_18bit_short_header_missing_lower_so();

  logger.info("Testing Control PDU packing/unpacking (12 bit)");
  srslog::flush();
  srsran::test_control_pdu_no_nack_12bit();
  srsran::test_control_pdu_with_nack_12bit();
  srsran::test_control_pdu_nacks_and_so_12bit();
  srsran::test_control_pdu_nacks_and_mixed_so_12bit();
  srsran::test_control_pdu_nacks_and_range_12bit();
  srsran::test_control_pdu_invalid_too_short_12bit();
  srsran::test_control_pdu_invalid_e1_extension_cross_boundary_12bit();
  srsran::test_control_pdu_invalid_e2_extension_cross_boundary_12bit();
  srsran::test_control_pdu_invalid_e3_extension_cross_boundary_12bit();

  logger.info("Testing Control PDU packing/unpacking (18 bit)");
  srslog::flush();
  srsran::test_control_pdu_no_nack_18bit();
  srsran::test_control_pdu_with_nack_18bit();
  srsran::test_control_pdu_nacks_and_so_18bit();
  srsran::test_control_pdu_nacks_and_mixed_so_18bit();
  srsran::test_control_pdu_nacks_and_range_18bit();
  srsran::test_control_pdu_invalid_too_short_18bit();
  srsran::test_control_pdu_invalid_e1_extension_cross_boundary_18bit();
  srsran::test_control_pdu_invalid_e2_extension_cross_boundary_18bit();
  srsran::test_control_pdu_invalid_e3_extension_cross_boundary_18bit();

  logger.info("Testing Status PDU functions (12 bit)");
  srslog::flush();
  srsran::rlc_am_sn_size sn_size = srsran::rlc_am_sn_size::size12bits;
  srsran::rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(sn_size);
  //  srsran::rlc_am_nr_control_pdu_test_nack_merge_range_sdu(sn_size);
  //  srsran::rlc_am_nr_control_pdu_test_nack_merge_sdu_range(sn_size);
  //  srsran::rlc_am_nr_control_pdu_test_nack_merge_range_range(sn_size);
  srsran::rlc_am_nr_control_pdu_test_trimming(sn_size);

  logger.info("Testing Status PDU functions (18 bit)");
  srslog::flush();
  sn_size = srsran::rlc_am_sn_size::size18bits;
  srsran::rlc_am_nr_control_pdu_test_nack_merge_sdu_sdu(sn_size);
  srsran::rlc_am_nr_control_pdu_test_nack_merge_range_sdu(sn_size);
  // srsran::rlc_am_nr_control_pdu_test_nack_merge_sdu_range(sn_size);
  // srsran::rlc_am_nr_control_pdu_test_nack_merge_range_range(sn_size);
  srsran::rlc_am_nr_control_pdu_test_trimming(sn_size);
}
