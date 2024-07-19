/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "integrity_engine_nia2_non_cmac.h"
#include "srsran/security/security.h"
#include "srsran/security/ssl.h"

using namespace srsran;
using namespace security;

integrity_engine_nia2_non_cmac::integrity_engine_nia2_non_cmac(sec_128_key        k_128_int_,
                                                               uint8_t            bearer_id_,
                                                               security_direction direction_) :
  k_128_int(k_128_int_), bearer_id(bearer_id_), direction(direction_)
{
}

security_result integrity_engine_nia2_non_cmac::protect_integrity(byte_buffer buf, uint32_t count)
{
  security_result   result{.buf = std::move(buf), .count = count};
  security::sec_mac mac = {};

  byte_buffer_view v{result.buf.value().begin(), result.buf.value().end()};

  uint32_t    len      = v.length();
  uint32_t    len_bits = len * 8;
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

  const uint32_t msg_len_block_8_with_padding = len + 8 + 16;
  srsran_assert(msg_len_block_8_with_padding <= sec_max_pdu_size,
                "{}: Maximum PDU length exceeded. len={} max_len={}",
                __FUNCTION__,
                msg_len_block_8_with_padding,
                sec_max_pdu_size);

  std::array<uint8_t, sec_max_pdu_size> M_buf;
  span<uint8_t>                         M(M_buf.data(), msg_len_block_8_with_padding);

  // Subkey L generation
  aes_setkey_enc(&ctx, k_128_int.data(), 128);
  aes_crypt_ecb(&ctx, aes_encrypt, const_zero, L);

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
  std::fill(M.begin(), M.end(), 0);
  M[0] = (count >> 24) & 0xff;
  M[1] = (count >> 16) & 0xff;
  M[2] = (count >> 8) & 0xff;
  M[3] = count & 0xff;
  M[4] = (bearer_id << 3) | (to_number(direction) << 2);
  for (i = 0; i < len; i++) {
    M[8 + i] = v[i];
  }

  // MAC generation
  n = (uint32_t)(ceilf((float)(len + 8) / (float)(16)));
  for (i = 0; i < 16; i++) {
    T[i] = 0;
  }
  for (i = 0; i < n - 1; i++) {
    for (j = 0; j < 16; j++) {
      tmp[j] = T[j] ^ M[i * 16 + j];
    }
    aes_crypt_ecb(&ctx, aes_encrypt, tmp, T);
  }
  pad_bits = ((len_bits) + 64) % 128;
  if (pad_bits == 0) {
    for (j = 0; j < 16; j++) {
      tmp[j] = T[j] ^ K1[j] ^ M[i * 16 + j];
    }
    aes_crypt_ecb(&ctx, aes_encrypt, tmp, T);
  } else {
    pad_bits = (128 - pad_bits) - 1;
    M[i * 16 + (15 - (pad_bits / 8))] |= 0x1 << (pad_bits % 8);
    for (j = 0; j < 16; j++) {
      tmp[j] = T[j] ^ K2[j] ^ M[i * 16 + j];
    }
    aes_crypt_ecb(&ctx, aes_encrypt, tmp, T);
  }

  for (i = 0; i < 4; i++) {
    mac[i] = T[i];
  }

  if (not result.buf->append(mac)) {
    result.buf = make_unexpected(security_error::buffer_failure);
  }
  return result;
}

security_result integrity_engine_nia2_non_cmac::verify_integrity(byte_buffer buf, uint32_t count)
{
  security_result result;
  return result;
}
