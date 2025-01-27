/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cbs/cbs_encoder.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math/math_utils.h"
#include <codecvt>
#include <locale>
#include <map>

using namespace srsran;

/// Description of a GSM-7 character (See TS23.038 Section 6.2.1).
struct gsm7_char {
  /// Character code.
  uint8_t code;
  /// Indicates if the character is in the extension table.
  bool is_ext_table;
};

/// GSM-7 default alphabet map, including the default extension table.
static const std::map<char, gsm7_char> gsm7_default_alphabet_map = {
    {'@', {0x00, false}},  {'$', {0x02, false}}, {'\n', {0x0A, false}}, {'_', {0x11, false}}, {' ', {0x20, false}},
    {'!', {0x21, false}},  {'"', {0x22, false}}, {'#', {0x23, false}},  {'%', {0x25, false}}, {'&', {0x26, false}},
    {'\'', {0x27, false}}, {'(', {0x28, false}}, {')', {0x29, false}},  {'*', {0x2A, false}}, {'+', {0x2B, false}},
    {',', {0x2C, false}},  {'-', {0x2D, false}}, {'.', {0x2E, false}},  {'/', {0x2F, false}}, {'0', {0x30, false}},
    {'1', {0x31, false}},  {'2', {0x32, false}}, {'3', {0x33, false}},  {'4', {0x34, false}}, {'5', {0x35, false}},
    {'6', {0x36, false}},  {'7', {0x37, false}}, {'8', {0x38, false}},  {'9', {0x39, false}}, {':', {0x3A, false}},
    {';', {0x3B, false}},  {'<', {0x3C, false}}, {'=', {0x3D, false}},  {'>', {0x3E, false}}, {'?', {0x3F, false}},
    {'A', {0x41, false}},  {'B', {0x42, false}}, {'C', {0x43, false}},  {'D', {0x44, false}}, {'E', {0x45, false}},
    {'F', {0x46, false}},  {'G', {0x47, false}}, {'H', {0x48, false}},  {'I', {0x49, false}}, {'J', {0x4A, false}},
    {'K', {0x4B, false}},  {'L', {0x4C, false}}, {'M', {0x4D, false}},  {'N', {0x4E, false}}, {'O', {0x4F, false}},
    {'P', {0x50, false}},  {'Q', {0x51, false}}, {'R', {0x52, false}},  {'S', {0x53, false}}, {'T', {0x54, false}},
    {'U', {0x55, false}},  {'V', {0x56, false}}, {'W', {0x57, false}},  {'X', {0x58, false}}, {'Y', {0x59, false}},
    {'Z', {0x5A, false}},  {'?', {0x60, false}}, {'a', {0x61, false}},  {'b', {0x62, false}}, {'c', {0x63, false}},
    {'d', {0x64, false}},  {'e', {0x65, false}}, {'f', {0x66, false}},  {'g', {0x67, false}}, {'h', {0x68, false}},
    {'i', {0x69, false}},  {'j', {0x6A, false}}, {'k', {0x6B, false}},  {'l', {0x6C, false}}, {'m', {0x6D, false}},
    {'n', {0x6E, false}},  {'o', {0x6F, false}}, {'p', {0x70, false}},  {'q', {0x71, false}}, {'r', {0x72, false}},
    {'s', {0x73, false}},  {'t', {0x74, false}}, {'u', {0x75, false}},  {'v', {0x76, false}}, {'w', {0x77, false}},
    {'x', {0x78, false}},  {'y', {0x79, false}}, {'z', {0x7A, false}},  {'^', {0x14, true}},  {'{', {0x28, true}},
    {'}', {0x29, true}},   {'\\', {0x2F, true}}, {'[', {0x3C, true}},   {'~', {0x3D, true}},  {']', {0x3E, true}},
    {'|', {0x40, true}}};

bool cbs_encoder::encode_gsm7(std::vector<uint8_t>& out, const std::string& data)
{
  unsigned bit_count = 0;
  unsigned bit_queue = 0;

  for (unsigned i_char = 0, i_char_end = data.length(); i_char != i_char_end; ++i_char) {
    // Convert from ASCII to GSM-7 using the general language table.
    auto gsm_char = gsm7_default_alphabet_map.find(data[i_char]);

    // If any character in the message is not in the GSM-7 default alphabet, encoding fails.
    if (gsm_char == gsm7_default_alphabet_map.end()) {
      return false;
    }

    // If the character is in the extension table, append the escape character.
    if (gsm_char->second.is_ext_table) {
      bit_queue |= (esc_code & 0x7FU) << bit_count;
      bit_count += 7;
    }

    // Add the GSM-7 character to the queue.
    bit_queue |= (gsm_char->second.code & 0x7FU) << bit_count;
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
    bit_queue |= (cr_code & 0x7FU) << 1U;
  }

  // Add the trailing bits to the output if there are any.
  if (bit_count > 0) {
    out.push_back(static_cast<uint8_t>(bit_queue));
  }

  return true;
}

