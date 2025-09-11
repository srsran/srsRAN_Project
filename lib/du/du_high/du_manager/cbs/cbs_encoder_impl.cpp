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

#include "cbs_encoder_impl.h"
#include "srsran/adt/span.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math/math_utils.h"
#include <algorithm>
#include <locale>

using namespace srsran;

namespace {

/// Description of a GSM-7 character (See TS23.038 Section 6.2.1).
struct gsm7_char {
  /// Represented character.
  char ascii_char;
  /// Character code.
  uint8_t gsm7_code;
  /// Indicates if the character is in the extension table.
  bool is_ext_table;
};

} // namespace

/// GSM-7 default alphabet map, including the default extension table.
static const std::array<gsm7_char, 96> gsm7_default_alphabet = {
    {{'@', 0x00, false},  {'$', 0x02, false}, {'\n', 0x0a, false}, {'_', 0x11, false}, {' ', 0x20, false},
     {'!', 0x21, false},  {'"', 0x22, false}, {'#', 0x23, false},  {'%', 0x25, false}, {'&', 0x26, false},
     {'\'', 0x27, false}, {'(', 0x28, false}, {')', 0x29, false},  {'*', 0x2a, false}, {'+', 0x2b, false},
     {',', 0x2c, false},  {'-', 0x2d, false}, {'.', 0x2e, false},  {'/', 0x2f, false}, {'0', 0x30, false},
     {'1', 0x31, false},  {'2', 0x32, false}, {'3', 0x33, false},  {'4', 0x34, false}, {'5', 0x35, false},
     {'6', 0x36, false},  {'7', 0x37, false}, {'8', 0x38, false},  {'9', 0x39, false}, {':', 0x3a, false},
     {';', 0x3b, false},  {'<', 0x3c, false}, {'=', 0x3d, false},  {'>', 0x3e, false}, {'?', 0x3f, false},
     {'A', 0x41, false},  {'B', 0x42, false}, {'C', 0x43, false},  {'D', 0x44, false}, {'E', 0x45, false},
     {'F', 0x46, false},  {'G', 0x47, false}, {'H', 0x48, false},  {'I', 0x49, false}, {'J', 0x4a, false},
     {'K', 0x4b, false},  {'L', 0x4c, false}, {'M', 0x4d, false},  {'N', 0x4e, false}, {'O', 0x4f, false},
     {'P', 0x50, false},  {'Q', 0x51, false}, {'R', 0x52, false},  {'S', 0x53, false}, {'T', 0x54, false},
     {'U', 0x55, false},  {'V', 0x56, false}, {'W', 0x57, false},  {'X', 0x58, false}, {'Y', 0x59, false},
     {'Z', 0x5a, false},  {'?', 0x60, false}, {'a', 0x61, false},  {'b', 0x62, false}, {'c', 0x63, false},
     {'d', 0x64, false},  {'e', 0x65, false}, {'f', 0x66, false},  {'g', 0x67, false}, {'h', 0x68, false},
     {'i', 0x69, false},  {'j', 0x6a, false}, {'k', 0x6b, false},  {'l', 0x6c, false}, {'m', 0x6d, false},
     {'n', 0x6e, false},  {'o', 0x6f, false}, {'p', 0x70, false},  {'q', 0x71, false}, {'r', 0x72, false},
     {'s', 0x73, false},  {'t', 0x74, false}, {'u', 0x75, false},  {'v', 0x76, false}, {'w', 0x77, false},
     {'x', 0x78, false},  {'y', 0x79, false}, {'z', 0x7a, false},  {'^', 0x14, true},  {'{', 0x28, true},
     {'}', 0x29, true},   {'\\', 0x2f, true}, {'[', 0x3c, true},   {'~', 0x3d, true},  {']', 0x3e, true},
     {'|', 0x40, true}}};

std::vector<uint8_t> cbs_encoder_impl::encode_cb_data(const std::string& message, unsigned data_coding_scheme) const
{
  std::vector<uint8_t> encoded_warning_message;

  cbs_encoder_impl::encoding_type encoding = cbs_encoder_impl::select_cbs_encoding(data_coding_scheme);
  switch (encoding) {
    case cbs_encoder_impl::encoding_type::GSM7:
      encoded_warning_message = cbs_encoder_impl::fill_cb_data_gsm7(message);
      break;
    case cbs_encoder_impl::encoding_type::UCS2:
      encoded_warning_message = cbs_encoder_impl::fill_cb_data_ucs2(message);
      break;
    case cbs_encoder_impl::encoding_type::INVALID:
    default:
      report_error("Invalid or unsupported CBS encoding type.");
  }

  return encoded_warning_message;
}

