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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/srslog/srslog.h"

#include <array>

namespace srsgnb {

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
 * Integrity Protection
 *****************************************************************************/
void security_nia1(const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   const byte_buffer&    msg,
                   sec_mac&              mac);

void security_nia1(const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   const byte_buffer&    msg,
                   uint32_t              msg_len,
                   sec_mac&              mac);

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/
byte_buffer security_nea1(const sec_128_as_key&   key,
                          uint32_t                count,
                          uint8_t                 bearer,
                          security_direction      direction,
                          const byte_buffer_view& msg);

byte_buffer security_nea1(const sec_128_as_key&   key,
                          uint32_t                count,
                          uint8_t                 bearer,
                          security_direction      direction,
                          const byte_buffer_view& msg,
                          uint32_t                msg_len);

byte_buffer security_nea2(const sec_128_as_key&   key,
                          uint32_t                count,
                          uint8_t                 bearer,
                          security_direction      direction,
                          const byte_buffer_view& msg);

byte_buffer security_nea2(const sec_128_as_key&   key,
                          uint32_t                count,
                          uint8_t                 bearer,
                          security_direction      direction,
                          const byte_buffer_view& msg,
                          uint32_t                msg_len);

byte_buffer security_nea3(const sec_128_as_key&   key,
                          uint32_t                count,
                          uint8_t                 bearer,
                          security_direction      direction,
                          const byte_buffer_view& msg);

byte_buffer security_nea3(const sec_128_as_key&   key,
                          uint32_t                count,
                          uint8_t                 bearer,
                          security_direction      direction,
                          const byte_buffer_view& msg,
                          uint32_t                msg_len);

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

} // namespace srsgnb

//
// Formatters
//
namespace fmt {

// Security direction
template <>
struct formatter<srsgnb::security_direction> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::security_direction dir, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"Uplink", "Downlink"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(dir)]);
  }
};
} // namespace fmt
