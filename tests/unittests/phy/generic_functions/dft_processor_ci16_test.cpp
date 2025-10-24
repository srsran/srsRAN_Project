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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_test.h"
#include "fmt/ostream.h"
#include <cmath>
#include <gtest/gtest.h>
#include <memory>
#include <random>

using namespace srsran;

// Random generator.
static std::mt19937 rgen(0);

static std::set<unsigned> dft_required_sizes = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};

// Maximum allowed peak error.
static float ASSERT_MAX_ERROR = 10;

namespace srsran {

static bool operator==(span<const ci16_t> lhs, span<const ci16_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](ci16_t left, ci16_t right) {
    int16_t abs_err = std::abs(left - right);
    return abs_err < ASSERT_MAX_ERROR;
  });
}

std::ostream& operator<<(std::ostream& os, dft_processor_ci16::direction direction)
{
  return os << dft_processor_ci16::direction_to_string(direction);
}

std::ostream& operator<<(std::ostream& os, span<const ci16_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

} // namespace srsran

using DFTprocessorParams = std::tuple<std::string, unsigned, dft_processor_ci16::direction>;

class DFTprocessorFixture : public ::testing::TestWithParam<DFTprocessorParams>
{
protected:
  static constexpr unsigned             nof_repetitions = 10;
  std::uniform_real_distribution<float> dist            = std::uniform_real_distribution<float>(-M_PI, +M_PI);

  std::shared_ptr<dft_processor_ci16_factory> dft_factory = nullptr;

  void SetUp() override
  {
    // Call the required dft factory.
    const std::string& dft_factory_str = std::get<0>(GetParam());
    if (dft_factory_str == "avx2") {
      dft_factory = create_dft_processor_ci16_factory_avx2();
    }
  }

  static void ditfft(span<cf_t> out, span<const cf_t> in, span<const cf_t> table, unsigned N, unsigned s)
  {
    if (N == 1) {
      // FFT of size 1.
      out[0] = in[0];
    } else if (N % 2 == 0) {
      // Radix 2.
      ditfft(out.first(N / 2), in, table, N / 2, 2 * s);
      ditfft(out.last(N / 2), in.last(in.size() - s), table, N / 2, 2 * s);
      for (unsigned k = 0, k_end = N / 2; k != k_end; ++k) {
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

  static void run_expected_dft(span<ci16_t> output, dft_processor_ci16::direction direction, span<const ci16_t> input)
  {
    // Formal checks to avoid zero division among other failures.
    srsran_assert(!input.empty(), "Empty input span");
    srsran_assert(input.size() == output.size(), "Input and output span sizes must be identical");

    // Derive parameters.
    unsigned size = input.size();
    float    sign = (direction == dft_processor_ci16::direction::direct) ? -1 : +1;
    float    N    = static_cast<float>(size);

    std::vector<cf_t> output_cf(output.size());
    std::vector<cf_t> input_cf(input.size());

    std::transform(input.begin(), input.end(), input_cf.begin(), [](ci16_t sample) { return to_cf(sample); });

    // Create exponential to avoid abusing std::exp.
    std::vector<cf_t> exp(size);
    for (unsigned idx = 0; idx != size; ++idx) {
      exp[idx] = std::polar(1.0F, sign * TWOPI * static_cast<float>(idx) / N);
    }

    // Compute theoretical discrete Fourier transform.
    ditfft(output_cf, input_cf, exp, N, 1);

    float scaling = 1 / static_cast<float>(N);
    srsvec::sc_prod(output_cf, output_cf, scaling);

    std::transform(output_cf.begin(), output_cf.end(), output.begin(), [](cf_t sample) { return to_ci16(sample); });
  }
};

TEST_P(DFTprocessorFixture, DFTProcessorUnittest)
{
  unsigned                      dft_size  = std::get<1>(GetParam());
  dft_processor_ci16::direction direction = std::get<2>(GetParam());

  // Make sure the factory is valid.
  ASSERT_NE(dft_factory, nullptr);

  // Create DFT configuration;
  dft_processor_ci16::configuration config;
  config.size = dft_size;
  config.dir  = direction;

  // Create processor.
  std::unique_ptr<dft_processor_ci16> dft = dft_factory->create(config);

  // Skip test case silently if the DFT processor is not available.
  if (dft == nullptr) {
    GTEST_SKIP();
  }

  // Create DFT buffers
  std::vector<ci16_t> input(dft_size);
  std::vector<ci16_t> output(dft_size);
  std::vector<ci16_t> expected_output(dft_size);

  // For each repetition...
  for (unsigned repetition = 0; repetition != nof_repetitions; ++repetition) {
    // Generate input random data.
    std::generate(input.begin(), input.end(), [this]() {
      static constexpr float amplitude = pow2(12);
      return to_ci16(std::polar(amplitude, dist(rgen)));
    });

    // Run DFT.
    dft->run(output, input);

    // Run expected DFT
    run_expected_dft(expected_output, direction, input);

    // Make sure the output matches the expected within the tolerances.
    ASSERT_EQ(span<const ci16_t>(expected_output), span<const ci16_t>(output)) << fmt::format("input=[{}]", input);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(DFTProcessorTest,
                         DFTprocessorFixture,
                         ::testing::Combine(::testing::Values("avx2"),
                                            ::testing::ValuesIn(dft_required_sizes),
                                            ::testing::Values(dft_processor_ci16::direction::direct,
                                                              dft_processor_ci16::direction::inverse)));
