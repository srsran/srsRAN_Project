/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ciphering_engine_nea3.h"
#include "srsran/security/ciphering.h"

using namespace srsran;
using namespace security;

ciphering_engine_nea3::ciphering_engine_nea3(sec_128_key        k_128_enc_,
                                             uint8_t            bearer_id_,
                                             security_direction direction_) :
  k_128_enc(k_128_enc_), bearer_id(bearer_id_), direction(direction_)
{
}

security_result ciphering_engine_nea3::apply_ciphering(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result  result{.buf = std::move(buf), .count = count};
  byte_buffer_view msg{result.buf.value().begin() + offset, result.buf.value().end()};

  security_nea3(k_128_enc, count, bearer_id, direction, msg);

  return result;
}
