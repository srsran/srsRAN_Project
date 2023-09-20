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

#include "ulsch_info_test_data.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "tbs={}; modulation={}; R={}; Oack={}; Ocsi1={}; Ocsi2={};",
             test_case.config.tbs,
             test_case.config.mcs_descr.modulation,
             test_case.config.mcs_descr.get_normalised_target_code_rate(),
             test_case.config.nof_harq_ack_bits,
             test_case.config.nof_csi_part1_bits,
             test_case.config.nof_csi_part2_bits);
  return os;
}

} // namespace srsran

using namespace srsran;

class UlschInfoFixture : public ::testing::TestWithParam<test_case_t>
{};

TEST_P(UlschInfoFixture, VectorTest)
{
  // Get combined parameter.
  const test_case_t& test_case = GetParam();

  // Get UL-SCH information parameters.
  ulsch_information info = get_ulsch_information(test_case.config);

  // Compare with expected.
  if (test_case.config.tbs > units::bits(0)) {
    ASSERT_EQ(test_case.info.sch.value().tb_crc_size, info.sch.value().tb_crc_size);
    ASSERT_EQ(test_case.info.sch.value().base_graph, info.sch.value().base_graph);
    ASSERT_EQ(test_case.info.sch.value().nof_cb, info.sch.value().nof_cb);
    ASSERT_EQ(test_case.info.sch.value().nof_filler_bits_per_cb, info.sch.value().nof_filler_bits_per_cb);
    ASSERT_EQ(test_case.info.sch.value().lifting_size, info.sch.value().lifting_size);
    ASSERT_EQ(test_case.info.sch.value().nof_bits_per_cb, info.sch.value().nof_bits_per_cb);
  }

  ASSERT_EQ(test_case.info.nof_ul_sch_bits, info.nof_ul_sch_bits);
  ASSERT_EQ(test_case.info.nof_harq_ack_bits, info.nof_harq_ack_bits);
  ASSERT_EQ(test_case.info.nof_harq_ack_rvd, info.nof_harq_ack_rvd);
  ASSERT_EQ(test_case.info.nof_csi_part1_bits, info.nof_csi_part1_bits);
  ASSERT_EQ(test_case.info.nof_csi_part2_bits, info.nof_csi_part2_bits);
  ASSERT_EQ(test_case.info.nof_harq_ack_re, info.nof_harq_ack_re);
  ASSERT_EQ(test_case.info.nof_csi_part1_re, info.nof_csi_part1_re);
  ASSERT_EQ(test_case.info.nof_csi_part2_re, info.nof_csi_part2_re);
}

INSTANTIATE_TEST_SUITE_P(UlschInfo, UlschInfoFixture, ::testing::ValuesIn(ulsch_info_test_data));
