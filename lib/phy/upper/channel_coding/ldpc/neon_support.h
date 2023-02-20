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

#include "srsran/support/srsran_assert.h"
#include <arm_neon.h>
#include <array>

namespace srsran {
/// Number of bytes in a NEON register.
constexpr unsigned NEON_SIZE_BYTE = 16;

namespace neon {

/// \brief Mimics an array of NEON registers.
/// \tparam nof_elements The number of NEON registers in the array.
template <size_t nof_elements>
class neon_array
{
public:
  /// Default constructor.
  neon_array() = default;

  /// Returns a pointer to the \c pos NEON register inside the array.
  int8x16_t* data_at(unsigned pos)
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    return reinterpret_cast<int8x16_t*>(inner_array.data() + pos * NEON_SIZE_BYTE);
  }

  /// Returns a read-only pointer to the \c pos NEON register inside the array.
  const int8x16_t* data_at(unsigned pos) const
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    return reinterpret_cast<int8x16_t*>(inner_array.data() + pos * NEON_SIZE_BYTE);
  }

  /// Returns a pointer to the byte at position <tt>pos * NEON_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * NEON_SIZE_BYTE + byte;
    srsgnb_assert(index < nof_elements * NEON_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (inner_array.data() + index);
  }

  /// Returns a read-only pointer to the \c pos NEON register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * NEON_SIZE_BYTE + byte;
    srsgnb_assert(index < nof_elements * NEON_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (inner_array.data() + index);
  }

  /// Sets the \c pos NEON register to \c val.
  void set_at(unsigned pos, int8x16_t val)
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    vst1q_s8(inner_array.data() + pos * NEON_SIZE_BYTE, val);
  }

  /// Gets the value stored in the \c pos NEON register.
  int8x16_t get_at(unsigned pos) const
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    return vld1q_s8(inner_array.data() + pos * NEON_SIZE_BYTE);
  }

private:
  /// Actual array where the NEON registers are stored.
  alignas(NEON_SIZE_BYTE) std::array<int8_t, nof_elements * NEON_SIZE_BYTE> inner_array;
};

/// \brief Mimics a span of NEON registers.
class neon_span
{
public:
  /// \brief Constructs a span from an \ref neon_array.
  ///
  /// \tparam N     Array length.
  /// \param arr    Array the span is a view of.
  /// \param offset First element of the array (a NEON register) viewed by the span.
  /// \param length Length of the span.
  template <size_t N>
  neon_span(neon_array<N>& arr, unsigned offset, unsigned length) :
    array_ptr(arr.data_at(offset, 0)), view_length(length)
  {
    srsgnb_assert(offset + view_length <= N, "Cannot take a span longer than the array.");
  }

  /// \brief Implicitly constructs a span that is a view over an entire \ref neon_array.
  template <size_t N>
  neon_span(neon_array<N>& arr) : neon_span(arr, 0, N)
  {
  }

  /// Returns a pointer to the \c pos NEON register inside the array.
  int8x16_t* data_at(unsigned pos)
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<int8x16_t*>(array_ptr) + pos;
  }

  /// Returns a read-only pointer to the \c pos NEON register inside the array.
  const int8x16_t* data_at(unsigned pos) const
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    return reinterpret_cast<const int8x16_t*>(array_ptr) + pos;
  }

  /// Returns a pointer to the byte at position <tt>pos * NEON_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * NEON_SIZE_BYTE + byte;
    srsgnb_assert(index < view_length * NEON_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  /// Returns a read-only pointer to the \c pos NEON register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * NEON_SIZE_BYTE + byte;
    srsgnb_assert(index < view_length * NEON_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // avxType object by reference.

  /// Sets the \c pos NEON register to \c val.
  void set_at(unsigned pos, int8x16_t val)
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    vst1q_s8(array_ptr + pos * NEON_SIZE_BYTE, val);
  }

  /// Gets the value stored in the \c pos NEON register.
  int8x16_t get_at(unsigned pos) const
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
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
  srsgnb_assert((sf > 0) && (sf <= 1), "Scaling factor out of range.");
  srsgnb_assert(max < 127, "Parameter max out of range.");

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
  srsgnb_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
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
  srsgnb_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
  std::memcpy(out, in + steps, ls - steps);
  std::memcpy(out + ls - steps, in, steps);
}

} // namespace srsran
