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

#include "s3g.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/srslog/srslog.h"
#include "ssl.h"
#include "zuc.h"

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
void generic_kdf(sec_as_key&                 key_out,
                 const sec_as_key&           key_in,
                 const fc_value              fc,
                 const std::vector<uint8_t>& p0,
                 const std::vector<uint8_t>& p1);

/// Algorithm key derivation function (RRC)
/// Ref: TS 33.501 Sec. A.8
void security_generate_k_rrc(sec_as_key&               k_rrc_enc,
                             sec_as_key&               k_rrc_int,
                             const sec_as_key&         k_gnb,
                             const ciphering_algorithm enc_alg_id,
                             const integrity_algorithm int_alg_id);

/// Algorithm key derivation function (UP)
/// Ref: TS 33.501 Sec. A.8
void security_generate_k_up(sec_as_key&               k_up_enc,
                            sec_as_key&               k_up_int,
                            const sec_as_key&         k_gnb,
                            const ciphering_algorithm enc_alg_id,
                            const integrity_algorithm int_alg_id);

/// Truncate 256-bit key to 128-bit key using the least significant bits.
/// Ref: TS 33.501 Sec. A.8
sec_128_as_key truncate_key(const sec_as_key& key_in);

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/

template <typename It>
void security_nia1(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   It                    msg_begin,
                   It                    msg_end,
                   uint32_t              msg_len)
{
  static_assert(std::is_same<typename It::value_type, uint8_t>::value, "Iterator value type is not uint8_t");
  uint32_t i = 0;

  // FIXME for now we copy the byte buffer to a contiguous piece of memory.
  // This will be fixed later.
  std::vector<uint8_t> continuous_buf;
  uint32_t             len = std::distance(msg_begin, msg_end);
  continuous_buf.reserve(len);

  for (uint32_t i = 0; i < len; i++) {
    continuous_buf.push_back(*msg_begin);
    msg_begin++;
  }

  if ((msg_len + 7) / 8 <= len) {
    uint8_t* m_ptr =
        s3g_f9(key.data(), count, bearer << 27, static_cast<uint8_t>(direction), continuous_buf.data(), msg_len);
    for (i = 0; i < 4; i++) {
      mac[i] = m_ptr[i];
    }
  }
}

template <typename It>
void security_nia1(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   It                    msg_begin,
                   It                    msg_end)
{
  security_nia1(mac, key, count, bearer, direction, msg_begin, msg_end, std::distance(msg_begin, msg_end) * 8);
}

