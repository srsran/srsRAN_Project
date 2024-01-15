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
/// \brief Short-block encoder unit test.
///
/// The test encodes messages of different lengths (from 1 to 11 bits) into codeblocks (whose length may depend on the
/// modulation scheme). Messages and expected codeblocks (for comparison) are provided by test vectors.

#include "short_block_encoder_test_data.h"
#include "srsran/phy/upper/channel_coding/short/short_block_encoder.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "message_length={} codeblock_length={}  mod={}",
             test_case.message_length,
             test_case.codeblock_length,
             test_case.mod);
  return os;
}

} // namespace srsran

namespace {
using ShortBlockEncoderParams = test_case_t;

class ShortBlockEncoderFixture : public ::testing::TestWithParam<ShortBlockEncoderParams>
{
protected:
  static std::unique_ptr<short_block_encoder> test_encoder;

  static void SetUpTestSuite()
  {
    // Create short block encoder.
    if (!test_encoder) {
      test_encoder = create_short_block_encoder();
      ASSERT_NE(test_encoder, nullptr) << "Cannot create short block encoder";
    }
  }

  void SetUp() override { ASSERT_NE(test_encoder, nullptr) << "Cannot create short block encoder"; }
};

std::unique_ptr<short_block_encoder> ShortBlockEncoderFixture::test_encoder = nullptr;

TEST_P(ShortBlockEncoderFixture, ShortBlockEncoderTest)
{
  for (const auto& test_data : short_block_encoder_test_data) {
    unsigned          nof_messages     = test_data.nof_messages;
    unsigned          message_length   = test_data.message_length;
    unsigned          codeblock_length = test_data.codeblock_length;
    modulation_scheme mod              = test_data.mod;

    const std::vector<uint8_t> messages = test_data.messages.read();
    ASSERT_EQ(messages.size(), nof_messages * message_length) << "Error reading messages.";
    const std::vector<uint8_t> codeblocks = test_data.codeblocks.read();
    ASSERT_EQ(codeblocks.size(), nof_messages * codeblock_length) << "Error reading codeblocks.";

    std::vector<uint8_t> codeblocks_test(codeblocks.size());
    for (unsigned msg_idx = 0; msg_idx != nof_messages; ++msg_idx) {
      span<const uint8_t> input  = span<const uint8_t>(messages).subspan(msg_idx * message_length, message_length);
      span<uint8_t>       output = span<uint8_t>(codeblocks_test).subspan(msg_idx * codeblock_length, codeblock_length);
      test_encoder->encode(output, input, mod);
    }
    ASSERT_TRUE(std::equal(codeblocks_test.cbegin(), codeblocks_test.cend(), codeblocks.cbegin()))
        << "Encoding went wrong.";
  }
}

INSTANTIATE_TEST_SUITE_P(ShortBlockEncoderTest,
                         ShortBlockEncoderFixture,
                         ::testing::ValuesIn(short_block_encoder_test_data));

} // end namespace