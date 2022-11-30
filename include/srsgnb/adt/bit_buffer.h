/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/span.h"

namespace srsgnb {

/// Describes a bit buffer that contains packed bits.
class bit_buffer
{
protected:
  /// Internal storage word.
  using word_t = uint8_t;

  /// Number of bits in one word.
  static constexpr unsigned bits_per_word = sizeof(word_t) * 8;

  /// Calculates the number of words required for a given number of bits.
  static constexpr unsigned calculate_nof_words(unsigned nof_bits)
  {
    return (nof_bits + (bits_per_word - 1)) / bits_per_word;
  }

  /// \brief Replaces the internal data container and resets the current size to zero.
  /// \remark The previous data container will still contain the data present before calling this method.
  void set_buffer(span<word_t> new_buffer) { buffer = new_buffer; }

  /// \brief Sets the accessible size of the bit buffer.
  /// \remark An assertion is thrown if the new size exceeds the maximum allowed size.
  void set_current_size(unsigned new_size)
  {
    srsgnb_assert(new_size <= buffer.size() * bits_per_word,
                  "The new size (i.e., {}) exceeds the maximum size allowed by the data storage (i.e., {}).",
                  new_size,
                  buffer.size() * bits_per_word);
    current_size = new_size;
  }

  /// Default constructor - it creates a bit buffer with an invalid data storage.
  bit_buffer() : buffer({}), current_size(0) {}

  /// Creates a bit buffer from a buffer and a size.
  bit_buffer(span<word_t> buffer_, unsigned current_size_ = 0) : buffer(buffer_), current_size(current_size_)
  {
    srsgnb_assert(nof_words() <= buffer.size(),
                  "The current size (i.e., {}) requires {} words which exceeds the maximum number of words (i.e., {}).",
                  current_size,
                  nof_words(),
                  buffer.size());
  }

public:
  /// Fill with zeros.
  void zero() { std::fill_n(buffer.begin(), nof_words(), 0); }

  /// \brief Inserts the \c count least significant bits from \c value_ starting at \c startpos.
  /// \tparam Integer Integer type of the bits to insert.
  /// \param[in] value_   Consecutive bits to insert.
  /// \param[in] startpos Starting bit position.
  /// \param[in] count    Number of bits to insert.
  ///  \remark An assertion is triggered if the number of bits is larger than the number of bits per word.
  ///  \remark An assertion is triggered if \c value_ exceeds the number of bits to insert.
  ///  \remark An assertion is triggered if range of bits exceed the maximum size of the buffer.
  template <typename Integer>
  void insert(Integer value_, unsigned startpos, unsigned count)
  {
    srsgnb_assert(count <= bits_per_word,
                  "The number of bits to insert (i.e., {}) exceeds the number of bits per word (i.e., {}).",
                  count,
                  static_cast<unsigned>(bits_per_word));
    srsgnb_assert(value_ < (1U << count),
                  "The value (i.e., {}) number of bits exceeds the number of bits to insert (i.e., {}).",
                  value_,
                  count);
    srsgnb_assert(startpos + count <= size(),
                  "The bit range starting at {} for {} bits exceed the buffer size (i.e., {}).",
                  startpos,
                  count,
                  size());
    unsigned start_word = startpos / bits_per_word;
    unsigned start_mod  = startpos % bits_per_word;
    word_t   value      = static_cast<word_t>(value_);

    assert(value_ <= mask_lsb_ones<word_t>(count));

    // If the start bit is aligned with the beginning of the word.
    if (start_mod == 0) {
      buffer[start_word] = (buffer[start_word] & mask_msb_zeros<word_t>(count)) | (value << (bits_per_word - count));
      return;
    }

    // If the insertion only affects to one word.
    if (start_mod + count <= bits_per_word) {
      // Extract word.
      word_t word = buffer[start_word];
      // Mask bits that are used.
      word &= static_cast<word_t>(mask_msb_ones<word_t>(start_mod) |
                                  mask_lsb_ones<word_t>(bits_per_word - start_mod - count));
      // Insert bits.
      word |= value << (bits_per_word - start_mod - count);
      // Insert word.
      buffer[start_word] = word;
      return;
    }

    buffer[start_word] =
        (buffer[start_word] & mask_msb_ones<word_t>(start_mod)) | (value >> (start_mod + count - bits_per_word));
    buffer[start_word + 1] = (buffer[start_word + 1] & mask_msb_zeros<word_t>(start_mod + count - bits_per_word)) |
                             (value << (2 * bits_per_word - start_mod - count));
  }

