/// \file
/// \brief Short-block encoder unit test.
///
/// The test encodes messages of different lengths (from 1 to 11 bits) into codeblocks (whose length may depend on the
/// modulation scheme). Messages and expected codeblocks (for comparison) are provided by test vectors.

#include "short_block_encoder_test_data.h"
#include "srsgnb/phy/upper/channel_coding/short/short_block_encoder.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

int main()
{
  std::unique_ptr<short_block_encoder> test_encoder = create_short_block_encoder();

  for (const auto& test_data : short_block_encoder_test_data) {
    unsigned          nof_messages     = test_data.nof_messages;
    unsigned          message_length   = test_data.message_length;
    unsigned          codeblock_length = test_data.codeblock_length;
    modulation_scheme mod              = test_data.mod;

    const std::vector<uint8_t> messages = test_data.messages.read();
    TESTASSERT_EQ(messages.size(), nof_messages * message_length, "Error reading messages.");
    const std::vector<uint8_t> codeblocks = test_data.codeblocks.read();
    TESTASSERT_EQ(codeblocks.size(), nof_messages * codeblock_length, "Error reading codeblocks.");

    std::vector<uint8_t> codeblocks_test(codeblocks.size());
    for (unsigned msg_idx = 0; msg_idx != nof_messages; ++msg_idx) {
      span<const uint8_t> input  = span<const uint8_t>(messages).subspan(msg_idx * message_length, message_length);
      span<uint8_t>       output = span<uint8_t>(codeblocks_test).subspan(msg_idx * codeblock_length, codeblock_length);
      test_encoder->encode(output, input, mod);
    }
    TESTASSERT(std::equal(codeblocks_test.cbegin(), codeblocks_test.cend(), codeblocks.cbegin()),
               "Encoding went wrong.");
  }
}