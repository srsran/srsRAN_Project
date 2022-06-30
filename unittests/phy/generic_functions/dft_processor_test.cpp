/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/generic_functions/generic_functions_factories.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsgnb_test.h"
#include <cmath>
#include <complex>
#include <getopt.h>
#include <memory>
#include <random>

using namespace srsgnb;

// Random generator.
static std::mt19937 rgen(0);

// Maximum allowed error.
static float ASSERT_MAX_ERROR = 4e-6;

void ditfft(span<cf_t> out, span<const cf_t> in, span<const cf_t> table, unsigned N, unsigned s)
{
  if (N == 1) {
    // FFT of size 1.
    out[0] = in[0];
  } else if (N % 2 == 0) {
    // Radix 2.
    ditfft(out.first(N / 2), in, table, N / 2, 2 * s);
    ditfft(out.last(N / 2), in.last(in.size() - s), table, N / 2, 2 * s);
    for (unsigned k = 0; k != N / 2; ++k) {
      cf_t p         = out[k];
      cf_t q         = table[k * (table.size() / N)] * out[k + N / 2];
      out[k]         = p + q;
      out[k + N / 2] = p - q;
    }
  } else {
    // FFT of any size N and stride s.
    for (unsigned k = 0; k != N; ++k) {
      cf_t sum = 0;
      for (unsigned n = 0; n != N; ++n) {
        sum += table[(s * n * k) % table.size()] * in[n * s];
      }
      out[k] = sum;
    }
  }
}

static void run_expected_dft(span<cf_t> output, dft_processor::direction direction, span<const cf_t> input)
{
  // Formal checks to avoid zero division among other failures.
  assert(!input.empty());
  assert(input.size() == output.size());

  // Derive parameters.
  unsigned size = input.size();
  float    sign = (direction == dft_processor::direction::DIRECT) ? -1 : +1;
  float    N    = static_cast<float>(size);

  // Create exponential to avoid abusing std::exp.
  std::vector<cf_t> exp(size);
  for (unsigned idx = 0; idx != size; ++idx) {
    exp[idx] = std::exp(COMPLEX_I * sign * TWOPI * static_cast<float>(idx) / N);
  }

  // Compute theoretical discrete fourier transform.
  ditfft(output, input, exp, N, 1);
}

static std::string dft_factory_str = "generic";
static unsigned    nof_repetitions = 10;

void usage(const char* prog)
{
  fmt::print("Usage: {} [-F DFT factory] [-R repetitions]\n", prog);
  fmt::print("\t-F Select DFT factory [Default {}]\n", dft_factory_str);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-h Show this message\n");
}

void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "F:R:h")) != -1) {
    switch (opt) {
      case 'F':
        dft_factory_str = std::string(optarg);
        break;
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);
  std::uniform_real_distribution<float> dist(-1.0, +1.0);

  std::shared_ptr<dft_processor_factory> dft_factory = nullptr;
  if (dft_factory_str == "generic") {
    dft_factory = create_dft_processor_factory_generic();
  } else if (dft_factory_str == "fftx") {
    dft_factory = create_dft_processor_factory_fftx();
  } else if (dft_factory_str == "fftw") {
    dft_factory = create_dft_processor_factory_fftw();
  } else {
    fmt::print("Invalid DFT factory.");
    return -1;
  }
  TESTASSERT(dft_factory, "DFT factory of type {} is not available.", dft_factory_str);

  // Test for the most common DFT sizes
  for (unsigned size : {128,  139,  256,  384,  512,  768,   839,   1024,  1536,  2048,
                        3072, 4096, 4608, 6144, 9216, 12288, 18432, 24576, 36864, 49152}) {
    for (dft_processor::direction direction : {dft_processor::direction::DIRECT, dft_processor::direction::INVERSE}) {
      // Create FFTW configuration;
      dft_processor::configuration config;
      config.size = size;
      config.dir  = direction;

      // Create processor
      std::unique_ptr<dft_processor> dft = dft_factory->create(config);

      // Skip test case silently if it is not available.
      if (dft == nullptr) {
        continue;
      }

      // Get DFT input buffer
      span<cf_t> input = dft->get_input();

      // Allocate golden output buffer.
      std::vector<cf_t> expected_output(size);

      // Maximum error for this size and direction.
      float max_error = 0.0F;

      // For each repetition...
      for (unsigned repetition = 0; repetition != nof_repetitions; ++repetition) {
        // Generate input random data.
        for (cf_t& value : input) {
          value = {dist(rgen), dist(rgen)};
        }

        // Run DFT.
        span<const cf_t> output = dft->run();

        // Run expected DFT
        run_expected_dft(expected_output, direction, input);

        // Assert output.
        for (unsigned idx = 0; idx != size; ++idx) {
          // Calculate absolute error normalised by the square root of the size.
          float error = std::abs(expected_output[idx] - output[idx]) / std::sqrt(size);

          // Update maximum error.
          max_error = std::max(max_error, error);
        }
      }

      // Set the next line to 1 for printing the maximum error for each case.
#if 0
      printf(
          "size=%d; dir=%s; max_error=%f;\n", size, dft_processor::direction_to_string(direction).c_str(), max_error);
#endif

      // Actual assertion.
      TESTASSERT(max_error < ASSERT_MAX_ERROR,
                 "Error {} exceeds maximum {} for {} DFT size {}.",
                 max_error,
                 ASSERT_MAX_ERROR,
                 dft_processor::direction_to_string(direction),
                 size);
    }
  }

  return 0;
}
