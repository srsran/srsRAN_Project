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

// 12 bits (NIA1, NEA1)
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count0_snlen12 =
    {0x80, 0x00, 0x28, 0xb7, 0xe0, 0xc5, 0x10, 0x48};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count1_snlen12 =
    {0x80, 0x01, 0x34, 0x68, 0xae, 0x56, 0xdc, 0x2c};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count2_snlen12 =
    {0x80, 0x02, 0x78, 0x72, 0x65, 0xa9, 0x09, 0xdb};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count2047_snlen12 =
    {0x87, 0xff, 0x39, 0xb4, 0x2c, 0x50, 0x9f, 0xf6};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count2048_snlen12 =
    {0x88, 0x00, 0xe3, 0x37, 0xfe, 0x24, 0xc3, 0xe9};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count2049_snlen12 =
    {0x88, 0x01, 0xdf, 0xc8, 0x5c, 0xa6, 0x9b, 0x11};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count4095_snlen12 =
    {0x8f, 0xff, 0x1a, 0x08, 0xbe, 0xa4, 0x32, 0x86};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count4096_snlen12 =
    {0x80, 0x00, 0x4f, 0xd2, 0x5c, 0xdc, 0x86, 0xfe};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count4097_snlen12 =
    {0x80, 0x01, 0x1d, 0x2c, 0x66, 0xd3, 0xcb, 0xd1};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count262143_snlen12 =
    {0x8f, 0xff, 0xa7, 0xe3, 0x3f, 0x0d, 0xc0, 0x94};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count262144_snlen12 =
    {0x80, 0x00, 0x6d, 0xc2, 0x76, 0xe8, 0xe1, 0x55};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count262145_snlen12 =
    {0x80, 0x01, 0xed, 0xb2, 0x64, 0x36, 0x40, 0x80};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count262146_snlen12 =
    {0x80, 0x02, 0xe0, 0x06, 0x0d, 0x18, 0xc1, 0xf6};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count262147_snlen12 =
    {0x80, 0x03, 0x10, 0x49, 0x14, 0x0d, 0xd1, 0x1b};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count262148_snlen12 =
    {0x80, 0x04, 0xfc, 0x2d, 0x3e, 0xcf, 0xb4, 0xa2};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo1_count4294967295_snlen12 =
    {0x8f, 0xff, 0xaa, 0x13, 0xb5, 0x1f, 0x8a, 0x68};

// 18 bits (NIA1, NEA1)
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count0_snlen18 =
    {0x80, 0x00, 0x00, 0x28, 0xb7, 0x87, 0xb7, 0x5f, 0xd7};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count1_snlen18 =
    {0x80, 0x00, 0x01, 0x34, 0x68, 0x44, 0x26, 0xcc, 0xe5};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count2_snlen18 =
    {0x80, 0x00, 0x02, 0x78, 0x72, 0xfc, 0xdf, 0xba, 0xa9};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count131071_snlen18 =
    {0x81, 0xff, 0xff, 0x74, 0x2f, 0xce, 0x85, 0x95, 0xc4};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count131072_snlen18 =
    {0x82, 0x00, 0x00, 0x23, 0xd6, 0x92, 0x6f, 0x3b, 0xb0};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count131073_snlen18 =
    {0x82, 0x00, 0x01, 0xcb, 0x24, 0x4b, 0x64, 0x0e, 0x88};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count262143_snlen18 =
    {0x83, 0xff, 0xff, 0x79, 0xae, 0xc4, 0x4e, 0xe8, 0x41};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count262144_snlen18 =
    {0x80, 0x00, 0x00, 0x74, 0xb7, 0x50, 0x4e, 0xb9, 0x0c};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count262145_snlen18 =
    {0x80, 0x00, 0x01, 0x40, 0x38, 0xec, 0x5a, 0x41, 0x19};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count262146_snlen18 =
    {0x80, 0x00, 0x02, 0x0c, 0x98, 0xdc, 0x6f, 0xf4, 0xc8};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count262147_snlen18 =
    {0x80, 0x00, 0x03, 0x71, 0x8c, 0x89, 0x7e, 0x99, 0x56};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count262148_snlen18 =
    {0x80, 0x00, 0x04, 0x5b, 0x48, 0xdd, 0xc5, 0xc0, 0x5a};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count334103_snlen18 =
    {0x81, 0x19, 0x17, 0x7c, 0xcb, 0x24, 0xf1, 0x05, 0xfc};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count334104_snlen18 =
    {0x81, 0x19, 0x18, 0x62, 0xbd, 0x16, 0x72, 0x33, 0x16};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo1_count4294967295_snlen18 =
    {0x83, 0xff, 0xff, 0xaa, 0x13, 0x1a, 0x29, 0x54, 0x12};

