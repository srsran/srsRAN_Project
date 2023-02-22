/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  // Get number of cyclic shifts.
  ulsch_information info = get_ulsch_information(test_case.config);

  // Compare with expected.
  ASSERT_EQ(test_case.info.sch.tb_crc_size, info.sch.tb_crc_size);
  ASSERT_EQ(test_case.info.sch.base_graph, info.sch.base_graph);
  ASSERT_EQ(test_case.info.sch.nof_cb, info.sch.nof_cb);
  ASSERT_EQ(test_case.info.sch.nof_filler_bits_per_cb, info.sch.nof_filler_bits_per_cb);
  ASSERT_EQ(test_case.info.sch.lifting_size, info.sch.lifting_size);
  ASSERT_EQ(test_case.info.sch.nof_bits_per_cb, info.sch.nof_bits_per_cb);
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
