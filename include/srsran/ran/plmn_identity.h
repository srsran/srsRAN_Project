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

#include "srsran/adt/expected.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/bcd_helper.h"

namespace srsran {

/// Identifier of a Mobile Country Code (MCC).
class mobile_country_code
{
  constexpr mobile_country_code(uint16_t mcc) : bcd(mcc) {}

public:
  mobile_country_code() = delete;

  /// Test value for mobile_country_code "001".
  static constexpr mobile_country_code test_value() { return mobile_country_code{0xf001}; }

  /// Conversion of a BCD-encoded MCC integer into a mobile_country_code.
  static expected<mobile_country_code> from_bcd(uint16_t bcd_mcc)
  {
    if (!bcd_helper::is_valid_mcc(bcd_mcc)) {
      return make_unexpected(default_error_t{});
    }
    return mobile_country_code{bcd_mcc};
  }

  /// Conversion of string of 3 digits into a mobile_country_code.
  static expected<mobile_country_code> from_string(const std::string& mcc_str)
  {
    uint16_t mcc;
    if (!bcd_helper::string_to_mcc(mcc_str, &mcc)) {
      return make_unexpected(default_error_t{});
    }
    return mobile_country_code{mcc};
  }

  /// Conversion of an array of 3 bytes into a mobile_country_code.
  static expected<mobile_country_code> from_bytes(const std::array<uint8_t, 3>& bytes)
  {
    uint16_t mcc;
    if (not bcd_helper::is_valid_mcc(bytes)) {
      return make_unexpected(default_error_t{});
    }
    bcd_helper::bytes_to_mcc(bytes.data(), &mcc);
    return mobile_country_code{mcc};
  }

  uint16_t to_bcd() const { return bcd; }

  std::string to_string() const
  {
    std::string str;
    bcd_helper::mcc_to_string(bcd, &str);
    return str;
  }

  std::array<uint8_t, 3> to_bytes() const
  {
    std::array<uint8_t, 3> bytes = {};
    bcd_helper::mcc_to_bytes(bcd, bytes.data());
    return bytes;
  }

  static size_t nof_digits() { return 3; }

  bool operator==(const mobile_country_code& rhs) const { return bcd == rhs.bcd; }
  bool operator!=(const mobile_country_code& rhs) const { return bcd != rhs.bcd; }

private:
  // BCD representation of MCC.
  uint16_t bcd;
};

/// Identifier of a Mobile Network Code (MNC).
class mobile_network_code
{
  constexpr mobile_network_code(uint16_t mnc) : bcd(mnc) {}

public:
  mobile_network_code() = delete;

  /// Test value for mobile_country_code "01".
  static constexpr mobile_network_code test_value() { return mobile_network_code{0xff01U}; }

  /// Conversion of a BCD-encoded MNC integer into a mobile_network_code.
  static expected<mobile_network_code> from_bcd(uint16_t bcd_mnc)
  {
    if (!bcd_helper::is_valid_mnc(bcd_mnc)) {
      return make_unexpected(default_error_t{});
    }
    return mobile_network_code{bcd_mnc};
  }

  /// Conversion of string of 2/3 digits into a mobile_network_code.
  static expected<mobile_network_code> from_string(const std::string& mnc_str)
  {
    uint16_t mnc;
    if (!bcd_helper::string_to_mnc(mnc_str, &mnc)) {
      return make_unexpected(default_error_t{});
    }
    return mobile_network_code{mnc};
  }

  /// Conversion of an array of 2/3 bytes into a mobile_network_code.
  static expected<mobile_network_code> from_bytes(span<const uint8_t> bytes)
  {
    if (not bcd_helper::is_valid_mnc(bytes)) {
      return make_unexpected(default_error_t{});
    }
    uint16_t mnc;
    bcd_helper::bytes_to_mnc(bytes.data(), &mnc, bytes.size());
    return mobile_network_code{mnc};
  }

  uint16_t to_bcd() const { return bcd; }

  std::string to_string() const
  {
    std::string str;
    bcd_helper::mnc_to_string(bcd, &str);
    return str;
  }

  static_vector<uint8_t, 3> to_bytes() const
  {
    static_vector<uint8_t, 3> bytes(3);
    uint8_t                   len;
    bcd_helper::mnc_to_bytes(bcd, bytes.data(), &len);
    bytes.resize(len);
    return bytes;
  }