// 12 bits (NIA2, NEA2)
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count0_snlen12 =
    {0x80, 0x00, 0xd9, 0x50, 0x8a, 0xe3, 0xe7, 0x06};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count1_snlen12 =
    {0x80, 0x01, 0x9e, 0xfe, 0xef, 0x99, 0x50, 0xa5};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count2_snlen12 =
    {0x80, 0x02, 0x7c, 0xab, 0x1a, 0x84, 0xfb, 0x88};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count2047_snlen12 =
    {0x87, 0xff, 0x12, 0x14, 0x9f, 0x4f, 0xb8, 0x30};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count2048_snlen12 =
    {0x88, 0x00, 0xb0, 0xae, 0x40, 0xe2, 0x31, 0x38};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count2049_snlen12 =
    {0x88, 0x01, 0x24, 0x07, 0x26, 0xd6, 0x3b, 0xbb};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count4095_snlen12 =
    {0x8f, 0xff, 0x50, 0xcf, 0xe6, 0x65, 0x0b, 0xba};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count4096_snlen12 =
    {0x80, 0x00, 0x36, 0xf4, 0x83, 0x8a, 0x60, 0x08};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count4097_snlen12 =
    {0x80, 0x01, 0xbb, 0xe6, 0x27, 0x7d, 0xc4, 0xaf};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count262143_snlen12 =
    {0x8f, 0xff, 0x95, 0xb5, 0x46, 0xdf, 0x58, 0x9e};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count262144_snlen12 =
    {0x80, 0x00, 0x6d, 0xc2, 0x76, 0xe8, 0xe1, 0x55};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count262145_snlen12 =
    {0x80, 0x01, 0x8c, 0xad, 0x9e, 0x27, 0xdc, 0xd7};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count262146_snlen12 =
    {0x80, 0x02, 0xdd, 0x8d, 0x0c, 0x3c, 0xe2, 0x36};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count262147_snlen12 =
    {0x80, 0x03, 0xf5, 0x47, 0x26, 0x07, 0x26, 0xce};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count262148_snlen12 =
    {0x80, 0x04, 0x53, 0x7c, 0xbc, 0x67, 0x99, 0x8f};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo2_count4294967295_snlen12 =
    {0x8f, 0xff, 0xdd, 0xf7, 0x1a, 0xf6, 0x79, 0x15};

// 18 bits (NIA2, NEA2)
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count0_snlen18 =
    {0x80, 0x00, 0x00, 0xd9, 0x50, 0x25, 0xdc, 0x83, 0xc9};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count1_snlen18 =
    {0x80, 0x00, 0x01, 0x9e, 0xfe, 0x2a, 0x62, 0xa4, 0x71};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count2_snlen18 =
    {0x80, 0x00, 0x02, 0x7c, 0xab, 0x9f, 0x26, 0x2c, 0x5f};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count131071_snlen18 =
    {0x81, 0xff, 0xff, 0x6b, 0x30, 0x12, 0xd1, 0x38, 0x1e};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count131072_snlen18 =
    {0x82, 0x00, 0x00, 0x50, 0x5e, 0x94, 0xd1, 0x3c, 0x3a};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count131073_snlen18 =
    {0x82, 0x00, 0x01, 0x6b, 0xf4, 0x65, 0xb2, 0xb3, 0x10};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count262143_snlen18 =
    {0x83, 0xff, 0xff, 0x95, 0xb5, 0x69, 0x09, 0x9c, 0x9e};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count262144_snlen18 =
    {0x80, 0x00, 0x00, 0x2b, 0x8d, 0x60, 0xc8, 0x16, 0x47};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count262145_snlen18 =
    {0x80, 0x00, 0x01, 0x8c, 0xad, 0x76, 0x33, 0x7e, 0xea};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count262146_snlen18 =
    {0x80, 0x00, 0x02, 0xdd, 0x8d, 0x00, 0x30, 0xb5, 0xe0};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count262147_snlen18 =
    {0x80, 0x00, 0x03, 0xf5, 0x47, 0xf9, 0xd4, 0xe4, 0x15};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count262148_snlen18 =
    {0x80, 0x00, 0x04, 0x53, 0x7c, 0x32, 0xa6, 0xa2, 0x6d};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count334103_snlen18 =
    {0x81, 0x19, 0x17, 0xe5, 0xc7, 0xa7, 0x9c, 0xf5, 0xa3};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count334104_snlen18 =
    {0x81, 0x19, 0x18, 0x1e, 0x63, 0x73, 0xbf, 0x89, 0xe3};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo2_count4294967295_snlen18 =
    {0x83, 0xff, 0xff, 0xdd, 0xf7, 0xe5, 0x59, 0x03, 0xc9};

