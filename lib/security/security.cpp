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
                           lcid_t                bearer,
                           uint8_t               direction,
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
    continuous_buf[i] = buf[i];
  }

  msg_len_bits   = buf.length() * 8;
  uint8_t* m_ptr = s3g_f9(key.data(), count, bearer << 27, direction, continuous_buf.data(), msg_len_bits);
  for (i = 0; i < 4; i++) {
    mac[i] = m_ptr[i];
  }
}

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/

void srsgnb::security_nea1(const sec_as_key& key,
                           uint32_t          count,
                           uint8_t           bearer,
                           uint8_t           direction,
                           uint8_t*          msg,
                           uint32_t          msg_len,
                           uint8_t*          msg_out)
{
  S3G_STATE state, *state_ptr;
  uint32_t  k[]  = {0, 0, 0, 0};
  uint32_t  iv[] = {0, 0, 0, 0};
  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;

  if (msg != nullptr && msg_out != nullptr) {
    state_ptr        = &state;
    msg_len_block_8  = (msg_len + 7) / 8;
    msg_len_block_32 = (msg_len + 31) / 32;

    // Transform key
    for (i = 3; i >= 0; i--) {
      k[i] = (key[4 * (3 - i) + 0] << 24) | (key[4 * (3 - i) + 1] << 16) | (key[4 * (3 - i) + 2] << 8) |
             (key[4 * (3 - i) + 3]);
    }

    // Construct iv
    iv[3] = count;
    iv[2] = ((bearer & 0x1F) << 27) | ((direction & 0x01) << 26);
    iv[1] = iv[3];
    iv[0] = iv[2];

    // Initialize keystream
    s3g_initialize(state_ptr, k, iv);

    // Generate keystream

    ks = (uint32_t*)calloc(msg_len_block_32, sizeof(uint32_t));
    s3g_generate_keystream(state_ptr, msg_len_block_32, ks);

    // Generate output except last block
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      msg_out[4 * i + 0] = msg[4 * i + 0] ^ ((ks[i] >> 24) & 0xFF);
      msg_out[4 * i + 1] = msg[4 * i + 1] ^ ((ks[i] >> 16) & 0xFF);
      msg_out[4 * i + 2] = msg[4 * i + 2] ^ ((ks[i] >> 8) & 0xFF);
      msg_out[4 * i + 3] = msg[4 * i + 3] ^ ((ks[i] & 0xFF));
    }

    // Process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      msg_out[i] = msg[i] ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xFF);
    }

    // Zero tailing bits
    zero_tailing_bits(msg_out, msg_len);

    // Clean up
    free(ks);
    s3g_deinitialize(state_ptr);
  }
}