  /// Number of digits of the MNC.
  size_t nof_digits() const { return (bcd & 0xf00U) == 0xf00U ? 2 : 3; }

  bool operator==(const mobile_network_code& rhs) const { return bcd == rhs.bcd; }
  bool operator!=(const mobile_network_code& rhs) const { return bcd != rhs.bcd; }

private:
  // BCD representation of MNC.
  uint16_t bcd;
};

/// Identifier of a Public Land Mobile Network (PLMN).
class plmn_identity
{
  constexpr plmn_identity(uint32_t bcd_plmn) : data(bcd_plmn) {}
  plmn_identity(uint16_t bcd_mcc, uint16_t bcd_mnc) { bcd_helper::ngap_mccmnc_to_plmn(bcd_mcc, bcd_mnc, &data); }

public:
  /// Test value for PLMN identity "00101".
  static constexpr plmn_identity test_value() { return plmn_identity{0xf110}; }

  plmn_identity() = delete;
  plmn_identity(mobile_country_code mcc, mobile_network_code mnc) : plmn_identity(mcc.to_bcd(), mnc.to_bcd()) {}

  /// Convert PLMN string representation to PLMN instance (e.g. "00101" for test network).
  static expected<plmn_identity> parse(const std::string& plmn_id_str)
  {
    uint32_t plmn = bcd_helper::plmn_string_to_bcd(plmn_id_str);
    if (plmn == 0) {
      return make_unexpected(default_error_t{});
    }
    return plmn_identity{plmn};
  }

  /// \brief Convert array of bytes representation to PLMN instance.
  ///
  /// The PLMN identity byte representation consists of 3 digits from MCC followed by either a filler digit plus 2
  /// digits from MNC (2 digit MNC) or 3 digits from MNC (3 digit MNC).
  static expected<plmn_identity> from_bytes(const std::array<uint8_t, 3>& bytes)
  {
    uint16_t mcc = 0xf000U + (static_cast<uint16_t>(bytes[0] & 0xfU) << 8U) + (bytes[0] & 0xf0U) + (bytes[1] & 0xfU);
    uint16_t mnc = 0xf000U + (static_cast<uint16_t>(bytes[1] & 0xf0U) << 4U) + ((bytes[2] & 0xf0U) >> 4U) +
                   ((bytes[2] & 0xfU) << 4U);
    if (not bcd_helper::is_valid_mcc(mcc) or not bcd_helper::is_valid_mnc(mnc)) {
      return make_unexpected(default_error_t{});
    }
    return plmn_identity{mcc, mnc};
  }

  uint32_t to_bcd() const { return data; }

  std::array<uint8_t, 3> to_bytes() const
  {
    std::array<uint8_t, 3> bytes = {};
    uint16_t               mcc   = bcd_helper::bcd_plmn_to_mcc(data);
    uint16_t               mnc   = bcd_helper::bcd_plmn_to_mnc(data);
    bytes[0]                     = ((mcc & 0xf00U) >> 8U) + (mcc & 0xf0U);
    bytes[1]                     = (mcc & 0xfU) + ((mnc & 0xf00U) >> 4U);
    bytes[2]                     = ((mnc & 0xfU) << 4U) + ((mnc & 0xf0U) >> 4U);
    return bytes;
  }

  mobile_country_code mcc() const { return mobile_country_code::from_bcd(bcd_helper::bcd_plmn_to_mcc(data)).value(); }

  mobile_network_code mnc() const { return mobile_network_code::from_bcd(bcd_helper::bcd_plmn_to_mnc(data)).value(); }

  /// Convert PLMN to string representation (e.g. "00101" for test network).
  std::string to_string() const { return bcd_helper::plmn_bcd_to_string(data); }

  bool operator==(const plmn_identity& rhs) const { return data == rhs.data; }
  bool operator!=(const plmn_identity& rhs) const { return data != rhs.data; }

private:
  uint32_t data;
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::mobile_country_code> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::mobile_country_code& val, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{}", val.to_string());
  }
};

template <>
struct formatter<srsran::mobile_network_code> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::mobile_network_code& val, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{}", val.to_string());
  }
};

template <>
struct formatter<srsran::plmn_identity> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::plmn_identity& val, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{}", val.to_string());
  }
};

} // namespace fmt