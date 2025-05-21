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

#include "pucch_detector_test_doubles.h"
#include "pucch_processor_format0_test_data.h"
#include "pucch_processor_test_fixture.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/channel_processors/pucch/formatters.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/support/math/complex_normal_random.h"
#include <gtest/gtest.h>

using namespace srsran;

using PucchProcessorFormat0Param   = test_case_t;
using PucchProcessorFormat0Fixture = PucchProcessorTestFixture<PucchProcessorFormat0Param>;
template <>
std::unique_ptr<pucch_processor> PucchProcessorFormat0Fixture::processor = nullptr;
template <>
std::unique_ptr<pucch_pdu_validator> PucchProcessorFormat0Fixture::validator = nullptr;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const pucch_processor::format0_configuration& config)
{
  return os << fmt::format("{}", config);
}

std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  os << tc.entry.config;
  return os;
}

std::ostream& operator<<(std::ostream& os, const uci_status& status)
{
  switch (status) {
    case uci_status::unknown:
      return os << "unknown";
    case uci_status::valid:
      return os << "valid";
    case uci_status::invalid:
    default:
      return os << "invalid";
  }
}

std::ostream& operator<<(std::ostream& os, span<const uint8_t> data)
{
  return os << fmt::format("{}", data);
}

} // namespace srsran

namespace {

TEST_P(PucchProcessorFormat0Fixture, FromVector)
{
  // Prepare resource grid.
  resource_grid_reader_spy grid;
  grid.write(GetParam().grid.read());

  const PucchProcessorFormat0Param& param = GetParam();

  const pucch_entry& entry = param.entry;
  // Make sure configuration is valid.
  ASSERT_TRUE(validator->is_valid(entry.config));

  pucch_processor_result result = processor->process(grid, entry.config);

  // Check channel state information.
  // Time alignment shouldn't exceed plus minus 3 us.
  std::optional<phy_time_unit> time_aligment = result.csi.get_time_alignment();
  ASSERT_FALSE(time_aligment.has_value());
  // EPRE depends on the number of entries.
  std::optional<float> epre_dB = result.csi.get_epre_dB();
  ASSERT_TRUE(epre_dB.has_value());
  ASSERT_NEAR(epre_dB.value(), 0.0, 0.2);
  // SINR should be larger than -5 dB.
  std::optional<float> sinr_dB = result.csi.get_sinr_dB();
  ASSERT_TRUE(sinr_dB.has_value());
  ASSERT_GT(sinr_dB.value(), -5.0) << "Entry configuration: " << entry.config;

  // The message shall be valid.
  ASSERT_EQ(result.message.get_status(), uci_status::valid);
  ASSERT_EQ(result.message.get_harq_ack_bits().size(), entry.ack_bits.size());
  if (!entry.ack_bits.empty()) {
    ASSERT_EQ(span<const uint8_t>(result.message.get_harq_ack_bits()), span<const uint8_t>(entry.ack_bits));
  }
  if (entry.sr.has_value()) {
    ASSERT_EQ(result.message.get_sr_bits().size(), 1);
    ASSERT_EQ(result.message.get_sr_bits().front(), entry.sr.value());
  } else {
    ASSERT_TRUE(result.message.get_sr_bits().empty());
  }
  ASSERT_TRUE(result.message.get_csi_part1_bits().empty());
  ASSERT_TRUE(result.message.get_csi_part2_bits().empty());
}

TEST_P(PucchProcessorFormat0Fixture, FalseAlarm)
{
  std::vector<resource_grid_reader_spy::expected_entry_t> res = GetParam().grid.read();

  complex_normal_distribution<cf_t> noise = {};
  std::mt19937                      rgen(12345);

  unsigned nof_trials = 200;
  // Acceptable probability of false alarm. The value is higher than the 1% given by the PUCCH requirements in TS38.104
  // Section 8.3.
  // Important: This is just a quick test, longer simulations are needed to properly estimate the PFA.
  float acceptable_pfa = 0.1;

  // Prepare resource grid.
  resource_grid_reader_spy grid;
  unsigned                 counter = 0;
  for (unsigned i = 0; i != nof_trials; ++i) {
    grid.reset();
    for (auto& entry : res) {
      entry.value = noise(rgen);
    }
    grid.write(res);

    const PucchProcessorFormat0Param& param = GetParam();
    const pucch_entry&                entry = param.entry;

    // Make sure configuration is valid.
    ASSERT_TRUE(validator->is_valid(entry.config));

    pucch_processor_result result = processor->process(grid, entry.config);

    counter += static_cast<unsigned>(result.message.get_status() == uci_status::valid);
  }

  // Assert that the probability of false alarm doesn't exceed the acceptable value.
  float pfa = static_cast<float>(counter) / nof_trials;
  ASSERT_TRUE(pfa <= acceptable_pfa) << fmt::format(
      "Probability of false alarms too high: {} while max is {} ({} hits out of {} trials).",
      pfa,
      acceptable_pfa,
      counter,
      nof_trials);
}

INSTANTIATE_TEST_SUITE_P(PucchProcessorFormat0,
                         PucchProcessorFormat0Fixture,
                         ::testing::ValuesIn(pucch_processor_format0_test_data));

} // namespace
