/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include <cstdint>
#include <type_traits>

namespace srsran {

/// \brief Returns an unsigned integer with the N most significant bits (MSB) set to zero, and the remaining bits set
/// to 1. e.g.
/// - mask_msb_zeros<uint8_t>(0) == 0xffU
/// - mask_msb_zeros<uint8_t>(1) == 0x7fU
/// - mask_msb_zeros<uint8_t>(2) == 0x3fU
/// - mask_msb_zeros<uint8_t>(8) == 0x00U
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of MSB bits set to zero.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_msb_zeros(size_t N)
{
  static_assert(std::is_unsigned_v<Integer>, "T must be unsigned integer");
  return (N == 0) ? static_cast<Integer>(-1) : ((N == sizeof(Integer) * 8U) ? 0 : (static_cast<Integer>(-1) >> (N)));
}

/// \brief Returns an unsigned integer with the N least significant bits (LSB) set to one, and the remaining bits set
/// to 1. e.g.
/// - mask_lsb_ones<uint8_t>(0) == 0x00U
/// - mask_lsb_ones<uint8_t>(1) == 0x01U
/// - mask_lsb_ones<uint8_t>(7) == 0x7fU
/// - mask_lsb_ones<uint8_t>(8) == 0xffU
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of LSB bits set to zero.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_lsb_ones(size_t N)
{
  return mask_msb_zeros<Integer>(sizeof(Integer) * 8U - N);
}

/// \brief Returns an unsigned integer with the N most significant bits (MSB) set to one, and the remaining bits set
/// to zero.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of MSB bits set to one.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_msb_ones(size_t N)
{
  return ~mask_msb_zeros<Integer>(N);
}

/// \brief Returns an unsigned integer with the N least significant bits (LSB) set to zero, and the remaining bits set
/// to one.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of LSB bits set to one.
/// \return Resulting integer bitmap.
template <typename Integer>
Integer mask_lsb_zeros(size_t N)
{
  return ~mask_lsb_ones<Integer>(N);
}

namespace detail {

template <typename Integer>
struct bitset_builtin_helper {
  /// \brief Returns the number of leading 0-bits in value, starting at the most significant bit position.
  /// If value is 0, the result is number of bits of Integer.
  static Integer zero_msb_count(Integer value)
  {
    if (value == 0) {
      return std::numeric_limits<Integer>::digits;
    }
    Integer ret = 0;
    for (Integer shift = std::numeric_limits<Integer>::digits >> 1; shift != 0; shift >>= 1) {
      Integer tmp = value >> shift;
      if (tmp != 0) {
        value = tmp;
      } else {
        ret |= shift;
      }
    }
    return ret;
  }
  /// \brief Returns the number of trailing 0-bits in value, starting at the least significant bit position.
  /// If value is 0, the result is number of bits of Integer.
  static Integer zero_lsb_count(Integer value)
  {
    if (value == 0) {
      return std::numeric_limits<Integer>::digits;
    }
    Integer ret = 0;
    for (Integer shift = std::numeric_limits<Integer>::digits >> 1, mask = std::numeric_limits<Integer>::max() >> shift;
         shift != 0;
         shift >>= 1, mask >>= shift) {
      if ((value & mask) == 0) {
        value >>= shift;
        ret |= shift;
      }
    }
    return ret;
  }

