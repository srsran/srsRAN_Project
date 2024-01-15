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

#include "lib/rlc/rlc_um_pdu.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/test_utils.h"

namespace srsran {

template <std::size_t N>
byte_buffer make_pdu_and_log(const std::array<uint8_t, N>& tv)
{
  byte_buffer pdu;
  report_error_if_not(pdu.append(tv), "Failed to append to byte_buffer");
  // write_pdu_to_pcap(4, tv.data(), tv.size()); TODO
  return pdu;
}

size_t copy_pdu_and_log(span<uint8_t> out_pdu, const span<uint8_t> in_pdu)
{
  TESTASSERT(in_pdu.size() <= out_pdu.size());
  std::copy(in_pdu.begin(), in_pdu.end(), out_pdu.begin());
  return in_pdu.size();
}

// Test RLC UM PDU with 6 Bit SN carrying the full SDU
void test_rlc_um_6bit_complete_sdu()
{
  test_delimit_logger delimiter{"UM PDU with 6 Bit SN and SI=full"};

  const int                                     header_len = 1, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {0x00, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};
  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size6bits, &hdr));
    TESTASSERT(hdr.si == rlc_si_field::full_sdu);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Test RLC UM PDU with 6 Bit SN carrying a first segment of an SDU
void test_rlc_um_6bit_first_segment()
{
  test_delimit_logger delimiter{"UM PDU with 6 Bit SN and SI=first"};
  // SN = 31
  const int                                     header_len = 1, payload_len = 4;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {0x5f, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>              tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size6bits, &hdr) != 0);

    TESTASSERT(hdr.si == rlc_si_field::first_segment);
    TESTASSERT(hdr.so == 0);
    TESTASSERT(hdr.sn == 31);
    TESTASSERT(hdr.sn_size == rlc_um_sn_size::size6bits);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Test RLC UM PDU with 6 Bit SN carrying a middle segment of an SDU
void test_rlc_um_6bit_middle_segment()
{
  test_delimit_logger delimiter{"UM PDU with 6 Bit SN and SI=middle"};
  // SN = 3
  const int                                           header_len = 3, payload_len = 4;
  const std::array<uint8_t, header_len + payload_len> tv_pdu = {0xc3, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>                    tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size6bits, &hdr) != 0);

    TESTASSERT(hdr.si == rlc_si_field::middle_segment);
    TESTASSERT(hdr.so == 258);
    TESTASSERT(hdr.sn == 3);
    TESTASSERT(hdr.sn_size == rlc_um_sn_size::size6bits);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Test RLC UM PDU with 6 Bit SN carrying the last segment of an SDU
void test_rlc_um_6bit_last_segment()
{
  test_delimit_logger delimiter{"UM PDU with 6 Bit SN and SI=last"};
  // SN = 1
  const int                                           header_len = 3, payload_len = 4;
  const std::array<uint8_t, header_len + payload_len> tv_pdu = {0x81, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  std::array<uint8_t, payload_len>                    tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size6bits, &hdr) != 0);

    TESTASSERT(hdr.si == rlc_si_field::last_segment);
    TESTASSERT(hdr.sn == 1);
    TESTASSERT(hdr.so == 258);
    TESTASSERT(hdr.sn_size == rlc_um_sn_size::size6bits);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Test RLC UM PDU with 12bit SN and full SDU
void test_rlc_um_12bit_complete_sdu()
{
  test_delimit_logger delimiter{"UM PDU with 12 Bit SN and SI=full"};

  const int                                     header_len = 1, payload_len = 87;
  std::array<uint8_t, header_len + payload_len> tv_pdu = {
      0x00, 0x80, 0x00, 0x01, 0x45, 0x00, 0x00, 0x54, 0x34, 0xee, 0x40, 0x00, 0x40, 0x01, 0x80, 0x67, 0xc0, 0xa8,
      0x02, 0x01, 0xc0, 0xa8, 0x02, 0x02, 0x08, 0x00, 0xf0, 0x38, 0x56, 0x9b, 0x00, 0x02, 0x74, 0x40, 0x35, 0x60,
      0x00, 0x00, 0x00, 0x00, 0x3e, 0xb6, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
      0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
      0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  std::array<uint8_t, payload_len> tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size12bits, &hdr) != 0);

    TESTASSERT(hdr.si == rlc_si_field::full_sdu);
    TESTASSERT(hdr.so == 0);
    TESTASSERT(hdr.sn == 0);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Unpack RLC UM 12bit SN first segment PDU SI=01 SN=5
void test_rlc_um_12bit_first_segment()
{
  test_delimit_logger delimiter{"UM PDU with 12 Bit SN and SI=first"};

  const int                header_len = 2, payload_len = 181;
  std::array<uint8_t, 183> tv_pdu = {
      0x40, 0x05, 0x80, 0x00, 0x05, 0x45, 0x00, 0x04, 0x04, 0xf1, 0xaf, 0x40, 0x00, 0x40, 0x01, 0xbd, 0xf5, 0xc0, 0xa8,
      0x03, 0x01, 0xc0, 0xa8, 0x03, 0x02, 0x08, 0x00, 0xbc, 0xea, 0x0b, 0x0d, 0x00, 0x07, 0x81, 0x10, 0x5b, 0x60, 0x00,
      0x00, 0x00, 0x00, 0xed, 0x4c, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
      0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
      0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
      0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
      0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
      0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
      0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95};
  std::array<uint8_t, payload_len> tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size12bits, &hdr) != 0);

    TESTASSERT(hdr.si == rlc_si_field::first_segment);
    TESTASSERT(hdr.so == 0);
    TESTASSERT(hdr.sn == 5);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Test RLC UM 12bit SN middle segment PDU SI=11 SN=5 SO=360