/// \brief Encodes an ASCII message into GSM-7 using the default alphabet and CBS packing of 7 bit characters.
///
/// \param[out] out The GSM-7 encoded and packed message.
/// \param[in] data Input data string in UTF-8 format.
/// \return \c True if the message was successfully encoded, \c false otherwise.
/// \remark An error message is thrown if any character in \c data is not present in the GSM-7 default alphabet and
/// its extension table or is not a printable ASCII character.
static bool encode_gsm7(std::vector<uint8_t>& out, const std::string& data);

/// \brief Encodes an UTF-8 message into UCS-2 as specified in TS23.038 Section 6.2.3.
///
/// Some code points supported by UTF-8 cannot be encoded in UCS-2. Since UCS-2 encodes each code point in a fixed
/// 16-bit value, code points outside the basic multilingual unicode plane are not supported by UCS-2. If the provided
/// data string contains any of such characters, an error message is displayed and the string is not encoded.
///
/// \param[out] out The UCS-2 encoded and packed message. Each 16-bit code point is packed in big-endian order.
/// \param[in] data Input data string in UTF-8 format.
/// \return \c True if the message was successfully encoded, \c false otherwise.
static bool encode_ucs2(std::vector<uint8_t>& out, const std::string& data);

/// \brief Gets the number of 7-bit GSM-7 characters required to encode a message.
///
/// The number of characters includes the escape characters required for accessing the extension tables. See TS23.038
/// Section 6.2.1.2.2.
///
/// \param[in] data Input data string.
/// \return The number of required 7-bit GSM-7 characters.
static unsigned get_nof_gsm7_chars(const std::string& data);

std::vector<uint8_t> cbs_encoder_impl::fill_cb_data_gsm7(const std::string& message)
{
  // Number of warning message characters that fit in an 82 byte CB-Data page, as per TS23.038 Section 6.1.2.2.
  static constexpr unsigned max_nof_gsm7_chars_page = 93;

  // Number of GSM-7 characters required to encode the warning message.
  unsigned encoded_message_len = get_nof_gsm7_chars(message);

  // Determine the number of information pages.
  unsigned nof_pages = srsran::divide_ceil(encoded_message_len, max_nof_gsm7_chars_page);

  srsran_assert(nof_pages > 0, "No warning message is provided");

  report_error_if_not(
      nof_pages <= max_nof_cb_data_pages,
      "GSM-7 message length (i.e. {} chars) is too long. Maximum length is {} chars without extension characters.",
      message.size(),
      max_nof_cb_data_pages * max_nof_gsm7_chars_page);

  // Create and size the CB data message. See TS23.041 Section 9.4.2.2.5.
  std::vector<uint8_t> cb_data(1 + nof_pages * (info_page_nof_bytes + info_len_nof_bytes));

  // Write the number of information pages.
  cb_data[0] = static_cast<uint8_t>(nof_pages);

  // Byte offset for the warning message.
  unsigned i_message_offset = 0;

  for (unsigned i_page = 0; i_page != nof_pages; ++i_page) {
    // Byte offset for the current page inside the CB-DATA IE.
    unsigned i_page_offset = 1 + i_page * (info_page_nof_bytes + info_len_nof_bytes);

    // Number of characters of the warning message to be encoded into the current information page.
    unsigned i_message_len =
        std::min(max_nof_gsm7_chars_page, static_cast<unsigned>(message.size()) - i_message_offset);

    // Segment of the message that will go in the current information page.
    std::string i_message_segment = message.substr(i_message_offset, i_message_len);

    // Number of GSM-7 characters belonging to the current segment. It depends on the number of characters (if any)
    // That belong to the GSM-7 default alphabet extension table.
    unsigned i_encoded_message_len = get_nof_gsm7_chars(i_message_segment);

    // If the encoded message length is too large to fit in the current information page, reduce the length of the
    // message segment to be encoded.
    if (i_encoded_message_len > max_nof_gsm7_chars_page) {
      i_message_len -= (i_encoded_message_len - max_nof_gsm7_chars_page);
      i_message_segment     = message.substr(i_message_offset, i_message_len);
      i_encoded_message_len = get_nof_gsm7_chars(i_message_segment);
    }

    // Encode the characters of the warning message that go into the current page.
    std::vector<uint8_t> encoded_message;
    if (!encode_gsm7(encoded_message, i_message_segment)) {
      report_error(
          "Warning message could not be encoded using GSM-7 default alphabet. Set the data coding scheme to a scheme "
          "using UCS-2.");
    }

    srsran_assert(encoded_message.size() <= info_page_nof_bytes,
                  "The encoded warning message size (i.e., {} bytes) exceeds the size of the CB-Data information page "
                  "(i.e. {} bytes).",
                  encoded_message.size(),
                  info_page_nof_bytes);

    // Copy the encoded message bytes into the information page.
    std::memcpy(&cb_data[i_page_offset], encoded_message.data(), encoded_message.size());

    // Add padding to unused information page bytes (See TS23.041 Section 9.3.19).
    if (i_encoded_message_len < info_page_nof_bytes) {
      std::fill(&cb_data[i_page_offset + encoded_message.size()], &cb_data[i_page_offset + info_page_nof_bytes], 0x00);
    }

    // Add message information page length at the end.
    cb_data[i_page_offset + info_page_nof_bytes] = static_cast<uint8_t>(encoded_message.size());

    i_message_offset += i_message_len;
  }

  return cb_data;
}