// 12 bits (NIA3, NEA3)
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count0_snlen12 =
    {0x80, 0x00, 0x3e, 0x36, 0xba, 0x3f, 0xf0, 0x32};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count1_snlen12 =
    {0x80, 0x01, 0x0f, 0xf6, 0xb7, 0xf1, 0x3e, 0x93};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count2_snlen12 =
    {0x80, 0x02, 0x39, 0x40, 0xe5, 0x05, 0x5b, 0xaf};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count2047_snlen12 =
    {0x87, 0xff, 0x4c, 0x68, 0x9c, 0xcd, 0x75, 0x7c};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count2048_snlen12 =
    {0x88, 0x00, 0x95, 0xfb, 0x3c, 0xcb, 0x17, 0xcf};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count2049_snlen12 =
    {0x88, 0x01, 0x1a, 0x67, 0xa4, 0xf9, 0xe1, 0x41};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count4095_snlen12 =
    {0x8f, 0xff, 0xa6, 0x9a, 0x69, 0x22, 0x41, 0xcc};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count4096_snlen12 =
    {0x80, 0x00, 0xc8, 0xe9, 0x15, 0x6a, 0x03, 0x9b};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count4097_snlen12 =
    {0x80, 0x01, 0xf6, 0xd6, 0xb9, 0x37, 0xdb, 0x21};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count262143_snlen12 =
    {0x8f, 0xff, 0x2e, 0xbe, 0x5e, 0x09, 0xed, 0x77};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count262144_snlen12 =
    {0x80, 0x00, 0xdc, 0x81, 0x92, 0x75, 0xf1, 0x96};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count262145_snlen12 =
    {0x80, 0x01, 0x36, 0x33, 0x71, 0xd7, 0x7a, 0xa8};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count262146_snlen12 =
    {0x80, 0x02, 0x0c, 0xa0, 0xf5, 0x16, 0x07, 0x6a};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count262147_snlen12 =
    {0x80, 0x03, 0xb6, 0xd0, 0x2b, 0x8b, 0x27, 0x83};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count262148_snlen12 =
    {0x80, 0x04, 0x28, 0x20, 0x3c, 0x45, 0x50, 0xa3};
const std::array<uint8_t, pdu_size_snlen12> pdu1_algo3_count4294967295_snlen12 =
    {0x8f, 0xff, 0x4d, 0x4d, 0x96, 0xc7, 0x28, 0xc2};

// 18 bits (NIA3, NEA3)
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count0_snlen18 =
    {0x80, 0x00, 0x00, 0x3e, 0x36, 0xac, 0xeb, 0x1f, 0x14};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count1_snlen18 =
    {0x80, 0x00, 0x01, 0x0f, 0xf6, 0xed, 0xf1, 0x4d, 0x72};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count2_snlen18 =
    {0x80, 0x00, 0x02, 0x39, 0x40, 0x48, 0x90, 0xca, 0x7c};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count131071_snlen18 =
    {0x81, 0xff, 0xff, 0x11, 0x15, 0x36, 0x6a, 0xdf, 0x5c};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count131072_snlen18 =
    {0x82, 0x00, 0x00, 0x7a, 0x63, 0xda, 0xbb, 0x22, 0x75};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count131073_snlen18 =
    {0x82, 0x00, 0x01, 0x39, 0x94, 0xae, 0x53, 0x35, 0x30};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count262143_snlen18 =
    {0x83, 0xff, 0xff, 0x2e, 0xbe, 0x55, 0xe3, 0x6c, 0x44};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count262144_snlen18 =
    {0x80, 0x00, 0x00, 0xdc, 0x81, 0xfa, 0x44, 0x80, 0xb4};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count262145_snlen18 =
    {0x80, 0x00, 0x01, 0x36, 0x33, 0x3b, 0x76, 0x44, 0x82};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count262146_snlen18 =
    {0x80, 0x00, 0x02, 0x0c, 0xa0, 0xc9, 0x21, 0x5d, 0xc8};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count262147_snlen18 =
    {0x80, 0x00, 0x03, 0xb6, 0xd0, 0x3a, 0x6f, 0x42, 0x68};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count262148_snlen18 =
    {0x80, 0x00, 0x04, 0x28, 0x20, 0xef, 0xc7, 0x01, 0x52};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count334103_snlen18 =
    {0x81, 0x19, 0x17, 0x98, 0xef, 0x5f, 0x09, 0xf6, 0x85};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count334104_snlen18 =
    {0x81, 0x19, 0x18, 0xaf, 0x11, 0xff, 0x9b, 0x1e, 0x6d};
