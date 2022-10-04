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
#include <arpa/inet.h>

#ifdef HAVE_MBEDTLS
#include "mbedtls/md5.h"
#endif
#ifdef HAVE_POLARSSL
#include "polarssl/md5.h"
#endif

#define FC_5G_K_GNB_STAR_DERIVATION 0x70
#define FC_5G_ALGORITHM_KEY_DERIVATION 0x69
#define FC_5G_KAUSF_DERIVATION 0x6a
#define FC_5G_RES_STAR_DERIVATION 0x6b
#define FC_5G_KSEAF_DERIVATION 0x6c
#define FC_5G_KAMF_DERIVATION 0x6d
#define FC_5G_KGNB_KN3IWF_DERIVATION 0x6e
#define FC_5G_NH_GNB_DERIVATION 0x6f

#define ALGO_5G_DISTINGUISHER_NAS_ENC_ALG 0x01
#define ALGO_5G_DISTINGUISHER_NAS_INT_ALG 0x02
#define ALGO_5G_DISTINGUISHER_RRC_ENC_ALG 0x03
#define ALGO_5G_DISTINGUISHER_RRC_INT_ALG 0x04
#define ALGO_5G_DISTINGUISHER_UP_ENC_ALG 0x05
#define ALGO_5G_DISTINGUISHER_UP_INT_ALG 0x06

using namespace srsgnb;

/******************************************************************************
 * Key Generation
 *****************************************************************************/
bool srsgnb::security_generate_k_nr_rrc(sec_as_key&               k_rrc_enc,
                                        sec_as_key&               k_rrc_int,
                                        const sec_as_key&         k_gnb,
                                        const ciphering_algorithm enc_alg_id,
                                        const integrity_algorithm int_alg_id)
{
  // Derive RRC ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_RRC_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(enc_alg_id);

  if (kdf_common(k_rrc_enc, k_gnb, FC_5G_ALGORITHM_KEY_DERIVATION, algo_distinguisher, algorithm_identity) != true) {
    return false;
  }

  // Derive RRC INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_RRC_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(int_alg_id);

  // Derive RRC int
  if (kdf_common(k_rrc_int, k_gnb, FC_5G_ALGORITHM_KEY_DERIVATION, algo_distinguisher, algorithm_identity) != true) {
    return false;
  }
  return true;
}

bool srsgnb::security_generate_k_nr_up(sec_as_key&               k_up_enc,
                                       sec_as_key&               k_up_int,
                                       const sec_as_key&         k_gnb,
                                       const ciphering_algorithm enc_alg_id,
                                       const integrity_algorithm int_alg_id)
{
  // Derive UP ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_UP_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(enc_alg_id);

  if (kdf_common(k_up_enc, k_gnb, FC_5G_ALGORITHM_KEY_DERIVATION, algo_distinguisher, algorithm_identity) != true) {
    return false;
  }

  // Derive UP INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_UP_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = static_cast<uint8_t>(int_alg_id);

  // Derive UP int
  if (kdf_common(k_up_int, k_gnb, FC_5G_ALGORITHM_KEY_DERIVATION, algo_distinguisher, algorithm_identity) != true) {
    return false;
  }
  return true;
}

bool srsgnb::kdf_common(sec_as_key&                 key_out,
                        const sec_as_key&           key_in,
                        const uint8_t               fc,
                        const std::vector<uint8_t>& P0,
                        const std::vector<uint8_t>& P1)
{
  uint8_t* s;
  uint32_t s_len = 1 + P0.size() + 2 + P1.size() + 2;

  s = (uint8_t*)calloc(s_len, sizeof(uint8_t));

  if (s == nullptr) {
    return false;
  }

  uint32_t i = 0;
  s[i]       = fc; // FC
  i++;

  // P0
  memcpy(&s[i], P0.data(), P0.size());
  i += P0.size();
  uint16_t p0_length_value = htons(P0.size());
  memcpy(&s[i], &p0_length_value, sizeof(p0_length_value));
  i += sizeof(p0_length_value);

  // P1
  memcpy(&s[i], P1.data(), P1.size());
  i += P1.size();
  uint16_t p1_length_value = htons(P1.size());
  memcpy(&s[i], &p1_length_value, sizeof(p1_length_value));
  i += sizeof(p1_length_value);

  sha256(key_in.data(), key_in.size(), s, i, key_out.data(), 0);
  free(s);

  return true;
}

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/

