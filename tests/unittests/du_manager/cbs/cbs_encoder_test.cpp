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

#include "srsran/adt/span.h"
#include "srsran/du/du_high/du_manager/cbs/cbs_encoder.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/units.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

namespace {

using cbsEncoderParams = std::tuple<unsigned, unsigned>;

class CbsEncoderFixtureHelpers
{
public:
  /// List of ASCII characters supported by the GSM-7 encoder.
  static const std::string gsm_7_supported_chars;

  /// Generates the test messages for the encoder.
  static std::string generate_cbs_message_gsm7(unsigned nof_characters)
  {
    std::string ascii_message;
    unsigned    remaining_nof_chars = nof_characters;
    while (remaining_nof_chars > 0) {
      // Append ASCII characters supported by GSM-7 encoder until the message reaches the desired size.
      size_t nof_chars_to_append  = std::min(static_cast<size_t>(remaining_nof_chars), gsm_7_supported_chars.length());
      std::string chars_to_append = gsm_7_supported_chars.substr(0, nof_chars_to_append);
      ascii_message.append(chars_to_append);
      remaining_nof_chars -= nof_chars_to_append;
    }
    return ascii_message;
  }

  /// Generates the test messages for the encoder. It also return the expected UCS2 encoded message.
  static std::pair<std::string, std::u16string> generate_cbs_message_ucs2(unsigned nof_characters)
  {
    std::string utf8_message;
    // Random distribution that can generate all possible UCS-2 characters.
    std::uniform_int_distribution<char16_t> dist(0x0000, 0xffff);

    // Generate random UCS-2 characters (UCS-2 is equivalent to UTF-16 for unicode characters in the basic
    // multilingual plane).
    std::u16string utf16_str;
    for (unsigned remaining_nof_chars = nof_characters; remaining_nof_chars != 0; --remaining_nof_chars) {
      char16_t ucs2_ch = dist(rgen);

      // Checks if the generated value is a valid UCS-2 codepoint.
      auto is_valid_ucs2_char = [](char16_t ucs2_char) {
        // It can't be in the High Surrogate code range.
        return (ucs2_char < 0xd800 || ucs2_char > 0xdbff) &&
               // It can't be in the Low Surrogate code range.
               (ucs2_char < 0xdc00 || ucs2_char > 0xdfff) &&
               // It can't be an unallocated code point.
               (ucs2_char < 0x2fe0 || ucs2_char > 0x2fef);
      };

      while (!is_valid_ucs2_char(ucs2_ch)) {
        ucs2_ch = dist(rgen);
      }
      utf16_str.push_back(ucs2_ch);
    }

    for (auto& ucs2_ch : utf16_str) {
      if (ucs2_ch <= 0x7f) {
        // One byte UTF-8 character.
        auto utf8_ch = static_cast<unsigned char>(ucs2_ch);
        utf8_message.push_back(utf8_ch);
      } else if (ucs2_ch <= 0x7ff) {
        auto utf8_lead_byte = 0xc0 | static_cast<unsigned char>(ucs2_ch >> 6U);
        auto utf8_byte_2    = 0x80 | static_cast<unsigned char>(ucs2_ch & 0x3f);
        utf8_message.push_back(utf8_lead_byte);
        utf8_message.push_back(utf8_byte_2);
      } else if (ucs2_ch <= 0xffff) {
        auto utf8_lead_byte = 0xe0 | static_cast<unsigned char>(ucs2_ch >> 12U);
        auto utf8_byte_2    = 0x80 | static_cast<unsigned char>((ucs2_ch >> 6U) & 0x3f);
        auto utf8_byte_3    = 0x80 | static_cast<unsigned char>(ucs2_ch & 0x3f);
        utf8_message.push_back(utf8_lead_byte);
        utf8_message.push_back(utf8_byte_2);
        utf8_message.push_back(utf8_byte_3);
      }
    }
    return {utf8_message, utf16_str};
  }
};

const std::string CbsEncoderFixtureHelpers::gsm_7_supported_chars =
    "@$\n_ !\"#%&\'()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ?abcdefghijklmnopqrstuvwxyz^{}\\[~]|";

class CbsEncoderFixture : public ::testing::TestWithParam<cbsEncoderParams>, public CbsEncoderFixtureHelpers
{
protected:
  // Input message size.
  unsigned size;

  // CBS Encoder to test.
  std::unique_ptr<cbs_encoder> encoder;

  void SetUp() override
  {
    encoder = create_cbs_encoder();
    ASSERT_NE(encoder, nullptr);

    // Get test parameters.
    size = std::get<0>(GetParam());
  }

