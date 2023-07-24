/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../../support/prach_buffer_test_doubles.h"
#include "ofdm_prach_demodulator_test_data.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/sc_prod.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

static float ASSERT_MAX_ERROR = 1e-3;

static std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

static bool operator==(span<const cf_t> lhs, span<const cf_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](cf_t lhs_val, cf_t rhs_val) {
    return (std::abs(lhs_val - rhs_val) <= ASSERT_MAX_ERROR);
  });
}

std::ostream& operator<<(std::ostream& os, const ofdm_prach_demodulator::configuration& config)
{
  fmt::print(os,
             "Format={}; nof_td_occasions={}; nof_fd_occasions={}; start_symbol={}; rb_offset={}; "
             "nof_prb_ul_grid={}; pusch_scs={};",
             to_string(config.format),
             config.nof_td_occasions,
             config.nof_fd_occasions,
             config.start_symbol,
             config.rb_offset,
             config.nof_prb_ul_grid,
             to_string(config.pusch_scs));
  return os;
}

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os, "srate={}; {}", test_case.context.srate, test_case.context.config);
  return os;
}

} // namespace srsran

using namespace srsran;

class ofdm_prach_demodulator_tester : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<ofdm_prach_demodulator> demodulator;

  void SetUp() override
  {
    sampling_rate srate = GetParam().context.srate;

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
    ASSERT_TRUE(dft_factory);

    std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_factory =
        create_ofdm_prach_demodulator_factory_sw(dft_factory, srate);
    ASSERT_TRUE(ofdm_factory);

    demodulator = ofdm_factory->create();
    ASSERT_TRUE(demodulator);
  }
};

TEST_P(ofdm_prach_demodulator_tester, vector)
{
  const test_case_t&                           test_case = GetParam();
  const ofdm_prach_demodulator::configuration& config    = test_case.context.config;

  bool                          long_preamble = is_long_preamble(config.format);
  std::unique_ptr<prach_buffer> output =
      long_preamble ? create_prach_buffer_long(1, config.nof_fd_occasions)
                    : create_prach_buffer_short(1, config.nof_td_occasions, config.nof_fd_occasions);

  // Read input waveform.
  std::vector<cf_t> input = test_case.input.read();

  // Read raw expected output.
  std::vector<cf_t> expected_output = test_case.output.read();

  // Select preamble information.
  prach_preamble_information preamble_info =
      long_preamble ? get_prach_preamble_long_info(config.format)
                    : get_prach_preamble_short_info(config.format, to_ra_subcarrier_spacing(config.pusch_scs), false);

  // Calculate number of symbols.
  unsigned nof_symbols = preamble_info.symbol_length.to_samples(ra_scs_to_Hz(preamble_info.scs));

  // Build expected buffer data.
  prach_buffer_spy expected_buffer(
      expected_output, config.nof_td_occasions, config.nof_fd_occasions, nof_symbols, preamble_info.sequence_length);

  // Run demodulator.
  demodulator->demodulate(*output, input, GetParam().context.config);

  // For each port, time-domain occasion, frequency-domain occasion and symbol, ...
  for (unsigned i_port = 0; i_port != 1; ++i_port) {
    for (unsigned i_td_occasion = 0; i_td_occasion != config.nof_td_occasions; ++i_td_occasion) {
      for (unsigned i_fd_occasion = 0; i_fd_occasion != config.nof_fd_occasions; ++i_fd_occasion) {
        for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
          ASSERT_EQ(span<const cf_t>(expected_buffer.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol)),
                    span<const cf_t>(output->get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol)));
        }
      }
    }
  }
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(ofdm_prach_demodulator_vectortest,
                         ofdm_prach_demodulator_tester,
                         ::testing::ValuesIn(ofdm_prach_demodulator_test_data));
