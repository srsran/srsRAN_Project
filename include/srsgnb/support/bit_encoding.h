/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_BIT_ENCODING_H
#define SRSGNB_BIT_ENCODING_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/srslog/bundled/fmt/format.h"

namespace srsgnb {

class bit_encoder
{
public:
  bit_encoder(byte_buffer& bytes) : writer(bytes) {}

  srsgnb::byte_buffer_view data() const { return writer.view(); }

  /// Gets number of written bytes.
  unsigned nof_bytes() const { return writer.length(); }

  /// Gets number of written bits.
  unsigned nof_bits() const { return 8U * (writer.length() - (offset == 0U ? 0U : 1U)) + offset; }

  /// Get offset of the next bit to be set.
  unsigned next_bit_offset() const { return offset; }

  /// Append provided bitmap into byte_buffer held by bit_encoder.
  /// \param val bitmap to be packed.
  /// \param n_bits number of bits to pack.
  void pack(uint64_t val, uint32_t n_bits);

  /// Append range of bytes into byte_buffer held by bit_encoder.
  /// \param bytes span of bytes.
  void pack_bytes(srsgnb::span<const uint8_t> bytes);

  /// Append bytes of a byte_buffer into byte_buffer held by bit_encoder.
  void pack_bytes(srsgnb::byte_buffer_view bytes);

  /// Pads held buffer with zeros until the next byte.
  void align_bytes_zero();

private:
  srsgnb::byte_buffer_writer writer;
  uint8_t                    offset = 0;
};

} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for bounded_bitset<N, reversed>
template <>
struct formatter<srsgnb::bit_encoder> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::bit_encoder& s, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (s.next_bit_offset() == 0) {
      fmt::format_to(ctx.out(), "{:b}", s.data());
    } else {
      fmt::format_to(ctx.out(), "{:b}", s.data().view(0, s.nof_bytes() - 1));
      uint8_t last_byte = s.data()[s.nof_bytes() - 1];
      last_byte         = last_byte >> (8U - s.next_bit_offset());
      fmt::format_to(ctx.out(), " {:0>{}b}", (s.next_bit_offset()), last_byte);
    }
    return ctx.out();
  }
};

} // namespace fmt

#endif // SRSGNB_BIT_ENCODING_H