  /// Checks that the encoded CB-Data IE matches the expected message.
  static void check_ucs2_encoded_message(std::vector<uint8_t>& cb_data_ucs2, const std::u16string& expected_message)
  {
    // Obtain the number of unicode characters in the expected message.
    std::size_t total_message_length = expected_message.length();

    // Determine the number of CB-Data IE pages.
    static constexpr units::bytes cb_data_info_page_nof_bytes{82};
    static constexpr units::bytes cb_data_info_len_nof_bytes{1};

    // Recall that an UCS-2 character occupies 16 bits.
    units::bytes nof_ucs2_bytes(total_message_length * 2);
    unsigned     nof_cb_data_pages = srsran::divide_ceil(nof_ucs2_bytes.value(), cb_data_info_page_nof_bytes.value());

    srsran_assert(nof_cb_data_pages <= 15, "Number of CB-Data IE pages exceeds the maximum of 15.");

    // Check that the number of information pages is written at the start of the CB-Data IE.
    ASSERT_EQ(cb_data_ucs2[0], static_cast<uint8_t>(nof_cb_data_pages));

    // Number of UCS-2 characters in a CB-Data IE page.
    unsigned nof_ucs2_chars_page = cb_data_info_page_nof_bytes.value() / 2;

    std::size_t decoded_message_len = 0;
    for (unsigned i_page = 0; i_page != nof_cb_data_pages; ++i_page) {
      // Extract characters in the current CB-Data IE page from the expected message.
      std::size_t i_page_expected_msg_begin = i_page * nof_ucs2_chars_page;
      std::size_t i_page_nof_ucs2_chars =
          std::min(static_cast<std::size_t>(nof_ucs2_chars_page), total_message_length - i_page_expected_msg_begin);

      std::u16string expected_msg_in_page = expected_message.substr(i_page_expected_msg_begin, i_page_nof_ucs2_chars);

      // Extract characters in the current CB-Data IE page from the encoded message. Recall that padding is used so that
      // The CB-Data Information page is always 82 bytes (See TS23.041 Section 9.3.19).
      units::bytes i_page_enc_msg_begin =
          units::bytes(1) + (cb_data_info_page_nof_bytes + cb_data_info_len_nof_bytes) * i_page;

      srsran::span<uint8_t> enc_msg_in_page(&cb_data_ucs2[i_page_enc_msg_begin.value()],
                                            cb_data_info_page_nof_bytes.value());

      // Extract the information page length from the encoded message. It is the last byte after the CB-Data information
      // page. This gives the number of data bytes in the information page (excluding the padding).
      units::bytes info_page_len(cb_data_ucs2[i_page_enc_msg_begin.value() + cb_data_info_page_nof_bytes.value()]);

      ASSERT_TRUE(info_page_len <= cb_data_info_page_nof_bytes)
          << "Extracted information page length exceeds the maximum value.";
      ASSERT_TRUE(info_page_len.value() % 2 == 0)
          << "The information page must contain an even number of bytes when UCS-2 encoding is used.";

      {
        // Number of UCS-2 encoded characters in the actual page (excluding padding).
        unsigned info_page_nof_ucs2 = info_page_len.value() / 2;

        ASSERT_EQ(info_page_nof_ucs2, i_page_nof_ucs2_chars)
            << "Value in the info page length field does not match the number of remaining characters";
      }

      // Convert the UCS-2 characters in the current information page into UTF-16 format. Recall that UCS-2
      // characters are encoded in big endian order inside the CB-Data IE.
      srsran::span<char16_t> ucs2_view(reinterpret_cast<char16_t*>(enc_msg_in_page.data()), i_page_nof_ucs2_chars);

      // Swap bytes.
      std::for_each(ucs2_view.begin(), ucs2_view.end(), [](auto& ucs2_char) { ucs2_char = ntohs(ucs2_char); });

      // Count the number of extracted UCS-2 characters.
      decoded_message_len += ucs2_view.size();

      span<char16_t> expected_message_view(expected_msg_in_page);
      ASSERT_EQ(ucs2_view, expected_message_view) << "Encoded message does not match the expected message.";
    }

    ASSERT_EQ(decoded_message_len, total_message_length)
        << "Number of decoded characters does not match the expected value.";
  }
};

TEST_P(CbsEncoderFixture, CbsEncoderTestGsm7)
{
  ASSERT_NE(encoder, nullptr);

  unsigned message_size = std::get<0>(GetParam());

  // Generate a CBS message that can be encoded using GSM-7.
  std::string message = generate_cbs_message_gsm7(message_size);

  // GSM-7 data coding scheme value.
  static constexpr unsigned gsm7_coding_scheme = 0x00;

  // Encode into a CB-Data IE.
  encoder->encode_cb_data(message, gsm7_coding_scheme);
}

TEST_P(CbsEncoderFixture, CbsEncoderTestUcs2)
{
  ASSERT_NE(encoder, nullptr);

  unsigned message_size = std::get<0>(GetParam());

  // Generate a CBS message that can be encoded using UCS-2.
  std::pair<std::string, std::u16string> messages = generate_cbs_message_ucs2(message_size);

  std::string    message          = std::get<0>(messages);
  std::u16string expected_message = std::get<1>(messages);

  // UCS-2 data coding scheme value.
  static constexpr unsigned ucs2_coding_scheme = 0x48;

  // Encode into a CB-Data IE.
  std::vector<uint8_t> ucs2_enc_message = encoder->encode_cb_data(message, ucs2_coding_scheme);

  // Test encoding.
  check_ucs2_encoded_message(ucs2_enc_message, expected_message);
}

class CbsEncodingDeathTestFixture : public ::testing::Test, public CbsEncoderFixtureHelpers
{
protected:
  // CBS Encoder to test.
  std::unique_ptr<cbs_encoder> encoder;

