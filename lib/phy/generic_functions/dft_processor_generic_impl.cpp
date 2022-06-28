
#include "dft_processor_generic_impl.h"
#include "srsgnb/support/math_utils.h"

#define HAVE_SSE
#include "../../srsvec/simd.h"

using namespace srsgnb;

template <unsigned N>
class generic_ditfft_N : public generic_fft_N
{
private:
  unsigned                stride;
  generic_ditfft_N<N / 2> radix2;
  std::array<cf_t, N>     table;

public:
  generic_ditfft_N(float sign, unsigned stride_) : stride(stride_), radix2(sign, 2 * stride)
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

template <>
class generic_ditfft_N<3> : public generic_fft_N
{
private:
  unsigned            stride;
  std::array<cf_t, 3> table;

public:
  generic_ditfft_N(float sign, unsigned stride_) : stride(stride_)
  {
    for (unsigned idx = 0; idx != 3; ++idx) {
      table[idx] = std::exp(COMPLEX_I * sign * TWOPI * static_cast<float>(idx) / 3.0f);
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    cf_t a0 = in[0];
    cf_t a1 = in[stride];
    cf_t a2 = in[2 * stride];

    out[0] = a0 + a1 + a2;
    out[1] = a0 + table[1] * a1 + table[2] * a2;
    out[2] = a0 + table[2] * a1 + table[1] * a2;
  }
};

template <>
class generic_ditfft_N<4> : public generic_fft_N
{
private:
  float    sign;
  unsigned stride;

public:
  generic_ditfft_N(float sign_, unsigned stride_) : sign(sign_), stride(stride_) {}

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

template <>
class generic_ditfft_N<9> : public generic_fft_N
{
private:
  unsigned            stride;
  std::array<cf_t, 9> table;

public:
  generic_ditfft_N(float sign, unsigned stride_) : stride(stride_)
  {
    for (unsigned idx = 0; idx != 9; ++idx) {
      table[idx] = std::exp(COMPLEX_I * sign * TWOPI * static_cast<float>(idx) / 9.0f);
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    cf_t a0 = in[0];
    cf_t a1 = in[stride];
    cf_t a2 = in[2 * stride];
    cf_t a3 = in[3 * stride];
    cf_t a4 = in[4 * stride];
    cf_t a5 = in[5 * stride];
    cf_t a6 = in[6 * stride];
    cf_t a7 = in[7 * stride];
    cf_t a8 = in[8 * stride];

    cf_t a0t0 = a0 * table[0];
    cf_t a3t0 = a3 * table[0];
    cf_t a6t0 = a6 * table[0];
    cf_t a3t3 = a3 * table[3];
    cf_t a3t6 = a3 * table[6];
    cf_t a6t3 = a6 * table[3];
    cf_t a6t6 = a6 * table[6];

    out[0] = a0t0 + a1 * table[0] + a2 * table[0] + a3t0 + a4 * table[0] + a5 * table[0] + a6t0 + a7 * table[0] +
             a8 * table[0];
    out[1] = a0t0 + a1 * table[1] + a2 * table[2] + a3t3 + a4 * table[4] + a5 * table[5] + a6t6 + a7 * table[7] +
             a8 * table[8];
    out[2] = a0t0 + a1 * table[2] + a2 * table[4] + a3t6 + a4 * table[8] + a5 * table[1] + a6t3 + a7 * table[5] +
             a8 * table[7];
    out[3] = a0t0 + a1 * table[3] + a2 * table[6] + a3t0 + a4 * table[3] + a5 * table[6] + a6t0 + a7 * table[3] +
             a8 * table[6];
    out[4] = a0t0 + a1 * table[4] + a2 * table[8] + a3t3 + a4 * table[7] + a5 * table[2] + a6t6 + a7 * table[1] +
             a8 * table[5];
    out[5] = a0t0 + a1 * table[5] + a2 * table[1] + a3t0 * table[6] + a4 * table[2] + a5 * table[7] + a6t3 +
             a7 * table[8] + a8 * table[4];
    out[6] = a0t0 + a1 * table[6] + a2 * table[3] + a3t0 + a4 * table[6] + a5 * table[3] + a6t0 + a7 * table[6] +
             a8 * table[3];
    out[7] = a0t0 + a1 * table[7] + a2 * table[5] + a3t3 + a4 * table[1] + a5 * table[8] + a6t6 + a7 * table[4] +
             a8 * table[2];
    out[8] = a0t0 + a1 * table[8] + a2 * table[7] + a3t6 + a4 * table[5] + a5 * table[4] + a6t3 + a7 * table[2] +
             a8 * table[1];
  }
};

template <unsigned N>
class generic_N : public generic_fft_N
{
private:
  float                   sign;
  std::array<cf_t, N * N> tables;

public:
  generic_N(float sign_) : sign(sign_)
  {
    for (unsigned k = 0; k != N; ++k) {
      for (unsigned n = 0; n != N; ++n) {
        tables[N * k + n] = std::exp(COMPLEX_I * sign * TWOPI * static_cast<float>(k * n) / static_cast<float>(N));
      }
    }
  }