template <typename It>
void security_nia2(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   It                    msg_begin,
                   It                    msg_end,
                   uint32_t              msg_len)
{
  static_assert(std::is_same<typename It::value_type, uint8_t>::value, "Iterator value type is not uint8_t");
  uint32_t    len             = std::distance(msg_begin, msg_end);
  uint32_t    msg_len_block_8 = (msg_len + 7) / 8;
  uint8_t     M[msg_len_block_8 + 8 + 16];
  aes_context ctx;
  uint32_t    i;
  uint32_t    j;
  uint32_t    n;
  uint32_t    pad_bits;
  uint8_t     const_zero[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t     L[16];
  uint8_t     K1[16];
  uint8_t     K2[16];
  uint8_t     T[16];
  uint8_t     tmp[16];

  if (msg_len_block_8 <= len) {
    // Subkey L generation
    aes_setkey_enc(&ctx, key.data(), 128);
    aes_crypt_ecb(&ctx, AES_ENCRYPT, const_zero, L);

    // Subkey K1 generation
    for (i = 0; i < 15; i++) {
      K1[i] = (L[i] << 1) | ((L[i + 1] >> 7) & 0x01);
    }
    K1[15] = L[15] << 1;
    if (L[0] & 0x80) {
      K1[15] ^= 0x87;
    }

    // Subkey K2 generation
    for (i = 0; i < 15; i++) {
      K2[i] = (K1[i] << 1) | ((K1[i + 1] >> 7) & 0x01);
    }
    K2[15] = K1[15] << 1;
    if (K1[0] & 0x80) {
      K2[15] ^= 0x87;
    }

    // Construct M
    memset(M, 0, msg_len_block_8 + 8 + 16);
    M[0] = (count >> 24) & 0xff;
    M[1] = (count >> 16) & 0xff;
    M[2] = (count >> 8) & 0xff;
    M[3] = count & 0xff;
    M[4] = (bearer << 3) | (to_number(direction) << 2);
    for (i = 0; i < msg_len_block_8; i++) {
      M[8 + i] = *msg_begin;
      msg_begin++;
    }

    // MAC generation
    n = (uint32_t)(ceilf((float)(msg_len_block_8 + 8) / (float)(16)));
    for (i = 0; i < 16; i++) {
      T[i] = 0;
    }
    for (i = 0; i < n - 1; i++) {
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    }
    pad_bits = ((msg_len) + 64) % 128;
    if (pad_bits == 0) {
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ K1[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    } else {
      pad_bits = (128 - pad_bits) - 1;
      M[i * 16 + (15 - (pad_bits / 8))] |= 0x1 << (pad_bits % 8);
      for (j = 0; j < 16; j++) {
        tmp[j] = T[j] ^ K2[j] ^ M[i * 16 + j];
      }
      aes_crypt_ecb(&ctx, AES_ENCRYPT, tmp, T);
    }

    for (i = 0; i < 4; i++) {
      mac[i] = T[i];
    }
  }
}

template <typename It>
void security_nia2(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   It                    msg_begin,
                   It                    msg_end)
{
  security_nia2(mac, key, count, bearer, direction, msg_begin, msg_end, std::distance(msg_begin, msg_end) * 8);
}

inline uint32_t GET_WORD(uint32_t* DATA, uint32_t i)
{
  uint32_t WORD, ti;
  ti = i % 32;
  if (ti == 0)
    WORD = DATA[i / 32];
  else
    WORD = (DATA[i / 32] << ti) | (DATA[i / 32 + 1] >> (32 - ti));
  return WORD;
}

template <typename It>
void security_nia3(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   It                    msg_begin,
                   It                    msg_end,
                   uint32_t              msg_len)
{
  static_assert(std::is_same<typename It::value_type, uint8_t>::value, "Iterator value type is not uint8_t");
  uint32_t len    = std::distance(msg_begin, msg_end);
  uint8_t  iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32_t* ks;
  uint32_t  msg_len_block_8 = (msg_len + 7) / 8;
  if (msg_len_block_8 <= len) {
    // Construct iv
    iv[0] = (count >> 24) & 0xff;
    iv[1] = (count >> 16) & 0xff;
    iv[2] = (count >> 8) & 0xff;
    iv[3] = count & 0xff;

    iv[4] = (bearer << 3) & 0xf8;
    iv[5] = iv[6] = iv[7] = 0;

    iv[8]  = ((count >> 24) & 0xff) ^ ((to_number(direction) & 1) << 7);
    iv[9]  = (count >> 16) & 0xff;
    iv[10] = (count >> 8) & 0xff;
    iv[11] = count & 0xff;

    iv[12] = iv[4];
    iv[13] = iv[5];
    iv[14] = iv[6] ^ ((to_number(direction) & 1) << 7);
    iv[15] = iv[7];

    zuc_state_t zuc_state;
    // Initialize keystream
    zuc_initialize(&zuc_state, key.data(), iv);

    // Generate keystream
    int N = msg_len + 64;
    int L = (N + 31) / 32;

    ks = (uint32_t*)calloc(L, sizeof(uint32_t));

    zuc_generate_keystream(&zuc_state, L, ks);

    uint32_t T = 0;
    for (uint32_t i = 0; i < msg_len; i++) {
      if (*msg_begin & (1 << (7 - (i % 8)))) {
        T ^= GET_WORD(ks, i);
      }
      if (i % 8 == 7) {
        // continue with next byte
        msg_begin++;
      }
    }

    T ^= GET_WORD(ks, msg_len);

    uint32_t mac_tmp = T ^ ks[L - 1];
    mac[0]           = (mac_tmp >> 24) & 0xff;
    mac[1]           = (mac_tmp >> 16) & 0xff;
    mac[2]           = (mac_tmp >> 8) & 0xff;
    mac[3]           = mac_tmp & 0xff;

    free(ks);
  }
}

template <typename It>
void security_nia3(sec_mac&              mac,
                   const sec_128_as_key& key,
                   uint32_t              count,
                   uint8_t               bearer,
                   security_direction    direction,
                   It                    msg_begin,
                   It                    msg_end)
{
  security_nia3(mac, key, count, bearer, direction, msg_begin, msg_end, std::distance(msg_begin, msg_end) * 8);
}

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/

template <typename It>
byte_buffer security_nea1(const sec_128_as_key& key,
                          uint32_t              count,
                          uint8_t               bearer,
                          security_direction    direction,
                          It                    msg_begin,
                          It                    msg_end,
                          uint32_t              msg_len)
{
  static_assert(std::is_same<typename It::value_type, uint8_t>::value, "Iterator value type is not uint8_t");
  S3G_STATE state, *state_ptr;
  uint32_t  k[]  = {0, 0, 0, 0};
  uint32_t  iv[] = {0, 0, 0, 0};
  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;
  uint32_t  len = std::distance(msg_begin, msg_end);

  byte_buffer msg_out;

  state_ptr        = &state;
  msg_len_block_8  = (msg_len + 7) / 8;
  msg_len_block_32 = (msg_len + 31) / 32;
  if (msg_len_block_8 <= len) {
    // Transform key
    for (i = 3; i >= 0; i--) {
      k[i] = (key[4 * (3 - i) + 0] << 24) | (key[4 * (3 - i) + 1] << 16) | (key[4 * (3 - i) + 2] << 8) |
             (key[4 * (3 - i) + 3]);
    }

    // Construct iv
    iv[3] = count;
    iv[2] = ((bearer & 0x1f) << 27) | ((static_cast<uint8_t>(direction) & 0x01) << 26);
    iv[1] = iv[3];
    iv[0] = iv[2];

    // Initialize keystream
    s3g_initialize(state_ptr, k, iv);

    // Generate keystream
    ks = (uint32_t*)calloc(msg_len_block_32, sizeof(uint32_t));
    s3g_generate_keystream(state_ptr, msg_len_block_32, ks);

    // Generate output except last block
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      msg_out.append(*msg_begin++ ^ ((ks[i] >> 24) & 0xff));
      msg_out.append(*msg_begin++ ^ ((ks[i] >> 16) & 0xff));
      msg_out.append(*msg_begin++ ^ ((ks[i] >> 8) & 0xff));
      msg_out.append(*msg_begin++ ^ ((ks[i]) & 0xff));
    }

    // process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      msg_out.append(*msg_begin++ ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xff));
    }

    // Zero tailing bits
    zero_tailing_bits(msg_out.back(), msg_len);

    // Clean up
    free(ks);
    s3g_deinitialize(state_ptr);
  }

  return msg_out;
}

