/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "fmt/format.h"

namespace srsran {

/// Encoder of bits in a byte buffer.
class bit_encoder
{
public:
  explicit bit_encoder(byte_buffer& bytes) : writer(bytes) {}

  /// Get a view to held byte buffer.
  srsran::byte_buffer_view data() const { return writer.view(); }

  /// Gets number of written bytes.
  unsigned nof_bytes() const { return writer.length(); }

  /// Gets number of written bits.
  unsigned nof_bits() const { return 8U * (writer.length() - (offset == 0U ? 0U : 1U)) + offset; }

  /// \brief Get offset of the next bit to be set. Offset 0 corresponds to the MSB of the current byte being packed,
  /// while offset 7 corresponds to LSB.
  unsigned next_bit_offset() const { return offset; }

  /// \brief Append provided bitmap into byte_buffer held by bit_encoder.
  /// The bitmap to pack is left aligned (i.e. MSB of bitmap will be the last bit to be packed in underlying buffer).
  /// Example:
  /// - Before:           [...][11______]
  /// - pack(0b1011, 4):  [...][111011__]
  /// - pack(0b01100, 5): [...][11101101][100_____]
  /// \param val bitmap to be packed.
  /// \param n_bits number of bits to pack.
  /// \return success or failure.
  bool pack(uint64_t val, uint32_t n_bits);

  /// Append range of bytes into byte_buffer held by bit_encoder.
  /// \param bytes span of bytes.
  void pack_bytes(srsran::span<const uint8_t> bytes);

  /// Append bytes of a byte_buffer into byte_buffer held by bit_encoder.
  void pack_bytes(srsran::byte_buffer_view bytes);

  /// Pads held buffer with zeros until the next byte.
  void align_bytes_zero();

private:
  /// Interface to byte buffer where bits are going to be appended.
  srsran::byte_buffer_writer writer;
  /// Offset of the next bit to be set. Values: (0..7).
  uint8_t offset = 0;
};

/// Decoder of bits stored in a byte_buffer.
class bit_decoder
{
public:
  explicit bit_decoder(srsran::byte_buffer_view buffer_) : buffer(buffer_), it(buffer.begin()) {}

  /// Get a view to held byte buffer.
  byte_buffer_view data() const { return buffer; }

  /// Gets number of read bytes.
  unsigned nof_bytes() const { return (it - buffer.begin()) + (offset != 0 ? 1U : 0U); }

  /// Gets number of read bits.
  unsigned nof_bits() const { return offset + 8U * static_cast<unsigned>(it - buffer.begin()); }

  /// Get offset of the next bit to be set.
  unsigned next_bit_offset() const { return offset; }

  /// Advance reader by n_bits bits.
  /// \param n_bits number of bits to advance the reader.
  /// \return true if bit jump did not exceed the end of the byte buffer. False, otherwise.
  bool advance_bits(uint32_t n_bits);

  /// Read bits from underlying byte_buffer pointed by bit_decoder.
  /// \param val bitmap of bits read.
  /// \param n_bits number of bits to read.
  /// \return true if read is successful. False, if the number of bits read exceeded the byte_buffer size.
  template <class T>
  bool unpack(T& val, uint32_t n_bits);

  /// Read unaligned bytes from underlying byte_buffer.
  /// \param bytes span of bytes where the result is stored. The span size defines the number of bytes to be read.
  /// \return true if successful. False if the number of bytes to be read exceeds the end of the byte_buffer.
  bool unpack_bytes(srsran::span<uint8_t> bytes);

  /// Create a view across \c n_bytes bytes of the underlying byte_buffer, starting at the first full byte from the
  /// current location. For this purpose, the decoder is first automatically aligned via \c align_bytes.
  /// \param n_bytes Number of bytes to read.
  /// \return byte_buffer_view across \c n_bytes bytes of the underlying byte_buffer.
  /// An empty view is returned if the number of bytes to be read exceeds the end of the byte_buffer.
  byte_buffer_view unpack_aligned_bytes(size_t n_bytes);

  /// Skip bits until the beginning of the next byte.
  void align_bytes();

private:
  srsran::byte_buffer_view            buffer;
  srsran::byte_buffer::const_iterator it;
  uint8_t                             offset = 0;
};

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for bit_encoder
template <>
struct formatter<srsran::bit_encoder> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::bit_encoder& s, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (s.next_bit_offset() == 0) {
      fmt::format_to(ctx.out(), "{:b}", s.data());
    } else {
      fmt::format_to(ctx.out(), "{:b}", s.data().view(0, s.nof_bytes() - 1));
      uint8_t last_byte = s.data()[s.nof_bytes() - 1];
      last_byte         = last_byte >> (8U - s.next_bit_offset());
      fmt::format_to(ctx.out(), " {:0>{}b}", last_byte, s.next_bit_offset());
    }
    return ctx.out();
  }
};

/// \brief Custom formatter for bit_encoder
template <>
struct formatter<srsran::bit_decoder> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::bit_decoder& s, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (s.next_bit_offset() == 0) {
      fmt::format_to(ctx.out(), "{:b}", s.data().view(0, s.nof_bytes()));
    } else {
      fmt::format_to(ctx.out(), "{:b}", s.data().view(0, s.nof_bytes() - 1));
      uint8_t last_byte = s.data()[s.nof_bytes() - 1];
      last_byte         = last_byte >> (8U - s.next_bit_offset());
      fmt::format_to(ctx.out(), " {:0>{}b}", last_byte, s.next_bit_offset());
    }
    return ctx.out();
  }
};

} // namespace fmt
