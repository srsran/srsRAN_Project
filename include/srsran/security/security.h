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

/******************************************************************************
 * Common security header - wraps ciphering/integrity check algorithms.
 *****************************************************************************/

#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"
#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace srsran {
namespace security {

/// MAC-I length in bytes
constexpr uint32_t sec_mac_len = 4;

/// Security Key length in bytes
constexpr uint32_t sec_key_len = 32;

/// Security Key length in bytes (for 128 bit algorithms)
constexpr uint32_t sec_128_key_len = 16;

enum class ciphering_algorithm {
  nea0 = 0,
  nea1,
  nea2,
  nea3,
};
inline unsigned to_number(ciphering_algorithm ciph_algo)
{
  return static_cast<unsigned>(ciph_algo);
}
inline ciphering_algorithm ciphering_algorithm_from_number(unsigned ciph_algo)
{
  srsran_assert(ciph_algo < 4, "Error converting ciphering algorithm");
  return static_cast<ciphering_algorithm>(ciph_algo);
}

enum class integrity_algorithm {
  nia0 = 0,
  nia1,
  nia2,
  nia3,
};
inline unsigned to_number(integrity_algorithm int_algo)
{
  return static_cast<unsigned>(int_algo);
}
inline integrity_algorithm integrity_algorithm_from_number(unsigned int_algo)
{
  srsran_assert(int_algo < 4, "Error converting integrity algorithm");
  return static_cast<integrity_algorithm>(int_algo);
}

/// FC Values
/// Ref: TS 33.501 Sec. A.1.2
enum class fc_value {
  algorithm_key_derivation = 0x69, ///< Algorithm key derivation functions (Sec. A.8)
  k_ng_ran_star_derivation = 0x70, ///< KNG-RAN* derivation function for target gNB (Sec. A.11)
};
constexpr uint8_t to_number(fc_value fc)
{
  return static_cast<uint8_t>(fc);
}

/// Security Algorithm Distinguisher
/// Ref: TS 33.501 Sec. A.8, Table A.8-1
enum class security_algo_distinguisher {
  rrc_enc_alg = 0x03, ///< N-RRC-enc-alg
  rrc_int_alg = 0x04, ///< N-RRC-int-alg
  up_enc_alg  = 0x05, ///< N-UP-enc-alg
  up_int_alg  = 0x06  ///< N-UP-int-alg
};
constexpr uint8_t to_number(security_algo_distinguisher algo)
{
  return static_cast<uint8_t>(algo);
}

enum class security_direction {
  uplink   = 0,
  downlink = 1,
};
constexpr uint8_t to_number(security_direction direction)
{
  return static_cast<uint8_t>(direction);
}

/// integrity/ciphering enabled
enum class integrity_enabled { off = 0, on = 1 };
enum class ciphering_enabled { off = 0, on = 1 };

using sec_mac = std::array<uint8_t, sec_mac_len>;

using sec_key     = std::array<uint8_t, sec_key_len>;
using sec_128_key = std::array<uint8_t, sec_128_key_len>;

/// Helper types to communicate the preferred algorithm list. NIA/NEA0...3.
constexpr uint16_t nof_pref_algos    = 4;
using preferred_integrity_algorithms = std::array<integrity_algorithm, nof_pref_algos>;
using preferred_ciphering_algorithms = std::array<ciphering_algorithm, nof_pref_algos>;

/// Helper types to communicate NIA/NEA1...3 support. Support of NEA/NIA0 is implicit.
constexpr uint16_t nof_supported_algos = 3;
using supported_algorithms             = std::array<bool, nof_supported_algos>;

/// Security domain, whether applies to RRC or UP
enum class sec_domain { rrc = 0, up = 1 };

struct sec_128_as_config {
  sec_domain                    domain;
  optional<sec_128_key>         k_128_int; // Optional in E1AP, see TS 38.463 Sec. 9.4.5
  sec_128_key                   k_128_enc;
  optional<integrity_algorithm> integ_algo; // Optional in E1AP, see TS 38.463 Sec. 9.4.5
  ciphering_algorithm           cipher_algo;

  bool operator==(const sec_128_as_config& rhs) const
  {
    if (integ_algo != rhs.integ_algo) {
      return false;
    }
    if (cipher_algo != rhs.cipher_algo) {
      return false;
    }
    if (domain != rhs.domain) {
      return false;
    }
    if (k_128_int != rhs.k_128_int) {
      return false;
    }
    if (k_128_enc != rhs.k_128_enc) {
      return false;
    }
    return true;
  }
};

struct sec_as_config {
  sec_domain                    domain;
  optional<sec_key>             k_int; // Optional in E1AP, see TS 38.463 Sec. 9.4.5
  sec_key                       k_enc;
  optional<integrity_algorithm> integ_algo; // Optional in E1AP, see TS 38.463 Sec. 9.4.5
  ciphering_algorithm           cipher_algo;
};

struct sec_selected_algos {
  bool                algos_selected = false;
  integrity_algorithm integ_algo;
  ciphering_algorithm cipher_algo;
};

struct sec_as_keys {
  sec_key k_rrc_int;
  sec_key k_rrc_enc;
  sec_key k_up_int;
  sec_key k_up_enc;
};

using sec_short_mac_i = std::array<uint8_t, 2>;

struct security_context {
  srslog::basic_logger&          logger = srslog::fetch_basic_logger("SEC");
  security::sec_key              k;
  security::supported_algorithms supported_int_algos;
  security::supported_algorithms supported_enc_algos;
  sec_selected_algos             sel_algos;
  sec_as_keys                    as_keys;

