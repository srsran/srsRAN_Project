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

#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/bit.h"
#include <random>

static std::mt19937 rgen(0);
static const float  assert_cf_max_error    = 1e-6;
static const float  assert_float_max_error = 1e-6;

using namespace srsgnb;

void test_unpack(unsigned N)
{
  std::uniform_int_distribution<unsigned> dist(0, (1U << N) - 1U);

  // Create random value to unpack
  unsigned value = dist(rgen);

  // Create destination
  srsvec::aligned_vec<uint8_t> unpacked(N);

  // Unpack
  bit_buffer bit_buf = unpacked;
  srsvec::bit_unpack(value, bit_buf, N);

  // Make sure the allocate dbvector size remains the same while all bits are taken in bit_buff
  assert(unpacked.size() == N);
  assert(bit_buf.size() == 0);

  // Assert each bit
  for (unsigned i = 0; i != N; i++) {
    uint8_t gold = (value >> (N - 1 - i)) & 1U;
    assert(gold == unpacked[i]);
  }
}

void test_unpack_vector(unsigned N)
{
  unsigned                                nbytes = N;
  unsigned                                nbits  = nbytes * 8;
  std::uniform_int_distribution<unsigned> dist(0, UINT8_MAX);

  // Create random value to unpack
  byte_buffer packed(nbytes);
  for (uint8_t& value : packed) {
    value = dist(rgen);
  }

  // Create destination
  srsvec::aligned_vec<uint8_t> unpacked(nbits);

  // Unpack
  srsvec::bit_unpack(packed, unpacked);

  // Assert each bit
  for (unsigned i = 0; i != nbits; i++) {
    unsigned byte_idx = i / 8;
    unsigned bit_idx  = i % 8;
    uint8_t  gold     = (packed[byte_idx] >> (7U - bit_idx)) & 1U;
    assert(gold == unpacked[i]);
  }
}

void test_pack(unsigned N)
{
  std::uniform_int_distribution<uint8_t> dist(0, 1U);

  // Create unpacked data
  srsvec::aligned_vec<uint8_t> unpacked(N);
  for (uint8_t& value : unpacked) {
    value = dist(rgen);
  }

  // Pack
  bit_buffer bit_buf = unpacked;
  unsigned   value   = srsvec::bit_pack(bit_buf, N);

  // Make sure the allocate dbvector size remains the same while all bits are taken in bit_buff
  assert(unpacked.size() == N);
  assert(bit_buf.size() == 0);

  // Assert each bit
  for (unsigned i = 0; i != N; i++) {
    uint8_t gold = (value >> (N - 1 - i)) & 1U;
    assert(gold == unpacked[i]);
  }
}

void test_pack_vector(unsigned N)
{
  unsigned                                nbytes = N;
  unsigned                                nbits  = nbytes * 8;
  std::uniform_int_distribution<unsigned> dist(0, 1U);

  // Create unpacked data
  srsvec::aligned_vec<uint8_t> unpacked(nbits);
  for (uint8_t& value : unpacked) {
    value = dist(rgen);
  }

  // Create destination
  byte_buffer packed(nbytes);

  // Unpack
  srsvec::bit_pack(unpacked, packed);

  // Assert each bit
  for (unsigned i = 0; i != nbits; i++) {
    unsigned byte_idx = i / 8;
    unsigned bit_idx  = i % 8;
    uint8_t  gold     = (packed[byte_idx] >> (7U - bit_idx)) & 1U;
    assert(gold == unpacked[i]);
  }
}

int main()
{
  std::vector<unsigned> sizes = {1, 5, 7, 19, 23, 32};

  for (unsigned N : sizes) {
    test_unpack(N);
    test_unpack_vector(N);
    test_pack(N);
    test_pack_vector(N);
  }
}