void test_rlc_um_12bit_middle_segment()
{
  test_delimit_logger delimiter{"UM PDU with 12 Bit SN and SI=middle"};

  const int                header_len = 4, payload_len = 179;
  std::array<uint8_t, 183> tv_pdu = {
      0xc0, 0x05, 0x01, 0x68, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
      0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
      0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d,
      0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90,
      0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3,
      0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
      0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
      0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc,
      0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
      0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb};
  std::array<uint8_t, payload_len> tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size12bits, &hdr) != 0);

    TESTASSERT(hdr.si == rlc_si_field::middle_segment);
    TESTASSERT(hdr.so == 360);
    TESTASSERT(hdr.sn == 5);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// Test RLC UM PDU 12bit SN last segment PDU SI=10 SN=1542 SO=972
void test_rlc_um_12bit_last_segment()
{
  test_delimit_logger delimiter{"UM PDU with 12 Bit SN and SI=last"};

  const int                header_len = 4, payload_len = 102;
  std::array<uint8_t, 106> tv_pdu = {0x86, 0x06, 0x03, 0xcc, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
                                     0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2,
                                     0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
                                     0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc,
                                     0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7};
  std::array<uint8_t, payload_len> tv_sdu = {};
  std::copy(tv_pdu.begin() + header_len, tv_pdu.end(), tv_sdu.begin());
  rlc_um_pdu_header hdr = {};

  {
    // Unpack
    byte_buffer buf = make_pdu_and_log(tv_pdu);
    TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size12bits, &hdr) != 0);

    TESTASSERT(hdr.si == rlc_si_field::last_segment);
    TESTASSERT(hdr.so == 972);
    TESTASSERT(hdr.sn == 1542);
  }
  {
    // Pack
    std::array<uint8_t, header_len + payload_len> buf;
    TESTASSERT(rlc_um_write_data_pdu_header(buf, hdr) == header_len);
    TESTASSERT_EQ(copy_pdu_and_log(span<uint8_t>(buf).subspan(header_len, payload_len), tv_sdu), payload_len);
    TESTASSERT(buf == tv_pdu);
  }
}

// This should fail unpacking because the PDU has reserved bits set
void test_rlc_um_6bit_malformed_pdu()
{
  test_delimit_logger    delimiter{"Malformed UM PDU with 6 Bit SN"};
  std::array<uint8_t, 7> tv  = {0x33, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  byte_buffer            buf = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_pdu_header hdr = {};
  TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size6bits, &hdr) == false);
}

// This should fail unpacking because the PDU has reserved bits set
void test_rlc_um_12bit_malformed_pdu()
{
  test_delimit_logger    delimiter{"Malformed UM PDU with 12 Bit SN"};
  std::array<uint8_t, 7> tv  = {0x33, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44};
  byte_buffer            buf = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_pdu_header hdr = {};
  TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size12bits, &hdr) == false);
}

// This should fail unpacking because the PDU header is too short (upper SO is missing)
void test_rlc_um_6bit_missing_upper_so()
{
  test_delimit_logger    delimiter{"Short UM PDU with 6 Bit SN (upper SO is missing)"};
  std::array<uint8_t, 1> tv  = {0b11000000};
  byte_buffer            buf = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_pdu_header hdr = {};
  TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size6bits, &hdr) == false);
}

// This should fail unpacking because the PDU header is too short (lower SO is missing)
void test_rlc_um_6bit_missing_lower_so()
{
  test_delimit_logger    delimiter{"Short UM PDU with 6 Bit SN (lower SO is missing)"};
  std::array<uint8_t, 2> tv  = {0b11000000, 0xaa};
  byte_buffer            buf = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_pdu_header hdr = {};
  TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size6bits, &hdr) == false);
}

// This should fail unpacking because the PDU header is too short (upper SO is missing)
void test_rlc_um_12bit_missing_upper_so()
{
  test_delimit_logger    delimiter{"Short UM PDU with 12 Bit SN (upper SO is missing)"};
  std::array<uint8_t, 2> tv  = {0b11000000, 0x00};
  byte_buffer            buf = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_pdu_header hdr = {};
  TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size12bits, &hdr) == false);
}

// This should fail unpacking because the PDU header is too short (lower SO is missing)
void test_rlc_um_12bit_missing_lower_so()
{
  test_delimit_logger    delimiter{"Short UM PDU with 12 Bit SN (lower SO is missing)"};
  std::array<uint8_t, 3> tv  = {0b11000000, 0x00, 0xaa};
  byte_buffer            buf = make_pdu_and_log(tv);

  // unpack PDU
  rlc_um_pdu_header hdr = {};
  TESTASSERT(rlc_um_read_data_pdu_header(buf, rlc_um_sn_size::size12bits, &hdr) == false);
}

} // namespace srsran

int main()
{
  auto& logger = srslog::fetch_basic_logger("TEST", false);
  logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  logger.info("Testing UM PDU packing/unpacking");
  srslog::flush();
  srsran::test_rlc_um_6bit_complete_sdu();
  srsran::test_rlc_um_6bit_first_segment();
  srsran::test_rlc_um_6bit_middle_segment();
  srsran::test_rlc_um_6bit_last_segment();

  srsran::test_rlc_um_12bit_complete_sdu();
  srsran::test_rlc_um_12bit_first_segment();
  srsran::test_rlc_um_12bit_middle_segment();
  srsran::test_rlc_um_12bit_last_segment();

  srsran::test_rlc_um_6bit_malformed_pdu();
  srsran::test_rlc_um_6bit_missing_upper_so();
  srsran::test_rlc_um_6bit_missing_lower_so();

  srsran::test_rlc_um_12bit_malformed_pdu();
  srsran::test_rlc_um_12bit_missing_upper_so();
  srsran::test_rlc_um_12bit_missing_lower_so();
}
