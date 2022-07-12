
#include "dft_processor_generic_impl.h"
#include "srsgnb/support/math_utils.h"
#include <numeric>

#include "../../srsvec/simd.h"

using namespace srsgnb;

// Implements a DFT of size N.
//
// The implementation follows a radix-2 decimation-in-time algorithm that rearranges the DFT equation into two parts: a
// sum over the even-numbered discrete-time indices n={0,2,4,...,N−2} and a sum over the odd-numbered indices
// n={1,3,5,...,N−1}
//
// It relies on templates for optimization purposes.
template <unsigned N>
class generic_dft_dit : public generic_dft_N
{
private:
  unsigned               stride;
  generic_dft_dit<N / 2> radix2;
  std::array<cf_t, N>    table;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_), radix2(sign, 2 * stride)
  {
    for (unsigned idx = 0; idx != N; ++idx) {
      table[idx] = std::exp(COMPLEX_I * sign * TWOPI * static_cast<float>(idx) / static_cast<float>(N));
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

// Implements a DFT of size 3.
template <>
class generic_dft_dit<3> : public generic_dft_N
{
private:
  unsigned stride;
  cf_t     cexp_pi_3;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_)
  {
    cexp_pi_3 = std::exp(COMPLEX_I * sign * TWOPI / 3.0F);
  }

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
class generic_dft_dit<4> : public generic_dft_N
{
private:
  float    sign;
  unsigned stride;

public:
  generic_dft_dit(float sign_, unsigned stride_) : sign(sign_), stride(stride_) {}

  void run(cf_t* out, const cf_t* in) const override
  {
    cf_t a0 = in[0];
    cf_t a1 = in[stride];
    cf_t a2 = in[2 * stride];
    cf_t a3 = in[3 * stride];

    cf_t p0 = a0 + a2;
    cf_t q0 = a0 - a2;

    cf_t p1 = a1 + a3;
    cf_t q1 = (COMPLEX_I * sign) * (a1 - a3);

    out[0] = p0 + p1;
    out[1] = q0 + q1;
    out[2] = p0 - p1;
    out[3] = q0 - q1;
  }
};

// Implements a DFT of size 9.
template <>
class generic_dft_dit<9> : public generic_dft_N
{
private:
  unsigned             stride;
  std::array<cf_t, 81> tables;

public:
  generic_dft_dit(float sign, unsigned stride_) : stride(stride_)
  {
    std::array<cf_t, 9> cexp;
    for (unsigned k = 0; k != 9; ++k) {
      cexp[k] = std::exp(COMPLEX_I * sign * TWOPI * static_cast<float>(k) / 9.0F);
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

// Implements a DFT algorithm of size N. It is thought for N being a prime number.
template <unsigned N>
class generic_dft_prime : public generic_dft_N
{
private:
  static constexpr unsigned K = divide_ceil(N, 2);
  float                     sign;
  std::array<cf_t, K * N>   tables;

public:
  generic_dft_prime(float sign_) : sign(sign_)
  {
    std::array<cf_t, N> cexp;
    for (unsigned k = 0; k != N; ++k) {
      cexp[k] = std::exp(COMPLEX_I * sign * TWOPI * static_cast<float>(k) / static_cast<float>(N));
    }

    for (unsigned k = 0; k != K; ++k) {
      for (unsigned n = 0; n != N; ++n) {
        tables[N * k + n] = cexp[(k * n) % N];
      }
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    out[0] = std::accumulate(in, in + N, cf_t());

    for (unsigned k = 1; k != K; ++k) {
      const cf_t* table = &tables[N * k];

      cf_t sum0 = in[0];
      cf_t sum1 = in[0];

      unsigned n = 1;

#if SRSRAN_SIMD_CF_SIZE
      simd_cf_t simd_sum0 = srsran_simd_cf_zero();
      simd_cf_t simd_sum1 = srsran_simd_cf_zero();
      for (; n != ((N - 1) / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE + 1; n += SRSRAN_SIMD_CF_SIZE) {
        simd_cf_t simd_in    = srsran_simd_cfi_loadu(&in[n]);
        simd_cf_t simd_table = srsran_simd_cfi_loadu(&table[n]);

        simd_f_t prod_rere = srsran_simd_f_mul(simd_in.re, simd_table.re);
        simd_f_t prod_imim = srsran_simd_f_mul(simd_in.im, simd_table.im);
        simd_f_t prod_reim = srsran_simd_f_mul(simd_in.re, simd_table.im);
        simd_f_t prod_imre = srsran_simd_f_mul(simd_in.im, simd_table.re);

        simd_cf_t prod0 = {srsran_simd_f_sub(prod_rere, prod_imim), srsran_simd_f_add(prod_imre, prod_reim)};
        simd_cf_t prod1 = {srsran_simd_f_add(prod_rere, prod_imim), srsran_simd_f_sub(prod_imre, prod_reim)};

        simd_sum0 = srsran_simd_cf_add(simd_sum0, prod0);
        simd_sum1 = srsran_simd_cf_add(simd_sum1, prod1);
      }

      // Reduce sums.
      std::array<cf_t, SRSRAN_SIMD_CF_SIZE> simd_sum0_vector;
      std::array<cf_t, SRSRAN_SIMD_CF_SIZE> simd_sum1_vector;
      srsran_simd_cfi_storeu(simd_sum0_vector.data(), simd_sum0);
      srsran_simd_cfi_storeu(simd_sum1_vector.data(), simd_sum1);
      sum0 = std::accumulate(simd_sum0_vector.begin(), simd_sum0_vector.end(), sum0);
      sum1 = std::accumulate(simd_sum1_vector.begin(), simd_sum1_vector.end(), sum1);
#endif // SRSRAN_SIMD_CF_SIZE

      for (; n != N; ++n) {
        float prod_rere = in[n].real() * table[n].real();
        float prod_imim = in[n].imag() * table[n].imag();
        float prod_reim = in[n].real() * table[n].imag();
        float prod_imre = in[n].imag() * table[n].real();
        sum0 += cf_t(prod_rere - prod_imim, prod_imre + prod_reim);
        sum1 += cf_t(prod_rere + prod_imim, prod_imre - prod_reim);
      }

      out[k]     = sum0;
      out[N - k] = sum1;
    }
  }
};

#define GENERIC_EVEN_SIZE(SIZE)                                                                                        \
  do {                                                                                                                 \
    if (input.size() == (SIZE)) {                                                                                      \
      generic_dft = std::make_unique<generic_dft_dit<SIZE>>(sign, 1);                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (false)

#define GENERIC_ODD_SIZE(SIZE)                                                                                         \
  do {                                                                                                                 \
    if (input.size() == (SIZE)) {                                                                                      \
      generic_dft = std::make_unique<generic_dft_prime<SIZE>>(sign);                                                   \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (false)

dft_processor_generic_impl::dft_processor_generic_impl(const configuration& dft_config) :
  dir(dft_config.dir), input(dft_config.size), output(dft_config.size)
{
  float sign = (dir == dft_processor::direction::DIRECT) ? -1 : +1;

  GENERIC_EVEN_SIZE(128);
  GENERIC_EVEN_SIZE(256);
  GENERIC_EVEN_SIZE(384);
  GENERIC_EVEN_SIZE(512);
  GENERIC_EVEN_SIZE(768);
  GENERIC_EVEN_SIZE(1024);
  GENERIC_EVEN_SIZE(1536);
  GENERIC_EVEN_SIZE(2048);
  GENERIC_EVEN_SIZE(3072);
  GENERIC_EVEN_SIZE(4096);
  GENERIC_EVEN_SIZE(4608);
  GENERIC_EVEN_SIZE(6144);
  GENERIC_EVEN_SIZE(9216);
  GENERIC_EVEN_SIZE(12288);
  GENERIC_EVEN_SIZE(18432);
  GENERIC_EVEN_SIZE(24576);
  GENERIC_EVEN_SIZE(36864);
  GENERIC_EVEN_SIZE(49152);

  // Used for generating frequency-domain short PRACH sequence.
  GENERIC_ODD_SIZE(139);
  // Used for generating frequency-domain long PRACH sequence.
  GENERIC_ODD_SIZE(839);
}

span<const cf_t> dft_processor_generic_impl::run()
{
  srsran_assert(generic_dft, "DFT was not created.");

  generic_dft->run(output.data(), input.data());

  // Return the view of the output data.
  return output;
}
