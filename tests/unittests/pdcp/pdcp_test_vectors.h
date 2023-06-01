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
#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/pdcp/pdcp_config.h"
#include <array>
#include <cstdint>

namespace srsran {
// Encryption and Integrity Keys (256 and 128 bits)
const std::array<uint8_t, 32> k_int = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
                                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
                                       0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
const std::array<uint8_t, 32> k_enc = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
                                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21,
                                       0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

const std::array<uint8_t, 16> k_128_int =
    {0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};
const std::array<uint8_t, 16> k_128_enc =
    {0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31};

// Test SDUs for TX.
const int                           sdu_size = 2;
const std::array<uint8_t, sdu_size> sdu1     = {0x18, 0xe2};
const std::array<uint8_t, sdu_size> sdu2     = {0xde, 0xad};

// Test PDUs for RX.
// Generated from SDU1, using NIA1 and NEA1.
const int pdu_size_snlen12 = 8;
const int pdu_size_snlen18 = 9;

// 12 bits
const std::array<uint8_t, pdu_size_snlen12> pdu1_count0_snlen12      = {0x80, 0x00, 0x28, 0xb7, 0xe0, 0xc5, 0x10, 0x48};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count1_snlen12      = {0x80, 0x01, 0x34, 0x68, 0xae, 0x56, 0xdc, 0x2c};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count2_snlen12      = {0x80, 0x02, 0x78, 0x72, 0x65, 0xa9, 0x09, 0xdb};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count2047_snlen12   = {0x87, 0xff, 0x39, 0xb4, 0x2c, 0x50, 0x9f, 0xf6};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count2048_snlen12   = {0x88, 0x00, 0xe3, 0x37, 0xfe, 0x24, 0xc3, 0xe9};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count4095_snlen12   = {0x8f, 0xff, 0x1a, 0x08, 0xbe, 0xa4, 0x32, 0x86};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count4096_snlen12   = {0x80, 0x00, 0x4f, 0xd2, 0x5c, 0xdc, 0x86, 0xfe};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count262143_snlen12 = {0x8f, 0xff, 0xa7, 0xe3, 0x3f, 0x0d, 0xc0, 0x94};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count262144_snlen12 = {0x80, 0x00, 0x6d, 0xc2, 0x76, 0xe8, 0xe1, 0x55};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count262145_snlen12 = {0x80, 0x01, 0xed, 0xb2, 0x64, 0x36, 0x40, 0x80};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count262146_snlen12 = {0x80, 0x02, 0xe0, 0x06, 0x0d, 0x18, 0xc1, 0xf6};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count262147_snlen12 = {0x80, 0x03, 0x10, 0x49, 0x14, 0x0d, 0xd1, 0x1b};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count262148_snlen12 = {0x80, 0x04, 0xfc, 0x2d, 0x3e, 0xcf, 0xb4, 0xa2};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count4294967295_snlen12 =
    {0x8f, 0xff, 0xaa, 0x13, 0xb5, 0x1f, 0x8a, 0x68};

// 18 bits
const std::array<uint8_t, pdu_size_snlen18> pdu1_count0_snlen18 =
    {0x80, 0x00, 0x00, 0x28, 0xb7, 0x87, 0xb7, 0x5f, 0xd7};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count1_snlen18 =
    {0x80, 0x00, 0x01, 0x34, 0x68, 0x44, 0x26, 0xcc, 0xe5};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count2_snlen18 =
    {0x80, 0x00, 0x02, 0x78, 0x72, 0xfc, 0xdf, 0xba, 0xa9};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count131071_snlen18 =
    {0x81, 0xff, 0xff, 0x74, 0x2f, 0xce, 0x85, 0x95, 0xc4};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count131072_snlen18 =
    {0x82, 0x00, 0x00, 0x23, 0xd6, 0x92, 0x6f, 0x3b, 0xb0};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count262143_snlen18 =
    {0x83, 0xff, 0xff, 0x79, 0xae, 0xc4, 0x4e, 0xe8, 0x41};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count262144_snlen18 =
    {0x80, 0x00, 0x00, 0x74, 0xb7, 0x50, 0x4e, 0xb9, 0x0c};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count262145_snlen18 =
    {0x80, 0x00, 0x01, 0xed, 0xb2, 0x7b, 0x93, 0x5d, 0xf6};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count262146_snlen18 =
    {0x80, 0x00, 0x02, 0xe0, 0x06, 0xc5, 0x93, 0xfd, 0x16};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count262147_snlen18 =
    {0x80, 0x00, 0x03, 0x10, 0x49, 0x58, 0x17, 0x34, 0x71};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count262148_snlen18 =
    {0x80, 0x00, 0x04, 0xfc, 0x2d, 0x35, 0x36, 0x76, 0x33};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count334103_snlen18 =
    {0x81, 0x19, 0x17, 0x95, 0x46, 0xd3, 0xf8, 0x79, 0x5c};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count334104_snlen18 =
    {0x81, 0x19, 0x18, 0x64, 0xb4, 0x72, 0x84, 0xc7, 0xe4};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count4294967295_snlen18 =
    {0x83, 0xff, 0xff, 0xaa, 0x13, 0x1a, 0x29, 0x54, 0x12};

/// \brief Gets expected PDU based on the COUNT being tested
/// \param sn_size SN size of the PDU test vector
/// \param count COUNT of the PDU test vector
/// \param exp_pdu Expected PDU that is set to the correct test vector
/// \return true if we found the PDU, false other wise
inline bool get_pdu_test_vector(pdcp_sn_size sn_size, uint32_t count, byte_buffer& exp_pdu)
{
  if (sn_size == pdcp_sn_size::size12bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer{pdu1_count0_snlen12};
        return true;
      case 1:
        exp_pdu = byte_buffer{pdu1_count1_snlen12};
        return true;
      case 2:
        exp_pdu = byte_buffer{pdu1_count2_snlen12};
        return true;
      case 2047:
        exp_pdu = byte_buffer{pdu1_count2047_snlen12};
        return true;
      case 2048:
        exp_pdu = byte_buffer{pdu1_count2048_snlen12};
        return true;
      case 4095:
        exp_pdu = byte_buffer{pdu1_count4095_snlen12};
        return true;
      case 4096:
        exp_pdu = byte_buffer{pdu1_count4096_snlen12};
        return true;
      case 262143:
        exp_pdu = byte_buffer{pdu1_count262143_snlen12};
        return true;
      case 262144:
        exp_pdu = byte_buffer{pdu1_count262144_snlen12};
        return true;
      case 262145:
        exp_pdu = byte_buffer{pdu1_count262145_snlen12};
        return true;
      case 262146:
        exp_pdu = byte_buffer{pdu1_count262146_snlen12};
        return true;
      case 262147:
        exp_pdu = byte_buffer{pdu1_count262147_snlen12};
        return true;
      case 262148:
        exp_pdu = byte_buffer{pdu1_count262148_snlen12};
        return true;
      case 4294967295:
        exp_pdu = byte_buffer{pdu1_count4294967295_snlen12};
        return true;
      default:
        return false;
    }
  } else if (sn_size == pdcp_sn_size::size18bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer{pdu1_count0_snlen18};
        return true;
      case 1:
        exp_pdu = byte_buffer{pdu1_count1_snlen18};
        return true;
      case 2:
        exp_pdu = byte_buffer{pdu1_count2_snlen18};
        return true;
      case 131071:
        exp_pdu = byte_buffer{pdu1_count131071_snlen18};
        return true;
      case 131072:
        exp_pdu = byte_buffer{pdu1_count131072_snlen18};
        return true;
      case 262143:
        exp_pdu = byte_buffer{pdu1_count262143_snlen18};
        return true;
      case 262144:
        exp_pdu = byte_buffer{pdu1_count262144_snlen18};
        return true;
      case 262145:
        exp_pdu = byte_buffer{pdu1_count262145_snlen18};
        return true;
      case 262146:
        exp_pdu = byte_buffer{pdu1_count262146_snlen18};
        return true;
      case 262147:
        exp_pdu = byte_buffer{pdu1_count262147_snlen18};
        return true;
      case 262148:
        exp_pdu = byte_buffer{pdu1_count262148_snlen18};
        return true;
      case 334103:
        exp_pdu = byte_buffer{pdu1_count334103_snlen18};
        return true;
      case 334104:
        exp_pdu = byte_buffer{pdu1_count334104_snlen18};
        return true;
      case 4294967295:
        exp_pdu = byte_buffer{pdu1_count4294967295_snlen18};
        return true;
      default:
        return false;
    }
  } else {
    return false;
  }
  return false;
}

} // namespace srsran