  security_context()  = default;
  ~security_context() = default;
  security_context(const security_context& sec_ctxt) :
    k(sec_ctxt.k),
    supported_int_algos(sec_ctxt.supported_int_algos),
    supported_enc_algos(sec_ctxt.supported_enc_algos),
    sel_algos(sec_ctxt.sel_algos),
    as_keys(sec_ctxt.as_keys)
  {
  }
  security_context& operator=(const security_context& sec_ctxt)
  {
    if (this == &sec_ctxt) {
      return *this;
    }
    k                   = sec_ctxt.k;
    supported_int_algos = sec_ctxt.supported_int_algos;
    supported_enc_algos = sec_ctxt.supported_enc_algos;
    sel_algos           = sec_ctxt.sel_algos;
    as_keys             = sec_ctxt.as_keys;
    return *this;
  }

  bool select_algorithms(preferred_integrity_algorithms pref_inte_list, preferred_ciphering_algorithms pref_ciph_list);
  void generate_as_keys();
  sec_as_config     get_as_config(sec_domain domain);
  sec_128_as_config get_128_as_config(sec_domain domain);
  void              horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn);
};

/******************************************************************************
 * Helper Functions
 *****************************************************************************/

/// \brief Fill tailing bits of the last byte of a contiguous memory with zeros.
/// \param[inout] data Pointer to the contiguous memory to operate on
/// \param[in] length_bits Number of occupied bits in the whole memory buffer that shall not be zeroed
inline void zero_tailing_bits(uint8_t* data, uint32_t length_bits)
{
  uint8_t bits = (8 - (length_bits & 0x07)) & 0x07;
  data[(length_bits + 7) / 8 - 1] &= (uint8_t)(0xff << bits);
}

/// \brief Fill tailing bits of a given byte with zeros.
/// \param[inout] tail_byte Reference to the byte to operate on
/// \param[in] length_bits Number of occupied bits in the tail_byte that shall not be zeroed
inline void zero_tailing_bits(uint8_t& tail_byte, uint8_t length_bits)
{
  uint8_t bits = (8 - (length_bits & 0x07)) & 0x07;
  tail_byte &= (uint8_t)(0xff << bits);
}

inline std::string sec_as_key_to_string(const sec_key& key)
{
  std::stringstream ss;
  for (auto val : key) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
  }
  return ss.str();
}

/******************************************************************************
 * Key Generation
 *****************************************************************************/

/// Generic key derivation function
/// Ref: TS 33.220 Sec. B.2
void generic_kdf(sec_key&                   key_out,
                 const sec_key&             key_in,
                 const fc_value             fc,
                 const span<const uint8_t>& p0,
                 const span<const uint8_t>& p1);

/// Algorithm key derivation function (RRC)
/// Ref: TS 33.501 Sec. A.8
void generate_k_rrc(sec_key&                  k_rrc_enc,
                    sec_key&                  k_rrc_int,
                    const sec_key&            k_gnb,
                    const ciphering_algorithm enc_alg_id,
                    const integrity_algorithm int_alg_id);

/// Algorithm key derivation function (UP)
/// Ref: TS 33.501 Sec. A.8
void generate_k_up(sec_key&                  k_up_enc,
                   sec_key&                  k_up_int,
                   const sec_key&            k_gnb,
                   const ciphering_algorithm enc_alg_id,
                   const integrity_algorithm int_alg_id);

/// K_NG-RAN* derivation function for target gNB
/// Ref: TS 33.501 Sec. A.11
void generate_k_ng_ran_star(sec_key&       k_star,
                            const sec_key& k,
                            const pci_t&   target_pci_,
                            const uint32_t target_ssb_arfcn_);

/// Truncate 256-bit key to 128-bit key using the least significant bits.
/// Ref: TS 33.501 Sec. A.8
sec_128_key truncate_key(const sec_key& key_in);

/// Truncate 256-bit keys to 128-bit keys using the least significant bits,
/// on a given security context.
sec_128_as_config truncate_config(const sec_as_config& cfg_in);

} // namespace security
} // namespace srsran

//
// Formatters
//
namespace fmt {

// Security direction
template <>
struct formatter<srsran::security::security_direction> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::security_direction dir, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"UL", "DL"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(dir)]);
  }
};

// Integrity enabled
template <>
struct formatter<srsran::security::integrity_enabled> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::integrity_enabled integrity_flag, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"off", "on"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(integrity_flag)]);
  }
};

// Ciphering enabled
template <>
struct formatter<srsran::security::ciphering_enabled> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::ciphering_enabled ciphering_flag, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"off", "on"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(ciphering_flag)]);
  }
};

// Supported algorithms
template <>
struct formatter<srsran::security::supported_algorithms> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::supported_algorithms algos, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "N[I|E]A1={}, N[I|E]A2={}, N[I|E]A3={}", algos[0], algos[1], algos[2]);
  }
};

// Preferred integrity protection algorithms list
template <>
struct formatter<srsran::security::preferred_integrity_algorithms> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::preferred_integrity_algorithms algos, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "NIA{}, NIA{}, NIA{}, NIA{}", algos[0], algos[1], algos[2], algos[3]);
  }
};

// Preferred ciphering algorithms list
template <>
struct formatter<srsran::security::preferred_ciphering_algorithms> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::preferred_ciphering_algorithms algos, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "NEA{}, NEA{}, NEA{}, NEA{}", algos[0], algos[1], algos[2], algos[3]);
  }
};

// Security domain
template <>
struct formatter<srsran::security::sec_domain> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::sec_domain domain, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"RRC", "UP"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(domain)]);
  }
};

} // namespace fmt
