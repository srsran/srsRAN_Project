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

#include "dft_processor_test_data.h"
#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/srsvec/copy.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

// Maximum allowed error.
static constexpr float ASSERT_MAX_ERROR = 4e-3;

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os,
             "size={} direction={}",
             test_case.config.size,
             test_case.config.dir == dft_processor::direction::DIRECT ? "direct" : "inverse");
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

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

} // namespace srsran

using DFTprocessorParams = std::tuple<std::string, test_case_t>;

class DFTprocessorFixture : public ::testing::TestWithParam<DFTprocessorParams>
{
protected:
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
};

TEST_P(DFTprocessorFixture, DFTProcessorUnittest)
{
  const test_case_t& test_case = std::get<1>(GetParam());

  // Make sure the factory is valid.
  ASSERT_NE(dft_factory, nullptr);

  // Load the input data.
  std::vector<cf_t> input = test_case.data.read();

  // Load the golden data.
  std::vector<cf_t> transform = test_case.transform.read();

  // Create DFT processor configuration.
  dft_processor::configuration config = test_case.config;

  // Create processor.
  std::unique_ptr<dft_processor> dft = dft_factory->create(config);

  // Provide the input data to the DFT.
  srsvec::copy(dft->get_input(), input);

  // Run DFT.
  span<const cf_t> dft_output = dft->run();

  // Assert shared channel data matches.
  ASSERT_EQ(span<const cf_t>(transform), dft_output);
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(DFTprocessorVectorTest,
                         DFTprocessorFixture,
                         ::testing::Combine(::testing::Values("generic"
#ifdef ENABLE_FFTW
                                                              ,
                                                              "fftw"
#endif // ENABLE_FFTW
                                                              ),
                                            ::testing::ValuesIn(dft_processor_test_data)));
