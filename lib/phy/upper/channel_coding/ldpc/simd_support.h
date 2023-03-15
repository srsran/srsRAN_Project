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
/// \brief SIMD support for LDPC.
///
/// Defines useful templates and functions to work with SIMD (AVX2, AVX512 and NEON) registers.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <array>

#if defined(__x86_64__)
#include <immintrin.h>
#endif // defined(__x86_64__)

#if defined(HAVE_NEON)
#include <arm_neon.h>
#endif // defined(HAVE_NEON)

namespace srsran {
/// Number of bytes in an AVX2 register.
constexpr unsigned AVX2_SIZE_BYTE = 32;
/// Number of bytes in an AVX512 register.
constexpr unsigned AVX512_SIZE_BYTE = 64;
/// Number of bytes in a NEON register.
constexpr unsigned NEON_SIZE_BYTE = 16;

namespace detail {

// These wrappers are needed to avoid attribute warnings about the SIMD vector types.
#if defined(__x86_64__)
struct m256_wrapper {
  using simdType = __m256i;
};

struct m512_wrapper {
  using simdType = __m512i;
};
#endif // defined(__x86_64__)

#if defined(HAVE_NEON)
struct int8x16_wrapper {
  using simdType = int8x16_t;
};
#endif // defined(HAVE_NEON)

/// \brief Mimics a span of SIMD registers.
/// \tparam simdWrapper   Wrapper for an SIMD vector type.
/// \tparam storageType   Underlying data type of the \c simd_span (either \c int8_t or \c const \c int8_t).
template <typename simdWrapper, typename storageType>
class simd_span
{
  static_assert(std::is_same<typename std::remove_cv<storageType>::type, int8_t>::value,
                "simd_span can only have (const) int8_t as underlying storage type.");

  using pointer = storageType*;

  /// Helper type for method overloading.
  template <typename HelpType>
  struct help_type {
    // Intentionally empty.
  };

public:
  using simdType                         = typename simdWrapper::simdType;
  static constexpr size_t SIMD_SIZE_BYTE = sizeof(simdType);

  /// Default constructor: empty view to a null pointer.
  simd_span() : array_ptr(nullptr), view_length(0) {}

  /// \brief Constructs a span from a standard array.
  ///
  /// \tparam IntType    Type of the standard array - must be an integer type of 8 bits.
  /// \tparam N          Number of elements of the standard array.
  /// \param[in] arr     Array the span is a view of.
  /// \param[in] offset  Offset from the beginning of the array (as a number of SIMD registers).
  /// \param[in] length  Number of elements spanned by the view (as a number of SIMD registers).
  template <typename IntType, size_t N>
  simd_span(std::array<IntType, N>& arr, size_t offset, size_t length) :
    array_ptr(reinterpret_cast<pointer>(arr.data()) + offset * SIMD_SIZE_BYTE), view_length(length)
  {
    static_assert(sizeof(IntType) == sizeof(storageType),
                  "simd_span can only be created from arrays of 1-byte integer types.");
    srsran_assert((offset + view_length) * SIMD_SIZE_BYTE <= N, "Cannot take a span longer than the array.");
  }

  /// \brief Constructs a span from a standard array.
  ///
  /// \tparam IntType    Type of the standard array - must be an integer type of 8 bits.
  /// \tparam N          Number of elements of the standard array.
  /// \param[in] arr     Array the span is a view of.
  /// \param[in] length  Number of elements spanned by the view (as a number of SIMD registers).
  template <typename IntType, size_t N>
  simd_span(std::array<IntType, N>& arr, size_t length) : simd_span(arr, 0, length)
  {
  }

  /// \brief Constructs an \c simd_span from a \c srsran::span.
  ///
  /// \tparam IntType   Type of the span - must be an integer type of 8 bits.
  /// \param[in] sp     Original span.
  /// \param[in] length Number of elements viewed by the \c simd_span (as a number of SIMD registers).
  template <typename IntType>
  simd_span(span<IntType> sp, size_t length) : array_ptr(reinterpret_cast<pointer>(sp.data())), view_length(length)
  {
    static_assert(sizeof(IntType) == sizeof(storageType),
                  "simd_span can only be created from arrays of 1-byte integer types.");
    srsran_assert(view_length * SIMD_SIZE_BYTE <= sp.size(),
                  "Cannot create an simd_span longer than the original span.");
  }

