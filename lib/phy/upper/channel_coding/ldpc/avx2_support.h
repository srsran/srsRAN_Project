/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief AVX2 support for LDPC.
///
/// Defines useful templates and functions to work with AVX (both AVX2 and AVX512) registers and the template
/// specializations for the AVX2 case.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <array>
#include <immintrin.h>

namespace srsran {
/// Number of bytes in an AVX2 register.
constexpr unsigned AVX2_SIZE_BYTE = 32;
/// Number of bytes in an AVX512 register.
constexpr unsigned AVX512_SIZE_BYTE = 64;

namespace detail {

// These wrappers are needed to avoid attribute warnings about the AVX vector types.
struct m256_wrapper {
  using avxType = __m256i;
};

struct m512_wrapper {
  using avxType = __m512i;
};

/// \brief Mimics a span of AVX registers.
/// \tparam avxWrapper   Wrapper for an AVX vector type.
template <typename avxWrapper>
class avx_span
{
  /// Helper type for method overloading.
  template <typename HelpType>
  struct help_type {
    // Intentionally empty.
  };

public:
  using avxType                         = typename avxWrapper::avxType;
  static constexpr size_t AVX_SIZE_BYTE = sizeof(avxType);

  /// Default constructor: empty view to a null pointer.
  avx_span() : array_ptr(nullptr), view_length(0) {}

  /// \brief Constructs a span from a standard array.
  ///
  /// \tparam IntType    Type of the standard array - must be an integer type of 8 bits.
  /// \tparam N          Number of elements of the standard array.
  /// \param[in] arr     Array the span is a view of.
  /// \param[in] offset  Offset from the beginning of the array (as a number of AVX registers).
  /// \param[in] length  Number of elements spanned by the view (as a number of AVX registers).
  template <typename IntType, size_t N>
  avx_span(std::array<IntType, N>& arr, size_t offset, size_t length) :
    array_ptr(reinterpret_cast<int8_t*>(arr.data()) + offset * AVX_SIZE_BYTE), view_length(length)
  {
    static_assert(sizeof(IntType) == sizeof(int8_t),
                  "avx_span can only be created from arrays of 1-byte integer types.");
    srsran_assert((offset + view_length) * AVX_SIZE_BYTE <= N, "Cannot take a span longer than the array.");
  }

  /// \brief Constructs a span from a standard array.
  ///
  /// \tparam IntType    Type of the standard array - must be an integer type of 8 bits.
  /// \tparam N          Number of elements of the standard array.
  /// \param[in] arr     Array the span is a view of.
  /// \param[in] length  Number of elements spanned by the view (as a number of AVX registers).
  template <typename IntType, size_t N>
  avx_span(std::array<IntType, N>& arr, size_t length) : avx_span(arr, 0, length)
  {
  }

  /// \brief Constructs an \c avx_span from a \c srsran::span.
  template <typename IntType>
  avx_span(span<IntType> sp, size_t length) : array_ptr(reinterpret_cast<int8_t*>(sp.data())), view_length(length)
  {
    static_assert(sizeof(IntType) == sizeof(int8_t),
                  "avx_span can only be created from arrays of 1-byte integer types.");
    static_assert(!std::is_const<IntType>::value, "Cannot create avx_span from span of const.");
    srsran_assert(view_length * AVX_SIZE_BYTE <= sp.size(), "Cannot create an avx_span longer than the original span.");
  }

  /// Returns a pointer to the \c pos AVX register inside the array.
  avxType* data_at(unsigned pos)
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<avxType*>(array_ptr) + pos;
  }

  /// Returns a read-only pointer to the \c pos AVX register inside the array.
  const avxType* data_at(unsigned pos) const
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<const avxType*>(array_ptr) + pos;
  }

  /// Returns a pointer to the byte at position <tt>pos * AVX_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * AVX_SIZE_BYTE + byte;
    srsran_assert(index < view_length * AVX_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  /// Returns a read-only pointer to the \c pos AVX register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * AVX_SIZE_BYTE + byte;
    srsran_assert(index < view_length * AVX_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // avxType object by reference.

  /// Sets the \c pos AVX2 register to \c val.
  void set_at(unsigned pos, __m256i val)
  {
    static_assert(AVX_SIZE_BYTE == AVX2_SIZE_BYTE, "Cannot set an AVX512 vector with an AVX2 vector.");
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(array_ptr) + pos, val);
  }

  /// Sets the \c pos AVX register to \c val.
  void set_at(unsigned pos, __m512i val)
  {
    static_assert(AVX_SIZE_BYTE == AVX512_SIZE_BYTE, "Cannot set an AVX2 vector with an AVX512 vector.");
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(array_ptr) + pos, val);
  }

