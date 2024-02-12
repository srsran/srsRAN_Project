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

#include "pbch_encoder_impl.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

// Implements TS 38.212 Table 7.1.1-1: Value of PBCH payload interleaver pattern G (j).
static const std::array<uint32_t, pbch_encoder::A> G = {16, 23, 18, 17, 8,  30, 10, 6,  24, 7,  0,  5,  3,  2,  1,  4,
                                                        9,  11, 12, 13, 14, 15, 19, 20, 21, 22, 25, 26, 27, 28, 29, 31};

#define PBCH_SFN_PAYLOAD_BEGIN 1
#define PBCH_SFN_PAYLOAD_LENGTH 6
#define PBCH_SFN_2ND_LSB_G (G[PBCH_SFN_PAYLOAD_LENGTH + 2])
#define PBCH_SFN_3RD_LSB_G (G[PBCH_SFN_PAYLOAD_LENGTH + 1])

void pbch_encoder_impl::payload_generate(span<uint8_t> a, const srsran::pbch_encoder::pbch_msg_t& msg)
{
  srsran_assert(a.size() == A, "Input span size must equal A");

  // Extract actual payload size
  uint32_t A_hat = 24;

  // Put actual payload
  uint32_t j_sfn   = 0;
  uint32_t j_other = 14;
  for (uint32_t i = 0; i < A_hat; i++) {
    if (i >= PBCH_SFN_PAYLOAD_BEGIN && i < (PBCH_SFN_PAYLOAD_BEGIN + PBCH_SFN_PAYLOAD_LENGTH)) {
      a[G[j_sfn++]] = msg.payload[i];
    } else {
      a[G[j_other++]] = msg.payload[i];
    }
  }

  // Put SFN in a_hat[A_hat] to a_hat[A_hat + 3]
  a[G[j_sfn++]] = (uint8_t)((msg.sfn >> 3U) & 1U); // 4th LSB of SFN
  a[G[j_sfn++]] = (uint8_t)((msg.sfn >> 2U) & 1U); // 3th LSB of SFN
  a[G[j_sfn++]] = (uint8_t)((msg.sfn >> 1U) & 1U); // 2th LSB of SFN
  a[G[j_sfn++]] = (uint8_t)((msg.sfn >> 0U) & 1U); // 1th LSB of SFN

  // Put HRF in a_hat[A_hat + 4]
  a[G[10]] = (msg.hrf ? 1 : 0);

  // Put SSB related in a_hat[A_hat + 5] to a_hat[A_hat + 7]
  if (msg.L_max == 64) {
    a[G[11]] = (uint8_t)((msg.ssb_idx >> 5U) & 1U); // 6th bit of SSB index
    a[G[12]] = (uint8_t)((msg.ssb_idx >> 4U) & 1U); // 5th bit of SSB index
    a[G[13]] = (uint8_t)((msg.ssb_idx >> 3U) & 1U); // 4th bit of SSB index
  } else {
    a[G[11]] = (uint8_t)((msg.k_ssb.to_uint() >> 4U) & 1U); // 6th bit of SSB index
    a[G[12]] = 0;                                           // Reserved
    a[G[13]] = 0;                                           // Reserved
  }
}

void pbch_encoder_impl::scramble(span<uint8_t>                           a_prime,
                                 const srsran::pbch_encoder::pbch_msg_t& msg,
                                 span<const uint8_t>                     a)
{
  // Initialize sequence.
  scrambler->init(msg.N_id);

  // Select value M
  uint32_t M = A - 3;
  if (msg.L_max == 64) {
    M = A - 6;
  }

  // Select value v
  uint32_t v = 2 * a[PBCH_SFN_3RD_LSB_G] + a[PBCH_SFN_2ND_LSB_G];

  // Advance sequence
  scrambler->advance(M * v);

  // Generate actual sequence
  static_bit_buffer<PAYLOAD_SIZE> c(PAYLOAD_SIZE);
  scrambler->generate(c);

  for (unsigned i = 0, j = 0; i != PAYLOAD_SIZE; ++i) {
    uint8_t s_i = c.extract(j, 1);

    // Check if i belongs to a SS/PBCH block index which is only multiplexed when L_max is 64
    bool is_ssb_idx = (i == G[11] || i == G[12] || i == G[13]) && msg.L_max == 64;

    // a i corresponds to any one of the bits belonging to the SS/PBCH block index, the half frame index, and 2 nd and
    // 3 rd least significant bits of the system frame number
    if (is_ssb_idx || i == G[10] || i == PBCH_SFN_2ND_LSB_G || i == PBCH_SFN_3RD_LSB_G) {
      s_i = 0;
    } else {
      j++;
    }

    a_prime[i] = a[i] ^ s_i;
  }
}

void pbch_encoder_impl::crc_attach(span<uint8_t> b, span<const uint8_t> a_prime)
{
  srsran_assert(a_prime.size() == A, "Input span size must equal A");
  srsran_assert(b.size() == B, "Output span size must equal B");

  // Copy data if pointers do not match.
  if (b.data() != a_prime.data()) {
    srsvec::copy(b.first(A), a_prime);
  }

  // Calculate checksum.
  unsigned checksum = crc24c->calculate_bit(a_prime);

  // Unpack and attach checksum.
  srsvec::bit_unpack(b.last(CRC_LEN), checksum, CRC_LEN);
}

void pbch_encoder_impl::channel_coding(span<uint8_t> d, span<const uint8_t> c)
{
  srsran_assert(c.size() == B, "Input span size must equal B");
  srsran_assert(d.size() == POLAR_N_MAX, "Output span size must equal POLAR_N_MAX");

  // 5.3.1.1 Interleaving.
  std::array<uint8_t, B> c_prime;
  interleaver->interleave(c_prime, c, polar_interleaver_direction::tx);

  // Channel allocation.
  std::array<uint8_t, POLAR_N_MAX> allocated;
  alloc->allocate(allocated, c_prime, *code);

  // Polar encoding.
  encoder->encode(d, allocated, code->get_n());
}

void pbch_encoder_impl::rate_matching(span<uint8_t> f, span<const uint8_t> d)
{
  srsran_assert(d.size() == POLAR_N_MAX, "Input span size must equal POLAR_N_MAX");
  srsran_assert(f.size() == E, "Output span size must equal B");

  rm->rate_match(f, d, *code);
}

void pbch_encoder_impl::encode(span<uint8_t> encoded, const srsran::pbch_encoder::pbch_msg_t& pbch_msg)
{
  report_fatal_error_if_not(
      encoded.size() == E, "Invalid encoded size ({}), expected {}.", encoded.size(), static_cast<unsigned>(E));

  // PBCH payload generation.
  std::array<uint8_t, A> a;
  payload_generate(a, pbch_msg);

  // Scrambling.
  std::array<uint8_t, A> a_prime;
  scramble(a_prime, pbch_msg, a);

  // CRC Attach.
  std::array<uint8_t, B> k;
  crc_attach(k, a_prime);

  // Channel coding.
  std::array<uint8_t, POLAR_N_MAX> d;
  channel_coding(d, k);

  // Rate matching.
  rate_matching(encoded, d);
}
