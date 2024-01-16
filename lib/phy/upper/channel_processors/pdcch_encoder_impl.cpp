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

#include "pdcch_encoder_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/srsvec/binary.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace pdcch_constants;

void pdcch_encoder_impl::crc_attach(span<uint8_t>& c, span<const uint8_t> a, unsigned rnti)
{
  std::array<uint8_t, RNTI_LEN> unpacked_rnti;

  // Unpack RNTI bits
  srsvec::bit_unpack(unpacked_rnti, rnti, RNTI_LEN);

  // Set first L bits to 1s
  std::fill(c.begin(), c.begin() + CRC_LEN, 1U);

  // Copy original payload to an offset of 24 bits
  srsvec::copy(c.subspan(CRC_LEN, a.size()), a);

  // Calculate checksum
  span<uint8_t> a_prime  = c.first(CRC_LEN + a.size());
  unsigned      checksum = crc24c->calculate_bit(a_prime);

  // Unpack and attach checksum
  srsvec::bit_unpack(c.last(CRC_LEN), checksum, CRC_LEN);

  // Scramble CRC parity bits with RNTI
  span<uint8_t> p = c.last(RNTI_LEN);
  srsvec::binary_xor(unpacked_rnti, p, p);

  // Skip first L 1s added for CRC calculation
  c = c.last(c.size() - CRC_LEN);
}

void pdcch_encoder_impl::channel_coding(span<uint8_t> d, span<const uint8_t> c)
{
  // 5.3.1.1 Interleaving
  static_vector<uint8_t, MAX_K> c_prime(c.size());
  interleaver->interleave(c_prime, c, polar_interleaver_direction::tx);

  // Channel allocation
  static_vector<uint8_t, polar_code::NMAX> allocated(code->get_N());
  alloc->allocate(allocated, c_prime, *code);

  // Polar encoding
  encoder->encode(d, allocated, code->get_n());
}

void pdcch_encoder_impl::rate_matching(span<uint8_t> f, span<const uint8_t> d)
{
  rm->rate_match(f, d, *code);
}

void pdcch_encoder_impl::encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config)
{
  srsran_assert(encoded.size() >= config.E, "Output data vector is too small to store encoded bits");

  // Configure Polar encoder
  uint16_t K = data.size() + CRC_LEN;
  code->set(K, config.E, polar_code::NMAX_LOG - 1, polar_code_ibil::not_present);

  // Attach CRC. Allocate extra L bits for leading 1s used in CRC calculation according to TS38.312 Section 7.3.2.
  span<uint8_t> c = span<uint8_t>(temp_c).first(K + CRC_LEN);
  crc_attach(c, data, config.rnti);

  // Encode
  span<uint8_t> d = span<uint8_t>(temp_d).first(code->get_N());
  channel_coding(d, c);

  // Rate match
  rate_matching(encoded, d);
}
