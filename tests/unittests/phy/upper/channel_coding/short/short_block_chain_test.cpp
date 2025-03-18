/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Short-block chain test.
///
/// Ensures short block messages encoded can be detected.

#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_coding/short/short_block_encoder.h"
#include "srsran/ran/uci/uci_info.h"
#include "srsran/srsvec/bit.h"
#include "fmt/ostream.h"
#include <algorithm>
#include <gtest/gtest.h>

/// \cond
using namespace srsran;

namespace {

using ShortBlockDetectorParams = unsigned;

class ShortBlockChainFixture : public ::testing::TestWithParam<ShortBlockDetectorParams>
{
protected:
  static std::shared_ptr<short_block_detector_factory> factory;
  static std::unique_ptr<short_block_encoder>          encoder;
  static std::unique_ptr<short_block_detector>         detector;

  static void SetUpTestSuite()
  {
    // Create short block detector factory.
    if (!factory) {
      factory = create_short_block_detector_factory_sw();
      ASSERT_NE(factory, nullptr) << "Cannot create short block detector factory";
    }

    // Create test encoder.
    if (!encoder) {
      encoder = create_short_block_encoder();
      ASSERT_NE(encoder, nullptr) << "Cannot create test detector";
    }

    // Create test detector.
    if (!detector) {
      detector = factory->create();
      ASSERT_NE(detector, nullptr) << "Cannot create test detector";
    }
  }

  void SetUp() override
  {
    ASSERT_NE(factory, nullptr) << "Cannot create short block detector factory";
    ASSERT_NE(encoder, nullptr) << "Cannot create test encoder";
    ASSERT_NE(detector, nullptr) << "Cannot create test detector";
  }
};

std::shared_ptr<short_block_detector_factory> ShortBlockChainFixture::factory  = nullptr;
std::unique_ptr<short_block_encoder>          ShortBlockChainFixture::encoder  = nullptr;
std::unique_ptr<short_block_detector>         ShortBlockChainFixture::detector = nullptr;

TEST_P(ShortBlockChainFixture, MinEncBits)
{
  static constexpr modulation_scheme modulation = modulation_scheme::QPSK;

  // Number of payload bits.
  unsigned nof_payload_bits = GetParam();

  // Minimum number of encoded bits.
  unsigned min_nof_encoded_bits = calculate_uci_min_encoded_bits(nof_payload_bits);

  // Iterate all possible message payload combinations.
  for (unsigned message = 0, message_end = 1U << nof_payload_bits; message != message_end; ++message) {
    // Unpack message.
    static_vector<uint8_t, 32> unpacked_message(nof_payload_bits);
    srsvec::bit_unpack(unpacked_message, nof_payload_bits, nof_payload_bits);

    // Encode message.
    static_vector<uint8_t, 32> codeword(min_nof_encoded_bits);
    encoder->encode(codeword, unpacked_message, modulation);

    // Generate soft bits.
    static_vector<log_likelihood_ratio, 32> soft_codeword(min_nof_encoded_bits);
    std::transform(codeword.begin(), codeword.end(), soft_codeword.begin(), [](uint8_t in) {
      return in ? log_likelihood_ratio::min() : log_likelihood_ratio::max();
    });

    // Detect block.
    static_vector<uint8_t, 32> detected_message(nof_payload_bits);
    bool                       detected = detector->detect(detected_message, soft_codeword, modulation);

    // Validate that the message is detected with the right contents.
    ASSERT_TRUE(detected);
    ASSERT_EQ(span<const uint8_t>(unpacked_message), span<const uint8_t>(detected_message));
  }
}

INSTANTIATE_TEST_SUITE_P(ShortBlockDetectorTest, ShortBlockChainFixture, ::testing::Range(1U, 12U));

} // end namespace

/// \endcond
