#include <pbch_encoder.h>
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

#include "pbch_encoder_impl.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/copy.h"

using namespace srsgnb;

pbch_encoder_impl::pbch_encoder_impl(args_t& args) : crc24c(std::move(args.crc24c)) {}

/*
 *  Implements TS 38.212 Table 7.1.1-1: Value of PBCH payload interleaver pattern G ( j )
 */
static const uint32_t G[32] = {16, 23, 18, 17, 8,  30, 10, 6,  24, 7,  0,  5,  3,  2,  1,  4,
                               9,  11, 12, 13, 14, 15, 19, 20, 21, 22, 25, 26, 27, 28, 29, 31};

#define PBCH_SFN_PAYLOAD_BEGIN 1
#define PBCH_SFN_PAYLOAD_LENGTH 6
#define PBCH_SFN_2ND_LSB_G (G[PBCH_SFN_PAYLOAD_LENGTH + 2])
#define PBCH_SFN_3RD_LSB_G (G[PBCH_SFN_PAYLOAD_LENGTH + 1])

void pbch_encoder_impl::payload_generate(const srsgnb::pbch_encoder::pbch_msg_t& msg, std::array<uint8_t, A>& a)
{
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
  a[G[j_sfn++]] = (uint8_t)((msg.sfn_4lsb >> 3U) & 1U); // 4th LSB of SFN
  a[G[j_sfn++]] = (uint8_t)((msg.sfn_4lsb >> 2U) & 1U); // 3th LSB of SFN
  a[G[j_sfn++]] = (uint8_t)((msg.sfn_4lsb >> 1U) & 1U); // 2th LSB of SFN
  a[G[j_sfn++]] = (uint8_t)((msg.sfn_4lsb >> 0U) & 1U); // 1th LSB of SFN

  // Put HRF in a_hat[A_hat + 4]
  a[G[10]] = (msg.hrf ? 1 : 0);

  // Put SSB related in a_hat[A_hat + 5] to a_hat[A_hat + 7]
  if (msg.Lmax == 64) {
    a[G[11]] = (uint8_t)((msg.ssb_idx >> 5U) & 1U); // 6th bit of SSB index
    a[G[12]] = (uint8_t)((msg.ssb_idx >> 4U) & 1U); // 5th bit of SSB index
    a[G[13]] = (uint8_t)((msg.ssb_idx >> 3U) & 1U); // 4th bit of SSB index
  } else {
    a[G[11]] = (uint8_t)msg.k_ssb_msb; // 6th bit of SSB index
    a[G[12]] = 0;                      // Reserved
    a[G[13]] = 0;                      // Reserved
  }
}

void pbch_encoder_impl::scramble(const srsgnb::pbch_encoder::pbch_msg_t& msg,
                                 const std::array<uint8_t, 32>&          a,
                                 std::array<uint8_t, 32>&                a_prime)
{
  uint32_t i = 0;
  uint32_t j = 0;

  // Initialise sequence
  scrambler->init(msg.N_id);

  // Select value M
  uint32_t M = A - 3;
  if (msg.Lmax == 64) {
    M = A - 6;
  }

  // Select value v
  uint32_t v = 2 * a[PBCH_SFN_3RD_LSB_G] + a[PBCH_SFN_2ND_LSB_G];

  // Advance sequence
  scrambler->advance(M * v);

  // Generate actual sequence
  std::array<uint8_t, A> c = {};
  scrambler->apply_xor(c, c);

  while (i < A) {
    uint8_t s_i = c[j];

    // Check if i belongs to a SS/PBCH block index which is only multiplexed when L_max is 64
    bool is_ssb_idx = (i == G[11] || i == G[12] || i == G[13]) && msg.Lmax == 64;

    // a i corresponds to any one of the bits belonging to the SS/PBCH block index, the half frame index, and 2 nd and
    // 3 rd least significant bits of the system frame number
    if (is_ssb_idx || i == G[10] || i == PBCH_SFN_2ND_LSB_G || i == PBCH_SFN_3RD_LSB_G) {
      s_i = 0;
    } else {
      j++;
    }

    a_prime[i] = a[i] ^ s_i;
    i++;
  }
}

void pbch_encoder_impl::crc_attach(std::array<uint8_t, A>& a_prime, std::array<uint8_t, B>& b)
{
  // Convert arrays to span
  span<uint8_t> a_prime_span = {a_prime};
  span<uint8_t> b_span       = {b};

  // Copy data if pointers do not match
  if (a_prime.data() != b.data()) {
    srsvec::copy(b_span.subspan(0, A), a_prime_span);
  }

  // Calculate checksum
  unsigned checksum = crc24c->calculate_bit(a_prime_span);

  // unpack and attach checksum
  span<uint8_t> p = b_span.last(CRC_LEN);
  srsvec::bit_unpack(checksum, p, CRC_LEN);
}

void pbch_encoder_impl::channel_coding(std::array<uint8_t, K> c, std::array<uint8_t, N> d)
{
  // 5.3.1.1 Interleaving
  std::array<uint8_t, K> c_prime;
  interleaver->interleave(c, c_prime, polar_interleaver_direction::tx);

  // Channel allocation
  std::array<uint8_t, POLAR_N> allocated;
  alloc->allocate(c_prime, allocated, *code);

  // Polar encoding
  std::array<uint8_t, POLAR_N> encoded;
  encoder->encode(allocated, code->get_n(), encoded);
}

void pbch_encoder_impl::encode(const srsgnb::pbch_encoder::pbch_msg_t& pbch_msg, std::array<uint8_t, 864>& encoded)
{
  // PBCH payload generation
  std::array<uint8_t, A> a = {};
  payload_generate(pbch_msg, a);

  // Scrambling
  std::array<uint8_t, A> a_prime = {};
  scramble(pbch_msg, a, a_prime);

  // CRC Attach
  std::array<uint8_t, K> k = {};
  crc_attach(a_prime, k);
}
