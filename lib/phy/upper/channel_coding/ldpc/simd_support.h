/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

/// \file
/// \brief SIMD support for LDPC.
///
/// Defines useful templates and functions to work with SIMD (AVX2, AVX512 and NEON) registers.

#pragma once

#include "simd_types.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <array>

namespace srsran {
/// Number of bytes in an AVX2 register.
constexpr unsigned AVX2_SIZE_BYTE = 32;
/// Number of bytes in an AVX512 register.
constexpr unsigned AVX512_SIZE_BYTE = 64;
/// Number of bytes in a NEON register.
constexpr unsigned NEON_SIZE_BYTE = 16;

namespace detail {

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

  /// Sets the \c pos AVX2 register to \c val.
  void set_at(unsigned pos, simd256_type val);

  /// Sets the \c pos AVX512 register to \c val.
  void set_at(unsigned pos, simd512_type val);

  /// Sets the \c pos NEON register to \c val.
  void set_at(unsigned pos, simd128_type val);

  /// Gets the value stored in the \c pos SIMD register.
  auto get_at(unsigned pos) const { return get_at(help_type<simdWrapper>(), pos); }

  /// Returns the number of SIMD registers viewed by the span.
  size_t size() const { return view_length; }

  /// Returns a view to bytes from the specified offset.
  ///
  /// \param[in] offset  Starting point of the view (as a number of SIMD registers).
  /// \param[in] length  Length of the view (as a number of bytes).
  span<storageType> plain_span(size_t offset, size_t length) const
  {
    unsigned offset_byte = offset * SIMD_SIZE_BYTE;
    srsran_assert(offset_byte + length <= view_length * SIMD_SIZE_BYTE, "Size out of bound.");
    return span<storageType>(array_ptr + offset_byte, length);
  }

private:
  /// Pointer to the first element viewed by the span.
  pointer array_ptr;
  /// Number of elements viewed by the span.
  size_t view_length;

  /// Overload of the get method for AVX2.
  simd256_type get_at(help_type<simd256_wrapper> /**/, unsigned pos) const;

  /// Overload of the get method for AVX512.
  simd512_type get_at(help_type<simd512_wrapper> /**/, unsigned pos) const;

  /// Overload of the get method for NEON.
  simd128_type get_at(help_type<simd128_wrapper> /**/, unsigned pos) const;
};

} // namespace detail

} // namespace srsran
