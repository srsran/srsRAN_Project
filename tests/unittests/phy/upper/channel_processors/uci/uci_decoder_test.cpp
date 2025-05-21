/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
/// \brief UCI decoder unit test.
///
/// Detects short messages from sequences of log-likelihood ratios. Both the LLR sequences and the corresponding
/// messages (for comparison purposes) are provided as test vectors.

#include "uci_decoder_test_data.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_processors/uci/factories.h"
#include <gtest/gtest.h>

/// \cond
using namespace srsran;

namespace srsran {
std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  return os << fmt::format("A: {} bits, E: {}, {}", tc.message_length, tc.llr_length, to_string(tc.config.modulation));
}

std::ostream& operator<<(std::ostream& os, span<const uint8_t> data)
{
  return os << fmt::format("{}", data);
}

} // namespace srsran

namespace {

class UCIDDecoderFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  // Creates the factories just once.
  static void SetUpTestSuite()
  {
    if (!uci_factory) {
      std::shared_ptr<short_block_detector_factory> short_block_factory = create_short_block_detector_factory_sw();
      ASSERT_TRUE(short_block_factory) << "Cannot create the short block detector factory.";

      std::shared_ptr<polar_factory> polar_decoder_factory = create_polar_factory_sw();
      ASSERT_TRUE(polar_decoder_factory) << "Cannot create the polar decoder factory.";

      std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
      ASSERT_TRUE(crc_calc_factory) << "Cannot create the CRC calculator factory.";

      uci_factory = create_uci_decoder_factory_generic(short_block_factory, polar_decoder_factory, crc_calc_factory);
      ASSERT_TRUE(uci_factory) << "Cannot create the UCI decoder factory.";
    }
  }

  static std::shared_ptr<uci_decoder_factory> uci_factory;
};

std::shared_ptr<uci_decoder_factory> UCIDDecoderFixture::uci_factory = nullptr;

TEST_P(UCIDDecoderFixture, UCIDecoderTest)
{
  // Check the factories again, since ASSERT_* is not fatal in SetUpTestSuite in old googletest releases.
  ASSERT_TRUE(uci_factory) << "UCI decoder factory not found";

  std::unique_ptr<uci_decoder> decoder = uci_factory->create();
  ASSERT_TRUE(decoder) << "Cannot create the UCI decoder.";

  test_case_t test_data = GetParam();

  uci_decoder::configuration dec_cfg = {};
  dec_cfg.modulation                 = test_data.config.modulation;

  const std::vector<uint8_t> message = test_data.message.read();
  ASSERT_EQ(message.size(), test_data.message_length) << "Error reading message.";
  std::vector<log_likelihood_ratio> llr = test_data.llr.read();
  ASSERT_EQ(llr.size(), test_data.llr_length) << "Error reading LLRs.";

  std::vector<uint8_t> message_test(message.size());
  uci_status           status = decoder->decode(message_test, llr, dec_cfg);
  ASSERT_NE(fmt::underlying(status), fmt::underlying(uci_status::unknown)) << "Decoder in an unknown status.";
  ASSERT_NE(fmt::underlying(status), fmt::underlying(uci_status::invalid)) << "Invalid detection.";
  ASSERT_EQ(span<const uint8_t>(message_test), span<const uint8_t>(message)) << "Detection went wrong.";
}

INSTANTIATE_TEST_SUITE_P(UCIDecoderSuite, UCIDDecoderFixture, ::testing::ValuesIn(uci_decoder_test_data));
} // namespace
/// \endcond
