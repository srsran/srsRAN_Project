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

#include "prach_configuration_test_data.h"
#include "srsran/ran/prach/prach_configuration.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t tc)
{
  fmt::print(os, "{} {} - index {}", to_string(tc.fr), (tc.dm == duplex_mode::FDD) ? "paired" : "unpaired", tc.index);
  return os;
}

} // namespace srsran

class PrachConfiguration : public testing::TestWithParam<test_case_t>
{};

TEST_P(PrachConfiguration, PrachConfigTest)
{
  const test_case_t&  ref_conf       = GetParam();
  prach_configuration generated_conf = prach_configuration_get(ref_conf.fr, ref_conf.dm, ref_conf.index);

  prach_format_type ref_format = ref_conf.config.format;

  if (ref_format == prach_format_type::invalid) {
    // We should never get here.
    FAIL() << "Invalid reference PRACH configuration.";
  }

  if ((ref_format == prach_format_type::A1_B1) || (ref_format == prach_format_type::A2_B2) ||
      (ref_format == prach_format_type::A3_B3) || (ref_format == prach_format_type::A3) ||
      (ref_format == prach_format_type::B1) || (ref_format == prach_format_type::C0) ||
      (ref_format == prach_format_type::C2)) {
    ASSERT_EQ(generated_conf.format, prach_format_type::invalid) << "This PRACH configuration should be unsupported.";
    return;
  }

  ASSERT_EQ(generated_conf.format, ref_format) << "PRACH format does not match.";
  ASSERT_EQ(generated_conf.x, ref_conf.config.x) << "SFN period 'x' does not match.";
  ASSERT_EQ(generated_conf.y, ref_conf.config.y) << "SFN offset 'y' does not match.";
  ASSERT_EQ(generated_conf.slots, ref_conf.config.slots) << "Number of subframes/slots does not match.";
  ASSERT_EQ(generated_conf.starting_symbol, ref_conf.config.starting_symbol) << "Starting symbol does not match.";
  ASSERT_EQ(generated_conf.nof_prach_slots_within_subframe, ref_conf.config.nof_prach_slots_within_subframe)
      << "Number of PRACH slots within a subframe/60-kHz slot does not match.";
  ASSERT_EQ(generated_conf.nof_occasions_within_slot, ref_conf.config.nof_occasions_within_slot)
      << "Number of PRACH occasions within a slot does not match.";
  ASSERT_EQ(generated_conf.duration, ref_conf.config.duration) << "PRACH duration does not match.";
}

INSTANTIATE_TEST_SUITE_P(PrachConfigCompare, PrachConfiguration, testing::ValuesIn(prach_configuration_test_data));
