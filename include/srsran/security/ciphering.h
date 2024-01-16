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

#include "s3g.h"
#include "security.h"
#include "ssl.h"
#include "zuc.h"
#include "srsran/adt/byte_buffer.h"

namespace srsran {
namespace security {

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/
inline void security_nea1(const sec_128_key& key,
                          uint32_t           count,
                          uint8_t            bearer,
                          security_direction direction,
                          byte_buffer_view&  msg,
                          uint32_t           msg_len)
{
  S3G_STATE state, *state_ptr;
  uint32_t  k[]  = {0, 0, 0, 0};
  uint32_t  iv[] = {0, 0, 0, 0};
  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;
  uint32_t  len = msg.length();

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
    uint32_t offset = 0;
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      msg[offset]     = msg[offset] ^ ((ks[i] >> 24) & 0xff);
      msg[offset + 1] = msg[offset + 1] ^ ((ks[i] >> 16) & 0xff);
      msg[offset + 2] = msg[offset + 2] ^ ((ks[i] >> 8) & 0xff);
      msg[offset + 3] = msg[offset + 3] ^ ((ks[i]) & 0xff);
      offset += 4;
    }

    // process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      msg[offset] = msg[offset] ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xff);
      offset++;
    }

    // Zero tailing bits
    zero_tailing_bits(msg[offset - 1], msg_len);

    // Clean up
    free(ks);
    s3g_deinitialize(state_ptr);
  }
}

inline void security_nea1(const sec_128_key& key,
                          uint32_t           count,
                          uint8_t            bearer,
                          security_direction direction,
                          byte_buffer_view&  msg)
{
  return security_nea1(key, count, bearer, direction, msg, msg.length() * 8);
}

inline void security_nea2(const sec_128_key& key,
                          uint32_t           count,
                          uint8_t            bearer,
                          security_direction direction,
                          byte_buffer_view&  msg,
                          uint32_t           msg_len)
{
  aes_context   ctx;
  unsigned char stream_blk[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char nonce_cnt[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int           ret;
  size_t        nc_off = 0;

  uint32_t msg_len_block_8 = (msg_len + 7) / 8;
  uint32_t len             = msg.length();

  ret = aes_setkey_enc(&ctx, key.data(), 128);
  if (ret != 0) {
    return;
  }

  if (msg_len_block_8 <= len) {
    if (ret == 0) {
      // Construct nonce
      nonce_cnt[0] = (count >> 24) & 0xff;
      nonce_cnt[1] = (count >> 16) & 0xff;
      nonce_cnt[2] = (count >> 8) & 0xff;
      nonce_cnt[3] = (count)&0xff;
      nonce_cnt[4] = ((bearer & 0x1f) << 3) | ((to_number(direction) & 0x01) << 2);

      // Encryption
      byte_buffer_segment_span_range segments = msg.modifiable_segments();
      for (const auto& segment : segments) {
        ret = aes_crypt_ctr(&ctx, segment.size(), &nc_off, nonce_cnt, stream_blk, segment.data(), segment.data());
      }
    }
  }
  if (ret == 0) {
    //  Zero tailing bits
    zero_tailing_bits(msg[msg.length() - 1], msg_len);
  }
}

inline void security_nea2(const sec_128_key& key,
                          uint32_t           count,
                          uint8_t            bearer,
                          security_direction direction,
                          byte_buffer_view&  msg)
{
  return security_nea2(key, count, bearer, direction, msg, msg.length() * 8);
}

inline void security_nea3(const sec_128_key& key,
                          uint32_t           count,
                          uint8_t            bearer,
                          security_direction direction,
                          byte_buffer_view&  msg,
                          uint32_t           msg_len)
{
  uint8_t iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8;
  uint32_t  msg_len_block_32;
  uint32_t  len = msg.length();

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
    uint32_t offset = 0;
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      msg[offset]     = msg[offset] ^ ((ks[i] >> 24) & 0xff);
      msg[offset + 1] = msg[offset + 1] ^ ((ks[i] >> 16) & 0xff);
      msg[offset + 2] = msg[offset + 2] ^ ((ks[i] >> 8) & 0xff);
      msg[offset + 3] = msg[offset + 3] ^ ((ks[i]) & 0xff);
      offset += 4;
    }

    // process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      msg[offset] = msg[offset] ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xff);
      offset++;
    }

    zero_tailing_bits(msg[msg.length() - 1], msg_len);

    // Clean up
    free(ks);
  }
}

inline void security_nea3(const sec_128_key& key,
                          uint32_t           count,
                          uint8_t            bearer,
                          security_direction direction,
                          byte_buffer_view&  msg)
{
  security_nea3(key, count, bearer, direction, msg, msg.length() * 8);
}

} // namespace security
} // namespace srsran
