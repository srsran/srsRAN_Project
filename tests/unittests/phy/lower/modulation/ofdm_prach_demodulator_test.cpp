/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofdm_prach_demodulator_test_data.h"
#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os,
             "srate={:.2f}MHz; Format={}; RBOffset={}; BW={}PRB; SCS={}kHz;",
             static_cast<double>(test_case.context.dft_size_15kHz * 15000) / 1e6,
             static_cast<unsigned>(test_case.context.config.format),
             test_case.context.config.rb_offset,
             test_case.context.config.nof_prb_ul_grid,
             scs_to_khz(test_case.context.config.pusch_scs));
  return os;
}

} // namespace srsgnb

using namespace srsgnb;

class ofdm_prach_demodulator_tester : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<ofdm_prach_demodulator> demodulator;

  void SetUp() override
  {
    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
    ASSERT_TRUE(dft_factory);

    unsigned dft_size_15kHz = GetParam().context.dft_size_15kHz;

    std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_factory =
        create_ofdm_prach_demodulator_factory_sw(dft_factory, dft_size_15kHz);
    ASSERT_TRUE(ofdm_factory);

    demodulator = ofdm_factory->create();
    ASSERT_TRUE(demodulator);
  }
};

TEST_P(ofdm_prach_demodulator_tester, vector)
{
  const test_case_t& test_case = GetParam();

  std::unique_ptr<prach_buffer> output =
      test_case.context.config.format.is_long_preamble() ? create_prach_buffer_long() : create_prach_buffer_short();
  unsigned sequence_length = output->get_sequence_length();

  std::vector<cf_t> input           = test_case.input.read();
  std::vector<cf_t> expected_output = test_case.output.read();

  // Make sure the expected output size is correct.
  ASSERT_EQ(expected_output.size() % sequence_length, 0);

  // Run demodulator.
  demodulator->demodulate(*output, input, GetParam().context.config);

  // Calculate amplitude correction.
  prach_preamble_information preamble_info  = get_prach_preamble_long_info(test_case.context.config.format);
  unsigned                   dft_size_prach = test_case.context.dft_size_15kHz * (15000 / preamble_info.scs.to_Hz());
  unsigned                   dft_size_pusch =
      test_case.context.dft_size_15kHz / pow2(to_numerology_value(test_case.context.config.pusch_scs));
  float scaling = std::sqrt(static_cast<float>(sequence_length * dft_size_pusch) / static_cast<float>(dft_size_prach));

  // For each PRACH symbol...
  for (unsigned symbol = 0, nof_symbols = expected_output.size() / sequence_length; symbol != nof_symbols; ++symbol) {
    span<const cf_t> expected_output_symbol =
        span<const cf_t>(expected_output).subspan(symbol * sequence_length, sequence_length);
    span<const cf_t> output_symbol = output->get_symbol(symbol);
    for (unsigned subc = 0; subc != sequence_length; ++subc) {
      EXPECT_NEAR(expected_output_symbol[subc].real(), output_symbol[subc].real() * scaling, 0.0001);
      EXPECT_NEAR(expected_output_symbol[subc].imag(), output_symbol[subc].imag() * scaling, 0.0001);
    }
  }
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(ofdm_prach_demodulator_vectortest,
                         ofdm_prach_demodulator_tester,
                         ::testing::ValuesIn(ofdm_prach_demodulator_test_data));
