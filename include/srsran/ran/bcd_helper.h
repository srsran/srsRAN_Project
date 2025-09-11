/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include <cstdint>
#include <string>

namespace srsran {

namespace bcd_helper {

/// Check if integer corresponds to a valid BCD-encoded MCC value (3 digits).
inline bool is_valid_mcc(uint16_t mcc)
{
  if ((mcc & 0xf000U) != 0xf000U) {
    return false;
  }
  if (((mcc & 0xf00U) >> 8U) > 9U || ((mcc & 0x0f0U) >> 4U) > 9U || (mcc & 0x00fU) > 9U) {
    return false;
  }
  return true;
}

/// Check if string corresponds to a valid MCC value (3 digits).
inline bool is_valid_mcc(const std::string& mcc_str)
{
  size_t len = mcc_str.size();
  if (len != 3) {
    return false;
  }
  if (!isdigit(mcc_str[0]) || !isdigit(mcc_str[1]) || !isdigit(mcc_str[2])) {
    return false;
  }
  return true;
}

/// Check if array of digits corresponds to a valid MCC value (3 digits).
inline bool is_valid_mcc(const std::array<uint8_t, 3>& bytes)
{
  for (uint8_t byte : bytes) {
    if (byte > 9) {
      return false;
    }
  }
  return true;
}

/// Check if integer corresponds to a valid BCD-encoded MNC value (2 or 3 digits).
inline bool is_valid_mnc(uint16_t mnc)
{
  if ((mnc & 0xf000) != 0xf000) {
    return false;
  }
  unsigned digit3 = mnc & 0xf00U;
  if (digit3 != 0xf00U && digit3 > 0x900U) {
    return false;
  }
  if ((mnc & 0x0f0U) > 0x90U || (mnc & 0x00fU) > 9U) {
    return false;
  }
  return true;
}

/// Check if string corresponds to a valid MNC value (2 or 3 digits).
inline bool is_valid_mnc(const std::string& mnc_str)
{
  size_t len = mnc_str.size();
  if (len != 3U && len != 2U) {
    return false;
  }
  for (unsigned i = 0; i != len; ++i) {
    if (!isdigit(mnc_str[i])) {
      return false;
    }
  }
  return true;
}

/// Check if array of digits corresponds to a valid MNC value (2 or 3 digits).
inline bool is_valid_mnc(span<const uint8_t> bytes)
{
  if (bytes.size() != 2 and bytes.size() != 3) {
    return false;
  }
  for (uint8_t byte : bytes) {
    if (byte > 9) {
      return false;
    }
  }
  return true;
}

inline bool is_valid_plmn(uint32_t bcd_plmn)
{
  if ((bcd_plmn & 0xff000000U) != 0) {
    return false;
  }
  return true;
}

/// Extract BCD-coded MCC from PLMN
inline uint16_t bcd_plmn_to_mcc(uint32_t plmn)
{
  if (not is_valid_plmn(plmn)) {
    return 0U;
  }
  uint16_t mcc = 0xf000U;
  mcc |= (plmn & 0x0f0000U) >> 8;  // MCC digit 1
  mcc |= (plmn & 0xf00000U) >> 16; // MCC digit 2
  mcc |= (plmn & 0x00f00U) >> 8;   // MCC digit 3
  return mcc;
}

/// Extract BCD-coded MCC from PLMN
inline uint16_t bcd_plmn_to_mnc(uint32_t plmn)
{
  if (not is_valid_plmn(plmn)) {
    return 0U;
  }
  bool     is_2_digit = (plmn & 0x00f000) >> 12 == 0xf;
  uint16_t mnc;
  mnc = 0xf000U;
  mnc |= is_2_digit ? 0x0f00U : (plmn & 0x00f000) >> 4; // MNC digit 1
  mnc |= (plmn & 0xf) << 4;                             // MNC digit 2
  mnc |= (plmn & 0xf0) >> 4;                            // MNC digit 3
  return mnc;
}

/// \brief Convert string to BCD-coded MCC.
/// Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xf.
/// MCC 001 results in 0xf001
inline bool string_to_mcc(const std::string& str, uint16_t* mcc)
{
  if (not bcd_helper::is_valid_mcc(str)) {
    return false;
  }
  *mcc = 0xf000U;
  *mcc |= static_cast<uint16_t>(str[0] - '0') << 8U;
  *mcc |= static_cast<uint16_t>(str[1] - '0') << 4U;
  *mcc |= static_cast<uint16_t>(str[2] - '0');
  return true;
}

inline bool mcc_to_string(uint16_t mcc, std::string* str)
{
  if (not bcd_helper::is_valid_mcc(mcc)) {
    return false;
  }
  *str = "";
  *str += ((mcc & 0x0f00U) >> 8U) + '0';
  *str += ((mcc & 0x00f0U) >> 4U) + '0';
  *str += (mcc & 0x000fU) + '0';
  return true;
}

/// Convert between array of bytes and BCD-coded MCC.
/// Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xf.
/// MCC 001 results in 0xf001
inline bool bytes_to_mcc(const uint8_t* bytes, uint16_t* mcc)
{
  *mcc = 0xf000;
  *mcc |= (((uint16_t)bytes[0]) << 8u);
  *mcc |= (((uint16_t)bytes[1]) << 4u);
  *mcc |= (uint16_t)bytes[2];
  return true;
}

inline bool mcc_to_bytes(uint16_t mcc, uint8_t* bytes)
{
  if ((mcc & 0xf000) != 0xf000) {
    return false;
  }
  bytes[0] = (uint8_t)((mcc & 0xf00) >> 8);
  bytes[1] = (uint8_t)((mcc & 0x0f0) >> 4);
  bytes[2] = (uint8_t)(mcc & 0x00f);
  return true;
}

inline std::string mcc_bytes_to_string(uint8_t* mcc_bytes)
{
  std::string mcc_str;
  uint16_t    mcc;
  bytes_to_mcc(&mcc_bytes[0], &mcc);
  if (!mcc_to_string(mcc, &mcc_str)) {
    mcc_str = "000";
  }
  return mcc_str;
}

/// Convert between string and BCD-coded MNC.
/// Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xf.
/// MNC 001 results in 0xf001
/// MNC 01 results in 0xff01
inline bool string_to_mnc(const std::string& str, uint16_t* mnc)
{
  if (not bcd_helper::is_valid_mnc(str)) {
    return false;
  }
  uint32_t len = str.size();
  if (len == 3) {
    *mnc = 0xf000;
    *mnc |= ((uint8_t)(str[0] - '0') << 8);
    *mnc |= ((uint8_t)(str[1] - '0') << 4);
    *mnc |= ((uint8_t)(str[2] - '0'));
  } else {
    *mnc = 0xff00;
    *mnc |= ((uint8_t)(str[0] - '0') << 4);
    *mnc |= ((uint8_t)(str[1] - '0'));
  }
  return true;
}

inline bool mnc_to_string(uint16_t mnc, std::string* str)
{
  if ((mnc & 0xf000) != 0xf000) {
    return false;
  }
  *str = "";
  if ((mnc & 0xff00) != 0xff00) {
    *str += ((mnc & 0x0f00) >> 8) + '0';
  }
  *str += ((mnc & 0x00f0) >> 4) + '0';
  *str += (mnc & 0x000f) + '0';
  return true;
}

/// Convert between array of bytes and BCD-coded MNC.
/// Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xf.
/// MNC 001 results in 0xf001
/// MNC 01 results in 0xff01
inline bool bytes_to_mnc(const uint8_t* bytes, uint16_t* mnc, uint8_t len)
{
  if (len != 3 && len != 2) {
    *mnc = 0;
    return false;
  } else if (len == 3) {
    *mnc = 0xf000;
    *mnc |= ((uint16_t)bytes[0]) << 8u;
    *mnc |= ((uint16_t)bytes[1]) << 4u;
    *mnc |= ((uint16_t)bytes[2]) << 0u;
  } else if (len == 2) {
    *mnc = 0xff00;
    *mnc |= ((uint16_t)bytes[0]) << 4u;
    *mnc |= ((uint16_t)bytes[1]) << 0u;
  }
  return true;
}

inline bool mnc_to_bytes(uint16_t mnc, uint8_t* bytes, uint8_t* len)
{
  if ((mnc & 0xf000) != 0xf000) {
    *len = 0;
    return false;
  }
  uint8_t count = 0;
  if ((mnc & 0xff00) != 0xff00) {
    bytes[count++] = (mnc & 0xf00) >> 8u;
  }
  bytes[count++] = (mnc & 0x00f0) >> 4u;
  bytes[count++] = (mnc & 0x000f);
  *len           = count;
  return true;
}

template <class Vec>
bool mnc_to_bytes(uint16_t mnc, Vec& vec)
{
  uint8_t len;
  uint8_t v[3];
  bool    ret = mnc_to_bytes(mnc, &v[0], &len);
  vec.resize(len);
  std::memcpy(&vec[0], &v[0], len);
  return ret;
}

inline std::string mnc_bytes_to_string(uint8_t* mnc_bytes, uint32_t nof_bytes)
{
  std::string mnc_str;
  uint16_t    mnc;
  bytes_to_mnc(&mnc_bytes[0], &mnc, nof_bytes);
  if (!mnc_to_string(mnc, &mnc_str)) {
    mnc_str = "000";
  }
  return mnc_str;
}

template <class Vec>
std::string mnc_bytes_to_string(Vec mnc_bytes)
{
  return mnc_bytes_to_string(&mnc_bytes[0], mnc_bytes.size());
}

/// Convert PLMN to BCD-coded MCC and MNC.
/// Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xf.
/// MNC 001 represented as 0xf001
/// MNC 01 represented as 0xff01
/// PLMN encoded as per TS 38.413 sec 9.3.3.5
inline bool ngap_plmn_to_mccmnc(uint32_t plmn, uint16_t* mcc, uint16_t* mnc)
{
  uint16_t mcc_tmp = bcd_plmn_to_mcc(plmn);
  if (mcc_tmp == 0) {
    return false;
  }
  uint16_t mnc_tmp = bcd_plmn_to_mnc(plmn);
  if (mnc_tmp == 0) {
    return false;
  }
  *mcc = mcc_tmp;
  *mnc = mnc_tmp;
  return true;
}

/// Convert BCD-coded MCC and MNC to PLMN.
/// Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xf.
/// MNC 001 represented as 0xf001
/// MNC 01 represented as 0xff01
/// PLMN encoded as per TS 38.413 sec 9.3.3.5
inline void ngap_mccmnc_to_plmn(uint16_t mcc, uint16_t mnc, uint32_t* plmn)
{
  uint8_t nibbles[6];
  nibbles[1] = (mcc & 0x0f00) >> 8; // MCC digit 1
  nibbles[0] = (mcc & 0x00f0) >> 4; // MCC digit 2
  nibbles[3] = (mcc & 0x000f);      // MCC digit 3

  if ((mnc & 0xff00) == 0xff00) {
    // 2-digit MNC
    nibbles[2] = 0x0f;                // MNC digit 1
    nibbles[5] = (mnc & 0x00f0) >> 4; // MNC digit 2
    nibbles[4] = (mnc & 0x000f);      // MNC digit 3
  } else {
    // 3-digit MNC
    nibbles[2] = (mnc & 0x0f00) >> 8; // MNC digit 1
    nibbles[5] = (mnc & 0x00f0) >> 4; // MNC digit 2
    nibbles[4] = (mnc & 0x000f);      // MNC digit 3
  }

  *plmn = 0x000000;
  *plmn |= nibbles[0] << 20;
  *plmn |= nibbles[1] << 16;
  *plmn |= nibbles[2] << 12;
  *plmn |= nibbles[3] << 8;
  *plmn |= nibbles[4] << 4;
  *plmn |= nibbles[5];
}

inline uint32_t plmn_string_to_bcd(const std::string& plmn)
{
  uint32_t bcd_plmn = 0;
  if (plmn.length() != 5 and plmn.length() != 6) {
    return bcd_plmn;
  }

  uint16_t mcc;
  if (string_to_mcc(plmn.substr(0, 3), &mcc) == false) {
    return bcd_plmn;
  }

  uint16_t mnc;
  if (string_to_mnc(plmn.substr(3), &mnc) == false) {
    return bcd_plmn;
  }

  ngap_mccmnc_to_plmn(mcc, mnc, &bcd_plmn);

  return bcd_plmn;
}

inline std::string plmn_bcd_to_string(uint32_t plmn)
{
  uint16_t mcc, mnc;
  if (not bcd_helper::ngap_plmn_to_mccmnc(plmn, &mcc, &mnc)) {
    return "";
  }
  std::string mcc_string, mnc_string;
  mcc_to_string(mcc, &mcc_string);
  mnc_to_string(mnc, &mnc_string);
  return mcc_string + mnc_string;
}

} // namespace bcd_helper
} // namespace srsran