  void run(cf_t* out, const cf_t* in) const override
  {
    for (unsigned k = 0; k != N; ++k) {
      cf_t sum = 0.0F;

      unsigned               n     = 0;
      const span<const cf_t> table = {&tables[N * k], N};

#if SRSRAN_SIMD_CF_SIZE
      simd_cf_t simd_sum = srsran_simd_cf_zero();
      for (; n != (N / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; n += SRSRAN_SIMD_CF_SIZE) {
        simd_cf_t simd_in    = srsran_simd_cfi_loadu(&in[n]);
        simd_cf_t simd_table = srsran_simd_cfi_loadu(&table[n]);
        simd_cf_t simd_prod  = srsran_simd_cf_prod(simd_in, simd_table);
        simd_sum             = srsran_simd_cf_add(simd_sum, simd_prod);
      }
      std::array<cf_t, SRSRAN_SIMD_CF_SIZE> simd_sum_vector;
      srsran_simd_cfi_storeu(simd_sum_vector.data(), simd_sum);
      for (unsigned i = 0; i != SRSRAN_SIMD_CF_SIZE; ++i) {
        sum += simd_sum_vector[i];
      }
#endif // SRSRAN_SIMD_CF_SIZE
      for (; n != N; ++n) {
        sum += in[n] * table[n];
      }

      out[k] = sum;
    }
  }
};

#define GENERIC_CREATE_SIZE(SIZE)                                                                                      \
  do {                                                                                                                 \
    if (input.size() == (SIZE)) {                                                                                      \
      generic_fft = std::make_unique<generic_ditfft_N<SIZE>>(sign, 1);                                                 \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (false)

dft_processor_generic_impl::dft_processor_generic_impl(const configuration& dft_config) :
  dir(dft_config.dir), input(dft_config.size), output(dft_config.size)
{
  float sign = (dir == dft_processor::direction::DIRECT) ? -1 : +1;

  GENERIC_CREATE_SIZE(128);
  GENERIC_CREATE_SIZE(256);
  GENERIC_CREATE_SIZE(384);
  GENERIC_CREATE_SIZE(512);
  GENERIC_CREATE_SIZE(768);
  GENERIC_CREATE_SIZE(1024);
  GENERIC_CREATE_SIZE(1536);
  GENERIC_CREATE_SIZE(2048);
  GENERIC_CREATE_SIZE(3072);
  GENERIC_CREATE_SIZE(4096);
  GENERIC_CREATE_SIZE(4608);
  GENERIC_CREATE_SIZE(6144);
  GENERIC_CREATE_SIZE(9216);
  GENERIC_CREATE_SIZE(12288);
  GENERIC_CREATE_SIZE(18432);
  GENERIC_CREATE_SIZE(24576);
  GENERIC_CREATE_SIZE(36864);
  GENERIC_CREATE_SIZE(49152);

  if (input.size() == 139) {
    generic_fft = std::make_unique<generic_N<139>>(sign);
    return;
  }
  if (input.size() == 839) {
    generic_fft = std::make_unique<generic_N<839>>(sign);
    return;
  }
}

span<const cf_t> dft_processor_generic_impl::run()
{
  srsran_assert(generic_fft, "DFT was not created.");

  generic_fft->run(output.data(), input.data());

  // Return the view of the output data.
  return output;
}
