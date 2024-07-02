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

#include "ciphering_engine_generic.h"
#include "srsran/security/ciphering.h"
#include "srsran/security/security.h"

using namespace srsran;
using namespace security;

ciphering_engine_generic::ciphering_engine_generic(sec_128_key         k_128_int_,
                                                   uint8_t             bearer_id_,
                                                   security_direction  direction_,
                                                   ciphering_algorithm cipher_algo_) :
  k_128_enc(k_128_int_), bearer_id(bearer_id_), direction(direction_), cipher_algo(cipher_algo_)
{
}

security_result ciphering_engine_generic::apply_ciphering(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result result{.buf = std::move(buf), .count = count};

  byte_buffer_view v{result.buf.value().begin() + offset, result.buf.value().end()};

  switch (cipher_algo) {
    case security::ciphering_algorithm::nea1:
      security_nea1(k_128_enc, count, bearer_id, direction, v);
      break;
    case security::ciphering_algorithm::nea2:
      security_nea2(k_128_enc, count, bearer_id, direction, v);
      break;
    case security::ciphering_algorithm::nea3:
      security_nea3(k_128_enc, count, bearer_id, direction, v);
      break;
    default:
      break;
  }

  return result;
}
