/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/span.h"
#include "srsran/cbs/cbs_encoder.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <arpa/inet.h>
#include <codecvt>
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

namespace {

using cbsEncoderParams = std::tuple<unsigned, unsigned>;

class CbsEncoderFixture : public ::testing::TestWithParam<cbsEncoderParams>
{
protected:
  // Input message size.
  unsigned size;

  // List of ASCII characters supported by the GSM-7 encoder.
  static const std::string gsm_7_supported_chars;

  void SetUp() override
  {
    // Get test parameters.
    size = std::get<0>(GetParam());
  }

  // Generates the test messages for the encoder.
  static std::string generate_cbs_message(unsigned nof_characters, cbs_encoder::encoding_type encoding)
  {
    std::string message;
    if (encoding == cbs_encoder::encoding_type::GSM7) {
      unsigned remaining_nof_chars = nof_characters;
      while (remaining_nof_chars > 0) {
        // Append ASCII characters supported by GSM-7 encoder until the message reaches the desired size.
        size_t nof_chars_to_append = std::min(static_cast<size_t>(remaining_nof_chars), gsm_7_supported_chars.length());
        std::string chars_to_append = gsm_7_supported_chars.substr(0, nof_chars_to_append);
        message.append(chars_to_append);
        remaining_nof_chars -= nof_chars_to_append;
      }
    } else if (encoding == cbs_encoder::encoding_type::UCS2) {
      // Random distribution that can generate all possible UCS-2 characters.
      std::uniform_int_distribution<char16_t> dist(0x0000, 0xFFFF);

      // Generate random UCS-2 characters (UCS-2 is equivalent to UTF-16 for unicode characters in the basic
      // multilingual plane).
      std::u16string utf16_str;
      for (unsigned remaining_nof_chars = nof_characters; remaining_nof_chars != 0; --remaining_nof_chars) {
        char16_t ucs2_ch = dist(rgen);

        // Checks if the generated value is a valid UCS-2 codepoint.
        auto is_valid_ucs2_char = [](char16_t ucs2_char) {
          // It can't be in the High Surrogate code range.
          return (ucs2_char < 0xD800 || ucs2_char > 0xDBFF) &&
                 // It can't be in the Low Surrogate code range.
                 (ucs2_char < 0xDC00 || ucs2_char > 0xDFFF) &&
                 // It can't be an unallocated code point.
                 (ucs2_char < 0x2FE0 || ucs2_char > 0x2FEF);
        };

        while (!is_valid_ucs2_char(ucs2_ch)) {
          ucs2_ch = dist(rgen);
        }
        utf16_str.push_back(ucs2_ch);
      }

      // Convert the generated UCS-2 characters to a UTF-8 string (expected format at the input of the encoder).
      std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2_converter;
      message = ucs2_converter.to_bytes(utf16_str);
    }

    return message;
  }

  // Checks that the encoded CB-Data IE matches the expected message.
  static void check_ucs2_encoded_message(std::vector<uint8_t>& cb_data_ucs2, const std::string& expected_message)
  {
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2_converter;
    std::u16string expected_message_utf16 = ucs2_converter.from_bytes(expected_message);

    // Obtain the number of unicode characters in the expected message.
    std::size_t total_message_length = expected_message_utf16.length();

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

      std::u16string expected_msg_in_page =
          expected_message_utf16.substr(i_page_expected_msg_begin, i_page_nof_ucs2_chars);

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

const std::string CbsEncoderFixture::gsm_7_supported_chars =
    "@$\n_ !\"#%&\'()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ?abcdefghijklmnopqrstuvwxyz^{}\\[~]|";

TEST_P(CbsEncoderFixture, CbsEncoderTest)
{
  unsigned message_size = std::get<0>(GetParam());

  // Test GSM-7 encoding.
  std::string message = generate_cbs_message(message_size, cbs_encoder::encoding_type::GSM7);
  cbs_encoder::fill_cb_data_gsm7(message);

  // Generate a CBS message that contains supported characters given the chosen encoding.
  message = generate_cbs_message(message_size, cbs_encoder::encoding_type::UCS2);

  // Encode into a CB-Data IE.
  std::vector<uint8_t> ucs2_enc_message = cbs_encoder::fill_cb_data_ucs2(message);

  // Test encoding.
  check_ucs2_encoded_message(ucs2_enc_message, message);
}

class CbsEncodingSelectionFixture : public ::testing::Test
{
protected:
  void SetUp() override {}
};

TEST_F(CbsEncodingSelectionFixture, CbsEncodingSelectionTest)
{
  // Supported data coding scheme values.
  auto is_coding_scheme_valid = [](uint8_t value) {
    // Languages using the default GSM-7 alphabet.
    return ((value >= 0x00) && (value <= 0x0F)) ||
           // Data coding / message handling, bit 2 must be set to 0.
           ((value >= 0xF0) && (value <= 0xFF) && ((value & 0b00000100) == 0)) ||
           // General data coding indication (no message class meaning), bit 2 must be set to 0.
           ((value >= 0x40) && (value <= 0x4F) && ((value & 0b00000100) == 0)) ||
           // General data coding indication (message class meaning), bit 2 must be set to 0.
           ((value >= 0x50) && (value <= 0x5F) && ((value & 0b00000100) == 0));
  };

  for (unsigned i_data_coding_scheme = 0; i_data_coding_scheme != 0xFF; ++i_data_coding_scheme) {
    cbs_encoder::encoding_type encoding = cbs_encoder::select_cbs_encoding(i_data_coding_scheme);
    if (is_coding_scheme_valid(i_data_coding_scheme)) {
      ASSERT_TRUE(encoding != cbs_encoder::encoding_type::INVALID);
    } else {
      ASSERT_TRUE(encoding == cbs_encoder::encoding_type::INVALID);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(CbsEncoderTest,
                         CbsEncoderFixture,
                         ::testing::Combine(
                             // Message length
                             ::testing::Values(1, 5, 19, 51, 113, 503),
                             // Test repetitions.
                             ::testing::Range(0U, 100U)));

} // namespace