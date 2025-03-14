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

#include "integrity_engine_generic.h"
#include "srsran/security/integrity.h"
#include "srsran/security/security.h"

using namespace srsran;
using namespace security;

integrity_engine_generic::integrity_engine_generic(sec_128_key         k_128_int_,
                                                   uint8_t             bearer_id_,
                                                   security_direction  direction_,
                                                   integrity_algorithm integ_algo_) :
  k_128_int(k_128_int_),
  bearer_id(bearer_id_),
  direction(direction_),
  integ_algo(integ_algo_),
  logger(srslog::fetch_basic_logger("SEC"))
{
}

security_result integrity_engine_generic::protect_integrity(byte_buffer buf, uint32_t count)
{
  security_result   result{.buf = std::move(buf), .count = count};
  security::sec_mac mac = {};

  byte_buffer_view v{result.buf.value().begin(), result.buf.value().end()};

  logger.debug("Applying integrity protection. count={}", count);
  logger.debug(v.begin(), v.end(), "Message input:");

  switch (integ_algo) {
    case security::integrity_algorithm::nia0:
      // TS 33.501, Sec. D.1
      // The NIA0 algorithm shall be implemented in such way that it shall generate a 32 bit MAC-I/NAS-MAC and
      // XMAC-I/XNAS-MAC of all zeroes (see sub-clause D.3.1).
      std::fill(mac.begin(), mac.end(), 0);
      break;
    case security::integrity_algorithm::nia1:
      security_nia1(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia2:
      security_nia2(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia3:
      security_nia3(mac, k_128_int, count, bearer_id, direction, v);
      break;
    default:
      break;
  }

  if (not result.buf->append(mac)) {
    result.buf = make_unexpected(security_error::buffer_failure);
  }

  logger.debug("Integrity protection applied. count={}", count);
  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC: {}", mac);
  logger.debug(result.buf.value().begin(), result.buf.value().end(), "Message output:");

  return result;
}

security_result integrity_engine_generic::verify_integrity(byte_buffer buf, uint32_t count)
{
  security_result   result{.buf = std::move(buf), .count = count};
  security::sec_mac mac = {};

  if (result.buf->length() <= mac.size()) {
    result.buf = make_unexpected(security_error::integrity_failure);
    return result;
  }

  byte_buffer_view v{result.buf.value(), 0, result.buf.value().length() - mac.size()};
  byte_buffer_view m{result.buf.value(), result.buf.value().length() - mac.size(), mac.size()};

  switch (integ_algo) {
    case security::integrity_algorithm::nia0:
      // TS 33.501, Sec. D.1
      // The NIA0 algorithm shall be implemented in such way that it shall generate a 32 bit MAC-I/NAS-MAC and
      // XMAC-I/XNAS-MAC of all zeroes (see sub-clause D.3.1).
      std::fill(mac.begin(), mac.end(), 0);
      break;
    case security::integrity_algorithm::nia1:
      security_nia1(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia2:
      security_nia2(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia3:
      security_nia3(mac, k_128_int, count, bearer_id, direction, v);
      break;
    default:
      break;
  }

  // Verify MAC
  if (!std::equal(mac.begin(), mac.end(), m.begin(), m.end())) {
    result.buf = make_unexpected(security_error::integrity_failure);
    security::sec_mac mac_rx;
    std::copy(m.begin(), m.end(), mac_rx.begin());
    span m_rx{mac.data(), sec_mac_len};
    logger.warning("Integrity check failed. count={}", count);
    logger.warning("K_int: {}", k_128_int);
    logger.warning("MAC received: {}", mac_rx);
    logger.warning("MAC expected: {}", mac);
    logger.warning(v.begin(), v.end(), "Message input:");
    return result;
  }
  logger.debug("Integrity check passed. count={}", count);
  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC: {}", mac);
  logger.debug(v.begin(), v.end(), "Message input:");

  // Trim MAC from PDU
  result.buf.value().trim_tail(mac.size());

  return result;
}
