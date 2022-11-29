/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsgnb_assert.h"

#ifdef HAVE_SSE
#include <immintrin.h>
#endif // HAVE_SSE

using namespace srsgnb;
using namespace srsvec;

namespace {

template <typename RetType = uint8_t>
RetType pack_8bit(span<const uint8_t> unpacked)
{
  srsgnb_assert(unpacked.size() == 8, "The amount of data to pack (i.e., {}) must be eight.", unpacked.size());
#if HAVE_SSE
  __m64 mask = _mm_cmpgt_pi8(*(reinterpret_cast<const __m64*>(unpacked.data())), _mm_set1_pi8(0));

  // Reverse
  mask = _mm_shuffle_pi8(mask, _mm_set_pi8(0, 1, 2, 3, 4, 5, 6, 7));

  // Get mask and write
  return static_cast<RetType>(_mm_movemask_pi8(mask));
#else
  RetType packed = 0;

  for (unsigned i = 0; i < unpacked.size(); i++) {
    packed |= static_cast<RetType>(unpacked[i] << (8 - i - 1U));
  }

  return packed;
#endif
}

} // namespace

span<uint8_t> srsgnb::srsvec::bit_unpack(span<uint8_t> bits, unsigned value, unsigned nof_bits)
{
  assert(bits.size() >= nof_bits);

  for (unsigned i = 0; i < nof_bits; i++) {
    bits[i] = (value >> (nof_bits - i - 1)) & 0x1;
  }

  return bits.last(bits.size() - nof_bits);
}

void srsgnb::srsvec::bit_unpack(span<uint8_t> unpacked, span<const uint8_t> packed)
{
  unsigned nbits  = unpacked.size();
  unsigned nbytes = packed.size();
  unsigned i;

  srsgnb_assert(divide_ceil(nbits, 8) == nbytes, "Inconsistent input sizes");

  for (i = 0; i < nbytes; i++) {
    unpacked = bit_unpack(unpacked, packed[i], 8);
  }
  if (nbits % 8) {
    bit_unpack(unpacked, packed[i] >> (8 - nbits % 8), nbits % 8);
  }
}

unsigned srsgnb::srsvec::bit_pack(span<const uint8_t>& bits, unsigned nof_bits)
{
  srsgnb_assert(nof_bits <= 32U, "Number of bits ({}) exceeds maximum (32).", nof_bits);

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
  srsgnb_assert(divide_ceil(unpacked.size(), 8) == packed.size(), "Inconsistent input sizes.");

  for (uint8_t& byte : packed) {
    byte     = pack_8bit(unpacked.first(8));
    unpacked = unpacked.last(unpacked.size() - 8);
  }
}

void srsgnb::srsvec::bit_pack(bit_buffer& packed, span<const uint8_t> unpacked)
{
  srsgnb_assert(packed.size() == unpacked.size(),
                "The packed nuber of bits (i.e.{}) must be equal to the number of unpacked bits (i.e., {})\n",
                packed.size(),
                unpacked.size());
  unsigned bit_offset = 0;
  for (unsigned i_byte = 0, i_byte_end = unpacked.size() / 8; i_byte != i_byte_end; ++i_byte, bit_offset += 8) {
    unsigned byte = pack_8bit<unsigned>(unpacked.first(8));
    unpacked      = unpacked.last(unpacked.size() - 8);
    packed.insert(byte, bit_offset, 8);
  }

  if (!unpacked.empty()) {
    std::array<uint8_t, 8> temp_unpacked;
    span<uint8_t>          unpacked2 = temp_unpacked;
    srsvec::copy(unpacked2.first(unpacked.size()), unpacked);
    srsvec::zero(unpacked2.last(8 - unpacked.size()));

    unsigned byte = pack_8bit<unsigned>(unpacked2) >> (8 - unpacked.size());
    packed.insert(byte, bit_offset, unpacked.size());
  }
}