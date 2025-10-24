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

#include "dft_processor_ci16_avx2.h"
#include <immintrin.h>

using namespace srsran;

namespace {

inline __m256i swap_re_im(__m256i in)
{
  __m256i shuffle_idx = _mm256_setr_epi8(2,
                                         3,
                                         0,
                                         1,
                                         6,
                                         7,
                                         4,
                                         5,
                                         10,
                                         11,
                                         8,
                                         9,
                                         14,
                                         15,
                                         12,
                                         13,
                                         18,
                                         19,
                                         16,
                                         17,
                                         22,
                                         23,
                                         20,
                                         21,
                                         26,
                                         27,
                                         24,
                                         25,
                                         30,
                                         31,
                                         28,
                                         29);

  return _mm256_shuffle_epi8(in, shuffle_idx);
}

inline __m128i divide2_i16(__m128i in)
{
  return _mm_srai_epi16(in, 1);
}

inline __m256i divide2_i16(__m256i in)
{
  return _mm256_srai_epi16(in, 1);
}

inline __m256i simd_mul(__m256i a_real, __m256i a_imag, __m256i b)
{
  __m256i b_swap = swap_re_im(b);
  return _mm256_add_epi16(_mm256_mulhrs_epi16(a_real, b), _mm256_mulhrs_epi16(a_imag, b_swap));
}

inline __m128i simd_mul(__m128i a_real, __m128i a_imag, __m128i b)
{
  __m128i b_swap = _mm_shuffle_epi8(b, _mm_setr_epi8(2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13));
  return _mm_add_epi16(_mm_mulhrs_epi16(a_real, b), _mm_mulhrs_epi16(a_imag, b_swap));
}

template <int Sign, unsigned Stride>
__m128i dft_dit_4(const ci16_t* in)
{
  // Shuffling indices.
  __m128i shuffle_idx = _mm_setr_epi8(0, 1, 2, 3, 6, 7, 4, 5, 8, 9, 10, 11, 14, 15, 12, 13);

  // Load inputs.
  __m128i simd_in0 = _mm_set1_epi32(reinterpret_cast<const uint32_t*>(in)[0]);
  __m128i simd_in1 = _mm_set1_epi32(reinterpret_cast<const uint32_t*>(in)[Stride]);
  __m128i simd_in2 = _mm_set1_epi32(reinterpret_cast<const uint32_t*>(in)[2 * Stride]);
  __m128i simd_in3 = _mm_set1_epi32(reinterpret_cast<const uint32_t*>(in)[3 * Stride]);

  // Amend signs for each of the SSE registers.
  simd_in1 = _mm_sign_epi16(simd_in1, _mm_setr_epi16(1, 1, Sign, -Sign, -1, -1, -Sign, Sign));
  simd_in2 = _mm_sign_epi16(simd_in2, _mm_setr_epi16(1, 1, -1, -1, 1, 1, -1, -1));
  simd_in3 = _mm_sign_epi16(simd_in3, _mm_setr_epi16(1, 1, -Sign, Sign, -1, -1, Sign, -Sign));

  // Accumulate registers 1 and 3, swap real and imaginary parts for the values multiplied by j.
  __m128i simd_out = _mm_add_epi16(simd_in1, simd_in3);
  simd_out         = _mm_shuffle_epi8(simd_out, shuffle_idx);

  // Finish accumulating all registers.
  simd_out = _mm_add_epi16(simd_out, simd_in0);
  simd_out = _mm_add_epi16(simd_out, simd_in2);

  // Divide output by 4.
  return _mm_srai_epi16(simd_out, 2);
}

template <int Sign, unsigned Stride>
__m256i dft_dit_8(const ci16_t* in)
{
  __m128i simd_p = dft_dit_4<Sign, 2 * Stride>(in);
  __m128i simd_q = dft_dit_4<Sign, 2 * Stride>(&in[Stride]);

  // Real and imaginary parts of complex exponential.
  const __m128i simd_table_re =
      _mm_setr_epi16(32767 / 2, 32767 / 2, 23170 / 2, 23170 / 2, 0, 0, -23170 / 2, -23170 / 2);
  const __m128i simd_table_im = _mm_setr_epi16(0,
                                               0,
                                               -Sign * 23170 / 2,
                                               Sign * 23170 / 2,
                                               -Sign * 32767 / 2,
                                               Sign * 32767 / 2,
                                               -Sign * 23170 / 2,
                                               Sign * 23170 / 2);

  // Calculate product of Q and the table.
  simd_q = simd_mul(simd_table_re, simd_table_im, simd_q);
  simd_p = divide2_i16(simd_p);

  // Calculate final result.
  return _mm256_setr_m128i(_mm_add_epi16(simd_p, simd_q), _mm_sub_epi16(simd_p, simd_q));
}

template <unsigned N, int Sign, unsigned Stride>
class dft_dit_template_avx2
{
public:
  dft_dit_template_avx2()
  {
    for (unsigned idx = 0, idx_end = N / 2; idx != idx_end; ++idx) {
      std::complex<double> coeff =
          std::polar(32767.0 / 2.0, -Sign * 2.0 * M_PI * static_cast<double>(idx) / static_cast<double>(N));
      table_re[2 * idx]     = static_cast<int16_t>(coeff.real());
      table_re[2 * idx + 1] = table_re[2 * idx];
      table_im[2 * idx]     = static_cast<int16_t>(coeff.imag());
      table_im[2 * idx + 1] = -table_im[2 * idx];
    }
  }