const std::array<uint8_t, pdu_size_snlen18> pdu1_algo3_count4294967295_snlen18 =
    {0x83, 0xff, 0xff, 0x4d, 0x4d, 0x1c, 0x25, 0x00, 0x21};

/// \brief Gets expected PDU based on the COUNT being tested (using NIA1/NEA1)
/// \param sn_size SN size of the PDU test vector
/// \param count COUNT of the PDU test vector
/// \param exp_pdu Expected PDU that is set to the correct test vector
/// \return true if we found the PDU, false other wise
inline bool get_pdu_test_vector_algo1(pdcp_sn_size sn_size, uint32_t count, byte_buffer& exp_pdu)
{
  if (sn_size == pdcp_sn_size::size12bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer::create(pdu1_algo1_count0_snlen12).value();
        return true;
      case 1:
        exp_pdu = byte_buffer::create(pdu1_algo1_count1_snlen12).value();
        return true;
      case 2:
        exp_pdu = byte_buffer::create(pdu1_algo1_count2_snlen12).value();
        return true;
      case 2047:
        exp_pdu = byte_buffer::create(pdu1_algo1_count2047_snlen12).value();
        return true;
      case 2048:
        exp_pdu = byte_buffer::create(pdu1_algo1_count2048_snlen12).value();
        return true;
      case 2049:
        exp_pdu = byte_buffer::create(pdu1_algo1_count2049_snlen12).value();
        return true;
      case 4095:
        exp_pdu = byte_buffer::create(pdu1_algo1_count4095_snlen12).value();
        return true;
      case 4096:
        exp_pdu = byte_buffer::create(pdu1_algo1_count4096_snlen12).value();
        return true;
      case 4097:
        exp_pdu = byte_buffer::create(pdu1_algo1_count4097_snlen12).value();
        return true;
      case 262143:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262143_snlen12).value();
        return true;
      case 262144:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262144_snlen12).value();
        return true;
      case 262145:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262145_snlen12).value();
        return true;
      case 262146:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262146_snlen12).value();
        return true;
      case 262147:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262147_snlen12).value();
        return true;
      case 262148:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262148_snlen12).value();
        return true;
      case 4294967295:
        exp_pdu = byte_buffer::create(pdu1_algo1_count4294967295_snlen12).value();
        return true;
      default:
        return false;
    }
  } else if (sn_size == pdcp_sn_size::size18bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer::create(pdu1_algo1_count0_snlen18).value();
        return true;
      case 1:
        exp_pdu = byte_buffer::create(pdu1_algo1_count1_snlen18).value();
        return true;
      case 2:
        exp_pdu = byte_buffer::create(pdu1_algo1_count2_snlen18).value();
        return true;
      case 131071:
        exp_pdu = byte_buffer::create(pdu1_algo1_count131071_snlen18).value();
        return true;
      case 131072:
        exp_pdu = byte_buffer::create(pdu1_algo1_count131072_snlen18).value();
        return true;
      case 131073:
        exp_pdu = byte_buffer::create(pdu1_algo1_count131073_snlen18).value();
        return true;
      case 262143:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262143_snlen18).value();
        return true;
      case 262144:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262144_snlen18).value();
        return true;
      case 262145:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262145_snlen18).value();
        return true;
      case 262146:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262146_snlen18).value();
        return true;
      case 262147:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262147_snlen18).value();
        return true;
      case 262148:
        exp_pdu = byte_buffer::create(pdu1_algo1_count262148_snlen18).value();
        return true;
      case 334103:
        exp_pdu = byte_buffer::create(pdu1_algo1_count334103_snlen18).value();
        return true;
      case 334104:
        exp_pdu = byte_buffer::create(pdu1_algo1_count334104_snlen18).value();
        return true;
      case 4294967295:
        exp_pdu = byte_buffer::create(pdu1_algo1_count4294967295_snlen18).value();
        return true;
      default:
        return false;
    }
  } else {
    return false;
  }
  return false;
}

