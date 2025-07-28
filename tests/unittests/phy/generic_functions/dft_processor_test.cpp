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
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"
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

static std::set<unsigned> dft_required_sizes = []() {
  // Initialize the DFT required sizes with the most common ones.
  std::set<unsigned> dft_sizes = {
      128, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 4608, 6144, 9216, 12288, 18432, 24576, 36864, 49152};

  // Append transform precoding sizes.
  span<const bool> transform_precoding_nof_prb = transform_precoding::get_valid_nof_prbs();
  for (auto it = std::find(transform_precoding_nof_prb.begin(), transform_precoding_nof_prb.end(), true);
       it != transform_precoding_nof_prb.end();
       it = std::find(it + 1, transform_precoding_nof_prb.end(), true)) {
    unsigned nof_prb = std::distance(transform_precoding_nof_prb.begin(), it);

    dft_sizes.emplace(nof_prb * NOF_SUBCARRIERS_PER_RB);
  }

  return dft_sizes;
}();

// Maximum allowed peak error.
static float ASSERT_MAX_ERROR = 1e-3;

namespace srsran {

static bool operator==(span<const cf_t> transform, span<const cf_t> dft_output)
{
  auto length = static_cast<float>(transform.size());
  return std::equal(transform.begin(),
                    transform.end(),
                    dft_output.begin(),
                    dft_output.end(),
                    [length](cf_t transform_val, cf_t dft_output_val) {
                      return (std::abs(transform_val - dft_output_val) / std::sqrt(length) <= ASSERT_MAX_ERROR);
                    });
}

std::ostream& operator<<(std::ostream& os, dft_processor::direction direction)
{
  return os << dft_processor::direction_to_string(direction);
}

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

} // namespace srsran

using DFTprocessorParams = std::tuple<std::string, unsigned, dft_processor::direction>;

class DFTprocessorFixture : public ::testing::TestWithParam<DFTprocessorParams>
{
protected:
  static constexpr unsigned             nof_repetitions = 10;
  std::uniform_real_distribution<float> dist            = std::uniform_real_distribution<float>(-M_PI, +M_PI);

  std::shared_ptr<dft_processor_factory> dft_factory = nullptr;

  void SetUp() override
  {
    // Call the required dft factory.
    const std::string& dft_factory_str = std::get<0>(GetParam());
    if (dft_factory_str == "generic") {
      dft_factory = create_dft_processor_factory_generic();
    } else if (dft_factory_str == "fftw") {
      dft_factory = create_dft_processor_factory_fftw_slow();
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
};

TEST_P(DFTprocessorFixture, DFTProcessorUnittest)
{
  unsigned                 dft_size  = std::get<1>(GetParam());
  dft_processor::direction direction = std::get<2>(GetParam());

  // Make sure the factory is valid.
  ASSERT_NE(dft_factory, nullptr);

  // Create DFT configuration;
  dft_processor::configuration config;
  config.size = dft_size;
  config.dir  = direction;

  // Create processor.
  std::unique_ptr<dft_processor> dft = dft_factory->create(config);

  // Skip test case silently if the DFT processor is not available.
  if (dft == nullptr) {
    GTEST_SKIP();
  }

  // Get DFT input buffer
  span<cf_t> input = dft->get_input();

  // Allocate golden output buffer.
  std::vector<cf_t> expected_output(dft_size);

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
    ASSERT_EQ(span<const cf_t>(expected_output), span<const cf_t>(output));
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(DFTProcessorTest,
                         DFTprocessorFixture,
                         ::testing::Combine(::testing::Values("generic"
#ifdef HAVE_FFTW
                                                              ,
                                                              "fftw"
#endif // HAVE_FFTW
                                                              ),
                                            ::testing::ValuesIn(dft_required_sizes),
                                            ::testing::Values(dft_processor::direction::DIRECT,
                                                              dft_processor::direction::INVERSE)));
