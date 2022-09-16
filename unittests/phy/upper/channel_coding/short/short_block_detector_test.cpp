/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Short-block detector unit test.
///
/// Detects short messages from sequences of log-likelihood ratios. Both the LLR sequences and the corresponding
/// messages (for comparison purposes) are provided as test vectors.

#include "short_block_detector_test_data.h"
#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsgnb/support/srsgnb_test.h"

/// \cond
using namespace srsgnb;
int main()
{
  std::shared_ptr<short_block_detector_factory> short_block_detector_factory = create_short_block_detector_factory_sw();
  TESTASSERT(short_block_detector_factory);

  std::unique_ptr<short_block_detector> test_detector = short_block_detector_factory->create();
  TESTASSERT(test_detector);

  for (const auto& test_data : short_block_detector_test_data) {
    unsigned          nof_messages     = test_data.nof_messages;
    unsigned          message_length   = test_data.message_length;
    unsigned          codeblock_length = test_data.codeblock_length;
    modulation_scheme mod              = test_data.mod;

    const std::vector<uint8_t> messages = test_data.messages.read();
    TESTASSERT_EQ(messages.size(), nof_messages * message_length, "Error reading messages.");
    std::vector<log_likelihood_ratio> codeblocks = test_data.codeblocks.read();
    TESTASSERT_EQ(codeblocks.size(), nof_messages * codeblock_length, "Error reading codeblocks.");

    std::vector<uint8_t> messages_test(messages.size());
    for (unsigned msg_idx = 0; msg_idx != nof_messages; ++msg_idx) {
      span<const log_likelihood_ratio> input =
          span<const log_likelihood_ratio>(codeblocks).subspan(msg_idx * codeblock_length, codeblock_length);
      span<uint8_t> output = span<uint8_t>(messages_test).subspan(msg_idx * message_length, message_length);
      TESTASSERT(test_detector->detect(output, input, mod), "Meaningless detection.");
    }
    TESTASSERT(std::equal(messages_test.cbegin(), messages_test.cend(), messages.cbegin()), "Detection went wrong.");
  }
}
/// \endcond