std::vector<uint8_t> cbs_encoder_impl::fill_cb_data_ucs2(const std::string& message)
{
  // Number of UCS-2 warning message characters that fit in an 82-byte CB-Data page.
  static constexpr unsigned max_nof_ucs2_chars_page = info_page_nof_bytes / 2;

  // Encode the characters of the warning message in one go.
  std::vector<uint8_t> encoded_message;
  if (!encode_ucs2(encoded_message, message)) {
    report_error("Warning message could not be encoded using UCS-2.");
  }

  // Determine the number of information pages.
  unsigned nof_pages = srsran::divide_ceil(encoded_message.size(), info_page_nof_bytes);

  srsran_assert(nof_pages > 0, "No warning message is provided");

  report_error_if_not(nof_pages <= max_nof_cb_data_pages,
                      "UCS-2 message length (i.e. {} characters) is too long. Maximum length is {} characters.",
                      encoded_message.size() / 2,
                      max_nof_cb_data_pages * max_nof_ucs2_chars_page);

  // Create and size the CB data message. See TS23.041 Section 9.4.2.2.5.
  std::vector<uint8_t> cb_data(1 + nof_pages * (info_page_nof_bytes + info_len_nof_bytes));

  // Write the number of information pages.
  cb_data[0] = static_cast<uint8_t>(nof_pages);

  // Byte offset for the encoded warning message.
  unsigned i_message_offset = 0;

  for (unsigned i_page = 0; i_page != nof_pages; ++i_page) {
    // Byte offset for the current page inside the CB-DATA IE.
    unsigned i_page_offset = 1 + i_page * (info_page_nof_bytes + info_len_nof_bytes);

    // Number of bytes of the current message segment.
    unsigned i_message_nof_bytes = std::min(static_cast<size_t>(info_page_nof_bytes),
                                            encoded_message.size() - static_cast<size_t>(i_message_offset));

    // Copy the encoded message bytes into the information page.
    std::memcpy(&cb_data[i_page_offset], &encoded_message[i_message_offset], i_message_nof_bytes);

    // Add padding to unused information page bytes (See TS23.041 Section 9.3.19).
    if (i_message_nof_bytes < info_page_nof_bytes) {
      std::fill(&cb_data[i_page_offset + i_message_nof_bytes], &cb_data[i_page_offset + info_page_nof_bytes], 0x00);
    }

    // Add message information page length at the end.
    cb_data[i_page_offset + info_page_nof_bytes] = static_cast<uint8_t>(i_message_nof_bytes);

    i_message_offset += i_message_nof_bytes;
  }

  return cb_data;
}

bool encode_gsm7(std::vector<uint8_t>& out, const std::string& data)
{
  // GSM-7 escape code.
  static constexpr uint8_t esc_code = 0x1b;
  // GSM-7 carriage return (CR) code.
  static constexpr uint8_t cr_code = 0x0d;

  unsigned bit_count = 0;
  unsigned bit_queue = 0;

  for (unsigned i_char = 0, i_char_end = data.length(); i_char != i_char_end; ++i_char) {
    // Convert from ASCII to GSM-7 using the general language table.
    const auto gsm_char = std::find_if(gsm7_default_alphabet.begin(),
                                       gsm7_default_alphabet.end(),
                                       [&data, i_char](const gsm7_char& c) { return data[i_char] == c.ascii_char; });

    // If any character in the message is not in the GSM-7 default alphabet, encoding fails.
    if (gsm_char == gsm7_default_alphabet.end()) {
      return false;
    }

    // If the character is in the extension table, append the escape character.
    if (gsm_char->is_ext_table) {
      bit_queue |= (esc_code & 0x7fU) << bit_count;
      bit_count += 7;
    }

    // Add the GSM-7 character to the queue.
    bit_queue |= (gsm_char->gsm7_code & 0x7fU) << bit_count;
    bit_count += 7;

    // Write full octets to the output.
    while (bit_count >= 8) {
      out.push_back(static_cast<uint8_t>(bit_queue));
      bit_count -= 8;
      bit_queue >>= 8U;
    }
  }

  // If there are 7 spare bits at the last octet, the receiver may confuse these for the @ character, therefore a
  // carriage return (CR) is appended. See TS23.038 Section 6.1.2.
  if (bit_count == 1) {
    bit_queue |= (cr_code & 0x7fU) << 1U;
  }

  // Add the trailing bits to the output if there are any.
  if (bit_count > 0) {
    out.push_back(static_cast<uint8_t>(bit_queue));
  }

  return true;
}

