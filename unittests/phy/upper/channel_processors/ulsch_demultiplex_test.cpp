/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include "ulsch_demultiplex_test_data.h"
#include <gtest/gtest.h>

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, const ulsch_demultiplex::configuration& config)
{
  fmt::print(os,
             "Modulation={}; nof_layers={}; nof_prb={}; t_alloc={}:{}; nof_harq_ack_rvd={}; dmrs_type={}; "
             "dmrs_mask={}; nof_cdm_groups_without_data={};",
             config.modulation,
             config.nof_layers,
             config.nof_prb,
             config.start_symbol_index,
             config.nof_symbols,
             config.nof_harq_ack_rvd,
             config.dmrs == dmrs_type::TYPE1 ? "type1" : "type2",
             config.dmrs_symbol_mask,
             config.nof_cdm_groups_without_data);
  return os;
}

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.context.config);
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const log_likelihood_ratio> data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const uint16_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

} // namespace srsgnb

using namespace srsgnb;

namespace {
class UlschDemultiplexFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::shared_ptr<ulsch_demultiplex_factory> factory;
  std::unique_ptr<ulsch_demultiplex>                demultiplexer = nullptr;

  static void SetUpTestSuite()
  {
    if (factory) {
      return;
    }
    factory = create_ulsch_demultiplex_factory_sw();
  }

  void SetUp() override
  {
    ASSERT_NE(factory, nullptr);

    demultiplexer = factory->create();
    ASSERT_NE(demultiplexer, nullptr);
  }
};

std::shared_ptr<ulsch_demultiplex_factory> UlschDemultiplexFixture::factory = nullptr;

TEST_P(UlschDemultiplexFixture, FromVector)
{
  const test_case_t& test_case = GetParam();

  // Load codeword input.
  std::vector<log_likelihood_ratio> input = test_case.input.read();

  // Load SCH Data output.
  std::vector<log_likelihood_ratio> expected_sch_data = test_case.output_ulsch.read();
  std::vector<log_likelihood_ratio> sch_data(expected_sch_data.size());

  // Load HARQ-ACK output.
  std::vector<log_likelihood_ratio> expected_harq_ack = test_case.output_harq_ack.read();
  std::vector<log_likelihood_ratio> harq_ack(expected_harq_ack.size());

  // Load CSI-Part1 output.
  std::vector<log_likelihood_ratio> expected_csi_part1 = test_case.output_csi_part1.read();
  std::vector<log_likelihood_ratio> csi_part1(expected_csi_part1.size());

  // Load CSI-Part2 output.
  std::vector<log_likelihood_ratio> expected_csi_part2 = test_case.output_csi_part2.read();
  std::vector<log_likelihood_ratio> csi_part2(expected_csi_part2.size());

  // Demultiplex.
  demultiplexer->demultiplex(sch_data, harq_ack, csi_part1, csi_part2, input, test_case.context.config);

  // Verify results.
  ASSERT_EQ(span<const log_likelihood_ratio>(sch_data), span<const log_likelihood_ratio>(expected_sch_data));
  ASSERT_EQ(span<const log_likelihood_ratio>(harq_ack), span<const log_likelihood_ratio>(expected_harq_ack));
  ASSERT_EQ(span<const log_likelihood_ratio>(csi_part1), span<const log_likelihood_ratio>(expected_csi_part1));
  ASSERT_EQ(span<const log_likelihood_ratio>(csi_part2), span<const log_likelihood_ratio>(expected_csi_part2));

  // Generate repetition placeholder list.
  ulsch_placeholder_list placeholders =
      demultiplexer->get_placeholders(test_case.context.msg_info, test_case.context.config);

  // Convert placeholders to vector.
  std::vector<uint16_t> placeholders_list;
  placeholders.for_each(test_case.context.config.modulation,
                        test_case.context.config.nof_layers,
                        [&placeholders_list](unsigned i_bit) { placeholders_list.emplace_back(i_bit); });

  // Load placeholder list from file.
  std::vector<uint16_t> expected_placeholders_list = test_case.placeholders.read();

  // Verify.
  ASSERT_EQ(span<const uint16_t>(placeholders_list), span<const uint16_t>(expected_placeholders_list));
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(UlschDemultiplex, UlschDemultiplexFixture, ::testing::ValuesIn(ulsch_demultiplex_test_data));

} // namespace