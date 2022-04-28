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

#include "srsgnb/srsvec/bit.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;
using namespace srsvec;

span<uint8_t> srsgnb::srsvec::bit_unpack(span<uint8_t> bits, unsigned value, unsigned nof_bits)
{
  assert(bits.size() >= nof_bits);

  for (unsigned i = 0; i < nof_bits; i++) {
    bits[i] = (value >> (nof_bits - i - 1)) & 0x1;
  }

  return bits.subspan(nof_bits, bits.size() - nof_bits);
}

void srsgnb::srsvec::bit_unpack(span<uint8_t> unpacked, span<const uint8_t> packed)
{
  unsigned nbits  = unpacked.size();
  unsigned nbytes = packed.size();
  unsigned i;

  srsran_assert(divide_ceil(nbits, 8) == nbytes, "Inconsistent input sizes");

  for (i = 0; i < nbytes; i++) {
    unpacked = bit_unpack(unpacked, packed[i], 8);
  }
  if (nbits % 8) {
    bit_unpack(unpacked, packed[i] >> (8 - nbits % 8), nbits % 8);
  }
}

unsigned srsgnb::srsvec::bit_pack(span<const uint8_t>& bits, unsigned nof_bits)
{
  unsigned value = 0;

  for (unsigned i = 0; i < nof_bits; i++) {
    value |= (unsigned)bits[i] << (nof_bits - i - 1U);
  }

  // Advance pointer
  bits = bits.last(bits.size() - nof_bits);

  return value;
}

void srsgnb::srsvec::bit_pack(span<uint8_t> packed, span<const uint8_t> unpacked)
{
  unsigned nbits  = unpacked.size();
  unsigned nbytes = packed.size();
  unsigned i;

  span<const uint8_t> unpack_tmp = unpacked;

  srsran_assert(divide_ceil(nbits, 8) == nbytes, "Inconsistent input sizes");

#ifdef HAVE_SSE
  for (i = 0; i < nbytes; i++) {
    // Get 8 Bit
    __m64 mask = _mm_cmpgt_pi8(*((__m64*)unpacked), _mm_set1_pi8(0));
    unpacked += 8;

    // Reverse
    mask = _mm_shuffle_pi8(mask, _mm_set_pi8(0, 1, 2, 3, 4, 5, 6, 7));

    // Get mask and write
    packed[i] = (uint8_t)_mm_movemask_pi8(mask);
  }
#else  /* HAVE_SSE */
  for (i = 0; i < nbits / 8; i++) {
    packed[i] = (uint8_t)bit_pack(unpack_tmp, 8);
  }
#endif /* HAVE_SSE */

  if (nbits % 8) {
    packed[i] = (uint8_t)bit_pack(unpack_tmp, nbits % 8);
    packed[i] <<= 8 - (nbits % 8);
  }
}
