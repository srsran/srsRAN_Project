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

/// \file
/// \brief Short-block detector unit test.
///
/// Detects short messages from sequences of log-likelihood ratios. Both the LLR sequences and the corresponding
/// messages (for comparison purposes) are provided as test vectors.

#include "short_block_detector_test_data.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "fmt/ostream.h"
#include <algorithm>
#include <gtest/gtest.h>

/// \cond
using namespace srsran;
namespace srsran {
std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "message_length={} codeblock_length={} mod={}",
             test_case.message_length,
             test_case.codeblock_length,
             test_case.mod);
  return os;
}

} // namespace srsran
namespace {

using ShortBlockDetectorParams = test_case_t;

class ShortBlockDetectorFixture : public ::testing::TestWithParam<ShortBlockDetectorParams>
{
protected:
  static std::shared_ptr<short_block_detector_factory> factory;
  static std::unique_ptr<short_block_detector>         test_detector;

  static void SetUpTestSuite()
  {
    // Create short block detector factory.
    if (!factory) {
      factory = create_short_block_detector_factory_sw();
      ASSERT_NE(factory, nullptr) << "Cannot create short block detector factory";
    }

    // Create test detector
    if (!test_detector) {
      test_detector = factory->create();
      ASSERT_NE(test_detector, nullptr) << "Cannot create test detector";
    }
  }

  void SetUp() override
  {
    ASSERT_NE(factory, nullptr) << "Cannot create short block detector factory";
    ASSERT_NE(test_detector, nullptr) << "Cannot create test detector";
  }
};

std::shared_ptr<short_block_detector_factory> ShortBlockDetectorFixture::factory       = nullptr;
std::unique_ptr<short_block_detector>         ShortBlockDetectorFixture::test_detector = nullptr;

TEST_P(ShortBlockDetectorFixture, ShortBlockDetectorTest)
{
  for (const auto& test_data : short_block_detector_test_data) {
    unsigned          nof_messages     = test_data.nof_messages;
    unsigned          message_length   = test_data.message_length;
    unsigned          codeblock_length = test_data.codeblock_length;
    modulation_scheme mod              = test_data.mod;

    const std::vector<uint8_t> messages = test_data.messages.read();
    ASSERT_EQ(messages.size(), nof_messages * message_length) << "Error reading messages.";
    std::vector<log_likelihood_ratio> codeblocks = test_data.codeblocks.read();
    ASSERT_EQ(codeblocks.size(), nof_messages * codeblock_length) << "Error reading codeblocks.";

    std::vector<uint8_t> messages_test(messages.size());
    for (unsigned msg_idx = 0; msg_idx != nof_messages; ++msg_idx) {
      span<const log_likelihood_ratio> input =
          span<const log_likelihood_ratio>(codeblocks).subspan(msg_idx * codeblock_length, codeblock_length);
      span<uint8_t> output = span<uint8_t>(messages_test).subspan(msg_idx * message_length, message_length);
      ASSERT_TRUE(test_detector->detect(output, input, mod)) << "Meaningless detection.";
    }
    ASSERT_EQ(span<const uint8_t>(messages_test), span<const uint8_t>(messages)) << "Detection went wrong.";
  }
}

TEST_P(ShortBlockDetectorFixture, ShortBlockDetectorTestZeroLLR)
{
  const test_case_t& test_data        = GetParam();
  unsigned           nof_messages     = test_data.nof_messages;
  unsigned           message_length   = test_data.message_length;
  unsigned           codeblock_length = test_data.codeblock_length;
  modulation_scheme  mod              = test_data.mod;

  for (unsigned msg_idx = 0; msg_idx != nof_messages; ++msg_idx) {
    std::vector<uint8_t>              output(message_length);
    std::vector<log_likelihood_ratio> input(nof_messages * codeblock_length, log_likelihood_ratio(0));

    // Detection must be invalid and all values to one.
    ASSERT_FALSE(test_detector->detect(output, input, mod)) << "Invalid detection.";
    ASSERT_TRUE(std::all_of(output.begin(), output.end(), [](uint8_t value) { return value == 1; }))
        << "All output bits must be one.";
  }
}

INSTANTIATE_TEST_SUITE_P(ShortBlockDetectorTest,
                         ShortBlockDetectorFixture,
                         ::testing::ValuesIn(short_block_detector_test_data));

} // end namespace

/// \encond