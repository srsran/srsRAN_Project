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
  /// Default constructor.
  avx2_array() = default;
  /// \brief Constructor: sets an initial margin.
  ///
  /// Some operations on the content of the array require that some extra memory is allocated before the start of the
  /// actual array.
  /// \param[in] mm Number of guard AVX2 registers allocated before the start of the array.
  /// \remark The useful size of the array is therefore <tt>nof_elements - mm<\tt>.
  explicit avx2_array(unsigned mm) : inner_array(), margin(mm)
  {
    srsgnb_assert(mm < nof_elements, "The margin must be smaller than the number of elements.");
  }

  /// Returns a pointer to the \c pos AVX2 register inside the array.
  __m256i* data_at(unsigned pos)
  {
    unsigned index = (pos + margin) * AVX2_SIZE_BYTE;
    return reinterpret_cast<__m256i*>(inner_array.data() + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const __m256i* data_at(unsigned pos) const
  {
    unsigned index = (pos + margin) * AVX2_SIZE_BYTE;
    return reinterpret_cast<const __m256i*>(inner_array.data() + index);
  }

  /// Returns a pointer to the byte at position <tt>pos * AVX2_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = (pos + margin) * AVX2_SIZE_BYTE + byte;
    return (inner_array.data() + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = (pos + margin) * AVX2_SIZE_BYTE + byte;
    return (inner_array.data() + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // __m256i object by reference.

  /// Sets the \c pos AVX2 register to \c val.
  void set_at(unsigned pos, __m256i val)
  {
    unsigned index = (pos + margin) * AVX2_SIZE_BYTE;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(inner_array.data() + index), val);
  }

  /// Gets the value stored in the \c pos AVX2 register.
  __m256i get_at(unsigned pos) const
  {
    unsigned index = (pos + margin) * AVX2_SIZE_BYTE;
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(inner_array.data() + index));
  }

private:
  /// Actual array where the AVX2 registers are stored.
  alignas(32) std::array<int8_t, nof_elements * AVX2_SIZE_BYTE> inner_array;
  /// Number of guard AVX2 registers at the beginning of the array.
  unsigned margin = 0;
};

} // namespace mm256

} // namespace srsgnb