/// \brief Gets expected PDU based on the COUNT being tested (using NIA1/NEA2)
/// \param sn_size SN size of the PDU test vector
/// \param count COUNT of the PDU test vector
/// \param exp_pdu Expected PDU that is set to the correct test vector
/// \return true if we found the PDU, false other wise
inline bool get_pdu_test_vector_algo2(pdcp_sn_size sn_size, uint32_t count, byte_buffer& exp_pdu)
{
  if (sn_size == pdcp_sn_size::size12bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer::create(pdu1_algo2_count0_snlen12).value();
        return true;
      case 1:
        exp_pdu = byte_buffer::create(pdu1_algo2_count1_snlen12).value();
        return true;
      case 2:
        exp_pdu = byte_buffer::create(pdu1_algo2_count2_snlen12).value();
        return true;
      case 2047:
        exp_pdu = byte_buffer::create(pdu1_algo2_count2047_snlen12).value();
        return true;
      case 2048:
        exp_pdu = byte_buffer::create(pdu1_algo2_count2048_snlen12).value();
        return true;
      case 2049:
        exp_pdu = byte_buffer::create(pdu1_algo2_count2049_snlen12).value();
        return true;
      case 4095:
        exp_pdu = byte_buffer::create(pdu1_algo2_count4095_snlen12).value();
        return true;
      case 4096:
        exp_pdu = byte_buffer::create(pdu1_algo2_count4096_snlen12).value();
        return true;
      case 4097:
        exp_pdu = byte_buffer::create(pdu1_algo2_count4097_snlen12).value();
        return true;
      case 262143:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262143_snlen12).value();
        return true;
      case 262144:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262144_snlen12).value();
        return true;
      case 262145:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262145_snlen12).value();
        return true;
      case 262146:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262146_snlen12).value();
        return true;
      case 262147:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262147_snlen12).value();
        return true;
      case 262148:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262148_snlen12).value();
        return true;
      case 4294967295:
        exp_pdu = byte_buffer::create(pdu1_algo2_count4294967295_snlen12).value();
        return true;
      default:
        return false;
    }
  } else if (sn_size == pdcp_sn_size::size18bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer::create(pdu1_algo2_count0_snlen18).value();
        return true;
      case 1:
        exp_pdu = byte_buffer::create(pdu1_algo2_count1_snlen18).value();
        return true;
      case 2:
        exp_pdu = byte_buffer::create(pdu1_algo2_count2_snlen18).value();
        return true;
      case 131071:
        exp_pdu = byte_buffer::create(pdu1_algo2_count131071_snlen18).value();
        return true;
      case 131072:
        exp_pdu = byte_buffer::create(pdu1_algo2_count131072_snlen18).value();
        return true;
      case 131073:
        exp_pdu = byte_buffer::create(pdu1_algo2_count131073_snlen18).value();
        return true;
      case 262143:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262143_snlen18).value();
        return true;
      case 262144:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262144_snlen18).value();
        return true;
      case 262145:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262145_snlen18).value();
        return true;
      case 262146:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262146_snlen18).value();
        return true;
      case 262147:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262147_snlen18).value();
        return true;
      case 262148:
        exp_pdu = byte_buffer::create(pdu1_algo2_count262148_snlen18).value();
        return true;
      case 334103:
        exp_pdu = byte_buffer::create(pdu1_algo2_count334103_snlen18).value();
        return true;
      case 334104:
        exp_pdu = byte_buffer::create(pdu1_algo2_count334104_snlen18).value();
        return true;
      case 4294967295:
        exp_pdu = byte_buffer::create(pdu1_algo2_count4294967295_snlen18).value();
        return true;
      default:
        return false;
    }
  } else {
    return false;
  }
  return false;
}

