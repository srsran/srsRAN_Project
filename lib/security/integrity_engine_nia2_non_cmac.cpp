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
  std::array<uint8_t, 16> l;

  // subkey L generation
  aes_setkey_enc(&ctx, k_128_int.data(), 128);
  aes_crypt_ecb(&ctx, aes_encrypt, zeros.data(), l.data());

  // subkey K1 generation
  for (uint32_t i = 0; i < 15; i++) {
    k1[i] = (l[i] << 1) | ((l[i + 1] >> 7) & 0x01);
  }
  k1[15] = l[15] << 1;
  if (l[0] & 0x80) {
    k1[15] ^= 0x87;
  }

  // subkey K2 generation
  for (uint32_t i = 0; i < 15; i++) {
    k2[i] = (k1[i] << 1) | ((k1[i + 1] >> 7) & 0x01);
  }
  k2[15] = k1[15] << 1;
  if (k1[0] & 0x80) {
    k2[15] ^= 0x87;
  }
}

security_result integrity_engine_nia2_non_cmac::protect_integrity(byte_buffer buf, uint32_t count)
{
  security_result   result{.buf = std::move(buf), .count = count};
  security::sec_mac mac = {};

  byte_buffer_view v{result.buf.value().begin(), result.buf.value().end()};

  uint32_t len      = v.length();
  uint32_t len_bits = len * 8;

  std::array<uint8_t, 16> tmp_mac;
  std::array<uint8_t, 16> tmp;

  const uint32_t msg_len_block_8_with_padding = len + 8 + 16;
  srsran_assert(msg_len_block_8_with_padding <= sec_max_pdu_size,
                "{}: Maximum PDU length exceeded. len={} max_len={}",
                __FUNCTION__,
                msg_len_block_8_with_padding,
                sec_max_pdu_size);

  span<uint8_t> msg(msg_buf.data(), msg_len_block_8_with_padding);

  // construct msg from preample, PDU and padding
  msg[0] = (count >> 24) & 0xff;
  msg[1] = (count >> 16) & 0xff;
  msg[2] = (count >> 8) & 0xff;
  msg[3] = count & 0xff;
  msg[4] = (bearer_id << 3) | (to_number(direction) << 2);
  std::fill(msg.begin() + 5, msg.begin() + 8, 0);
  std::copy(v.begin(), v.end(), msg.begin() + 8);
  std::fill(msg.begin() + len + 8, msg.end(), 0);

  // MAC generation
  const uint32_t n = ceilf((float)(len + 8) / (float)(16));
  std::fill(tmp_mac.begin(), tmp_mac.end(), 0);
  for (uint32_t i = 0; i < (n - 1); i++) {
    for (uint32_t j = 0; j < 16; j++) {
      tmp[j] = tmp_mac[j] ^ msg[i * 16 + j];
    }
    aes_crypt_ecb(&ctx, aes_encrypt, tmp.data(), tmp_mac.data());
  }
  uint32_t pad_bits = ((len_bits) + 64) % 128;
  if (pad_bits == 0) {
    for (uint32_t j = 0; j < 16; j++) {
      tmp[j] = tmp_mac[j] ^ k1[j] ^ msg[(n - 1) * 16 + j];
    }
    aes_crypt_ecb(&ctx, aes_encrypt, tmp.data(), tmp_mac.data());
  } else {
    pad_bits = (128 - pad_bits) - 1;
    msg[(n - 1) * 16 + (15 - (pad_bits / 8))] |= 0x1 << (pad_bits % 8);
    for (uint32_t j = 0; j < 16; j++) {
      tmp[j] = tmp_mac[j] ^ k2[j] ^ msg[(n - 1) * 16 + j];
    }
    aes_crypt_ecb(&ctx, aes_encrypt, tmp.data(), tmp_mac.data());
  }

  // copy first 4 bytes
  std::copy(tmp_mac.begin(), tmp_mac.begin() + 4, mac.begin());

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
