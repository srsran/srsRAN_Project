/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "integrity_engine_nia2_cmac.h"
#include "mbedtls/cmac.h"
#include "srsran/security/security.h"

using namespace srsran;
using namespace security;

#ifdef MBEDTLS_CMAC_C

integrity_engine_nia2_cmac::integrity_engine_nia2_cmac(sec_128_key        k_128_int_,
                                                       uint8_t            bearer_id_,
                                                       security_direction direction_) :
  k_128_int(k_128_int_), bearer_id(bearer_id_), direction(direction_), logger(srslog::fetch_basic_logger("SEC"))
{
  cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
  if (cipher_info == nullptr) {
    srsran_assertion_failure("Failure in mbedtls_cipher_info_from_type");
    return;
  }
  mbedtls_cipher_init(&ctx);

  int ret;
  ret = mbedtls_cipher_setup(&ctx, cipher_info);
  if (ret != 0) {
    srsran_assertion_failure("Failure in mbedtls_cipher_setup");
    return;
  }

  ret = mbedtls_cipher_cmac_starts(&ctx, k_128_int.data(), 128);
  if (ret != 0) {
    srsran_assertion_failure("Failure in mbedtls_cipher_cmac_starts");
    return;
  }
}

integrity_engine_nia2_cmac::~integrity_engine_nia2_cmac()
{
  mbedtls_cipher_free(&ctx);
}

expected<security::sec_mac, security_error> integrity_engine_nia2_cmac::compute_mac(const byte_buffer_view v,
                                                                                    uint32_t               count)
{
  security::sec_mac mac = {};

  // reset state machine
  int ret;
  ret = mbedtls_cipher_cmac_reset(&ctx);
  if (ret != 0) {
    return make_unexpected(security_error::integrity_failure);
  }

  // process preamble
  std::array<uint8_t, 8> preamble = {};
  preamble[0]                     = (count >> 24) & 0xff;
  preamble[1]                     = (count >> 16) & 0xff;
  preamble[2]                     = (count >> 8) & 0xff;
  preamble[3]                     = count & 0xff;
  preamble[4]                     = (bearer_id << 3) | (to_number(direction) << 2);
  ret                             = mbedtls_cipher_cmac_update(&ctx, preamble.data(), preamble.size());
  if (ret != 0) {
    return make_unexpected(security_error::integrity_failure);
  }

  // process PDU segments
  const_byte_buffer_segment_span_range segments = v.segments();
  for (const auto& segment : segments) {
    ret = mbedtls_cipher_cmac_update(&ctx, segment.data(), segment.size());
    if (ret != 0) {
      return make_unexpected(security_error::integrity_failure);
    }
  }

  // complete CMAC computation
  std::array<uint8_t, 16> tmp_mac;
  ret = mbedtls_cipher_cmac_finish(&ctx, tmp_mac.data());
  if (ret != 0) {
    return make_unexpected(security_error::integrity_failure);
  }

  // copy first 4 bytes
  std::copy(tmp_mac.begin(), tmp_mac.begin() + 4, mac.begin());

  return mac;
}

security_result integrity_engine_nia2_cmac::protect_integrity(byte_buffer buf, uint32_t count)
{
  security_result  result{.buf = std::move(buf), .count = count};
  byte_buffer_view v{result.buf.value().begin(), result.buf.value().end()};

  expected<security::sec_mac, security_error> mac = compute_mac(v, count);

  logger.debug("Applying integrity protection. count={}", count);
  logger.debug(v.begin(), v.end(), "Message input:");

  if (not mac.has_value()) {
    result.buf = make_unexpected(mac.error());
    return result;
  }

  if (not result.buf->append(mac.value())) {
    result.buf = make_unexpected(security_error::buffer_failure);
  }

  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC: {}", mac.value());
  logger.debug(result.buf.value().begin(), result.buf.value().end(), "Message output:");

  return result;
}

security_result integrity_engine_nia2_cmac::verify_integrity(byte_buffer buf, uint32_t count)
{
  security_result result{.buf = std::move(buf), .count = count};

  if (result.buf->length() <= sec_mac_len) {
    result.buf = make_unexpected(security_error::integrity_failure);
    return result;
  }

  byte_buffer_view v{result.buf.value(), 0, result.buf.value().length() - sec_mac_len};
  byte_buffer_view m{result.buf.value(), result.buf.value().length() - sec_mac_len, sec_mac_len};

  // compute MAC
  expected<security::sec_mac, security_error> mac = compute_mac(v, count);

  if (not mac.has_value()) {
    result.buf = make_unexpected(mac.error());
    return result;
  }

  // verify MAC
  if (!std::equal(mac.value().begin(), mac.value().end(), m.begin(), m.end())) {
    security::sec_mac mac_rx;
    std::copy(m.begin(), m.end(), mac_rx.begin());
    result.buf = make_unexpected(security_error::integrity_failure);
    logger.warning("Integrity check failed. count={}", count);
    logger.warning("K_int: {}", k_128_int);
    logger.warning("MAC received: {}", mac_rx);
    logger.warning("MAC expected: {}", mac.value());
    logger.warning(v.begin(), v.end(), "Message input:");
    return result;
  }
  logger.debug("Integrity check passed. count={}", count);
  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC: {}", mac.value());
  logger.debug(v.begin(), v.end(), "Message input:");

  // trim MAC from PDU
  result.buf.value().trim_tail(sec_mac_len);

  return result;
}

#endif // MBEDTLS_CMAC_C