  /// Returns a pointer to the \c pos SIMD register inside the array.
  simdType* data_at(unsigned pos)
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<simdType*>(array_ptr) + pos;
  }

  /// Returns a read-only pointer to the \c pos SIMD register inside the array.
  const simdType* data_at(unsigned pos) const
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<const simdType*>(array_ptr) + pos;
  }

  /// Returns a pointer to the byte at position <tt>pos * SIMD_SIZE_BYTE + byte</tt> inside the array.
  pointer data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * SIMD_SIZE_BYTE + byte;
    srsran_assert(index < view_length * SIMD_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  /// Returns a read-only pointer to the \c pos SIMD register inside the array.
  const pointer data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * SIMD_SIZE_BYTE + byte;
    srsran_assert(index < view_length * SIMD_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // simdType object by reference.

#if defined(__x86_64__)
  /// Sets the \c pos AVX2 register to \c val.
  void set_at(unsigned pos, __m256i val)
  {
    static_assert(SIMD_SIZE_BYTE == AVX2_SIZE_BYTE, "Cannot set an AVX512 vector with an AVX2 vector.");
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(array_ptr) + pos, val);
  }

  /// Sets the \c pos AVX512 register to \c val.
  void set_at(unsigned pos, __m512i val)
  {
    static_assert(SIMD_SIZE_BYTE == AVX512_SIZE_BYTE, "Cannot set an AVX2 vector with an AVX512 vector.");
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(array_ptr) + pos, val);
  }
#endif // defined(__x86_64__)

#if defined(HAVE_NEON)
  /// Sets the \c pos NEON register to \c val.
  void set_at(unsigned pos, int8x16_t val)
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    vst1q_s8(array_ptr + pos * NEON_SIZE_BYTE, val);
  }
#endif // defined(HAVE_NEON)

  /// Gets the value stored in the \c pos SIMD register.
  simdType get_at(unsigned pos) const
  {
    return get_at(help_type<simdWrapper>(), pos);
  }

  /// Returns the number of SIMD registers viewed by the span.
  size_t size() const
  {
    return view_length;
  }

  simd_span first(size_t length)
  {
    srsran_assert(length <= view_length, "Trying to create a subspan longer than the span itself.");
    return {array_ptr, length};
  }

  simd_span subspan(size_t offset, size_t length)
  {
    srsran_assert(offset + length <= view_length, "Trying to create a subspan longer than the span itself.");
    return {array_ptr + offset * SIMD_SIZE_BYTE, length};
  }

private:
  /// Pointer to the first element viewed by the span.
  pointer array_ptr;
  /// Number of elements viewed by the span.
  size_t view_length;

  /// Constructor for generating subspans.
  simd_span(pointer ptr, size_t length) : array_ptr(ptr), view_length(length) {}

  template <typename T>
  auto get_at(help_type<T> /**/, unsigned /**/) const
  {
    return;
  }

#if defined(__x86_64__)
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
#endif // defined(__x86_64__)

#if defined(HAVE_NEON)
  /// Specialization of the get method for NEON.
  int8x16_t get_at(help_type<int8x16_wrapper> /**/, unsigned pos) const
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return vld1q_s8(array_ptr + pos * NEON_SIZE_BYTE);
  }
#endif // defined(HAVE_NEON)
};

} // namespace detail

/// \brief Rotates the contents of a node towards the left by \c steps chars, that is the \c steps * 8 least significant
/// bits become the most significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first byte of the output rotated node.
/// \param[in]  in        Pointer to the first byte of the input node to rotate.
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
/// \param[out] out       Pointer to the first byte of the output rotated node.
/// \param[in]  in        Pointer to the first byte of the input node to rotate.
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
