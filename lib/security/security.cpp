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

#include "srsgnb/security/security.h"
#include "s3g.h"
#include "ssl.h"
#include "zuc.h"

#ifdef HAVE_MBEDTLS
#include "mbedtls/md5.h"
#endif
#ifdef HAVE_POLARSSL
#include "polarssl/md5.h"
#endif

using namespace srsgnb;

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/

void srsgnb::security_nia1(const sec_128_as_key& key,
                           uint32_t              count,
                           uint8_t               bearer,
                           security_direction    direction,
                           const byte_buffer&    buf,
                           sec_mac&              mac)
{
  uint32_t msg_len_bits = 0;
  uint32_t i            = 0;

  // FIXME for now we copy the byte buffer to a contiguous piece of memory.
  // This will be fixed later.
  std::vector<uint8_t> continuous_buf;
  continuous_buf.reserve(buf.length());

  for (uint32_t i = 0; i < buf.length(); i++) {
    continuous_buf.push_back(buf[i]);
  }

  msg_len_bits = buf.length() * 8;
  uint8_t* m_ptr =
      s3g_f9(key.data(), count, bearer << 27, static_cast<uint8_t>(direction), continuous_buf.data(), msg_len_bits);
  for (i = 0; i < 4; i++) {
    mac[i] = m_ptr[i];
  }
}

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/

byte_buffer srsgnb::security_nea1(const sec_128_as_key&   key,
                                  uint32_t                count,
                                  uint8_t                 bearer,
                                  security_direction      direction,
                                  const byte_buffer_view& msg)
{
  return security_nea1(key, count, bearer, direction, msg, msg.length() * 8);
}

byte_buffer srsgnb::security_nea1(const sec_128_as_key&   key,
                                  uint32_t                count,
                                  uint8_t                 bearer,
                                  security_direction      direction,
                                  const byte_buffer_view& msg,
                                  uint32_t                msg_len)
{
  S3G_STATE state, *state_ptr;
  uint32_t  k[]  = {0, 0, 0, 0};
  uint32_t  iv[] = {0, 0, 0, 0};
  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;

  byte_buffer msg_out;

  state_ptr        = &state;
  msg_len_block_8  = (msg_len + 7) / 8;
  msg_len_block_32 = (msg_len + 31) / 32;
  if (msg_len_block_8 <= msg.length()) {
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
    byte_buffer::const_iterator msg_it = msg.begin();
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      msg_out.append(*msg_it++ ^ ((ks[i] >> 24) & 0xff));
      msg_out.append(*msg_it++ ^ ((ks[i] >> 16) & 0xff));
      msg_out.append(*msg_it++ ^ ((ks[i] >> 8) & 0xff));
      msg_out.append(*msg_it++ ^ ((ks[i]) & 0xff));
    }

    // process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      msg_out.append(*msg_it++ ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xff));
    }

    // Zero tailing bits
    zero_tailing_bits(msg_out.back(), msg_len);

    // Clean up
    free(ks);
    s3g_deinitialize(state_ptr);
  }

  return msg_out;
}

byte_buffer srsgnb::security_nea2(const sec_128_as_key&   key,
                                  uint32_t                count,
                                  uint8_t                 bearer,
                                  security_direction      direction,
                                  const byte_buffer_view& msg)
{
  return security_nea2(key, count, bearer, direction, msg, msg.length() * 8);
}

byte_buffer srsgnb::security_nea2(const sec_128_as_key&   key,
                                  uint32_t                count,
                                  uint8_t                 bearer,
                                  security_direction      direction,
                                  const byte_buffer_view& msg,
                                  uint32_t                msg_len)
{
  aes_context   ctx;
  unsigned char stream_blk[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char nonce_cnt[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int           ret;
  size_t        nc_off          = 0;
  uint32_t      msg_len_block_8 = (msg_len + 7) / 8;

  byte_buffer msg_out;

  ret = aes_setkey_enc(&ctx, key.data(), 128);

  if (msg_len_block_8 <= msg.length()) {
    if (ret == 0) {
      // Construct nonce
      nonce_cnt[0] = (count >> 24) & 0xff;
      nonce_cnt[1] = (count >> 16) & 0xff;
      nonce_cnt[2] = (count >> 8) & 0xff;
      nonce_cnt[3] = (count)&0xff;
      nonce_cnt[4] = ((bearer & 0x1f) << 3) | ((to_number(direction) & 0x01) << 2);

      // Encryption
      byte_buffer::const_iterator msg_it = msg.begin();
      for (uint32_t i = 0; i < msg_len_block_8; i++) {
        // FIXME: call aes_crypt_ctr in larger contiguous chunks when available/supported
        uint8_t out;
        ret = aes_crypt_ctr(&ctx, 1, &nc_off, nonce_cnt, stream_blk, &(*msg_it++), &out);
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

byte_buffer srsgnb::security_nea3(const sec_128_as_key&   key,
                                  uint32_t                count,
                                  uint8_t                 bearer,
                                  security_direction      direction,
                                  const byte_buffer_view& msg)
{
  return security_nea3(key, count, bearer, direction, msg, msg.length() * 8);
}

byte_buffer srsgnb::security_nea3(const sec_128_as_key&   key,
                                  uint32_t                count,
                                  uint8_t                 bearer,
                                  security_direction      direction,
                                  const byte_buffer_view& msg_,
                                  uint32_t                msg_len)
{
  uint8_t iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;

  byte_buffer msg_out;

  msg_len_block_8  = (msg_len + 7) / 8;
  msg_len_block_32 = (msg_len + 31) / 32;

  // temporary conversion into contiguous memory
  constexpr uint16_t                     pdcp_max_sdu_size = 9000;
  std::array<uint8_t, pdcp_max_sdu_size> msg               = {};
  std::array<uint8_t, pdcp_max_sdu_size> out               = {};
  std::copy(msg_.begin(), msg_.end(), msg.begin());

  if (msg_len_block_8 <= msg_.length()) {
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
      out[4 * i + 0] = msg[4 * i + 0] ^ ((ks[i] >> 24) & 0xff);
      out[4 * i + 1] = msg[4 * i + 1] ^ ((ks[i] >> 16) & 0xff);
      out[4 * i + 2] = msg[4 * i + 2] ^ ((ks[i] >> 8) & 0xff);
      out[4 * i + 3] = msg[4 * i + 3] ^ ((ks[i] & 0xff));
    }

    // Process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      out[i] = msg[i] ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xff);
    }

    // Zero tailing bits
    zero_tailing_bits(out.data(), msg_len);

    for (i = 0; i < (int32_t)msg_len_block_8; i++) {
      msg_out.append(out[i]);
    }

    // zero_tailing_bits(msg_out.back(), msg_len);

    // Clean up
    free(ks);
    // zuc_deinitialize(state_ptr);
  }

  return msg_out;
}