unsigned get_nof_gsm7_chars(const std::string& data)
{
  unsigned nof_chars = 0;
  for (unsigned i_char = 0, i_char_end = data.length(); i_char != i_char_end; ++i_char) {
    // Each ASCII character is converted into a GSM-7 character.
    ++nof_chars;

    const auto gsm_char = std::find_if(gsm7_default_alphabet.begin(),
                                       gsm7_default_alphabet.end(),
                                       [&data, i_char](const gsm7_char& c) { return data[i_char] == c.ascii_char; });

    // If the character is in the extension table, an additional escape character is appended.
    if ((gsm_char != gsm7_default_alphabet.end()) && gsm_char->is_ext_table) {
      ++nof_chars;
    }
  }

  return nof_chars;
}

bool encode_ucs2(std::vector<uint8_t>& out, const std::string& data)
{
  std::u16string   ucs2_data;
  span<const char> data_view(data.data(), data.size());

  // Obtain the Unicode code point from the UTF-8 string and convert to UCS-2.
  while (!data_view.empty()) {
    uint32_t unicode_symbol = 0;
    size_t   nof_bytes      = 0;

    // Obtain the UTF-8 lead byte.
    uint8_t leading_char = data_view[0];

    if (leading_char <= 0x7f) {
      // One byte code point (ASCII characters).
      unicode_symbol = leading_char;
      nof_bytes      = 1;
    } else if ((leading_char & 0xe0) == 0xc0) {
      // Two byte code point (Other languages).
      unicode_symbol = leading_char & 0x1f;
      nof_bytes      = 2;
    } else if ((leading_char & 0xf0) == 0xe0) {
      // Three byte code point (rest of the basic multilingual plane).
      unicode_symbol = leading_char & 0x0f;
      nof_bytes      = 3;
    }

    // Do not encode 4 byte UTF-8 characters, as they lie outside the basic multilingual plane and are not supported by
    // the UCS-2 code.

    // If the UTF-8 character is not valid, stop encoding.
    if ((nof_bytes == 0) || (data_view.size() < nof_bytes)) {
      return false;
    }

    // Write the remaining bytes.
    for (unsigned i_byte = 1; i_byte < nof_bytes; ++i_byte) {
      unicode_symbol = (unicode_symbol << 6) | (data_view[i_byte] & 0x3f);
    }

    // Advance data view.
    data_view = data_view.last(data_view.size() - nof_bytes);

    // Encode the 32-bit code point into UCS-2, if possible.
    if (unicode_symbol > 0xffff) {
      return false;
    }

    ucs2_data.push_back(static_cast<char16_t>(unicode_symbol));
  }

  // Number of encoded UCS-2 bytes.
  size_t nof_out_bytes = ucs2_data.size() * sizeof(char16_t);

  // Reserve memory in output vector.
  out.reserve(nof_out_bytes);

  // Add the UCS-2 characters into the vector, in big-endian order (MSB first).
  for (auto& ucs2_char : ucs2_data) {
    out.push_back(reinterpret_cast<char*>(&ucs2_char)[1]);
    out.push_back(reinterpret_cast<char*>(&ucs2_char)[0]);
  }

  return true;
}

cbs_encoder_impl::encoding_type cbs_encoder_impl::select_cbs_encoding(uint8_t data_coding_scheme)
{
  {
    uint8_t coding_group = data_coding_scheme >> 4U;
    uint8_t bits3_0      = data_coding_scheme & 0x0f;
    switch (coding_group) {
      case 0x0:
        // Languages using the GSM-7 default alphabet.
        return cbs_encoder_impl::encoding_type::GSM7;
      case 0x4:
        // General data coding indication, uncompressed and no message class meaning on bits 1..0.
      case 0x5: {
        // General data coding indication, uncompressed and message class meaning on bits 1..0.
        uint8_t coding_bits = bits3_0 >> 2U;
        // Bits 3..2 indicate the message encoding.
        if (coding_bits == 0U) {
          return cbs_encoder_impl::encoding_type::GSM7;
        }
        if (coding_bits == 2U) {
          return cbs_encoder_impl::encoding_type::UCS2;
        }
        return cbs_encoder_impl::encoding_type::INVALID;
      }
      case 0xf:
        // Data coding / message handling.
        if (((bits3_0 >> 2U) & 1U) == 0U) {
          return cbs_encoder_impl::encoding_type::GSM7;
        }
        return cbs_encoder_impl::encoding_type::INVALID;
      default:
        // All unsupported or reserved coding groups.
        return cbs_encoder_impl::encoding_type::INVALID;
    }
  }
}
