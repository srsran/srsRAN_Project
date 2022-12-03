/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief AVX2 support for LDPC.

#pragma once

#include "srsgnb/support/srsgnb_assert.h"
#include <array>
#include <immintrin.h>

namespace srsgnb {
/// Number of bytes in an AVX2 register.
constexpr unsigned AVX2_SIZE_BYTE = 32;

namespace mm256 {
/// \brief Mimics an array of AVX2 registers.
/// \tparam nof_elements The number of AVX2 registers in the array.
template <size_t nof_elements>
class avx2_array
{
public:
  /// Returns a pointer to the \c pos AVX2 register inside the array.
  __m256i* data_at(unsigned pos)
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return reinterpret_cast<__m256i*>(inner_array.data() + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const __m256i* data_at(unsigned pos) const
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return reinterpret_cast<const __m256i*>(inner_array.data() + index);
  }

  /// Returns a pointer to the byte at position <tt>pos * AVX2_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * AVX2_SIZE_BYTE + byte;
    srsgnb_assert(index < nof_elements * AVX2_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (inner_array.data() + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * AVX2_SIZE_BYTE + byte;
    srsgnb_assert(index < nof_elements * AVX2_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (inner_array.data() + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // __m256i object by reference.

  /// Sets the \c pos AVX2 register to \c val.
  void set_at(unsigned pos, __m256i val)
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(inner_array.data() + index), val);
  }

  /// Gets the value stored in the \c pos AVX2 register.
  __m256i get_at(unsigned pos) const
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(inner_array.data() + index));
  }

private:
  /// Actual array where the AVX2 registers are stored.
  alignas(32) std::array<int8_t, nof_elements * AVX2_SIZE_BYTE> inner_array;
};

} // namespace mm256

/// \brief Rotates the contents of a node towards the left by \c steps chars, that is the \c steps * 8 least significant
/// bits become the most significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first AVX2 block of the output rotated node.
/// \param[in]  in        Pointer to the first AVX2 block of the input node to rotate.
/// \param[in]  steps     The order of the rotation as a number of chars.
/// \param[in]  ls        The size of the node (lifting size).
/// \remark Cannot be used to override memory.
inline void rotate_node_left(int8_t* out, const int8_t* in, unsigned steps, unsigned ls)
{
  srsgnb_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
  std::memcpy(out, in + ls - steps, steps);
  std::memcpy(out + steps, in, ls - steps);
}

/// \brief Rotates the contents of a node towards the right by \c steps chars, that is the \c steps * 8 most significant
/// bits become the least significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first AVX2 block of the output rotated node.
/// \param[in]  in        Pointer to the first AVX2 block of the input node to rotate.
/// \param[in]  steps     The order of the rotation as a number of chars.
/// \param[in]  ls        The size of the node (lifting size).
/// \remark Cannot be used to override memory.
inline void rotate_node_right(int8_t* out, const int8_t* in, unsigned steps, unsigned ls)
{
  srsgnb_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
  std::memcpy(out, in + steps, ls - steps);
  std::memcpy(out + ls - steps, in, steps);
}

} // namespace srsgnb
