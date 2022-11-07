/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

/******************************************************************************
 * Common security header - wraps ciphering/integrity check algorithms.
 *****************************************************************************/

#include "srsgnb/adt/span.h"
#include "fmt/format.h"
#include <array>
#include <cstdint>

namespace srsgnb {
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

enum class integrity_algorithm {
  nia0 = 0,
  nia1,
  nia2,
  nia3,
};

/// FC Values
/// Ref: TS 33.501 Sec. A.1.2
enum class fc_value {
  algorithm_key_derivation = 0x69, ///< Algorithm key derivation functions (Sec. A.8)
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

using sec_mac = std::array<uint8_t, sec_mac_len>;

using sec_as_key     = std::array<uint8_t, sec_key_len>;
using sec_128_as_key = std::array<uint8_t, sec_128_key_len>;

struct sec_128_as_config {
  sec_128_as_key      k_128_rrc_int;
  sec_128_as_key      k_128_rrc_enc;
  sec_128_as_key      k_128_up_int;
  sec_128_as_key      k_128_up_enc;
  integrity_algorithm integ_algo;
  ciphering_algorithm cipher_algo;
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

/******************************************************************************
 * Key Generation
 *****************************************************************************/

/// Generic key derivation function
/// Ref: TS 33.220 Sec. B.2
void generic_kdf(sec_as_key&         key_out,
                 const sec_as_key&   key_in,
                 const fc_value      fc,
                 span<const uint8_t> p0,
                 span<const uint8_t> p1);

/// Algorithm key derivation function (RRC)
/// Ref: TS 33.501 Sec. A.8
void generate_k_rrc(sec_as_key&               k_rrc_enc,
                    sec_as_key&               k_rrc_int,
                    const sec_as_key&         k_gnb,
                    const ciphering_algorithm enc_alg_id,
                    const integrity_algorithm int_alg_id);

/// Algorithm key derivation function (UP)
/// Ref: TS 33.501 Sec. A.8
void generate_k_up(sec_as_key&               k_up_enc,
                   sec_as_key&               k_up_int,
                   const sec_as_key&         k_gnb,
                   const ciphering_algorithm enc_alg_id,
                   const integrity_algorithm int_alg_id);

/// Truncate 256-bit key to 128-bit key using the least significant bits.
/// Ref: TS 33.501 Sec. A.8
sec_128_as_key truncate_key(const sec_as_key& key_in);

} // namespace security
} // namespace srsgnb

//
// Formatters
//
namespace fmt {

// Security direction
template <>
struct formatter<srsgnb::security::security_direction> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::security::security_direction dir, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"Uplink", "Downlink"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(dir)]);
  }
};
} // namespace fmt