template <typename It>
byte_buffer security_nea1(const sec_128_as_key& key,
                          uint32_t              count,
                          uint8_t               bearer,
                          security_direction    direction,
                          It                    msg_begin,
                          It                    msg_end)
{
  return security_nea1(key, count, bearer, direction, msg_begin, msg_end, std::distance(msg_begin, msg_end) * 8);
}

template <typename It>
byte_buffer security_nea2(const sec_128_as_key& key,
                          uint32_t              count,
                          uint8_t               bearer,
                          security_direction    direction,
                          It                    msg_begin,
                          It                    msg_end,
                          uint32_t              msg_len)
{
  static_assert(std::is_same<typename It::value_type, uint8_t>::value, "Iterator value type is not uint8_t");
  aes_context   ctx;
  unsigned char stream_blk[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char nonce_cnt[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int           ret;
  size_t        nc_off          = 0;
  uint32_t      msg_len_block_8 = (msg_len + 7) / 8;
  uint32_t      len             = std::distance(msg_begin, msg_end);

  byte_buffer msg_out;

  ret = aes_setkey_enc(&ctx, key.data(), 128);

  if (msg_len_block_8 <= len) {
    if (ret == 0) {
      // Construct nonce
      nonce_cnt[0] = (count >> 24) & 0xff;
      nonce_cnt[1] = (count >> 16) & 0xff;
      nonce_cnt[2] = (count >> 8) & 0xff;
      nonce_cnt[3] = (count)&0xff;
      nonce_cnt[4] = ((bearer & 0x1f) << 3) | ((to_number(direction) & 0x01) << 2);

      // Encryption
      for (uint32_t i = 0; i < msg_len_block_8; i++) {
        // FIXME: call aes_crypt_ctr in larger contiguous chunks when available/supported
        uint8_t out;
        ret = aes_crypt_ctr(&ctx, 1, &nc_off, nonce_cnt, stream_blk, &(*msg_begin++), &out);
        msg_out.append(out);
      }
    }

    if (ret == 0) {
      // Zero tailing bits
      zero_tailing_bits(msg_out.back(), msg_len);
    }
  }

  return msg_out;
}

template <typename It>
byte_buffer security_nea2(const sec_128_as_key& key,
                          uint32_t              count,
                          uint8_t               bearer,
                          security_direction    direction,
                          It                    msg_begin,
                          It                    msg_end)
{
  return security_nea2(key, count, bearer, direction, msg_begin, msg_end, std::distance(msg_begin, msg_end) * 8);
}

template <typename It>
byte_buffer security_nea3(const sec_128_as_key& key,
                          uint32_t              count,
                          uint8_t               bearer,
                          security_direction    direction,
                          It                    msg_begin,
                          It                    msg_end,
                          uint32_t              msg_len)
{
  static_assert(std::is_same<typename It::value_type, uint8_t>::value, "Iterator value type is not uint8_t");
  uint8_t iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;
  uint32_t  len = std::distance(msg_begin, msg_end);

  byte_buffer msg_out;

  msg_len_block_8  = (msg_len + 7) / 8;
  msg_len_block_32 = (msg_len + 31) / 32;
  if (msg_len_block_8 <= len) {
    // Construct iv
    iv[0]  = (count >> 24) & 0xff;
    iv[1]  = (count >> 16) & 0xff;
    iv[2]  = (count >> 8) & 0xff;
    iv[3]  = (count)&0xff;
    iv[4]  = ((bearer & 0x1f) << 3) | ((to_number(direction) & 0x01) << 2);
    iv[5]  = 0;
    iv[6]  = 0;
    iv[7]  = 0;
    iv[8]  = iv[0];
    iv[9]  = iv[1];
    iv[10] = iv[2];
    iv[11] = iv[3];
    iv[12] = iv[4];
    iv[13] = iv[5];
    iv[14] = iv[6];
    iv[15] = iv[7];

    zuc_state_t zuc_state;
    // Initialize keystream
    zuc_initialize(&zuc_state, key.data(), iv);

    // Generate keystream

    ks = (uint32_t*)calloc(msg_len_block_32, sizeof(uint32_t));
    zuc_generate_keystream(&zuc_state, msg_len_block_32, ks);

    // Generate output except last block
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      msg_out.append(*msg_begin++ ^ ((ks[i] >> 24) & 0xff));
      msg_out.append(*msg_begin++ ^ ((ks[i] >> 16) & 0xff));
      msg_out.append(*msg_begin++ ^ ((ks[i] >> 8) & 0xff));
      msg_out.append(*msg_begin++ ^ ((ks[i] & 0xff)));
    }

    // Process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      msg_out.append(*msg_begin++ ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xff));
    }

    zero_tailing_bits(msg_out.back(), msg_len);

    // Clean up
    free(ks);
  }

  return msg_out;
}

template <typename It>
byte_buffer security_nea3(const sec_128_as_key& key,
                          uint32_t              count,
                          uint8_t               bearer,
                          security_direction    direction,
                          It                    msg_begin,
                          It                    msg_end)
{
  return security_nea3(key, count, bearer, direction, msg_begin, msg_end, std::distance(msg_begin, msg_end) * 8);
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