  /// \brief Extracts a consecutive \c count number of bits starting at \c startpos.
  ///
  /// Extracts \c count bits starting at \c startpos position and stores them at the \c count least significant bits.
  ///
  /// \tparam Integer Integer type of the bits to insert.
  /// \param[in] startpos Starting bit position.
  /// \param[in] count    Number of bits to insert.
  /// \return The corresponding bits occupying the \c count least significant bits.
  ///  \remark An assertion is triggered if the number of bits is larger than the number of bits per word.
  ///  \remark An assertion is triggered if range of bits exceed the maximum size of the buffer.
  template <typename Integer = uint8_t>
  Integer extract(unsigned startpos, unsigned count) const
  {
    srsgnb_assert(count <= bits_per_word,
                  "The number of bits to insert (i.e., {}) exceeds the number of bits per word (i.e., {}).",
                  count,
                  static_cast<unsigned>(bits_per_word));
    srsgnb_assert(startpos + count <= size(),
                  "The bit range starting at {} for {} bits exceed the buffer size (i.e., {}).",
                  startpos,
                  count,
                  size());
    unsigned start_word = startpos / bits_per_word;
    unsigned start_mod  = startpos % bits_per_word;

    // If the operation does not cross boundaries between words.
    if (start_mod == 0) {
      return buffer[start_word] >> (bits_per_word - count);
    }

    // If the insertion only affects to one word.
    if (start_mod + count <= bits_per_word) {
      return (buffer[start_word] >> (bits_per_word - start_mod - count)) & mask_lsb_ones<Integer>(count);
    }

    word_t first_word  = buffer[start_word];
    word_t second_word = buffer[start_word + 1];

    unsigned shift = (start_mod + count) - bits_per_word;

    Integer result = static_cast<Integer>(first_word << shift);
    result |= static_cast<Integer>(second_word >> (bits_per_word - shift));
    result &= mask_lsb_ones<Integer>(count);

    return result;
  }

  /// Gets the current bit buffer size.
  size_t size() const { return current_size; }

  /// Converts the bit buffer into a binary string.
  template <typename OutputIt>
  OutputIt to_bin_string(OutputIt&& mem_buffer) const
  {
    if (nof_words() > 1) {
      fmt::format_to(mem_buffer, "{:08B}", buffer.first(nof_words() - 1));
    }

    unsigned byte_remainder = current_size % bits_per_word;

    fmt::format_to(mem_buffer, " {:0{}B}", buffer[nof_words() - 1] >> (bits_per_word - byte_remainder), byte_remainder);
    return mem_buffer;
  }

  /// Converts the bit buffer into a hexadecimal string.
  template <typename OutputIt>
  OutputIt to_hex_string(OutputIt&& mem_buffer) const
  {
    fmt::format_to(mem_buffer, "{:02X}", buffer.first(nof_words()));
    return mem_buffer;
  }

  /// Determines whether the bit buffer is empty.
  bool is_empty() const { return current_size == 0; }

  /// Copy assign operator.
  bit_buffer& operator=(const bit_buffer& other)
  {
    buffer       = other.buffer;
    current_size = other.current_size;
    return *this;
  }

private:
  /// Determines the number of words that are currently used.
  unsigned nof_words() const { return calculate_nof_words(size()); }

  /// Data storage.
  span<word_t> buffer;
  /// Current size in bits.
  size_t current_size;
};

/// \brief Implements a bit buffer that uses dynamic memory.
/// \tparam SizeInBits Maximum size in bits.
template <unsigned SizeInBits>
class static_bit_buffer : public bit_buffer
{
public:
  /// Default static bit buffer constructor of size zero.
  static_bit_buffer(unsigned nof_bits = 0) : bit_buffer(buffer, nof_bits) {}

  /// Resizes the bit buffer.
  void resize(unsigned new_size) { set_current_size(new_size); }

private:
  /// Static memory container.
  std::array<bit_buffer::word_t, calculate_nof_words(SizeInBits)> buffer;
};

/// Implements a bit buffer that uses dynamic memory.
class dynamic_bit_buffer : public bit_buffer
{
public:
  /// Creates a dynamic bit buffer that contains \c nof_bits.
  dynamic_bit_buffer(unsigned nof_bits) : buffer(calculate_nof_words(nof_bits))
  {
    set_buffer(buffer);
    set_current_size(nof_bits);
  }

  /// Resizes the bit buffer.
  void resize(unsigned new_size)
  {
    buffer.resize(new_size);
    set_buffer(buffer);
    set_current_size(new_size);
  }

private:
  /// Dynamic memory container.
  std::vector<bit_buffer::word_t> buffer;
};

} // namespace srsgnb

namespace fmt {

template <>
struct formatter<srsgnb::bit_buffer> {
  enum { hexadecimal, binary } mode = binary;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'x') {
        mode = hexadecimal;
      }
      ++it;
    }

    return it;
  }

  template <typename FormatContext>
  auto format(const srsgnb::bit_buffer& s, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return s.template to_hex_string(ctx.out());
    }
    return s.template to_bin_string(ctx.out());
  }
};

} // namespace fmt
