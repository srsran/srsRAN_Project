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

#include "ciphering_engine_nea1.h"
#include "srsran/security/ciphering.h"

using namespace srsran;
using namespace security;

ciphering_engine_nea1::ciphering_engine_nea1(sec_128_key        k_128_enc_,
                                             uint8_t            bearer_id_,
                                             security_direction direction_) :
  k_128_enc(k_128_enc_), bearer_id(bearer_id_), direction(direction_)
{
}

security_result ciphering_engine_nea1::apply_ciphering(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result  result{.buf = std::move(buf), .count = count};
  byte_buffer_view msg{result.buf.value().begin() + offset, result.buf.value().end()};

  security_nea1(k_128_enc, count, bearer_id, direction, msg);

  return result;
}