void srsgnb::security_nia1(sec_mac&              mac,
                           const sec_128_as_key& key,
                           uint32_t              count,
                           uint8_t               bearer,
                           security_direction    direction,
                           byte_buffer_view      msg)
{
  security_nia1(mac, key, count, bearer, direction, msg, msg.length() * 8);
}

void srsgnb::security_nia1(sec_mac&              mac,
                           const sec_128_as_key& key,
                           uint32_t              count,
                           uint8_t               bearer,
                           security_direction    direction,
                           byte_buffer_view      msg,
                           uint32_t              msg_len)
{
  uint32_t i = 0;

  // FIXME for now we copy the byte buffer to a contiguous piece of memory.
  // This will be fixed later.
  std::vector<uint8_t> continuous_buf;
  continuous_buf.reserve(msg.length());

  for (uint32_t i = 0; i < msg.length(); i++) {
    continuous_buf.push_back(msg[i]);
  }

  if ((msg_len + 7) / 8 <= msg.length()) {
    uint8_t* m_ptr =
        s3g_f9(key.data(), count, bearer << 27, static_cast<uint8_t>(direction), continuous_buf.data(), msg_len);
    for (i = 0; i < 4; i++) {
      mac[i] = m_ptr[i];
    }
  }
}

void srsgnb::security_nia2(sec_mac&              mac,
                           const sec_128_as_key& key,
                           uint32_t              count,
                           uint8_t               bearer,
                           security_direction    direction,
                           byte_buffer_view      msg)
{
  security_nia2(mac, key, count, bearer, direction, msg, msg.length() * 8);
}

void srsgnb::security_nia2(sec_mac&              mac,
                           const sec_128_as_key& key,
                           uint32_t              count,
                           uint8_t               bearer,
                           security_direction    direction,
                           byte_buffer_view      msg,
                           uint32_t              msg_len)
{
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

  if (msg_len_block_8 <= msg.length()) {
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
      M[8 + i] = msg[i];
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

void srsgnb::security_nia3(sec_mac&              mac,
                           const sec_128_as_key& key,
                           uint32_t              count,
                           uint8_t               bearer,
                           security_direction    direction,
                           byte_buffer_view      msg)
{
  security_nia3(mac, key, count, bearer, direction, msg, msg.length() * 8);
}

static uint32_t GET_WORD(uint32_t* DATA, uint32_t i)
{
  uint32_t WORD, ti;
  ti = i % 32;
  if (ti == 0)
    WORD = DATA[i / 32];
  else
    WORD = (DATA[i / 32] << ti) | (DATA[i / 32 + 1] >> (32 - ti));
  return WORD;
}

void srsgnb::security_nia3(sec_mac&              mac,
                           const sec_128_as_key& key,
                           uint32_t              count,
                           uint8_t               bearer,
                           security_direction    direction,
                           byte_buffer_view      msg,
                           uint32_t              msg_len)
{
  uint8_t iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32_t* ks;
  uint32_t  msg_len_block_8 = (msg_len + 7) / 8;
  if (msg_len_block_8 <= msg.length()) {
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

    byte_buffer::const_iterator msg_it = msg.begin();

    uint32_t T = 0;
    for (uint32_t i = 0; i < msg_len; i++) {
      if (*msg_it & (1 << (7 - (i % 8)))) {
        T ^= GET_WORD(ks, i);
      }
      if (i % 8 == 7) {
        // continue with next byte
        msg_it++;
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
                                  const byte_buffer_view& msg,
                                  uint32_t                msg_len)
{
  uint8_t iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;

  byte_buffer msg_out;

  msg_len_block_8  = (msg_len + 7) / 8;
  msg_len_block_32 = (msg_len + 31) / 32;
  if (msg_len_block_8 <= msg.length()) {
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
    byte_buffer::const_iterator msg_it = msg.begin();
    for (i = 0; i < (int32_t)msg_len_block_32 - 1; i++) {
      msg_out.append(*msg_it++ ^ ((ks[i] >> 24) & 0xff));
      msg_out.append(*msg_it++ ^ ((ks[i] >> 16) & 0xff));
      msg_out.append(*msg_it++ ^ ((ks[i] >> 8) & 0xff));
      msg_out.append(*msg_it++ ^ ((ks[i] & 0xff)));
    }

    // Process last bytes
    for (i = (msg_len_block_32 - 1) * 4; i < (int32_t)msg_len_block_8; i++) {
      msg_out.append(*msg_it++ ^ ((ks[i / 4] >> ((3 - (i % 4)) * 8)) & 0xff));
    }

    zero_tailing_bits(msg_out.back(), msg_len);

    // Clean up
    free(ks);
  }

  return msg_out;
}
