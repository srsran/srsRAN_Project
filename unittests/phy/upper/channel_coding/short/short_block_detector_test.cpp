/// \file
/// \brief Short-block detector unit test.
///
/// Detects short messages from sequences of log-likelihood ratios. Both the LLR sequences and the corresponding
/// messages (for comparison purposes) are provided as test vectors.

#include "short_block_detector_test_data.h"
#include "srsgnb/phy/upper/channel_coding/short/short_block_detector.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
int main()
{
  std::unique_ptr<short_block_detector> test_detector = create_short_block_detector();

  for (const auto& test_data : short_block_detector_test_data) {
    unsigned          nof_messages     = test_data.nof_messages;
    unsigned          message_length   = test_data.message_length;
    unsigned          codeblock_length = test_data.codeblock_length;
    modulation_scheme mod              = test_data.mod;

    const std::vector<uint8_t> messages = test_data.messages.read();
    TESTASSERT_EQ(messages.size(), nof_messages * message_length, "Error reading messages.");
    const std::vector<int8_t> codeblocks = test_data.codeblocks.read();
    TESTASSERT_EQ(codeblocks.size(), nof_messages * codeblock_length, "Error reading codeblocks.");

    std::vector<uint8_t> messages_test(messages.size());
    for (unsigned msg_idx = 0; msg_idx != nof_messages; ++msg_idx) {
      span<const int8_t> input  = span<const int8_t>(codeblocks).subspan(msg_idx * codeblock_length, codeblock_length);
      span<uint8_t>      output = span<uint8_t>(messages_test).subspan(msg_idx * message_length, message_length);
      TESTASSERT(test_detector->detect(output, input, mod), "Meaningless detection.");
    }
    TESTASSERT(std::equal(messages_test.cbegin(), messages_test.cend(), messages.cbegin()), "Detection went wrong.");
  }
}