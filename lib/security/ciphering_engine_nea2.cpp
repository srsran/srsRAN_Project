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

#include "ciphering_engine_nea2.h"

using namespace srsran;
using namespace security;

ciphering_engine_nea2::ciphering_engine_nea2(sec_128_key k_128_enc, uint8_t bearer_id_, security_direction direction_) :
  bearer_id(bearer_id_), direction(direction_)
{
  int ret = aes_setkey_enc(&ctx, k_128_enc.data(), 128);
  if (ret != 0) {
    srsran_assertion_failure("Failure in aes_setkey_enc");
    return;
  }
}

security_result ciphering_engine_nea2::apply_ciphering(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result  result{.buf = std::move(buf), .count = count};
  byte_buffer_view msg{result.buf.value().begin() + offset, result.buf.value().end()};

  unsigned char stream_blk[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char nonce_cnt[16]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  size_t        nc_off         = 0;

  // Construct nonce
  nonce_cnt[0] = (count >> 24) & 0xff;
  nonce_cnt[1] = (count >> 16) & 0xff;
  nonce_cnt[2] = (count >> 8) & 0xff;
  nonce_cnt[3] = count & 0xff;
  nonce_cnt[4] = ((bearer_id & 0x1f) << 3) | ((to_number(direction) & 0x01) << 2);

  // Encryption
  byte_buffer_segment_span_range segments = msg.modifiable_segments();
  for (const auto& segment : segments) {
    int ret = aes_crypt_ctr(&ctx, segment.size(), &nc_off, nonce_cnt, stream_blk, segment.data(), segment.data());
    if (ret != 0) {
      result.buf = make_unexpected(security_error::ciphering_failure);
      return result;
    }
  }

  return result;
}
