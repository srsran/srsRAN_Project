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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_test.h"
#include <cmath>
#include <getopt.h>
#include <memory>
#include <random>

using namespace srsran;

// Random generator.
static std::mt19937 rgen(0);

// Maximum allowed mean squared error.
static float ASSERT_MAX_MSE = 1e-6;
// Maximum allowed peak error.
static float ASSERT_MAX_ERROR = 1e-3;

static void ditfft(span<cf_t> out, span<const cf_t> in, span<const cf_t> table, unsigned N, unsigned s)
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
  srsran_assert(!input.empty(), "Empty input span");
  srsran_assert(input.size() == output.size(), "Input and output span sizes must be identical");

  // Derive parameters.
  unsigned size = input.size();
  float    sign = (direction == dft_processor::direction::DIRECT) ? -1 : +1;
  float    N    = static_cast<float>(size);

  // Create exponential to avoid abusing std::exp.
  std::vector<cf_t> exp(size);
  for (unsigned idx = 0; idx != size; ++idx) {
    exp[idx] = std::polar(1.0F, sign * TWOPI * static_cast<float>(idx) / N);
  }

  // Compute theoretical discrete fourier transform.
  ditfft(output, input, exp, N, 1);
}

namespace srsran {
static bool operator==(span<const cf_t> left, span<const cf_t> right)
{
  unsigned size = left.size();

  // Temporary vector to store the difference.
  std::vector<cf_t> diff(left.size());

  // Calculate difference between the expected output and the actual output.
  for (unsigned idx = 0; idx != size; ++idx) {
    diff[idx] = left[idx] - right[idx];
  }

  // Mean square error.
  float mse = srsvec::average_power(diff);

  // Maximum error.
  std::pair<unsigned, float> max_abs_error = srsvec::max_abs_element(diff);

  // Vectors are equal only if
  return (mse < ASSERT_MAX_MSE) && (max_abs_error.second < ASSERT_MAX_ERROR);
}

} // namespace srsran

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
  std::uniform_real_distribution<float> dist(-M_PI, +M_PI);

  std::shared_ptr<dft_processor_factory> dft_factory = nullptr;
  if (dft_factory_str == "generic") {
    dft_factory = create_dft_processor_factory_generic();
  } else if (dft_factory_str == "fftw") {
    dft_factory = create_dft_processor_factory_fftw_slow();
  } else {
    fmt::print("Invalid DFT factory.");
    return -1;
  }
  TESTASSERT(dft_factory, "DFT factory of type {} is not available.", dft_factory_str);

  // Test for the most common DFT sizes
  for (unsigned size :
       {128, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 4608, 6144, 9216, 12288, 18432, 24576, 36864, 49152}) {
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

      // For each repetition...
      for (unsigned repetition = 0; repetition != nof_repetitions; ++repetition) {
        // Generate input random data.
        for (cf_t& value : input) {
          value = std::polar(1.0F, dist(rgen));
        }

        // Run DFT.
        span<const cf_t> output = dft->run();

        // Run expected DFT
        run_expected_dft(expected_output, direction, input);

        // Make sure the output matches the expected within the tolerances.
        TESTASSERT_EQ(span<const cf_t>(expected_output), span<const cf_t>(output));
      }
    }
  }

  return 0;
}
