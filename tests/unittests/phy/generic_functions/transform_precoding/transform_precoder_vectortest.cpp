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

#include "transform_precoder_test_data.h"
#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoding_factories.h"
#include <gtest/gtest.h>
#include <srsran/phy/constants.h>

using namespace srsran;

namespace srsran {

bool operator==(span<const cf_t> lhs, span<const cf_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](cf_t left, cf_t right) {
    return std::abs(left - right) < 1e-6;
  });
}

std::ostream& operator<<(std::ostream& os, span<const cbf16_t> data)
{
  return os << fmt::format("{}", data);
}

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  return os << fmt::format("{} {} {}",
                           test_case.M_rb,
                           test_case.deprecode_input.get_file_name(),
                           test_case.deprecode_output.get_file_name());
}

} // namespace srsran

namespace {

class TransformPrecodingFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  void SetUp() override
  {
    if (!precoder) {
      std::shared_ptr<dft_processor_factory> dft_proc_factory = create_dft_processor_factory_fftw_slow();
      ASSERT_TRUE(dft_proc_factory);

      std::shared_ptr<transform_precoder_factory> precoder_factory =
          create_dft_transform_precoder_factory(std::move(dft_proc_factory), MAX_RB);
      ASSERT_TRUE(precoder_factory);

      precoder = precoder_factory->create();
      ASSERT_TRUE(precoder);
    }
  }

  static std::unique_ptr<transform_precoder> precoder;
};

std::unique_ptr<transform_precoder> TransformPrecodingFixture::precoder = nullptr;

} // namespace

TEST_P(TransformPrecodingFixture, FromVector)
{
  const test_case_t& test_case = GetParam();

  // Extract number of resource blocks.
  unsigned M_rb = test_case.M_rb;
  unsigned M_sc = M_rb * NRE;

  // Read precoder inputs and outputs.
  std::vector<cf_t> deprecode_input    = test_case.deprecode_input.read();
  std::vector<cf_t> deprecode_expected = test_case.deprecode_output.read();
  ASSERT_EQ(deprecode_input.size(), deprecode_expected.size());

  std::vector<cf_t> deprecode_output(M_sc);

  unsigned nof_ofdm_symbols = deprecode_input.size() / M_sc;
  for (unsigned i_ofdm_symbol = 0; i_ofdm_symbol != nof_ofdm_symbols; ++i_ofdm_symbol) {
    span<const cf_t> expected = span<const cf_t>(deprecode_expected).subspan(i_ofdm_symbol * M_sc, M_sc);
    span<cf_t>       input    = span<cf_t>(deprecode_input).subspan(i_ofdm_symbol * M_sc, M_sc);
    span<cf_t>       output   = span<cf_t>(deprecode_output);
    precoder->deprecode_ofdm_symbol(output, input);

    ASSERT_EQ(expected, span<const cf_t>(output));
  }
}

INSTANTIATE_TEST_SUITE_P(FromVector, TransformPrecodingFixture, ::testing::ValuesIn(transform_precoder_test_data));