  /// Gets the value stored in the \c pos AVX register.
  avxType get_at(unsigned pos) const { return get_at(help_type<avxWrapper>(), pos); }

  /// Returns the number of AVX registers viewed by the span.
  size_t size() const { return view_length; }

  avx_span first(size_t length)
  {
    srsran_assert(length <= view_length, "Trying to create a subspan longer than the span itself.");
    return {array_ptr, length};
  }

  avx_span subspan(size_t offset, size_t length)
  {
    srsran_assert(offset + length <= view_length, "Trying to create a subspan longer than the span itself.");
    return {array_ptr + offset * AVX_SIZE_BYTE, length};
  }

private:
  /// Pointer to the first element viewed by the span.
  int8_t* array_ptr;
  /// Number of elements viewed by the span.
  size_t view_length;

  template <typename T>
  auto get_at(help_type<T> /**/, unsigned /**/) const
  {
    return;
  }

  /// Specialization of the get method for AVX2.
  __m256i get_at(help_type<m256_wrapper> /**/, unsigned pos) const
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(array_ptr) + pos);
  }

  /// Specialization of the get method for AVX512.
  __m512i get_at(help_type<m512_wrapper> /**/, unsigned pos) const
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(array_ptr) + pos);
  }

  /// Constructor for generating subspans.
  avx_span(int8_t* ptr, size_t length) : array_ptr(ptr), view_length(length) {}
};
} // namespace detail

namespace mm256 {

using avx2_span = detail::avx_span<detail::m256_wrapper>;

/// \brief Scales packed 8-bit integers in \c a by the scaling factor \c sf.
///
/// Values of \c a larger than \c max or smaller than \c -max are forwarded unaltered.
/// \param[in] a   Vector of packed 8-bit integers.
/// \param[in] sf  Scaling factor (0, 1].
/// \param[in] max Maximum input value (in absolute value) to which the scaling is applied [0, 127).
/// \return    Vector of packed 8-bit integers with the scaling result.
inline __m256i scale_epi8(__m256i a, float sf, uint8_t max)
{
  srsran_assert((sf > 0) && (sf <= 1), "Scaling factor out of range.");
  srsran_assert(max < 127, "Parameter max out of range.");

  if (sf >= .9999) {
    return a;
  }

  static const __m256i mask_even_epi8 = _mm256_set1_epi16(0x00ff);
  // FLOAT2INT = 2^16 = 65536
  static constexpr unsigned FLOAT2INT = 1U << 16U;
  const __m256i             MAX_epi8  = _mm256_set1_epi8(max);
  const __m256i             MIN_epi8  = _mm256_set1_epi8(-max);

  // Create a register filled with copies of the scaling factor times FLOAT2INT. Note that the upper bound on sf implies
  // that (sf * FLOAT2INT) can be represented over 16 bits.
  __m256i sf_epi16 = _mm256_set1_epi16(static_cast<uint16_t>(sf * FLOAT2INT));
  // Even bytes in a, alternated with a zero-valued byte.
  __m256i even_epi16 = _mm256_and_si256(a, mask_even_epi8);
  // Odd bytes in a, moved to the even positions and alternated with a zero-valued byte.
  __m256i odd_epi16 = _mm256_srli_epi16(a, 8);

  // This is equivalent to multiply each byte by sf: indeed, we multiply each byte by (sf * FLOAT2INT), store the result
  // in over 32 bit and pick only the 16 most significant bits (i.e., we divide by FLOAT2INT).
  __m256i p_even_epi16 = _mm256_mulhi_epu16(even_epi16, sf_epi16);
  __m256i p_odd_epi16  = _mm256_mulhi_epu16(odd_epi16, sf_epi16);

  // Move the odd bits back to their original positions.
  p_odd_epi16 = _mm256_slli_epi16(p_odd_epi16, 8);

  // Combine even and odd bits. Note that the "odd" bytes of p_even_epi16 are equal to 0 (the result of multiplying a
  // byte by a 16-bit value will occupy at most 24 of the 32 bits).
  __m256i product_epi8 = _mm256_xor_si256(p_even_epi16, p_odd_epi16);

  // Replace values corresponding to "large" inputs with their original values.
  __m256i mask_epi8 = _mm256_cmpgt_epi8(a, MAX_epi8);
  product_epi8      = _mm256_blendv_epi8(product_epi8, a, mask_epi8);
  mask_epi8         = _mm256_cmpgt_epi8(MIN_epi8, a);
  product_epi8      = _mm256_blendv_epi8(product_epi8, a, mask_epi8);
  return product_epi8;
}
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
  srsran_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
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
  srsran_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
  std::memcpy(out, in + steps, ls - steps);
  std::memcpy(out + ls - steps, in, steps);
}

} // namespace srsran