  __attribute_noinline__ void run(ci16_t* out, const ci16_t* in) const
  {
    // Radix 2.
    radix2.run(&out[0], in);
    radix2.run(&out[N / 2], &in[Stride]);

    unsigned k = 0;

    for (unsigned k_end = ((N / 2) / 8) * 8; k != k_end; k += 8) {
      __m256i simd_table_re = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&table_re[2 * k]));
      __m256i simd_table_im = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&table_im[2 * k]));
      __m256i simd_p        = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&out[k]));
      __m256i simd_q        = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&out[k + N / 2]));

      simd_q = simd_mul(simd_table_re, simd_table_im, simd_q);
      simd_p = divide2_i16(simd_p);

      _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[k]), _mm256_add_epi16(simd_p, simd_q));
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[k + N / 2]), _mm256_sub_epi16(simd_p, simd_q));
    }

    for (unsigned k_end = ((N / 2) / 4) * 4; k != k_end; k += 4) {
      __m128i simd_table_re = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&table_re[2 * k]));
      __m128i simd_table_im = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&table_im[2 * k]));
      __m128i simd_p        = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&out[k]));
      __m128i simd_q        = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&out[k + N / 2]));

      simd_q = simd_mul(simd_table_re, simd_table_im, simd_q);
      simd_p = divide2_i16(simd_p);

      _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[k]), _mm_add_epi16(simd_p, simd_q));
      _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[k + N / 2]), _mm_sub_epi16(simd_p, simd_q));
    }

    srsran_assert(k == N / 2, "k is {} but expected {}.", k, N / 2);
  }

private:
  dft_dit_template_avx2<N / 2, Sign, 2 * Stride> radix2;
  std::array<int16_t, N>                         table_re;
  std::array<int16_t, N>                         table_im;
};

