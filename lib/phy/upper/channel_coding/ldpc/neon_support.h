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
/// \brief NEON support for LDPC.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <arm_neon.h>
#include <array>

namespace srsran {
/// Number of bytes in a NEON register.
constexpr unsigned NEON_SIZE_BYTE = 16;

namespace neon {
/// \brief Mimics a span of NEON registers.
class neon_span
{
public:
  /// \brief Constructs a span from a standard array.
  ///
  /// \tparam IntType    Type of the standard array - must be an integer type of 8 bits.
  /// \tparam N          Number of elements of the standard array.
  /// \param[in] arr     Array the span is a view of.
  /// \param[in] offset  Offset from the beginning of the array (as a number of NEON registers).
  /// \param[in] length  Number of elements spanned by the view (as a number of NEON registers).
  template <typename IntType, size_t N>
  neon_span(std::array<IntType, N>& arr, size_t offset, size_t length) :
    array_ptr(reinterpret_cast<int8_t*>(arr.data()) + offset * NEON_SIZE_BYTE), view_length(length)
  {
    static_assert(sizeof(IntType) == sizeof(int8_t),
                  "neon_span can only be created from arrays of 1-byte integer types.");
    srsran_assert((offset + view_length) * NEON_SIZE_BYTE <= N,
                  "Cannot create a neon_span longer than the original array.");
  }

  /// \brief Constructs a span from a standard array.
  ///
  /// \tparam IntType    Type of the standard array - must be an integer type of 8 bits.
  /// \tparam N          Number of elements of the standard array.
  /// \param[in] arr     Array the span is a view of.
  /// \param[in] length  Number of elements spanned by the view (as a number of NEON registers).
  template <typename IntType, size_t N>
  neon_span(std::array<IntType, N>& arr, size_t length) : neon_span(arr, 0, length)
  {
  }

  /// \brief Constructs an \c avx_span from a \c srsran::span.
  ///
  /// \tparam IntType   Type of the span - must be an integer type of 8 bits.
  /// \param[in] sp     Original span.
  /// \param[in] length Number of elements viewed by the \c neon_span (as a number of NEON registers).
  template <typename IntType>
  neon_span(span<IntType> sp, size_t length) : array_ptr(reinterpret_cast<int8_t*>(sp.data())), view_length(length)
  {
    static_assert(sizeof(IntType) == sizeof(int8_t),
                  "neon_span can only be created from arrays of 1-byte integer types.");
    static_assert(!std::is_const<IntType>::value, "Cannot create neon_span from span of const.");
    srsran_assert(view_length * NEON_SIZE_BYTE <= sp.size(),
                  "Cannot create a neon_span longer than the original span.");
  }

  /// Returns a pointer to the \c pos NEON register inside the array.
  int8x16_t* data_at(unsigned pos)
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<int8x16_t*>(array_ptr) + pos;
  }

  /// Returns a read-only pointer to the \c pos NEON register inside the array.
  const int8x16_t* data_at(unsigned pos) const
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<const int8x16_t*>(array_ptr) + pos;
  }

  /// Returns a pointer to the byte at position <tt>pos * NEON_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * NEON_SIZE_BYTE + byte;
    srsran_assert(index < view_length * NEON_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  /// Returns a read-only pointer to the \c pos NEON register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * NEON_SIZE_BYTE + byte;
    srsran_assert(index < view_length * NEON_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // int8x16_t object by reference.

  /// Sets the \c pos NEON register to \c val.
  void set_at(unsigned pos, int8x16_t val)
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    vst1q_s8(array_ptr + pos * NEON_SIZE_BYTE, val);
  }

  /// Gets the value stored in the \c pos NEON register.
  int8x16_t get_at(unsigned pos) const
  {
    srsran_assert(pos < view_length, "Index {} out of bound.", pos);
    return vld1q_s8(array_ptr + pos * NEON_SIZE_BYTE);
  }

  /// Returns the number of NEON registers viewed by the span.
  size_t size() const { return view_length; }

private:
  /// Pointer to the first element viewed by the span.
  int8_t* array_ptr;
  /// Number of elements viewed by the span.
  size_t view_length;
};

/// \brief Scales packed 8-bit integers in \c a by the scaling factor \c sf.
///
/// Values of \c a larger than \c max or smaller than \c -max are forwarded unaltered.
/// \param[in] a   Vector of packed 8-bit integers.
/// \param[in] sf  Scaling factor (0, 1].
/// \param[in] max Maximum input value (in absolute value) to which the scaling is applied [0, 127).
/// \return    Vector of packed 8-bit integers with the scaling result.
inline int8x16_t scale_s8(int8x16_t a, float sf, uint8_t max)
{
  srsran_assert((sf > 0) && (sf <= 1), "Scaling factor out of range.");
  srsran_assert(max < 127, "Parameter max out of range.");

  if (sf >= .9999) {
    return a;
  }
  // FLOAT2INT = 2^8 = 256
  static constexpr unsigned FLOAT2INT       = 1U << 8U;
  const int8x16_t           MAX_s8          = vdupq_n_s8(max);
  const int8x16_t           MIN_s8          = vdupq_n_s8(-max);
  static const uint16x8_t   mask_even_bytes = vdupq_n_u16(0x00ff);
  static const uint16x8_t   mask_odd_bytes  = vdupq_n_u16(0xff00);

  // Scaling factor times FLOAT2INT. Note that the upper bound on sf implies
  // that (sf * FLOAT2INT) can be represented over 8 bits.
  uint8x8_t    sf_u8               = vdup_n_u8(static_cast<uint8_t>(sf * FLOAT2INT));
  uint8x16x2_t even_odd_deinterlvd = vuzpq_u8(vreinterpretq_u8_s8(a), vreinterpretq_u8_s8(a));
  // The above instruction results in:
  // even bytes of input vector placed in interleaved.val[0]
  // odd bytes of input vector  placed in interleaved.val[1]
  uint16x8_t p_even_u16 = vmull_u8(vget_low_u8(even_odd_deinterlvd.val[0]), sf_u8);
  uint16x8_t p_odd_u16  = vmull_u8(vget_low_u8(even_odd_deinterlvd.val[1]), sf_u8);
  // Shift right MSB byte in every element of p_even_u16, mask it and xor with odd bytes stored in p_odd_u16.
  p_odd_u16        = vandq_u16(p_odd_u16, mask_odd_bytes);
  p_even_u16       = vandq_u16(vshrq_n_u16(p_even_u16, 8), mask_even_bytes);
  int8x16_t result = vreinterpretq_s8_u16(vorrq_u16(p_even_u16, p_odd_u16));

  // Replace values corresponding to "large" inputs with their original values.
  uint8x16_t mask_u8 = vcgtq_s8(a, MAX_s8);
  result             = vbslq_s8(mask_u8, a, result);
  mask_u8            = vcgtq_s8(MIN_s8, a);
  result             = vbslq_s8(mask_u8, a, result);
  return result;
}

} // namespace neon

/// \brief Rotates the contents of a node towards the left by \c steps chars, that is the \c steps * 8 least significant
/// bits become the most significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first NEON block of the output rotated node.
/// \param[in]  in        Pointer to the first NEON block of the input node to rotate.
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
/// \param[out] out       Pointer to the first NEON block of the output rotated node.
/// \param[in]  in        Pointer to the first NEON block of the input node to rotate.
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