/// \brief Gets expected PDU based on the COUNT being tested (using NIA3/NEA3)
/// \param sn_size SN size of the PDU test vector
/// \param count COUNT of the PDU test vector
/// \param exp_pdu Expected PDU that is set to the correct test vector
/// \return true if we found the PDU, false other wise
inline bool get_pdu_test_vector_algo3(pdcp_sn_size sn_size, uint32_t count, byte_buffer& exp_pdu)
{
  if (sn_size == pdcp_sn_size::size12bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer::create(pdu1_algo3_count0_snlen12).value();
        return true;
      case 1:
        exp_pdu = byte_buffer::create(pdu1_algo3_count1_snlen12).value();
        return true;
      case 2:
        exp_pdu = byte_buffer::create(pdu1_algo3_count2_snlen12).value();
        return true;
      case 2047:
        exp_pdu = byte_buffer::create(pdu1_algo3_count2047_snlen12).value();
        return true;
      case 2048:
        exp_pdu = byte_buffer::create(pdu1_algo3_count2048_snlen12).value();
        return true;
      case 2049:
        exp_pdu = byte_buffer::create(pdu1_algo3_count2049_snlen12).value();
        return true;
      case 4095:
        exp_pdu = byte_buffer::create(pdu1_algo3_count4095_snlen12).value();
        return true;
      case 4096:
        exp_pdu = byte_buffer::create(pdu1_algo3_count4096_snlen12).value();
        return true;
      case 4097:
        exp_pdu = byte_buffer::create(pdu1_algo3_count4097_snlen12).value();
        return true;
      case 262143:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262143_snlen12).value();
        return true;
      case 262144:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262144_snlen12).value();
        return true;
      case 262145:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262145_snlen12).value();
        return true;
      case 262146:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262146_snlen12).value();
        return true;
      case 262147:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262147_snlen12).value();
        return true;
      case 262148:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262148_snlen12).value();
        return true;
      case 4294967295:
        exp_pdu = byte_buffer::create(pdu1_algo3_count4294967295_snlen12).value();
        return true;
      default:
        return false;
    }
  } else if (sn_size == pdcp_sn_size::size18bits) {
    switch (count) {
      case 0:
        exp_pdu = byte_buffer::create(pdu1_algo3_count0_snlen18).value();
        return true;
      case 1:
        exp_pdu = byte_buffer::create(pdu1_algo3_count1_snlen18).value();
        return true;
      case 2:
        exp_pdu = byte_buffer::create(pdu1_algo3_count2_snlen18).value();
        return true;
      case 131071:
        exp_pdu = byte_buffer::create(pdu1_algo3_count131071_snlen18).value();
        return true;
      case 131072:
        exp_pdu = byte_buffer::create(pdu1_algo3_count131072_snlen18).value();
        return true;
      case 131073:
        exp_pdu = byte_buffer::create(pdu1_algo3_count131073_snlen18).value();
        return true;
      case 262143:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262143_snlen18).value();
        return true;
      case 262144:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262144_snlen18).value();
        return true;
      case 262145:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262145_snlen18).value();
        return true;
      case 262146:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262146_snlen18).value();
        return true;
      case 262147:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262147_snlen18).value();
        return true;
      case 262148:
        exp_pdu = byte_buffer::create(pdu1_algo3_count262148_snlen18).value();
        return true;
      case 334103:
        exp_pdu = byte_buffer::create(pdu1_algo3_count334103_snlen18).value();
        return true;
      case 334104:
        exp_pdu = byte_buffer::create(pdu1_algo3_count334104_snlen18).value();
        return true;
      case 4294967295:
        exp_pdu = byte_buffer::create(pdu1_algo3_count4294967295_snlen18).value();
        return true;
      default:
        return false;
    }
  } else {
    return false;
  }
  return false;
}

/// \brief Gets expected PDU based on the COUNT being tested
/// \param sn_size SN size of the PDU test vector
/// \param count COUNT of the PDU test vector
/// \param exp_pdu Expected PDU that is set to the correct test vector
/// \param algos Algorithms to be used. 1 is NIA1 and NEA1, 2 is NIA2 and NEA2, etc.
/// \return true if we found the PDU, false other wise
inline bool get_pdu_test_vector(pdcp_sn_size sn_size, uint32_t count, byte_buffer& exp_pdu, unsigned algos)
{
  switch (algos) {
    case 1:
      return get_pdu_test_vector_algo1(sn_size, count, exp_pdu);
    case 2:
      return get_pdu_test_vector_algo2(sn_size, count, exp_pdu);
    case 3:
      return get_pdu_test_vector_algo3(sn_size, count, exp_pdu);
  }
  return false;
}

} // namespace srsran
