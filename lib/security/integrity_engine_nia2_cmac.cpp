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

#include "integrity_engine_nia2_cmac.h"
#include "mbedtls/cmac.h"
#include "srsran/security/security.h"

using namespace srsran;
using namespace security;

#ifdef MBEDTLS_CMAC_C

integrity_engine_nia2_cmac::integrity_engine_nia2_cmac(sec_128_key        k_128_int_,
                                                       uint8_t            bearer_id_,
                                                       security_direction direction_) :
  k_128_int(k_128_int_), bearer_id(bearer_id_), direction(direction_)
{
}

security_result integrity_engine_nia2_cmac::protect_integrity(byte_buffer buf, uint32_t count)
{
  security_result   result{.buf = std::move(buf), .count = count};
  security::sec_mac mac = {};

  byte_buffer_view v{result.buf.value().begin(), result.buf.value().end()};

  int                          ret;
  mbedtls_cipher_context_t     ctx;
  const mbedtls_cipher_info_t* cipher_info;
  cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
  if (cipher_info == nullptr) {
    result.buf = make_unexpected(security_error::integrity_failure);
    return result;
  }
  mbedtls_cipher_init(&ctx);

  ret = mbedtls_cipher_setup(&ctx, cipher_info);
  if (ret != 0) {
    result.buf = make_unexpected(security_error::integrity_failure);
    return result;
  }

  ret = mbedtls_cipher_cmac_starts(&ctx, k_128_int.data(), 128);
  if (ret != 0) {
    result.buf = make_unexpected(security_error::integrity_failure);
    return result;
  }

  std::array<uint8_t, 8> preamble = {};
  preamble[0]                     = (count >> 24) & 0xff;
  preamble[1]                     = (count >> 16) & 0xff;
  preamble[2]                     = (count >> 8) & 0xff;
  preamble[3]                     = count & 0xff;
  preamble[4]                     = (bearer_id << 3) | (to_number(direction) << 2);
  ret                             = mbedtls_cipher_cmac_update(&ctx, preamble.data(), preamble.size());

  const_byte_buffer_segment_span_range segments = v.segments();
  for (const auto& segment : segments) {
    ret = mbedtls_cipher_cmac_update(&ctx, segment.data(), segment.size());
    if (ret != 0) {
      result.buf = make_unexpected(security_error::integrity_failure);
      return result;
    }
  }
  uint8_t tmp_mac[16];
  ret = mbedtls_cipher_cmac_finish(&ctx, tmp_mac);
  if (ret != 0) {
    result.buf = make_unexpected(security_error::integrity_failure);
    return result;
  }
  mbedtls_cipher_free(&ctx);
  for (int i = 0; i < 4; ++i) {
    mac[i] = tmp_mac[i];
  }

  if (not result.buf->append(mac)) {
    result.buf = make_unexpected(security_error::buffer_failure);
  }
  return result;
}

security_result integrity_engine_nia2_cmac::verify_integrity(byte_buffer buf, uint32_t count)
{
  security_result result;
  return result;
}

#endif // MBEDTLS_CMAC_C
