/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include <array>
#include <cstdint>

namespace srsgnb {
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
const int                                   pdu_size_snlen12       = 8;
const int                                   pdu_size_snlen18       = 9;
const std::array<uint8_t, pdu_size_snlen12> pdu1_count0_snlen12    = {0x80, 0x00, 0x28, 0xb7, 0xe0, 0xc5, 0x10, 0x48};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count2048_snlen12 = {0x88, 0x00, 0xe3, 0x37, 0xfe, 0x24, 0xc3, 0xe9};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count4096_snlen12 = {0x80, 0x00, 0x4f, 0xd2, 0x5c, 0xdc, 0x86, 0xfe};
const std::array<uint8_t, pdu_size_snlen12> pdu1_count4294967295_snlen12 =
    {0x8f, 0xff, 0xaa, 0x13, 0xb5, 0x1f, 0x8a, 0x68};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count0_snlen18 =
    {0x80, 0x00, 0x00, 0x28, 0xb7, 0x87, 0xb7, 0x5f, 0xd7};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count131072_snlen18 =
    {0x82, 0x00, 0x00, 0x23, 0xd6, 0x92, 0x6f, 0x3b, 0xb0};
const std::array<uint8_t, pdu_size_snlen18> pdu1_count262144_snlen18 =
    {0x80, 0x00, 0x00, 0x74, 0xb7, 0x50, 0x4e, 0xb9, 0x0c};
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
      case 2048:
        exp_pdu = byte_buffer{pdu1_count2048_snlen12};
        return true;
      case 4096:
        exp_pdu = byte_buffer{pdu1_count4096_snlen12};
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
      case 131072:
        exp_pdu = byte_buffer{pdu1_count131072_snlen18};
        return true;
      case 262144:
        exp_pdu = byte_buffer{pdu1_count262144_snlen18};
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

} // namespace srsgnb
