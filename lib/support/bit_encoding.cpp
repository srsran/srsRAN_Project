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
#include "srsgnb/support/math_utils.h"

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool bit_decoder::advance_bits(uint32_t n_bits)
{
  uint32_t extra_bits     = (offset + n_bits) % 8U;
  uint32_t bytes_required = divide_ceil(offset + n_bits, 8U);
  uint32_t bytes_offset   = (offset + n_bits) / 8U;

  if (bytes_required > buffer.end() - it) {
    return false;
  }
  it += bytes_offset;
  offset = extra_bits;
  return true;
}

template <class T>
bool bit_decoder::unpack(T& val, uint32_t n_bits)
{
  srsran_assert(n_bits <= sizeof(T) * 8U, "unpack_bits() only supports up to {} bits", sizeof(T) * 8U);
  val = 0;
  while (n_bits > 0U) {
    if (it == buffer.end()) {
      return false;
    }
    if ((uint32_t)(8U - offset) > n_bits) {
      uint8_t mask = (uint8_t)(1u << (8u - offset)) - (uint8_t)(1u << (8u - offset - n_bits));
      val += ((uint32_t)((*it) & mask)) >> (8u - offset - n_bits);
      offset += n_bits;
      n_bits = 0;
    } else {
      auto mask = static_cast<uint8_t>((1u << (8u - offset)) - 1u);
      val += ((uint32_t)((*it) & mask)) << (n_bits - 8 + offset);
      n_bits -= 8 - offset;
      offset = 0;
      ++it;
    }
  }
  return true;
}
template bool bit_decoder::unpack<bool>(bool&, uint32_t n_bits);
template bool bit_decoder::unpack<int8_t>(int8_t&, uint32_t n_bits);
template bool bit_decoder::unpack<uint8_t>(uint8_t&, uint32_t n_bits);
template bool bit_decoder::unpack<int16_t>(int16_t&, uint32_t n_bits);
template bool bit_decoder::unpack<uint16_t>(uint16_t&, uint32_t n_bits);
template bool bit_decoder::unpack<int32_t>(int32_t&, uint32_t n_bits);
template bool bit_decoder::unpack<uint32_t>(uint32_t&, uint32_t n_bits);
template bool bit_decoder::unpack<int64_t>(int64_t&, uint32_t n_bits);
template bool bit_decoder::unpack<uint64_t>(uint64_t&, uint32_t n_bits);

bool bit_decoder::unpack_bytes(srsgnb::span<uint8_t> bytes)
{
  if (bytes.empty()) {
    return true;
  }
  if (static_cast<std::ptrdiff_t>(bytes.size()) > buffer.end() - it) {
    return false;
  }
  if (offset == 0) {
    // Aligned case
    std::copy(buffer.begin(), buffer.end(), bytes.begin());
    it += bytes.size();
  } else {
    // Unaligned case
    for (uint32_t i = 0; i < bytes.size(); ++i) {
      if (not unpack(bytes[i], 8)) {
        return false;
      }
    }
  }
  return true;
}

void bit_decoder::align_bytes()
{
  if (offset != 0) {
    srsran_sanity_check(it != buffer.end(), "Invalid bit_decoder state");
    ++it;
    offset = 0;
  }
}
