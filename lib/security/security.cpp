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

#define FC_EPS_K_ASME_DERIVATION 0x10
#define FC_EPS_K_ENB_DERIVATION 0x11
#define FC_EPS_NH_DERIVATION 0x12
#define FC_EPS_K_ENB_STAR_DERIVATION 0x13
#define FC_EPS_ALGORITHM_KEY_DERIVATION 0x15

#define ALGO_EPS_DISTINGUISHER_NAS_ENC_ALG 0x01
#define ALGO_EPS_DISTINGUISHER_NAS_INT_ALG 0x02
#define ALGO_EPS_DISTINGUISHER_RRC_ENC_ALG 0x03
#define ALGO_EPS_DISTINGUISHER_RRC_INT_ALG 0x04
#define ALGO_EPS_DISTINGUISHER_UP_ENC_ALG 0x05
#define ALGO_EPS_DISTINGUISHER_UP_INT_ALG 0x06

#define FC_5G_K_GNB_STAR_DERIVATION 0x70
#define FC_5G_ALGORITHM_KEY_DERIVATION 0x69
#define FC_5G_KAUSF_DERIVATION 0x6A
#define FC_5G_RES_STAR_DERIVATION 0x6B
#define FC_5G_KSEAF_DERIVATION 0x6C
#define FC_5G_KAMF_DERIVATION 0x6D
#define FC_5G_KGNB_KN3IWF_DERIVATION 0x6E
#define FC_5G_NH_GNB_DERIVATION 0x6F

#define ALGO_5G_DISTINGUISHER_NAS_ENC_ALG 0x01
#define ALGO_5G_DISTINGUISHER_NAS_INT_ALG 0x02
#define ALGO_5G_DISTINGUISHER_RRC_ENC_ALG 0x03
#define ALGO_5G_DISTINGUISHER_RRC_INT_ALG 0x04
#define ALGO_5G_DISTINGUISHER_UP_ENC_ALG 0x05
#define ALGO_5G_DISTINGUISHER_UP_INT_ALG 0x06
namespace srsgnb {

/******************************************************************************
 * Integrity Protection
 *****************************************************************************/

void security_128_eia1(const as_key_t key,
                       uint32_t       count,
                       uint32_t       bearer,
                       uint8_t        direction,
                       uint8_t*       msg,
                       uint32_t       msg_len,
                       mac_t&         mac)
{
  uint32_t msg_len_bits = 0;
  uint32_t i            = 0;
  uint8_t* m_ptr;

  msg_len_bits = msg_len * 8;
  m_ptr        = s3g_f9(key.data(), count, bearer << 27, direction, msg, msg_len_bits);
  for (i = 0; i < 4; i++) {
    mac[i] = m_ptr[i];
  }
}

/*
uint8_t security_128_eia2(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia2(key, count, bearer, direction, msg, msg_len, mac);
}

uint8_t security_128_eia3(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia3(key, count, bearer, direction, msg, msg_len * 8, mac);
}
*/
/*
uint8_t security_md5(const uint8_t* input, size_t len, uint8_t* output)
{
  memset(output, 0x00, 16);
#ifdef HAVE_MBEDTLS
  mbedtls_md5(input, len, output);
#endif // HAVE_MBEDTLS
#ifdef HAVE_POLARSSL
  md5(input, len, output);
#endif
  return SRSRAN_SUCCESS;
}
*/
/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/

void security_128_eea1(uint8_t* key,
                       uint32_t count,
                       uint8_t  bearer,
                       uint8_t  direction,
                       uint8_t* msg,
                       uint32_t msg_len,
                       uint8_t* msg_out)
{
  S3G_STATE state, *state_ptr;
  uint32_t  k[]  = {0, 0, 0, 0};
  uint32_t  iv[] = {0, 0, 0, 0};
  uint32_t* ks;
  int32_t   i;
  uint32_t  msg_len_block_8, msg_len_block_32;

  if (key != NULL && msg != NULL && msg_out != NULL) {
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

/*
uint8_t security_128_eea2(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{
  return liblte_security_encryption_eea2(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}

uint8_t security_128_eea3(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{
  return liblte_security_encryption_eea3(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}
*/
} // namespace srsgnb
