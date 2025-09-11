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

#include "dft_processor_generic_impl.h"
#include "srsran/srsvec/simd.h"
#include "srsran/support/math/math_utils.h"
#include <cmath>

using namespace srsran;

namespace {

// Implements a DFT of any arbitrary size.
template <unsigned N, typename Enable = void>
class generic_dft_dit : public generic_dft_N
{
private:
  unsigned                stride;
  std::array<cf_t, N * N> tables;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_)
  {
    std::array<cf_t, N> cexp;
    for (unsigned k = 0; k != N; ++k) {
      cexp[k] = std::polar(1.0F, sign * TWOPI * static_cast<float>(k) / static_cast<float>(N));
    }

    for (unsigned k = 0; k != N; ++k) {
      for (unsigned n = 0; n != N; ++n) {
        tables[N * k + n] = cexp[(k * n) % N];
      }
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    std::array<cf_t, N> in_block;
    cf_t                sum = 0;
    for (unsigned n = 0; n != N; ++n) {
      in_block[n] = in[stride * n];
      sum += in_block[n];
    }
    out[0] = sum;

    for (unsigned k = 1; k != N; ++k) {
      const cf_t* table = &tables[N * k];

      sum = in_block[0];

      for (unsigned n = 1; n != N; ++n) {
        cf_t in_sample = in_block[n];
        cf_t coeff     = table[n];
        sum += in_sample * coeff;
      }

      out[k] = sum;
    }
  }
};

// Implements a DFT of size N multiple of 2, but not of 3 or 5.
//
// The implementation follows a radix-2 decimation-in-time algorithm that rearranges the DFT equation into two parts: a
// sum over the even-numbered discrete-time indices n={0,2,4,...,N−2} and a sum over the odd-numbered indices
// n={1,3,5,...,N−1}
//
// It relies on templates for optimization purposes.
template <unsigned N>
class generic_dft_dit<N, std::enable_if_t<((N % 2 == 0) && (N % 3 != 0) && (N % 5 != 0))>> : public generic_dft_N
{
private:
  unsigned               stride;
  generic_dft_dit<N / 2> radix2;
  std::array<cf_t, N>    table;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_), radix2(sign, 2 * stride)
  {
    for (unsigned idx = 0; idx != N; ++idx) {
      table[idx] = std::polar(1.0F, sign * TWOPI * static_cast<float>(idx) / static_cast<float>(N));
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    // Radix 2.
    radix2.run(&out[0], in);
    radix2.run(&out[N / 2], &in[stride]);

    unsigned k = 0;

#if SRSRAN_SIMD_CF_SIZE
    if (N / 2 >= SRSRAN_SIMD_CF_SIZE) {
      for (; k != ((N / 2) / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; k += SRSRAN_SIMD_CF_SIZE) {
        simd_cf_t p = srsran_simd_cfi_loadu(&out[k]);
        simd_cf_t q = srsran_simd_cf_prod(srsran_simd_cfi_loadu(&table[k]), srsran_simd_cfi_loadu(&out[k + N / 2]));
        srsran_simd_cfi_storeu(&out[k], srsran_simd_cf_add(p, q));
        srsran_simd_cfi_storeu(&out[k + N / 2], srsran_simd_cf_sub(p, q));
      }
    }
#endif // SRSRAN_SIMD_CF_SIZE

    for (; k != N / 2; ++k) {
      cf_t p         = out[k];
      cf_t q         = table[k] * out[k + N / 2];
      out[k]         = p + q;
      out[k + N / 2] = p - q;
    }
  }
};

// Implements a DFT of size N multiple of 3, but not of 5.
template <unsigned N>
class generic_dft_dit<N, std::enable_if_t<((N % 3 == 0) && (N % 5 != 0))>> : public generic_dft_N
{
private:
  static constexpr unsigned N_3 = N / 3;
  unsigned                  stride;
  generic_dft_dit<N_3>      radix3;
  std::array<cf_t, N>       table;
  std::array<float, N>      table_re;
  std::array<float, N>      table_im;
  std::array<cf_t, N>       table2;
  std::array<float, N>      table2_re;
  std::array<float, N>      table2_im;
  const cf_t                w1;
  const cf_t                w2;

public:
  generic_dft_dit(float sign, unsigned stride_) :
    stride(stride_),
    radix3(sign, 3 * stride),
    w1(std::polar(1.0f, sign * 2.0f * static_cast<float>(M_PI) / 3.0f)),
    w2(std::polar(1.0f, sign * 4.0f * static_cast<float>(M_PI) / 3.0f))
  {
    for (unsigned idx = 0; idx != N_3; ++idx) {
      table[idx]     = std::polar(1.0F, sign * TWOPI * static_cast<float>(idx) / static_cast<float>(N));
      table_re[idx]  = table[idx].real();
      table_im[idx]  = table[idx].imag();
      table2[idx]    = std::polar(1.0F, 2 * sign * TWOPI * static_cast<float>(idx) / static_cast<float>(N));
      table2_re[idx] = table2[idx].real();
      table2_im[idx] = table2[idx].imag();
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    // Radix 3.
    radix3.run(&out[0], in);
    radix3.run(&out[N_3], &in[stride]);
    radix3.run(&out[2 * N_3], &in[2 * stride]);

    unsigned k = 0;

#if SRSRAN_SIMD_CF_SIZE
    simd_cf_t w1_simd = srsran_simd_cf_set1(w1);
    simd_cf_t w2_simd = srsran_simd_cf_set1(w2);
    for (; k != (N_3 / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; k += SRSRAN_SIMD_CF_SIZE) {
      simd_cf_t p_simd = srsran_simd_cfi_loadu(&out[k]);
      simd_cf_t q_simd = srsran_simd_cf_loadu(&table_re[k], &table_im[k]) * srsran_simd_cfi_loadu(&out[k + N_3]);
      simd_cf_t r_simd = srsran_simd_cf_loadu(&table2_re[k], &table2_im[k]) * srsran_simd_cfi_loadu(&out[k + 2 * N_3]);

      // Radix-3 butterfly (like a 3-point DFT).
      srsran_simd_cfi_storeu(&out[k], p_simd + q_simd + r_simd);
      srsran_simd_cfi_storeu(&out[k + N_3], p_simd + q_simd * w1_simd + r_simd * w2_simd);
      srsran_simd_cfi_storeu(&out[k + 2 * N_3], p_simd + q_simd * w2_simd + r_simd * w1_simd);
    }
#endif // SRSRAN_SIMD_CF_SIZE

    for (; k != N_3; ++k) {
      cf_t p = out[k];
      cf_t q = table[k] * out[k + N_3];
      cf_t r = table2[k] * out[k + 2 * N_3];

      // Radix-3 butterfly (like a 3-point DFT).
      out[k]           = p + q + r;
      out[k + N_3]     = p + q * w1 + r * w2;
      out[k + 2 * N_3] = p + q * w2 + r * w1;
    }
  }
};

// Implements a DFT of size N multiple of 5.
template <unsigned N>
class generic_dft_dit<N, std::enable_if_t<(N % 5 == 0)>> : public generic_dft_N
{
private:
  unsigned               stride;
  generic_dft_dit<N / 5> radix5;
  std::array<cf_t, N>    table;
  const cf_t             w1;
  const cf_t             w2;
  const cf_t             w3;
  const cf_t             w4;

public:
  generic_dft_dit(float sign, unsigned stride_) :
    stride(stride_),
    radix5(sign, 5 * stride),
    w1(std::polar(1.0f, sign * 2.0f * static_cast<float>(M_PI) / 5.0f)),
    w2(std::polar(1.0f, sign * 4.0f * static_cast<float>(M_PI) / 5.0f)),
    w3(std::polar(1.0f, sign * 6.0f * static_cast<float>(M_PI) / 5.0f)),
    w4(std::polar(1.0f, sign * 8.0f * static_cast<float>(M_PI) / 5.0f))
  {
    for (unsigned idx = 0; idx != N; ++idx) {
      table[idx] = std::polar(1.0F, sign * TWOPI * static_cast<float>(idx) / static_cast<float>(N));
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    // Radix 5.
    radix5.run(&out[0], in);
    radix5.run(&out[N / 5], in + stride);
    radix5.run(&out[2 * N / 5], in + 2 * stride);
    radix5.run(&out[3 * N / 5], in + 3 * stride);
    radix5.run(&out[4 * N / 5], in + 4 * stride);

    unsigned k = 0;

    for (; k != N / 5; ++k) {
      cf_t a0 = out[k];
      cf_t a1 = table[k] * out[k + N / 5];
      cf_t a2 = table[2 * k % N] * out[k + 2 * N / 5];
      cf_t a3 = table[3 * k % N] * out[k + 3 * N / 5];
      cf_t a4 = table[4 * k % N] * out[k + 4 * N / 5];

      // Radix-5 butterfly (like a 5-point DFT).
      out[k]             = a0 + a1 + a2 + a3 + a4;
      out[k + N / 5]     = a0 + a1 * w1 + a2 * w2 + a3 * w3 + a4 * w4;
      out[k + 2 * N / 5] = a0 + a1 * w2 + a2 * w4 + a3 * w1 + a4 * w3;
      out[k + 3 * N / 5] = a0 + a1 * w3 + a2 * w1 + a3 * w4 + a4 * w2;
      out[k + 4 * N / 5] = a0 + a1 * w4 + a2 * w3 + a3 * w2 + a4 * w1;
    }
  }
};

// Implements a DFT of size 3.
template <>
class generic_dft_dit<3> : public generic_dft_N
{
private:
  unsigned stride;
  cf_t     cexp_pi_3;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_) { cexp_pi_3 = std::polar(1.0F, sign * TWOPI / 3.0F); }

  void run(cf_t* out, const cf_t* in) const override
  {
    cf_t a0 = in[0];
    cf_t a1 = in[stride];
    cf_t a2 = in[2 * stride];

    float a1_rere = a1.real() * cexp_pi_3.real();
    float a1_reim = a1.real() * cexp_pi_3.imag();
    float a1_imre = a1.imag() * cexp_pi_3.real();
    float a1_imim = a1.imag() * cexp_pi_3.imag();

    float a2_rere = a2.real() * cexp_pi_3.real();
    float a2_reim = a2.real() * cexp_pi_3.imag();
    float a2_imre = a2.imag() * cexp_pi_3.real();
    float a2_imim = a2.imag() * cexp_pi_3.imag();

    out[0] = a0 + a1 + a2;
    out[1] = a0 + cf_t(a1_rere - a1_imim, a1_imre + a1_reim) + cf_t(a2_rere + a2_imim, a2_imre - a2_reim);
    out[2] = a0 + cf_t(a1_rere + a1_imim, a1_imre - a1_reim) + cf_t(a2_rere - a2_imim, a2_imre + a2_reim);
  }
};

// Implements a DFT of size 4.
template <>
class generic_dft_dit<4, void> : public generic_dft_N
{
private:
  float    sign;
  unsigned stride;

public:
  generic_dft_dit(float sign_, unsigned stride_) : sign(sign_), stride(stride_) {}

  void run(cf_t* out, const cf_t* in) const override
  {
    using namespace std::complex_literals;

    cf_t a0 = in[0];
    cf_t a1 = in[stride];
    cf_t a2 = in[2 * stride];
    cf_t a3 = in[3 * stride];

    cf_t p0 = a0 + a2;
    cf_t q0 = a0 - a2;

    cf_t p1 = a1 + a3;
    cf_t q1 = (1.0if * sign) * (a1 - a3);

    out[0] = p0 + p1;
    out[1] = q0 + q1;
    out[2] = p0 - p1;
    out[3] = q0 - q1;
  }
};

#if SRSRAN_SIMD_CF_SIZE > 4

// Implements a DFT of size SRSRAN_SIMD_CF_SIZE.
template <>
class generic_dft_dit<SRSRAN_SIMD_CF_SIZE> : public generic_dft_N
{
private:
  static constexpr unsigned N = SRSRAN_SIMD_CF_SIZE;
  unsigned                  stride;
  std::array<float, N * N>  tables_re;
  std::array<float, N * N>  tables_im;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_)
  {
    std::array<cf_t, N> cexp;
    for (unsigned k = 0; k != N; ++k) {
      cexp[k] = std::polar(1.0F, sign * TWOPI * static_cast<float>(k) / static_cast<float>(N));
    }

    for (unsigned k = 0; k != N; ++k) {
      for (unsigned n = 0; n != N; ++n) {
        cf_t w               = cexp[(k * n) % N];
        tables_re[N * k + n] = w.real();
        tables_im[N * k + n] = w.imag();
      }
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    simd_cf_t p_simd = srsran_simd_cf_set1(in[0]);
    for (unsigned n = 1; n != N; ++n) {
      simd_cf_t p_in_simd = srsran_simd_cf_set1(in[stride * n]);
      simd_cf_t w_simd    = srsran_simd_cf_loadu(&tables_re[n * N], &tables_im[n * N]);

      p_simd = srsran_simd_cf_add(p_simd, srsran_simd_cf_prod(p_in_simd, w_simd));
    }

    srsran_simd_cfi_storeu(out, p_simd);
  }
};

#endif // SRSRAN_SIMD_CF_SIZE > 4

#if (2 * SRSRAN_SIMD_CF_SIZE) > 4

// Implements a DFT of size 2 * SRSRAN_SIMD_CF_SIZE.
template <>
class generic_dft_dit<2 * SRSRAN_SIMD_CF_SIZE> : public generic_dft_N
{
private:
  static constexpr unsigned N = SRSRAN_SIMD_CF_SIZE;
  unsigned                  stride;
  alignas(SIMD_BYTE_ALIGN) std::array<float, N * N> tables_re;
  alignas(SIMD_BYTE_ALIGN) std::array<float, N * N> tables_im;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_)
  {
    std::array<cf_t, N> cexp;
    for (unsigned k = 0; k != N; ++k) {
      cexp[k] = std::polar(1.0F, sign * TWOPI * static_cast<float>(k) / static_cast<float>(N));
    }

    for (unsigned k = 0; k != N; ++k) {
      for (unsigned n = 0; n != N; ++n) {
        cf_t w               = cexp[(k * n) % N];
        tables_re[N * k + n] = w.real();
        tables_im[N * k + n] = w.imag();
      }
    }

    for (unsigned k = 0; k != N; ++k) {
      cf_t w       = std::polar(1.0F, sign * TWOPI * static_cast<float>(k) / static_cast<float>(2 * N));
      tables_re[k] = w.real();
      tables_im[k] = w.imag();
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    simd_cf_t p_simd = srsran_simd_cf_set1(*in);
    in += stride;
    simd_cf_t q_simd = srsran_simd_cf_set1(*in);
    in += stride;
    for (unsigned n = 1; n != N; ++n) {
      simd_cf_t p_in_simd = srsran_simd_cf_set1(*in);
      in += stride;
      simd_cf_t q_in_simd = srsran_simd_cf_set1(*in);
      in += stride;
      simd_cf_t w_simd = srsran_simd_cf_load(&tables_re[N * n], &tables_im[N * n]);

      p_simd += p_in_simd * w_simd;
      q_simd += q_in_simd * w_simd;
    }

    q_simd = q_simd * srsran_simd_cf_load(tables_re.data(), tables_im.data());

    srsran_simd_cfi_storeu(out, p_simd + q_simd);
    srsran_simd_cfi_storeu(out + N, p_simd - q_simd);
  }
};

#endif // (2 * SRSRAN_SIMD_CF_SIZE) > 4

// Implements a DFT of size 9.
template <>
class generic_dft_dit<9, void> : public generic_dft_N
{
private:
  unsigned             stride;
  std::array<cf_t, 81> tables;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_)
  {
    std::array<cf_t, 9> cexp;
    for (unsigned k = 0; k != 9; ++k) {
      cexp[k] = std::polar(1.0F, sign * TWOPI * static_cast<float>(k) / 9.0F);
    }

    for (unsigned k = 0; k != 9; ++k) {
      for (unsigned n = 0; n != 9; ++n) {
        tables[9 * k + n] = cexp[(k * n) % 9];
      }
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    cf_t sum = 0;
    for (unsigned n = 0; n != 9; ++n) {
      sum += in[stride * n];
    }
    out[0] = sum;

    for (unsigned k = 1; k != 5; ++k) {
      const cf_t* table = &tables[9 * k];

      cf_t sum0 = in[0];
      cf_t sum1 = in[0];

      for (unsigned n = 1; n != 9; ++n) {
        float prod_rere = in[stride * n].real() * table[n].real();
        float prod_imim = in[stride * n].imag() * table[n].imag();
        float prod_reim = in[stride * n].real() * table[n].imag();
        float prod_imre = in[stride * n].imag() * table[n].real();
        sum0 += cf_t(prod_rere - prod_imim, prod_imre + prod_reim);
        sum1 += cf_t(prod_rere + prod_imim, prod_imre - prod_reim);
      }

      out[k]     = sum0;
      out[9 - k] = sum1;
    }
  }
};

} // namespace

#define CREATE_GENERIC_DFT_DIT(SIZE)                                                                                   \
  do {                                                                                                                 \
    if (input.size() == (SIZE)) {                                                                                      \
      generic_dft = std::make_unique<generic_dft_dit<SIZE>>(sign, 1);                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (false)

dft_processor_generic_impl::dft_processor_generic_impl(const configuration& dft_config) :
  dir(dft_config.dir), input(dft_config.size), output(dft_config.size)
{
  float sign = (dir == dft_processor::direction::DIRECT) ? -1 : +1;

  CREATE_GENERIC_DFT_DIT(12);
  CREATE_GENERIC_DFT_DIT(24);
  CREATE_GENERIC_DFT_DIT(36);
  CREATE_GENERIC_DFT_DIT(48);
  CREATE_GENERIC_DFT_DIT(60);
  CREATE_GENERIC_DFT_DIT(72);
  CREATE_GENERIC_DFT_DIT(96);
  CREATE_GENERIC_DFT_DIT(108);
  CREATE_GENERIC_DFT_DIT(120);
  CREATE_GENERIC_DFT_DIT(128);
  CREATE_GENERIC_DFT_DIT(144);
  CREATE_GENERIC_DFT_DIT(180);
  CREATE_GENERIC_DFT_DIT(192);
  CREATE_GENERIC_DFT_DIT(216);
  CREATE_GENERIC_DFT_DIT(240);
  CREATE_GENERIC_DFT_DIT(256);
  CREATE_GENERIC_DFT_DIT(288);
  CREATE_GENERIC_DFT_DIT(300);
  CREATE_GENERIC_DFT_DIT(324);
  CREATE_GENERIC_DFT_DIT(360);
  CREATE_GENERIC_DFT_DIT(384);
  CREATE_GENERIC_DFT_DIT(432);
  CREATE_GENERIC_DFT_DIT(480);
  CREATE_GENERIC_DFT_DIT(512);
  CREATE_GENERIC_DFT_DIT(540);
  CREATE_GENERIC_DFT_DIT(576);
  CREATE_GENERIC_DFT_DIT(600);
  CREATE_GENERIC_DFT_DIT(648);
  CREATE_GENERIC_DFT_DIT(720);
  CREATE_GENERIC_DFT_DIT(768);
  CREATE_GENERIC_DFT_DIT(864);
  CREATE_GENERIC_DFT_DIT(900);
  CREATE_GENERIC_DFT_DIT(960);
  CREATE_GENERIC_DFT_DIT(972);
  CREATE_GENERIC_DFT_DIT(1024);
  CREATE_GENERIC_DFT_DIT(1080);
  CREATE_GENERIC_DFT_DIT(1152);
  CREATE_GENERIC_DFT_DIT(1200);
  CREATE_GENERIC_DFT_DIT(1296);
  CREATE_GENERIC_DFT_DIT(1440);
  CREATE_GENERIC_DFT_DIT(1500);
  CREATE_GENERIC_DFT_DIT(1536);
  CREATE_GENERIC_DFT_DIT(1620);
  CREATE_GENERIC_DFT_DIT(1728);
  CREATE_GENERIC_DFT_DIT(1800);
  CREATE_GENERIC_DFT_DIT(1920);
  CREATE_GENERIC_DFT_DIT(1944);
  CREATE_GENERIC_DFT_DIT(2048);
  CREATE_GENERIC_DFT_DIT(2160);
  CREATE_GENERIC_DFT_DIT(2304);
  CREATE_GENERIC_DFT_DIT(2400);
  CREATE_GENERIC_DFT_DIT(2592);
  CREATE_GENERIC_DFT_DIT(2700);
  CREATE_GENERIC_DFT_DIT(2880);
  CREATE_GENERIC_DFT_DIT(2916);
  CREATE_GENERIC_DFT_DIT(3000);
  CREATE_GENERIC_DFT_DIT(3072);
  CREATE_GENERIC_DFT_DIT(3240);
  CREATE_GENERIC_DFT_DIT(4096);
  CREATE_GENERIC_DFT_DIT(4608);
  CREATE_GENERIC_DFT_DIT(6144);
  CREATE_GENERIC_DFT_DIT(8192);
  CREATE_GENERIC_DFT_DIT(9216);
  CREATE_GENERIC_DFT_DIT(12288);
  CREATE_GENERIC_DFT_DIT(18432);
  CREATE_GENERIC_DFT_DIT(24576);
  CREATE_GENERIC_DFT_DIT(36864);
  CREATE_GENERIC_DFT_DIT(49152);
  CREATE_GENERIC_DFT_DIT(98304);
}

span<const cf_t> dft_processor_generic_impl::run()
{
  srsran_assert(generic_dft, "DFT was not created.");

  generic_dft->run(output.data(), input.data());

  // Return the view of the output data.
  return output;
}
