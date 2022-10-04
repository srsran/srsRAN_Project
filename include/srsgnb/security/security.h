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

/// FC Values
/// Ref: TS 33.501 Sec. A.1.2
enum class fc_value {
  k_gnb_star_derivation    = 0x70,
  algorithm_key_derivation = 0x69,
  kausf_derivation         = 0x6a,
  res_star_derivation      = 0x6b,
  kseaf_derivation         = 0x6c,
  kamf_derivation          = 0x6d,
  kgnb_kn3iwf_derivation   = 0x6e,
  nh_gnb_derivation        = 0x6f
};
constexpr uint8_t to_number(fc_value fc)
{
  return static_cast<uint8_t>(fc);
}

/// Security Algorithm Distinguisher
/// Ref: TS 33.501 Sec. A.8
enum class security_algo_distinguisher {
  nas_enc_alg = 0x01,
  nas_int_alg = 0x02,
  rrc_enc_alg = 0x03,
  rrc_int_alg = 0x04,
  up_enc_alg  = 0x05,
  up_int_alg  = 0x06
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
 * Key Generation
 *****************************************************************************/
void kdf_common(sec_as_key&                 key_out,
                const sec_as_key&           key_in,
                const fc_value              fc,
                const std::vector<uint8_t>& p0,
                const std::vector<uint8_t>& p1);

void security_generate_k_rrc(sec_as_key&               k_rrc_enc,
                             sec_as_key&               k_rrc_int,
                             const sec_as_key&         k_gnb,
                             const ciphering_algorithm enc_alg_id,
                             const integrity_algorithm int_alg_id);

void security_generate_k_up(sec_as_key&               k_up_enc,
                            sec_as_key&               k_up_int,
                            const sec_as_key&         k_gnb,
                            const ciphering_algorithm enc_alg_id,
                            const integrity_algorithm int_alg_id);

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/
void security_nia1(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   byte_buffer_view      msg);

void security_nia1(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   byte_buffer_view      msg,
                   uint32_t              msg_len);

void security_nia2(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   byte_buffer_view      msg);

void security_nia2(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   byte_buffer_view      msg,
                   uint32_t              msg_len);

void security_nia3(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   byte_buffer_view      msg);

void security_nia3(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   byte_buffer_view      msg,
                   uint32_t              msg_len);

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