  /// \brief Returns the number of 1-bits in value.
  static int count_ones(Integer value)
  {
    // Note: use an "int" for count triggers popcount optimization if SSE instructions are enabled.
    int c = 0;
    for (Integer w = value; w > 0; c++) {
      w &= w - 1;
    }
    return c;
  }
};

#ifdef __GNUC__ // clang and gcc

/// Specializations of bitset operations for unsigned.
template <>
struct bitset_builtin_helper<unsigned> {
  static unsigned zero_msb_count(unsigned value)
  {
    return (value) ? __builtin_clz(value) : std::numeric_limits<unsigned>::digits;
  }
  static unsigned zero_lsb_count(unsigned value)
  {
    return (value) ? __builtin_ctz(value) : std::numeric_limits<unsigned>::digits;
  }
  static int count_ones(unsigned value) { return __builtin_popcount(value); }
};

/// Specializations of bitset operations for unsigned long.
template <>
struct bitset_builtin_helper<unsigned long> {
  static unsigned long zero_msb_count(unsigned long value)
  {
    return (value) ? __builtin_clzl(value) : std::numeric_limits<unsigned long>::digits;
  }
  static unsigned long zero_lsb_count(unsigned long value)
  {
    return (value) ? __builtin_ctzl(value) : std::numeric_limits<unsigned long>::digits;
  }
  static int count_ones(unsigned long value) { return __builtin_popcountl(value); }
};

/// Specializations of bitset operations for unsigned long long.
template <>
struct bitset_builtin_helper<unsigned long long> {
  static unsigned long long zero_msb_count(unsigned long long value)
  {
    return (value) ? __builtin_clzll(value) : std::numeric_limits<unsigned long long>::digits;
  }
  static unsigned long long zero_lsb_count(unsigned long long value)
  {
    return (value) ? __builtin_ctzll(value) : std::numeric_limits<unsigned long long>::digits;
  }
  static int count_ones(unsigned long long value) { return __builtin_popcountll(value); }
};

#endif

/// \brief Knuth's swap of upper and lower sections of a bitset.
/// \param m Mask of bits to swap.
/// \param k shift amount.
template <typename T, T m, int k>
T swapbits(T p)
{
  T q = ((p >> k) ^ p) & m;
  return p ^ q ^ (q << k);
}

} // namespace detail

/// \brief Knuth's 64-bit reverse. E.g. 0x0000000000000001 -> 0x8000000000000000.
/// For more information see: https://matthewarcus.wordpress.com/2012/11/18/reversing-a-64-bit-word/
/// \param n Number to reverse.
/// \return Reversed number.
inline uint64_t bit_reverse(uint64_t n)
{
  static const uint64_t m0 = 0x5555555555555555LLU;
  static const uint64_t m1 = 0x0300c0303030c303LLU;
  static const uint64_t m2 = 0x00c0300c03f0003fLLU;
  static const uint64_t m3 = 0x00000ffc00003fffLLU;
  n                        = ((n >> 1U) & m0) | (n & m0) << 1U;
  n                        = detail::swapbits<uint64_t, m1, 4>(n);
  n                        = detail::swapbits<uint64_t, m2, 8>(n);
  n                        = detail::swapbits<uint64_t, m3, 20>(n);
  n                        = (n >> 34U) | (n << 30U);
  return n;
}

/// \brief Counts the number of contiguous bits set to zero, starting from the MSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return count of bits set to zero.
template <typename Integer>
Integer zero_msb_count(Integer value)
{
  return (~value) ? detail::bitset_builtin_helper<Integer>::zero_msb_count(value) : 0;
}

/// \brief Finds the position of the first bit set to one, starting from the MSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return MSB position with the bit set to one. The MSB has position sizeof(Integer) * 8 - 1.
template <typename Integer>
Integer find_first_msb_one(Integer value)
{
  return (value) ? (sizeof(Integer) * 8U - 1 - detail::bitset_builtin_helper<Integer>::zero_msb_count(value))
                 : std::numeric_limits<Integer>::digits;
}

/// \brief Finds the position of the first bit set to one, starting from the LSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return LSB position with the bit set to one. The LSB has position zero.
template <typename Integer>
Integer find_first_lsb_one(Integer value)
{
  return detail::bitset_builtin_helper<Integer>::zero_lsb_count(value);
}

/// \brief Counts the number of bits set to one in an integer.
template <typename Integer>
int count_ones(Integer value)
{
  return detail::bitset_builtin_helper<Integer>::count_ones(value);
}

} // namespace srsran
