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

#include "pusch_decoder_buffer_test_doubles.h"
#include "ulsch_demultiplex_test_data.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/srsvec/bit.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const ulsch_demultiplex::configuration& config)
{
  fmt::print(os,
             "Modulation={}; nof_layers={}; nof_prb={}; t_alloc={}:{}; nof_harq_ack_rvd={}; dmrs_type={}; "
             "dmrs_mask={}; nof_cdm_groups_without_data={};",
             to_string(config.modulation),
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

} // namespace srsran

using namespace srsran;

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

TEST_P(UlschDemultiplexFixture, AllFromVector)
{
  const test_case_t& test_case = GetParam();

  // Load codeword after scrambling.
  std::vector<log_likelihood_ratio> demodulated = test_case.codeword.read();

  // Load unpacked scrambling sequence.
  std::vector<uint8_t> unpacked_scrambling_seq = test_case.scrambling_seq.read();

  // Pack scrambling sequence.
  dynamic_bit_buffer scrambling_seq(unpacked_scrambling_seq.size());
  srsvec::bit_pack(scrambling_seq, unpacked_scrambling_seq);

  // Load SCH Data output.
  std::vector<log_likelihood_ratio> expected_sch_data = test_case.output_ulsch.read();
  pusch_decoder_buffer_spy          sch_data(expected_sch_data.size());

  // Load HARQ-ACK output.
  std::vector<log_likelihood_ratio> expected_harq_ack = test_case.output_harq_ack.read();
  pusch_decoder_buffer_spy          harq_ack(expected_harq_ack.size());

  // Load CSI Part 1 output.
  std::vector<log_likelihood_ratio> expected_csi_part1 = test_case.output_csi_part1.read();
  pusch_decoder_buffer_spy          csi_part1(expected_csi_part1.size());

  // Load CSI Part 2 output.
  std::vector<log_likelihood_ratio> expected_csi_part2 = test_case.output_csi_part2.read();
  pusch_decoder_buffer_spy          csi_part2(expected_csi_part2.size());

  // Prepare demultiplexer.
  pusch_codeword_buffer& codeword_buffer =
      demultiplexer->demultiplex(sch_data, harq_ack, csi_part1, test_case.context.config);

  if (!expected_csi_part2.empty()) {
    demultiplexer->set_csi_part2(
        csi_part2, test_case.context.nof_csi_part2_bits, test_case.context.nof_enc_csi_part2_bits);
  }

  // Write input and signal end of the codeword.
  codeword_buffer.on_new_block(demodulated, scrambling_seq);
  codeword_buffer.on_end_codeword();

  // Verify results.
  ASSERT_EQ(sch_data.get_data(), span<const log_likelihood_ratio>(expected_sch_data));
  ASSERT_EQ(harq_ack.get_data(), span<const log_likelihood_ratio>(expected_harq_ack));
  ASSERT_EQ(csi_part1.get_data(), span<const log_likelihood_ratio>(expected_csi_part1));
  ASSERT_EQ(csi_part2.get_data(), span<const log_likelihood_ratio>(expected_csi_part2));
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(UlschDemultiplex, UlschDemultiplexFixture, ::testing::ValuesIn(ulsch_demultiplex_test_data));

} // namespace