  void SetUp() override
  {
    encoder = create_cbs_encoder();
    ASSERT_NE(encoder, nullptr);
  }
};

TEST_F(CbsEncodingDeathTestFixture, CbsEncodingSelectionDeathTest)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  ASSERT_NE(encoder, nullptr);

  // Supported data coding scheme values.
  auto is_coding_scheme_valid = [](uint8_t value) {
    // Languages using the default GSM-7 alphabet.
    return ((value >= 0x00) && (value <= 0x0f)) ||
           // Data coding / message handling, bit 2 must be set to 0.
           ((value >= 0xf0) && (value <= 0xff) && ((value & 0b00000100) == 0)) ||
           // General data coding indication (no message class meaning), bit 2 must be set to 0.
           ((value >= 0x40) && (value <= 0x4f) && ((value & 0b00000100) == 0)) ||
           // General data coding indication (message class meaning), bit 2 must be set to 0.
           ((value >= 0x50) && (value <= 0x5f) && ((value & 0b00000100) == 0));
  };

  for (unsigned i_data_coding_scheme = 0; i_data_coding_scheme != 0xff; ++i_data_coding_scheme) {
    if (is_coding_scheme_valid(i_data_coding_scheme)) {
      ASSERT_NO_FATAL_FAILURE(encoder->encode_cb_data("Test message", i_data_coding_scheme));
    } else {
      ASSERT_DEATH(
          { encoder->encode_cb_data("Test message", i_data_coding_scheme); },
          "Invalid or unsupported CBS encoding type.");
    }
  }
}

TEST_F(CbsEncodingDeathTestFixture, CbsEncodingMessageSizeDeathTest)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  ASSERT_NE(encoder, nullptr);

  {
    // Maximum supported message size is 15 (CB Data pages) times 93 (number of GSM-7 characters in a page).
    unsigned max_supported_gsm7_message_size = 15 * 93;

    std::string message = generate_cbs_message_gsm7(max_supported_gsm7_message_size + 1);

    // Attempt to encode a message that is too long.
    ASSERT_DEATH(
        { encoder->encode_cb_data(message, 0x00); },
        fmt::format(
            R"(GSM-7 message length \(i\.e\. {} chars\) is too long\. Maximum length is {} chars without extension characters\.)",
            message.size(),
            max_supported_gsm7_message_size));
  }

  { // Maximum supported message size is 15 (CB Data pages) times 41 (number of UCS-2 characters in a page).
    unsigned max_supported_ucs2_message_size = 15 * 41;

    std::pair<std::string, std::u16string> messages = generate_cbs_message_ucs2(max_supported_ucs2_message_size + 1);
    std::string                            message  = std::get<0>(messages);

    // Attempt to encode a message that is too long.
    ASSERT_DEATH(
        { encoder->encode_cb_data(message, 0x48); },
        fmt::format(R"(UCS-2 message length \(i\.e\. {} characters\) is too long\. Maximum length is {} characters\.)",
                    max_supported_ucs2_message_size + 1,
                    max_supported_ucs2_message_size));
  }
}

TEST_F(CbsEncodingDeathTestFixture, CbsEncodingUnsupportedCharactersDeathTest)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  ASSERT_NE(encoder, nullptr);

  {
    std::string message = generate_cbs_message_gsm7(10);

    // Append character not suppored by the GSM-7 encoder.
    message.append("à");

    // Attempt to encode a message that is too long.
    ASSERT_DEATH(
        { encoder->encode_cb_data(message, 0x00); },
        R"(Warning message could not be encoded using GSM-7 default alphabet\. Set the data coding scheme to a scheme using UCS-2\.)");
  }

  {
    std::pair<std::string, std::u16string> messages = generate_cbs_message_ucs2(10);
    std::string                            message  = std::get<0>(messages);

    // Append character not suppored by the GSM-7 encoder.
    message.append("\xF0\x91\xBE\xBF");

    // Attempt to encode a message that is too long.
    ASSERT_DEATH({ encoder->encode_cb_data(message, 0x48); }, R"(Warning message could not be encoded using UCS-2\.)");
  }
}

INSTANTIATE_TEST_SUITE_P(CbsEncoderTestUcs2,
                         CbsEncoderFixture,
                         ::testing::Combine(
                             // Message length.
                             ::testing::Values(1, 5, 19, 51, 113, 503),
                             // Test repetitions.
                             ::testing::Range(0U, 100U)));

} // namespace