template <int Sign, unsigned Stride>
class dft_dit_template_avx2<16, Sign, Stride>
{
public:
  __always_inline void run(ci16_t* out, const ci16_t* in) const
  {
    __m256i simd_p = dft_dit_8<Sign, 2 * Stride>(in);
    __m256i simd_q = dft_dit_8<Sign, 2 * Stride>(&in[Stride]);

    // Real and imaginary parts of complex exponential.
    const __m256i simd_table_re = _mm256_setr_epi16(
        32767, 32767, 30273, 30273, 23170, 23170, 12539, 12539, 0, 0, -12539, -12539, -23170, -23170, -30273, -30273);
    const __m256i simd_table_im = _mm256_setr_epi16(0,
                                                    0,
                                                    -Sign * 12539,
                                                    Sign * 12539,
                                                    -Sign * 23170,
                                                    Sign * 23170,
                                                    -Sign * 30273,
                                                    Sign * 30273,
                                                    -Sign * 32767,
                                                    Sign * 32767,
                                                    -Sign * 30273,
                                                    Sign * 30273,
                                                    -Sign * 23170,
                                                    Sign * 23170,
                                                    -Sign * 12539,
                                                    Sign * 12539);

    simd_q = simd_mul(simd_table_re, simd_table_im, simd_q);
    simd_q = divide2_i16(simd_q);
    simd_p = divide2_i16(simd_p);

    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[0]), _mm256_add_epi16(simd_p, simd_q));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[8]), _mm256_sub_epi16(simd_p, simd_q));
  }
};

// Implements a DFT of size 8.
template <int Sign, unsigned Stride>
class dft_dit_template_avx2<8, Sign, Stride>
{
public:
  void run(ci16_t* out, const ci16_t* in) const
  {
    __m256i simd_in = dft_dit_8<Sign, Stride>(in);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), simd_in);
  }
};

// Implements a DFT of size 4.
template <int Sign, unsigned Stride>
class dft_dit_template_avx2<4, Sign, Stride>
{
public:
  void run(ci16_t* out, const ci16_t* in) const
  {
    __m128i simd_in = dft_dit_4<Sign, Stride>(in);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out), simd_in);
  }
};

/// Templated DFT processor for 16-bit complex integers.
template <unsigned Size, int Sign>
class dft_processor_ci16_avx2_impl : public dft_processor_ci16
{
public:
  // See interface for documentation.
  direction get_direction() const override { return (direction_sign == 1) ? direction::direct : direction::inverse; }

  // See interface for documentation.
  unsigned get_size() const override { return dft_size; }

  // See interface for documentation.
  void run(span<ci16_t> out, span<const ci16_t> in) override
  {
    srsran_assert(out.size() == dft_size,
                  "The DFT output size (i.e., {}) is not equal to the DFT size (i.e., {})",
                  out.size(),
                  dft_size);
    srsran_assert(in.size() == dft_size,
                  "The DFT input size (i.e., {}) is not equal to the DFT size (i.e., {})",
                  in.size(),
                  dft_size);
    dft.run(out.data(), in.data());
  }

private:
  /// DFT direction.
  static constexpr int direction_sign = Sign;
  /// DFT size.
  static constexpr unsigned dft_size = Size;
  /// Internal DFT.
  dft_dit_template_avx2<Size, Sign, 1> dft;
};

/// Recursive DFT creation.
template <unsigned THIS_SIZE>
std::unique_ptr<dft_processor_ci16> create_dft_ci16_avx2(unsigned dft_size, dft_processor_ci16::direction direction)
{
  if (dft_size == THIS_SIZE) {
    if (direction == dft_processor_ci16::direction::direct) {
      return std::make_unique<dft_processor_ci16_avx2_impl<THIS_SIZE, -1>>();
    } else {
      return std::make_unique<dft_processor_ci16_avx2_impl<THIS_SIZE, 1>>();
    }
  }

  return create_dft_ci16_avx2<THIS_SIZE / 2>(dft_size, direction);
}

template <>
std::unique_ptr<dft_processor_ci16> create_dft_ci16_avx2<2>(unsigned, dft_processor_ci16::direction)
{
  return nullptr;
}

} // namespace

std::unique_ptr<dft_processor_ci16> srsran::create_dft_processor_ci16_avx2(unsigned                      dft_size,
                                                                           dft_processor_ci16::direction direction)
{
  return create_dft_ci16_avx2<8192>(dft_size, direction);
}
