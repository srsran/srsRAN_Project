/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/bit_encoding.h"
#include "srsgnb/srslog/bundled/fmt/ostream.h"

using namespace srsgnb;

void bit_encoder::pack(uint64_t val, uint32_t n_bits)
{
  srsran_assert(n_bits < 64U, "Invalid number of bits={} passed to pack()", n_bits);
  while (n_bits > 0U) {
    if (offset == 0U) {
      writer.append(0U);
    }
    uint64_t mask = (1UL << n_bits) - 1UL; // bitmap of n_bits ones.
    val           = val & mask;
    if (static_cast<uint32_t>(8U - offset) > n_bits) {
      // in case n_bits < number of bits left in current byte (ie, last iteration).
      auto shifted_val = static_cast<uint8_t>(val << (8U - offset - n_bits));
      writer.back() += shifted_val;
      offset += n_bits;
      n_bits = 0U;
    } else {
      // in case, space in current byte is lower or equal to n_bits (ie, not the last iteration).
      auto shifted_val = static_cast<uint8_t>(val >> (n_bits - 8U + offset));
      writer.back() += shifted_val;
      n_bits -= 8U - offset;
      offset = 0U;
    }
  }
}

void bit_encoder::pack_bytes(srsgnb::span<const uint8_t> bytes)
{
  if (bytes.empty()) {
    return;
  }
  if (offset == 0) {
    // Aligned case
    writer.append(bytes);
  } else {
    for (uint8_t byte : bytes) {
      pack(byte, 8U);
    }
  }
}

void bit_encoder::pack_bytes(srsgnb::byte_buffer_view bytes)
{
  if (bytes.empty()) {
    return;
  }
  if (offset == 0) {
    // Aligned case.
    writer.append(bytes);
  } else {
    for (uint8_t byte : bytes) {
      pack(byte, 8U);
    }
  }
}

void bit_encoder::align_bytes_zero()
{
  offset = 0;
}