unsigned cbs_encoder::get_nof_gsm7_chars(const std::string& data)
{
  unsigned nof_chars = 0;
  for (unsigned i_char = 0, i_char_end = data.length(); i_char != i_char_end; ++i_char) {
    // Each ASCII character is converted into a GSM-7 character.
    ++nof_chars;

    gsm7_char gsm_char = gsm7_default_alphabet_map.find(data[i_char])->second;

    // If the character is in the extension table, an additional escape character is appended.
    if (gsm_char.is_ext_table) {
      ++nof_chars;
    }
  }

  return nof_chars;
}

bool cbs_encoder::encode_ucs2(std::vector<uint8_t>& out, const std::string& data)
{
  // Convert from UTF-8 (8bit string variable length encoding) to UCS-2 (16 bit string fixed length encoding).
  std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2_converter;
  std::u16string                                              ucs2_data;

  // Attempt conversion to UCS-2. If any character requires more than two bytes, skip encoding.
  try {
    ucs2_data = ucs2_converter.from_bytes(data);
  } catch (std::range_error& e) {
    return false;
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

std::vector<uint8_t> cbs_encoder::fill_cb_data_gsm7(const std::string& message)
{
  // Number of warning message characters that fit in an 82 byte CB-Data page, as per TS23.038 Section 6.1.2.2.
  static constexpr unsigned max_nof_gsm7_chars_page = 93;

  // Number of GSM-7 characters required to encode the warning message.
  unsigned encoded_message_len = cbs_encoder::get_nof_gsm7_chars(message);

  // Determine the number of information pages.
  unsigned nof_pages = srsran::divide_ceil(encoded_message_len, max_nof_gsm7_chars_page);

  srsran_assert(nof_pages > 0, "No warning message is provided");

  if (nof_pages > max_nof_cb_data_pages) {
    report_error(
        "GSM-7 message length (i.e. {} chars) is too long. Maximum length is {} chars without extension characters.",
        message.size(),
        max_nof_cb_data_pages * max_nof_gsm7_chars_page);
  }

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
    unsigned i_encoded_message_len = cbs_encoder::get_nof_gsm7_chars(i_message_segment);

    // If the encoded message length is too large to fit in the current information page, reduce the length of the
    // message segment to be encoded.
    if (i_encoded_message_len > max_nof_gsm7_chars_page) {
      i_message_len -= (i_encoded_message_len - max_nof_gsm7_chars_page);
      i_message_segment     = message.substr(i_message_offset, i_message_len);
      i_encoded_message_len = cbs_encoder::get_nof_gsm7_chars(i_message_segment);
    }

    // Encode the characters of the warning message that go into the current page.
    std::vector<uint8_t> encoded_message;
    if (!cbs_encoder::encode_gsm7(encoded_message, i_message_segment)) {
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
    memcpy(&cb_data[i_page_offset], encoded_message.data(), encoded_message.size());

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

std::vector<uint8_t> cbs_encoder::fill_cb_data_ucs2(const std::string& message)
{
  // Number of UCS-2 warning message characters that fit in an 82-byte CB-Data page.
  static constexpr unsigned max_nof_ucs2_chars_page = info_page_nof_bytes / 2;

  // Encode the characters of the warning message in one go.
  std::vector<uint8_t> encoded_message;
  if (!cbs_encoder::encode_ucs2(encoded_message, message)) {
    report_error("Warning message could not be encoded using UCS-2.");
  }

  // Determine the number of information pages.
  unsigned nof_pages = srsran::divide_ceil(encoded_message.size(), info_page_nof_bytes);

  srsran_assert(nof_pages > 0, "No warning message is provided");

  if (nof_pages > max_nof_cb_data_pages) {
    report_error("UCS-2 message length (i.e. {} chars) is too long. Maximum length is {} chars.",
                 message.size(),
                 max_nof_cb_data_pages * max_nof_ucs2_chars_page);
  }

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
    memcpy(&cb_data[i_page_offset], &encoded_message[i_message_offset], i_message_nof_bytes);

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

cbs_encoder::encoding_type cbs_encoder::select_cbs_encoding(uint8_t data_coding_scheme)
{
  {
    uint8_t coding_group = data_coding_scheme >> 4U;
    uint8_t bits3_0      = data_coding_scheme & 0x0F;
    switch (coding_group) {
      case 0x0:
        // Languages using the GSM-7 default alphabet.
        return cbs_encoder::encoding_type::GSM7;
      case 0x4:
        // General data coding indication, uncompressed and no message class meaning on bits 1..0.
      case 0x5: {
        // General data coding indication, uncompressed and message class meaning on bits 1..0.
        uint8_t coding_bits = bits3_0 >> 2U;
        // Bits 3..2 indicate the message encoding.
        if (coding_bits == 0U) {
          return cbs_encoder::encoding_type::GSM7;
        }
        if (coding_bits == 2U) {
          return cbs_encoder::encoding_type::UCS2;
        }
        return cbs_encoder::encoding_type::INVALID;
      }
      case 0xF:
        // Data coding / message handling.
        if (((bits3_0 >> 2U) & 1U) == 0U) {
          return cbs_encoder::encoding_type::GSM7;
        }
        return cbs_encoder::encoding_type::INVALID;
      default:
        // All unsupported or reserved coding groups.
        return cbs_encoder::encoding_type::INVALID;
    }
  }
}