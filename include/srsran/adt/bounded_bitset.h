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

#pragma once

#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/support/math/bit_ops.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"
#include <cinttypes>
#include <string>

namespace srsran {

namespace detail {

struct default_bounded_bitset_tag {};

} // namespace detail

/// \brief Represents a dynamically-sized bitset with an upper bound capacity of N bits.
///
/// The bounded_bitset is represented internally via an array of uint64_t, with each integer storing a bitmap.
/// This class also offers many standard logic manipulation methods, like ::any(), operators &=, &, |=, |, etc. and
/// utility methods to convert the bitset into strings or integers.
///
/// Depending on the passed \c LowestInfoBitIsMSB template argument, this class can represent the bits of the bitset in
/// different orders. E.g.
///
/// bounded_bitset<6, false> a(5); // Bitset of 5 bits. LSB is Lowest Information Bit (bit 0).
/// a.set(1);
/// assert(a.to_uint64() == 0b00010);
/// bounded_bitset<6, true> b(5); // Bitset of 5 bits. MSB is Lowest Information Bit (bit 0).
/// b.set(1);
/// assert(a.to_uint64() == 0b0100000000000000);
///
/// The \c LowestInfoBitIsMSB template argument also affects the default string representation of the bitset. E.g.
/// fmt::print("{:b}", a); // prints "00010".
/// fmt::print("{:x}", a); // prints "2".
/// fmt::print("{:b}", b); // prints "01000".
/// fmt::print("{:x}", b); // prints "8".
///
/// However, it does not affect the information bit position string representation, e.g.
/// fmt::print("{:n}", a); // prints "0".
/// fmt::print("{:n}", b); // prints "1".
///
/// \tparam N Upper bound for bitset size in number of bits.
/// \tparam LowestInfoBitIsMSB Bit index order in memory. If set to (false / true), the bit index 0 (Lowest Information
/// Bit) corresponds to either the LSB or MSB of the bitset. Note that this argument has an effect on the underlying
/// bitset memory layout.
template <size_t N, bool LowestInfoBitIsMSB = false, typename Tag = detail::default_bounded_bitset_tag>
class bounded_bitset
{
  using word_t                          = uint64_t;
  static constexpr size_t bits_per_word = 8U * sizeof(word_t);

public:
  constexpr bounded_bitset() = default;

  constexpr explicit bounded_bitset(size_t cur_size_) : cur_size(cur_size_)
  {
    report_fatal_error_if_not(cur_size_ <= max_size(),
                              "The bounded_bitset current size cannot exceed its maximum size");
  }

  constexpr bounded_bitset(const bounded_bitset& other) noexcept : cur_size(other.cur_size)
  {
    std::copy(other.buffer.begin(), other.buffer.begin() + nof_words_(), buffer.begin());
  }

  /// \brief Constructs a bitset using iterators.
  ///
  /// The constructed bitset size is equal to <tt> end - begin </tt> size. The values in the list are mapped one to one
  /// starting from begin.
  ///
  /// \tparam Iterator Boolean iterator type.
  /// \param[in] begin Begin iterator.
  /// \param[in] end End iterator.
  template <typename Iterator,
            std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<Iterator>::value_type, bool>, int> = 0>
  constexpr bounded_bitset(Iterator begin, Iterator end)
  {
    resize(end - begin);
    auto it = begin;
    for (size_t count = 0; count != cur_size; ++count) {
      set_(count, *it);
      ++it;
    }
  }

  /// \brief Constructs a bitset from an initializer list.
  ///
  /// The constructed bitset size is equal to \c values size. The values in the list are mapped one to one.
  ///
  /// \param[in] values Boolean initializer list.
  constexpr bounded_bitset(const std::initializer_list<const bool>& values)
  {
    resize(values.size());
    auto it = values.begin();
    for (size_t count = 0; count != cur_size; ++count) {
      set_(count, *it);
      ++it;
    }
  }

  constexpr bounded_bitset& operator=(const bounded_bitset& other) noexcept
  {
    if (this != &other) {
      // In case of shrink, reset erased bits.
      // Note: We use < comparison because other can be larger than this.
      for (size_t i = other.nof_words_(), sz = nof_words_(); i < sz; ++i) {
        buffer[i] = static_cast<word_t>(0);
      }
      cur_size = other.cur_size;
      std::copy(other.buffer.begin(), other.buffer.begin() + nof_words_(), buffer.begin());
    }
    return *this;
  }

  template <typename BoundedBitSet>
  BoundedBitSet convert_to() const
  {
    static_assert(BoundedBitSet::max_size() == max_size() and BoundedBitSet::bit_order() == bit_order(),
                  "Conversion only supported for same N and LowestInfoBitIsMSB");
    // Just the tag changes.
    BoundedBitSet ret(size());
    ret.buffer = buffer;
    return ret;
  }

  static constexpr bool bit_order() noexcept { return LowestInfoBitIsMSB; }

  /// Capacity of the bounded_bitset in bits.
  static constexpr size_t max_size() noexcept { return N; }

  /// Current size of the bounded_bitset in bits.
  SRSRAN_FORCE_INLINE constexpr size_t size() const noexcept { return cur_size; }

  /// Returns true if the bounded_bitset size is 0.
  SRSRAN_FORCE_INLINE constexpr bool empty() const noexcept { return size() == 0; }

  /// \brief Resize of the bounded_bitset. If <tt> new_size > max_size() </tt>, an assertion is triggered. The newly
  /// created are set to zero.
  constexpr void resize(size_t new_size)
  {
    if (new_size == cur_size) {
      return;
    }
    report_fatal_error_if_not(
        new_size <= max_size(), "ERROR: new size='{}' exceeds bitset capacity='{}'", new_size, max_size());
    const size_t prev_size = size();
    cur_size               = new_size;
    if (new_size < prev_size) {
      // Shrinking case. Need to sanitize removed bits.
      sanitize_();
      const size_t prev_nof_words = divide_ceil(prev_size, bits_per_word);
      const size_t new_nof_words  = divide_ceil(new_size, bits_per_word);
      srsran_assume(prev_nof_words <= buffer.size());
      for (size_t i = new_nof_words; i < prev_nof_words; ++i) {
        buffer[i] = static_cast<word_t>(0);
      }
    }
  }

  /// \brief Set bit with provided index to either true or false. Assertion is triggered if pos >= max_size().
  /// \param[in] pos Position in bitset.
  /// \param[in] val Value to set the bit.
  void set(size_t pos, bool val)
  {
    assert_within_bounds_(pos, true);
    set_(pos, val);
  }

  /// \brief Set bit with provided index to true. Assertion is triggered if pos >= N.
  /// \param[in] pos Position in bitset.
  void set(size_t pos)
  {
    assert_within_bounds_(pos, true);
    set_(pos);
  }

  /// \brief Set bit with provided index to false. Assertion is triggered if pos >= N.
  /// \param[in] pos Position in bitset.
  void reset(size_t pos)
  {
    assert_within_bounds_(pos, true);
    reset_(pos);
  }

  /// Reset all bits in the bounded_bitset to false. The size of the bitset is maintained.
  void reset() noexcept
  {
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] = static_cast<word_t>(0);
    }
  }

  /// Set all bits in the bounded_bitset to true/false. The size of the bitset is maintained.
  void fill(bool val = true) noexcept
  {
    if (not val) {
      reset();
      return;
    }
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] = static_cast<word_t>(-1);
    }
    sanitize_();
  }

  /// \brief Appends a bit with value \c val to the set.
  ///
  /// Assertion is triggered if the resultant size exceeds the maximum size of the bitset.
  void push_back(bool val)
  {
    size_t bitpos = size();
    resize(bitpos + 1);
    set(bitpos, val);
  }

  /// \brief Appends \c nof_bits bits to the set.
  ///
  /// The least \c nof_bits significant bits of \c val are appended to the set, starting from the most significant bit
  /// and finishing with the least significant bit.
  ///
  /// Assertion is triggered if the resultant size exceeds the maximum size of the bitset.
  template <typename Integer>
  void push_back(Integer val, unsigned nof_bits)
  {
    static_assert(std::is_unsigned_v<Integer>, "push_back only works for unsigned integers");
    unsigned bitpos = size();
    resize(bitpos + nof_bits);
    for (unsigned bit_index = 0; bit_index != nof_bits; ++bit_index) {
      set(bitpos + bit_index, (val >> (nof_bits - 1 - bit_index)) & 1U);
    }
  }

  /// \brief Extracts \c nof_bits starting from \c startpos.
  ///
  /// \return An unsigned integer containing \c nof_bits of the set where starting with the most significant bit and
  /// finishing with the most significant bit.
  /// \remark An assertion is triggered if the bit range exceed the set size.
  template <typename Integer = unsigned>
  Integer extract(unsigned startpos, unsigned nof_bits) const
  {
    static_assert(std::is_unsigned_v<Integer>, "Extract only works for unsigned integers");
    srsran_assert(nof_bits <= sizeof(Integer) * 8,
                  "The number of bits (i.e., {}) exceeds the destination bit-width (i.e., {}).",
                  nof_bits,
                  sizeof(Integer) * 8);
    srsran_assert(startpos + nof_bits <= size(),
                  "The start position (i.e., {}) plus the number of bits (i.e., {}) exceed the current size (i.e., {})",
                  startpos,
                  nof_bits,
                  size());

    Integer val = 0;

    for (unsigned bit_index = 0; bit_index != nof_bits; ++bit_index) {
      if (test(startpos + bit_index)) {
        val |= 1U << (nof_bits - 1 - bit_index);
      }
    }

    return val;
  }

  /// \brief Kronecker product of the bitset with another bitset.
  ///
  /// Expands the bitset by a factor of \c other.size() replacing every \c true bit with the contents of \c other and
  /// every \c false bit with \c other.size() \c false bits.
  ///
  /// \tparam Factor    Maximum expansion factor.
  /// \param[in] other  Bitset used for expansion.
  /// \return The result of the bitset product.
  /// \remark The current implementation supports only a bitset containing one word. An assertion is triggered if \c
  /// other contains more than one word.
  template <unsigned long Factor>
  bounded_bitset<Factor * N> kronecker_product(const bounded_bitset<Factor>& other) const
  {
    static_assert(Factor <= bits_per_word,
                  "The current algorithm does not support a filter containing more than one word.");

    // Prepare an empty result.
    bounded_bitset<Factor * N> result(size() * other.size());

    // Places the contents of other centered at the positions indicated by the true bits.
    auto kronecker_expansion = [&other, &result](unsigned bit_index) {
      unsigned bitpos = bit_index * Factor;
      word_t   word   = other.buffer[0];

      unsigned bit_offset = bitpos % bits_per_word;
      unsigned word_index = bitpos / bits_per_word;

      result.buffer[word_index] |= (word << bit_offset);
      if (bit_offset && (bit_offset + other.size() > bits_per_word)) {
        result.buffer[word_index + 1] |= (word >> (bits_per_word - bit_offset));
      }
    };

    if (is_contiguous(true)) {
      int i_begin = find_lowest(true);
      int i_end   = find_highest(true) + 1;

      if ((i_begin < 0) || (i_end <= 0)) {
        // Empty bitset.
        return result;
      }

      // If the bitset in contiguous and the other bitset is all set, then use fill.
      if (other.all()) {
        result.fill(i_begin * other.size(), i_end * other.size());
      } else {
        // Otherwise, place the contents of other into contiguous bit positions.
        for (int i_bit = i_begin; i_bit != i_end; ++i_bit) {
          kronecker_expansion(i_bit);
        }
      }

      srsran_assert(count() * other.count() == result.count(),
                    "The resultant number of ones is not consistent with inputs. It expected {} but got {}.",
                    count() * other.count(),
                    result.count());
      return result;
    }

    // Place the contents of other into arbitrary bit positions.
    for_each(0, size(), kronecker_expansion);

    srsran_assert(count() * other.count() == result.count(),
                  "The resultant number of ones is not consistent with inputs. It expected {} but got {}.",
                  count() * other.count(),
                  result.count());

    return result;
  }

  /// \brief Check if bit with provided index is set to true.
  /// \param[in] pos Position in bitset.
  /// \return Returns true if bit at position pos is set.
  [[nodiscard]] constexpr bool test(size_t pos) const
  {
    assert_within_bounds_(pos, true);
    return test_(pos);
  }

  /// \brief Toggle the value at position pos. Assertion is triggered if pos >= N.
  /// \param[in] pos Position in bitset.
  void flip(size_t pos)
  {
    assert_within_bounds_(pos, true);
    if (test(pos)) {
      reset_(pos);
    } else {
      set_(pos);
    }
  }

  /// \brief Toggle values of bits in bitset.
  /// \return Returns this object.
  bounded_bitset& flip() noexcept
  {
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] = ~buffer[i];
    }
    sanitize_();
    return *this;
  }

  /// \brief Fills range of bits to either true or false.
  /// \param[in] startpos Starting bit index that will be set.
  /// \param[in] endpos End bit index (excluding) where the bits stop being set.
  /// \param[in] value Set bit range values to either true or false.
  /// \return Returns a reference to this object.
  bounded_bitset& fill(size_t startpos, size_t endpos, bool value = true)
  {
    find_first_word_(startpos, endpos, [this, value](size_t word_idx, const word_t& mask) {
      srsran_assume(word_idx < buffer.size());
      if (value) {
        buffer[word_idx] |= mask;
      } else {
        buffer[word_idx] &= ~mask;
      }
      return false;
    });
    return *this;
  }

  /// \brief Returns bounded_bitset<> that represents a slice or subview of the original bounded_bitset. Unless
  /// it is specified, the returned slice has the same template parameters "N" and "LowestInfoBitIsMSB" of "this".
  ///
  /// \param[in] startpos The bit index where the subview starts.
  /// \param[in] endpos The bit index where the subview stops.
  template <size_t N2 = N, typename NewTag = Tag>
  bounded_bitset<N2, LowestInfoBitIsMSB, NewTag> slice(size_t startpos, size_t endpos) const
  {
    bounded_bitset<N2, LowestInfoBitIsMSB> sliced(endpos - startpos);
    const unsigned                         start_word = startpos / bits_per_word;
    unsigned                               start_mod  = startpos % bits_per_word;
    const auto                             nwords     = nof_words_();

    if (start_mod != 0) {
      if constexpr (LowestInfoBitIsMSB) {
        const auto left_mask  = mask_msb_ones<word_t>(bits_per_word - start_mod);
        const auto right_mask = mask_msb_ones<word_t>(start_mod);
        for (unsigned i = 0, sl_nw = sliced.nof_words_(); i != sl_nw; ++i) {
          sliced.buffer[i] = (buffer[i + start_word] << start_mod) & left_mask;
          if (i + start_word + 1 < nwords) {
            sliced.buffer[i] |= (buffer[i + start_word + 1] & right_mask) >> (bits_per_word - start_mod);
          }
        }
      } else {
        const auto left_mask  = mask_lsb_ones<word_t>(bits_per_word - start_mod);
        const auto right_mask = mask_lsb_ones<word_t>(start_mod);
        for (unsigned i = 0, sl_nw = sliced.nof_words_(); i != sl_nw; ++i) {
          sliced.buffer[i] = (buffer[i + start_word] >> start_mod) & left_mask;
          if (i + start_word + 1 < nwords) {
            sliced.buffer[i] |= (buffer[i + start_word + 1] & right_mask) << (bits_per_word - start_mod);
          }
        }
      }
    } else {
      for (unsigned i = 0, sl_nw = sliced.nof_words_(); i != sl_nw; ++i) {
        sliced.buffer[i] = buffer[i + start_word];
      }
    }
    sliced.sanitize_();
    return sliced;
  }

  /// \brief Finds the lowest bit with value set to the value passed as argument.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  int find_lowest(bool value = true) const noexcept { return find_lowest(0, size(), value); }

  /// \brief Finds, within a range of bit indexes, the lowest bit with value set to the value passed as argument.
  /// \param[in] startpos Starting bit index for the search.
  /// \param[in] endpos End bit index for the search.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  int find_lowest(size_t startpos, size_t endpos, bool value = true) const noexcept
  {
    int pos = -1;
    find_first_word_(startpos, endpos, [this, value, &pos](size_t word_idx, const word_t& mask) {
      srsran_assume(word_idx < buffer.size());
      word_t w = value ? buffer[word_idx] : ~buffer[word_idx];
      w &= mask;
      if (w != 0) {
        // Found bit. Store its position.
        pos = word_idx * bits_per_word;
        if constexpr (LowestInfoBitIsMSB) {
          pos += convert_bitpos_(find_first_msb_one(w));
        } else {
          pos += find_first_lsb_one(w);
        }
        return true;
      }
      return false;
    });
    return pos;
  }

  /// \brief Executes a function for all \c true (or all \c false) bits in the given bitset interval.
  ///
  /// \param[in] startpos Smallest bit index considered for the function execution (included).
  /// \param[in] endpos   Largest bit index considered for the function execution (excluded).
  /// \param[in] function Function to execute - the signature should be compatible with <tt>void ()(unsigned)</tt>.
  /// \param[in] value    Bit value that triggers the function execution.
  template <class T>
  void for_each(size_t startpos, size_t endpos, T&& function, bool value = true) const noexcept
  {
    static_assert(std::is_convertible_v<T, std::function<void(size_t)>>,
                  "The function must have void(size_t) signature.");
    static_assert(!LowestInfoBitIsMSB, "The for_each method is not yet available for reversed bitsets.");

    assert_range_bounds_(startpos, endpos);

    if (startpos == endpos) {
      return;
    }

    if ((value && all(startpos, endpos)) || (!value && none(startpos, endpos))) {
      for (size_t bitpos = startpos; bitpos != endpos; ++bitpos) {
        function(bitpos);
      }
      return;
    }

    size_t startword = startpos / bits_per_word;
    size_t lastword  = (endpos + bits_per_word - 1) / bits_per_word;
    for (size_t i = startword; i != lastword; ++i) {
      word_t w = buffer[i];
      if (not value) {
        w = ~w;
      }

      if (w == 0) {
        continue;
      }

      if (i == startword) {
        w &= mask_lsb_zeros<word_t>(startpos % bits_per_word);
      }

      if ((i == lastword - 1) && (endpos % bits_per_word != 0)) {
        w &= mask_lsb_ones<word_t>(endpos % bits_per_word);
      }

      // Process presets of 4 bits.
      unsigned bitpos = i * bits_per_word;
      for (; w != 0; w = w >> 4, bitpos += 4) {
        switch (w & 0xf) {
          case 0B0000:
            // All bits are false, skip.
            break;
          case 0B0001:
            function(bitpos + 0);
            break;
          case 0B0010:
            function(bitpos + 1);
            break;
          case 0B0011:
            function(bitpos + 0);
            function(bitpos + 1);
            break;
          case 0B0100:
            function(bitpos + 2);
            break;
          case 0B0101:
            function(bitpos + 0);
            function(bitpos + 2);
            break;
          case 0B0110:
            function(bitpos + 1);
            function(bitpos + 2);
            break;
          case 0B0111:
            function(bitpos + 0);
            function(bitpos + 1);
            function(bitpos + 2);
            break;
          case 0B1000:
            function(bitpos + 3);
            break;
          case 0B1001:
            function(bitpos + 0);
            function(bitpos + 3);
            break;
          case 0B1010:
            function(bitpos + 1);
            function(bitpos + 3);
            break;
          case 0B1011:
            function(bitpos + 0);
            function(bitpos + 1);
            function(bitpos + 3);
            break;
          case 0B1100:
            function(bitpos + 2);
            function(bitpos + 3);
            break;
          case 0B1101:
            function(bitpos + 0);
            function(bitpos + 2);
            function(bitpos + 3);
            break;
          case 0B1110:
            function(bitpos + 1);
            function(bitpos + 2);
            function(bitpos + 3);
            break;
          case 0B1111:
          default:
            function(bitpos + 0);
            function(bitpos + 1);
            function(bitpos + 2);
            function(bitpos + 3);
            break;
        }
      }
    }
  }

  /// \brief Checks if all bits in the bitset are set to 1.
  /// \return Returns true if all bits are 1.
  bool all() const noexcept
  {
    const size_t nw = nof_words_();
    if (nw == 0) {
      return true;
    }
    word_t allset = ~static_cast<word_t>(0);
    for (size_t i = 0; i < nw - 1; i++) {
      if (buffer[i] != allset) {
        return false;
      }
    }
    if constexpr (LowestInfoBitIsMSB) {
      return buffer[nw - 1] == (allset << (nw * bits_per_word - size()));
    } else {
      return buffer[nw - 1] == (allset >> (nw * bits_per_word - size()));
    }
  }

  /// \brief Checks if all bits within a bit index range are set to 1.
  /// \return Returns true if all the bits within the range are 1.
  bool all(size_t start, size_t stop) const
  {
    bool not_all_found = find_first_word_(start, stop, [this](size_t word_idx, const word_t& mask) {
      srsran_assume(word_idx <= buffer.size());
      return (buffer[word_idx] | ~mask) != ~static_cast<word_t>(0);
    });
    return !not_all_found;
  }

  /// \brief Finds the highest bit with value set to the value passed as argument.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  int find_highest(bool value = true) const noexcept { return find_highest(0, size(), value); }

  /// \brief Finds, within a range of bit indexes, the highest bit with value set to the value passed as argument.
  /// \param[in] startpos Starting bit index for the search.
  /// \param[in] endpos End bit index for the search.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  int find_highest(size_t startpos, size_t endpos, bool value = true) const noexcept
  {
    assert_range_bounds_(startpos, endpos);
    if (startpos == endpos) {
      return -1;
    }
    size_t startword = startpos / bits_per_word;
    size_t lastword  = (endpos - 1) / bits_per_word;

    for (size_t i = lastword; i != startword - 1; --i) {
      word_t w = buffer[i];
      if (not value) {
        w = ~w;
      }

      if (i == startword) {
        size_t removed_bits = startpos % bits_per_word;
        if constexpr (LowestInfoBitIsMSB) {
          w &= mask_msb_zeros<word_t>(removed_bits);
        } else {
          w &= mask_lsb_zeros<word_t>(removed_bits);
        }
      }
      if (i == lastword) {
        size_t kept_bits = ((endpos - 1) % bits_per_word) + 1;
        if constexpr (LowestInfoBitIsMSB) {
          w &= mask_msb_ones<word_t>(kept_bits);
        } else {
          w &= mask_lsb_ones<word_t>(kept_bits);
        }
      }
      if (w != 0) {
        if constexpr (LowestInfoBitIsMSB) {
          return static_cast<int>(i * bits_per_word + convert_bitpos_(find_first_lsb_one(w)));
        } else {
          return static_cast<int>(i * bits_per_word + find_first_msb_one(w));
        }
      }
    }
    return -1;
  }

  /// \brief Checks if at least one bit in the bitset is set to 1.
  /// \return Returns true if at least one bit is 1.
  bool any() const noexcept
  {
    for (size_t i = 0, sz = nof_words_(); i != sz; ++i) {
      if (buffer[i] != static_cast<word_t>(0)) {
        return true;
      }
    }
    return false;
  }

  /// \brief Checks if at least one bit in the bitset is set to 1 within a bit index range.
  /// \return Returns true if at least one bit equal to 1 was found within the range.
  bool any(size_t start, size_t stop) const
  {
    bool any_found = find_first_word_(start, stop, [this](size_t word_idx, const word_t& mask) {
      srsran_assume(word_idx < buffer.size());
      return (buffer[word_idx] & mask) != static_cast<word_t>(0);
    });
    return any_found;
  }

  /// \brief Checks if at no bit in the bitset is set to 1.
  /// \return Returns true if no bit equal to 1 was found.
  bool none() const noexcept { return !any(); }

  /// \brief Checks whether no bit is set within the given index range.
  /// \return True if no bit is equal to 1, false otherwise.
  bool none(size_t start, size_t stop) const noexcept { return !any(start, stop); }

  /// \brief Determines whether all bits with value set to \c value are contiguous.
  ///
  /// Bits with the same value are contiguous if:
  /// 1. one bit with \c value is found,
  /// 2. no bit with \c value is found, and
  /// 3. all bits with \c value are consecutive.
  ///
  /// \param[in] startpos Starting bit index for the search.
  /// \param[in] endpos End bit index for the search.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  bool is_contiguous(bool value = true) const noexcept
  {
    // Find the lowest value.
    int startpos = find_lowest(0, size(), value);

    // Condition 1. No value is found.
    if (startpos == -1) {
      return true;
    }

    // Find the highest value.
    int endpos = find_highest(startpos + 1, size(), value);

    // Condition 2. There is only one bit with the value (in startpos).
    if (endpos == -1) {
      return true;
    }

    // Count the number of elements set to value.
    size_t value_count = count();
    if (not value) {
      value_count = size() - value_count;
    }

    // Condition 3. The number of elements must match with the start to end number of elements.
    return (value_count == static_cast<size_t>((endpos + 1) - startpos));
  }

  /// \brief Counts the number of bits set to 1.
  /// \return Returns the number of bits set to 1.
  size_t count() const noexcept
  {
    int result = 0;
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      result += count_ones(buffer[i]);
    }
    return result;
  }

  /// \brief Compares two bitsets.
  /// \return Returns true if both bitsets are equal in size and values of bits.
  bool operator==(const bounded_bitset& other) const noexcept
  {
    if (size() != other.size()) {
      return false;
    }
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      if (buffer[i] != other.buffer[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const bounded_bitset& other) const noexcept { return not(*this == other); }

  /// \brief Applies bitwise OR operation lhs |= rhs.
  /// \param[in] other Bitset which corresponds to the rhs of the operation.
  /// \return This object updated after the bitwise OR operation.
  bounded_bitset& operator|=(const bounded_bitset& other)
  {
    srsran_assert(other.size() == size(),
                  "ERROR: operator|= called for bitsets of different sizes ('{}'!='{}')",
                  size(),
                  other.size());
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] |= other.buffer[i];
    }
    return *this;
  }

  /// \brief Applies bitwise AND operation lhs &= rhs.
  /// \param[in] other Bitset which corresponds to the rhs of the operation.
  /// \return This object updated after the bitwise AND operation.
  bounded_bitset& operator&=(const bounded_bitset& other)
  {
    srsran_assert(other.size() == size(),
                  "ERROR: operator&= called for bitsets of different sizes ('{}'!='{}')",
                  size(),
                  other.size());
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] &= other.buffer[i];
    }
    return *this;
  }

  /// \brief Flips values of bits in the bitset.
  /// \return Returns reference to this object, updated after the flip operation.
  bounded_bitset operator~() const noexcept
  {
    bounded_bitset ret(*this);
    ret.flip();
    return ret;
  }

  /// \brief Conversion of bounded_bitset to unsigned integer of 64 bits. If bitset size is larger than 64 bits, an
  /// assertion is triggered.
  /// \return Unsigned integer representation of the bounded_bitset.
  uint64_t to_uint64() const
  {
    srsran_assert(nof_words_() == 1, "ERROR: cannot convert bitset of size='{}' to uint64_t", size());
    if constexpr (LowestInfoBitIsMSB) {
      return buffer[0] >> (bits_per_word - (size() % bits_per_word));
    }
    return buffer[0];
  }

  /// \brief Conversion of unsigned integer of 64 bits to bounded_bitset. If passed bitmap doesn't fit in the bitset,
  /// an assertion is triggered.
  /// \param[in] v Integer bitmap that is going to be stored in the bitset.
  void from_uint64(uint64_t v)
  {
    srsran_assert(nof_words_() == 1, "ERROR: cannot convert bitset of size='{}' to uint64_t", size());
    srsran_assert((size() == 64U) || (v < (static_cast<uint64_t>(1U) << size())),
                  "ERROR: Provided mask='{}' does not fit in bitset of size='{}'",
                  v,
                  size());
    buffer[0] = v;
  }

  /// \brief Converts the bitset to an array of packed bits. Each element of the resulting array will contain a bitmap.
  /// The LowInfoBitIsMSB template parameter defines the order of bits in the resulting array.
  /// \tparam UnsignedInteger Value type of the array where packed bits will be stored. It must be an unsigned integer.
  /// \param[in] packed_bits Array where packed bits will be stored. The array size must be equal or larger than the
  /// bitset size (in bits) divided by \c sizeof(UnsignedInteger) * 8U (the number of bits per integer).
  /// \return Returns the number of positions of \c packed_bits that were written during the function call.
  template <typename UnsignedInteger>
  size_t to_packed_bits(span<UnsignedInteger> packed_bits) const
  {
    static_assert(sizeof(UnsignedInteger) <= sizeof(word_t), "ERROR: provided array type is too large");
    static_assert(std::is_unsigned_v<UnsignedInteger>, "Only unsigned integers are supported");
    static constexpr size_t steps_per_word      = sizeof(word_t) / sizeof(UnsignedInteger);
    static constexpr size_t bits_per_integer    = sizeof(UnsignedInteger) * 8U;
    static constexpr auto   integer_mask        = mask_lsb_ones<word_t>(bits_per_integer);
    const word_t            sz                  = size();
    const unsigned          last_word_steps     = divide_ceil(sz % bits_per_word, bits_per_integer);
    const unsigned          nof_words           = nof_words_();
    const unsigned          nof_integers_packed = (nof_words - 1) * steps_per_word + last_word_steps;
    srsran_assert(
        packed_bits.size() >= nof_integers_packed, "ERROR: provided array size='{}' is too small", packed_bits.size());

    size_t count = 0;
    if (not LowestInfoBitIsMSB) {
      for (unsigned i = 0; i != nof_words; ++i) {
        unsigned nof_steps = i == nof_words - 1 ? last_word_steps : steps_per_word;
        for (unsigned j = 0; j != nof_steps; ++j) {
          packed_bits[count++] = (buffer[i] >> (j * steps_per_word)) & integer_mask;
        }
      }
    } else {
      for (unsigned i = 0; i != nof_words; ++i) {
        word_t   w         = buffer[i];
        unsigned nof_steps = steps_per_word;
        if (i == nof_words - 1) {
          nof_steps = last_word_steps;
        }
        for (unsigned j = 0; j != nof_steps; ++j) {
          packed_bits[count++] = (w >> (bits_per_word - (j + 1) * bits_per_integer)) & integer_mask;
        }
      }
    }

    return nof_integers_packed;
  }

  /// \brief Converts the bitset to an array of unpacked bits, i.e. an array where each element represents a single
  /// boolean. The order of bits in the resulting array matches the bit information order in the bitset and the template
  /// parameter \c LowInfoBitIsMSB has no effect. That means that \c unpacked_bits[i] will be equal to \c
  /// bitset.test(i).
  ///
  /// \tparam UnsignedInteger Value type of the array where packed bits will be stored. It must be an
  /// unsigned integer. \param[in] unpacked_bits Array where the unpacked bits will be stored. The array size must be
  /// equal or larger than the bitset size (in bits). \return Returns the number of bits packed.
  template <typename UnsignedInteger>
  void to_unpacked_bits(span<UnsignedInteger> unpacked_bits) const
  {
    static_assert(std::is_unsigned_v<UnsignedInteger>, "Only unsigned integers are supported");
    srsran_assert(size() == unpacked_bits.size(),
                  "ERROR: provided array size='{}' does not match bitset size='{}'",
                  unpacked_bits.size(),
                  size());

    for (unsigned i = 0, ie = size(); i != ie; ++i) {
      unpacked_bits[i] = test(i);
    }
  }

  /// \brief Generates a list of bit positions corresponding to the information bits set to one or zero.
  ///
  /// The bit positions correspond to the location of each bit within the information bit word stored in the bitset,
  /// regardless of the bit index order in memory given by \ref LowestInfoBitIsMSB.
  ///
  /// \param[in] value Selects the bits whose positions are returned. Set to \c true to find ones, \c false for zeros.
  /// \return A list containing the bit positions.
  static_vector<size_t, N> get_bit_positions(bool value = true) const
  {
    static_vector<size_t, N> positions;

    for (size_t i_bit = 0, sz = size(); i_bit != sz;) {
      // Find the next bit position of the bit set to value.
      int next_position = find_lowest(i_bit, sz, value);
      if (next_position < 0) {
        break;
      }

      // If a bit was found, add to the list.
      positions.emplace_back(static_cast<size_t>(next_position));

      // Exclude the evaluated bit range from the next search.
      i_bit = next_position + 1;
    }

    return positions;
  }

private:
  template <size_t N2, bool reversed2, typename Tag2>
  friend class bounded_bitset;
  friend struct fmt::formatter<bounded_bitset<N, LowestInfoBitIsMSB, Tag>>;

  // Capacity of the underlying array in number of words.
  static constexpr size_t max_nof_words_() noexcept { return (N + bits_per_word - 1) / bits_per_word; }

  std::array<word_t, max_nof_words_()> buffer{};
  size_t                               cur_size = 0;

  constexpr void sanitize_()
  {
    const size_t n = size() % bits_per_word;
    if (n != 0) {
      const size_t nwords = nof_words_();
      if constexpr (LowestInfoBitIsMSB) {
        buffer[nwords - 1] &= ~((~static_cast<word_t>(0)) >> n);
      } else {
        buffer[nwords - 1] &= ~((~static_cast<word_t>(0)) << n);
      }
    }
  }

  SRSRAN_FORCE_INLINE size_t convert_bitpos_(size_t bitpos) const noexcept
  {
    if constexpr (LowestInfoBitIsMSB) {
      return bits_per_word - 1 - (bitpos % bits_per_word);
    } else {
      return bitpos;
    }
  }

  SRSRAN_FORCE_INLINE constexpr bool test_(size_t bitpos) const noexcept
  {
    const size_t word_idx = bitpos / bits_per_word;
    srsran_assume(word_idx < buffer.size());
    const word_t bitmask = maskbit(bitpos);
    return ((buffer[word_idx] & bitmask) != static_cast<word_t>(0));
  }

  constexpr void set_(size_t bitpos, bool val) noexcept
  {
    if (val) {
      set_(bitpos);
    } else {
      reset_(bitpos);
    }
  }
  constexpr void set_(size_t bitpos) noexcept
  {
    const size_t word_idx = bitpos / bits_per_word;
    srsran_assume(word_idx < buffer.size());
    buffer[word_idx] |= maskbit(bitpos);
  }

  constexpr void reset_(size_t bitpos) noexcept
  {
    const size_t word_idx = bitpos / bits_per_word;
    srsran_assume(word_idx < buffer.size());
    buffer[word_idx] &= ~maskbit(bitpos);
  }

  /// Number of words currently in use.
  SRSRAN_FORCE_INLINE size_t nof_words_() const noexcept { return divide_ceil(size(), bits_per_word); }

  constexpr size_t word_idx_(size_t bitidx) const { return bitidx / bits_per_word; }

  constexpr void assert_within_bounds_(size_t pos, bool strict) const noexcept
  {
    srsran_assert(pos < size() or (not strict and pos == size()),
                  "ERROR: index='{}' is out-of-bounds for bitset of size='{}'",
                  pos,
                  size());
  }

  constexpr void assert_range_bounds_(size_t startpos, size_t endpos) const noexcept
  {
    srsran_assert(startpos <= endpos and endpos <= size(),
                  "ERROR: range ['{}', '{}') out-of-bounds for bitsize of size='{}'",
                  startpos,
                  endpos,
                  size());
  }

  SRSRAN_FORCE_INLINE static constexpr word_t maskbit(size_t pos) noexcept
  {
    if constexpr (LowestInfoBitIsMSB) {
      return static_cast<word_t>(1U) << (bits_per_word - 1 - (pos % bits_per_word));
    } else {
      return static_cast<word_t>(1U) << (pos % bits_per_word);
    }
  }

  /// \brief Finds first word, aka integer bitmap, within the provided bit index bounds for which the provided
  /// callback returns true. This helper function iterates through the bounded_bitset on a word-by-word basis. Note
  /// that operations over words are generally faster than operations over individual bits. For each iterated word, a
  /// mask of the selected bits is computed (which depends on the provided "start" and "stop" bit indexes), and the
  /// provided callback is invoked. The callback receives as arguments the word index and the selected-bit mask. If
  /// the callback returns true, the iteration stops and the function returns true.
  ///
  /// \param start first bit index of the bounded_bitset.
  /// \param stop end bit index of the bounded_bitset.
  /// \param c Callback with signature "bool(size_t word_index, word_t active_mask)" called for each word of the
  /// bitset.
  ///          When this callback returns true, the iteration is notify_stop.
  /// \return true if the provided callback returns true for a given word. False otherwise.
  template <typename C>
  bool find_first_word_(size_t start, size_t stop, const C& c) const
  {
    assert_range_bounds_(start, stop);
    const size_t startmod   = start % bits_per_word;
    const size_t stopmod    = stop % bits_per_word;
    const size_t start_word = word_idx_(start);
    const size_t end_word   = word_idx_(stop) + (stopmod != 0 ? 1U : 0U);
    if (start == stop) {
      return false;
    }

    size_t i = start_word;
    if (startmod != 0) {
      // the first word is not complete. We need to apply a mask.
      word_t mask = ~static_cast<word_t>(0);
      if constexpr (LowestInfoBitIsMSB) {
        mask &= mask_msb_zeros<word_t>(startmod);
      } else {
        mask &= mask_lsb_zeros<word_t>(startmod);
      }
      if (i == end_word - 1) {
        // first word is also the last. We apply the endmask as well.
        if constexpr (LowestInfoBitIsMSB) {
          mask &= mask_lsb_zeros<word_t>(end_word * bits_per_word - stop);
        } else {
          mask &= mask_msb_zeros<word_t>(end_word * bits_per_word - stop);
        }
        // return right away.
        return c(i, mask);
      }
      if (c(i, mask)) {
        return true;
      }
      // first word now processed.
      ++i;
    }

    // Iterate through full words.
    for (; i != end_word - 1; ++i) {
      if (c(i, ~static_cast<word_t>(0))) {
        return true;
      }
    }

    // Last word reached. Apply mask if required.
    word_t mask = ~static_cast<word_t>(0);
    if (stopmod != 0) {
      if constexpr (LowestInfoBitIsMSB) {
        mask &= mask_lsb_zeros<word_t>(end_word * bits_per_word - stop);
      } else {
        mask &= mask_msb_zeros<word_t>(end_word * bits_per_word - stop);
      }
    }
    return c(i, mask);
  }

  /// \brief Formatting helper to convert bitset to string of bits.
  /// \tparam OutputIt Output fmt memory buffer type.
  /// \param[out] mem_buffer Fmt memory buffer.
  /// \return The memory buffer passed as argument.
  template <typename OutputIt>
  OutputIt to_string_of_bits(OutputIt&& mem_buffer, bool reverse) const
  {
    if (size() == 0) {
      return mem_buffer;
    }

    reverse = reverse ^ LowestInfoBitIsMSB;

    std::string s;
    s.assign(size(), '0');
    if (!reverse) {
      for (size_t i = size(); i != 0; --i) {
        fmt::format_to(mem_buffer, "{}", test(i - 1) ? '1' : '0');
      }
    } else {
      for (size_t i = 0; i != size(); ++i) {
        fmt::format_to(mem_buffer, "{}", test(i) ? '1' : '0');
      }
    }
    return mem_buffer;
  }

  /// \brief Formatting helper to convert bitset to hexadecimal digits.
  /// \tparam OutputIt Output fmt memory buffer type.
  /// \param[out] mem_buffer Fmt memory buffer.
  /// \param[in] reverse In which bit order to represent this bitset.
  /// \return The memory buffer passed as argument.
  template <typename OutputIt>
  OutputIt to_string_of_hex(OutputIt&& mem_buffer, bool reverse) const
  {
    const size_t sz = size();
    if (sz == 0) {
      return mem_buffer;
    }
    const size_t rem_bits   = sz % bits_per_word;
    const size_t rem_digits = divide_ceil(rem_bits, 4U);
    const size_t nwords     = nof_words_();

    if (not reverse) {
      if constexpr (LowestInfoBitIsMSB) {
        unsigned i = 0;
        for (; i != nwords - 1; ++i) {
          uint64_t w = buffer[i];
          fmt::format_to(mem_buffer, "{:0>16x}", w);
        }
        word_t w = buffer[i] >> (bits_per_word - rem_bits);
        fmt::format_to(mem_buffer, "{:0>{}x}", w, rem_digits);
      } else {
        int    i = nwords - 1;
        word_t w = buffer[i];
        fmt::format_to(mem_buffer, "{:0>{}x}", w, rem_digits);
        // remaining words will occupy 16 hex digits each (4 bits per hex digit).
        for (--i; i >= 0; --i) {
          fmt::format_to(mem_buffer, "{:0>16x}", buffer[i]);
        }
      }
    } else {
      if constexpr (LowestInfoBitIsMSB) {
        // first, potentially incomplete, word
        int    i = nwords - 1;
        word_t w = bit_reverse(buffer[i]);
        fmt::format_to(mem_buffer, "{:0>{}x}", w, rem_digits);
        for (--i; i >= 0; --i) {
          fmt::format_to(mem_buffer, "{:0>16x}", bit_reverse(buffer[i]));
        }
      } else {
        unsigned i = 0;
        for (; i != nwords - 1; ++i) {
          uint64_t w = bit_reverse(buffer[i]);
          fmt::format_to(mem_buffer, "{:0>16x}", w);
        }
        word_t w = bit_reverse(buffer[i]) >> (bits_per_word - rem_bits);
        fmt::format_to(mem_buffer, "{:0>{}x}", w, rem_digits);
      }
    }
    return mem_buffer;
  }
};

/// \brief Bitwise AND operation result = lhs & rhs.
/// \return new bounded_bitset that results from the Bitwise AND operation.
template <size_t N, bool LowestInfoBitIsMSB>
inline bounded_bitset<N, LowestInfoBitIsMSB> operator&(const bounded_bitset<N, LowestInfoBitIsMSB>& lhs,
                                                       const bounded_bitset<N, LowestInfoBitIsMSB>& rhs) noexcept
{
  bounded_bitset<N, LowestInfoBitIsMSB> res(lhs);
  res &= rhs;
  return res;
}

/// \brief Bitwise AND operation result = lhs | rhs.
/// \return new bounded_bitset that results from the Bitwise OR operation.
template <size_t N, bool LowestInfoBitIsMSB>
inline bounded_bitset<N, LowestInfoBitIsMSB> operator|(const bounded_bitset<N, LowestInfoBitIsMSB>& lhs,
                                                       const bounded_bitset<N, LowestInfoBitIsMSB>& rhs) noexcept
{
  bounded_bitset<N, LowestInfoBitIsMSB> res(lhs);
  res |= rhs;
  return res;
}

/// \brief Flip bits from left to right.
/// \return new bounded_bitset that results from the fliplr operation.
template <size_t N, bool LowestInfoBitIsMSB>
inline bounded_bitset<N, LowestInfoBitIsMSB> fliplr(const bounded_bitset<N, LowestInfoBitIsMSB>& other) noexcept
{
  bounded_bitset<N, LowestInfoBitIsMSB> ret(other.size());
  for (uint32_t i = 0; i < ret.size(); ++i) {
    if (other.test(i)) {
      ret.set(ret.size() - 1 - i);
    }
  }
  return ret;
}

/// \brief Divides a bitset of size "S" into "M" smaller bitsets, where each bitset has length "L=S/M". A bitwise-or
/// operation is performed across bitsets. At the end, a slice with an offset "O" and length "K" is taken from the
/// bitset of length "L" that resulted from the bitwise-or operation.
/// This operation is equivalent to reshaping an array of bits of size "S" into a matrix of dimensions "(M, L)" and
/// applying an "or" operation across all bits of each column. The resulting array of "L" bits, is then sliced with
/// an offset "O" and length "K".
/// The operation asserts if "S % L != 0".
/// E.g. Consider the bitset 1000 0100 0000 1001 (S=16), L=4, O=1, K=2. This function performs the following steps:
/// 1. Break the bitset into M=S/L=4 parts: {1000, 0100, 0000, 1001}.
/// 2. Bitwise-or all the M parts: 1101.
/// 3. Slice the bitset obtained in 2. with offset O=1 and slice length K=2: 10.
///
/// \tparam N2 maximum bitset size for returned bitset.
/// \param[in] other original bitset of length "S".
/// \param[in] fold_length length of each folded bitset "L".
/// \param[in] slice_offset offset from where to slice each fold "O".
/// \param[in] slice_length length of the slice taken from each fold "K".
/// \return bitset of size slice_length with the or-accumulated folds.
template <size_t N2, size_t N, bool LowestInfoBitIsMSB>
inline bounded_bitset<N2, LowestInfoBitIsMSB> fold_and_accumulate(const bounded_bitset<N, LowestInfoBitIsMSB>& other,
                                                                  size_t fold_length,
                                                                  size_t slice_offset,
                                                                  size_t slice_length) noexcept
{
  srsran_assert(
      other.size() % fold_length == 0, "Invalid fold length={} for bitset of size={}", fold_length, other.size());
  bounded_bitset<N2, LowestInfoBitIsMSB> ret(slice_length);
  for (size_t i = 0; i != other.size(); i += fold_length) {
    ret |= other.template slice<N2>(i + slice_offset, i + slice_offset + slice_length);
  }
  return ret;
}

/// \brief Performs the fold and accumulate operation, but without slicing at the end.
///
/// \tparam N2 maximum bitset size for returned bitset.
/// \tparam LowestInfoBitIsMSB internal bit order representation of returned bitset.
/// \param[in] other original bitset from where folds are generated.
/// \param[in] fold_length length of each fold bitset.
/// \return bitset of size fold_length with the accumulated folds.
template <size_t N2, size_t N, bool LowestInfoBitIsMSB>
inline bounded_bitset<N2, LowestInfoBitIsMSB> fold_and_accumulate(const bounded_bitset<N, LowestInfoBitIsMSB>& other,
                                                                  size_t fold_length) noexcept
{
  return fold_and_accumulate<N2, N, LowestInfoBitIsMSB>(other, fold_length, 0, fold_length);
}

/// \brief Executes a function for all \c true (or all \c false) intervals in the given bitset interval.
///
/// The method calls \c function for each interval, passing the first and last bit positions of the interval.
///
/// \param[in] startpos Smallest bit index considered for the function execution (included).
/// \param[in] endpos   Largest bit index considered for the function execution (excluded).
/// \param[in] function Function to execute - the signature should be compatible with <tt>void ()(size_t,size_t)</tt>.
/// \param[in] value    Bit value that triggers the function execution.
template <size_t N, bool LowestInfoBitIsMSB, typename Tag, class Func>
void for_each_interval(const bounded_bitset<N, LowestInfoBitIsMSB, Tag>& bitset,
                       size_t                                            startpos,
                       size_t                                            endpos,
                       Func&&                                            function,
                       bool                                              value = true)
{
  static_assert(std::is_convertible_v<Func, std::function<void(size_t, size_t)>>,
                "The function must have void(size_t) signature.");

  // Iterate for all intervals.
  for (int start_interval = bitset.find_lowest(startpos, endpos, value); start_interval != (-1);
       start_interval     = bitset.find_lowest(start_interval, endpos, value)) {
    // Find the end of the current interval.
    int end_interval = bitset.find_lowest(start_interval, endpos, !value);

    // If no more ending, force the end position.
    if (end_interval == -1) {
      end_interval = endpos;
    }

    // Call function.
    function(start_interval, end_interval);

    // Advance interval.
    start_interval = end_interval;
  }
}

// Executes a function for all \c true (or all \c false) intervals in the given bitset.
template <size_t N, bool LowestInfoBitIsMSB, typename Tag, class Func>
void for_each_interval(const bounded_bitset<N, LowestInfoBitIsMSB, Tag>& bitset, Func&& function, bool value = true)
{
  for_each_interval(bitset, 0, bitset.size(), function, value);
}

/// Converts a list of bit positions to a bounded_bitset.
template <size_t N,
          bool   LowestInfoBitIsMSB = false,
          typename Tag              = detail::default_bounded_bitset_tag,
          typename RangeType,
          typename PosInteger = typename RangeType::value_type>
bounded_bitset<N, LowestInfoBitIsMSB, Tag> bit_positions_to_bitset(const RangeType& bit_positions)
{
  bounded_bitset<N, LowestInfoBitIsMSB, Tag> result(N);
  int                                        max_pos = -1;
  for (PosInteger pos : bit_positions) {
    result.set(static_cast<size_t>(pos));
    max_pos = std::max(max_pos, static_cast<int>(pos));
  }
  result.resize(max_pos + 1);
  return result;
}

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for bounded_bitset<N, LowestInfoBitIsMSB, Tag>
template <size_t N, bool LowestInfoBitIsMSB, typename Tag>
struct formatter<srsran::bounded_bitset<N, LowestInfoBitIsMSB, Tag>> {
  enum { hexadecimal, binary, bit_positions, intervals } mode = binary;
  enum { forward, reverse } order                             = forward;
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'x') {
        mode = hexadecimal;
      }
      if (*it == 'r') {
        order = reverse;
      }
      if (*it == 'n') {
        mode = bit_positions;
      }
      if (*it == 'i') {
        mode = intervals;
      }
      ++it;
    }

    return it;
  }

  template <typename FormatContext>
  auto format(const srsran::bounded_bitset<N, LowestInfoBitIsMSB, Tag>& s, FormatContext& ctx) const
  {
    if (mode == hexadecimal) {
      return s.template to_string_of_hex<decltype(std::declval<FormatContext>().out())>(ctx.out(), order == reverse);
    }

    if (mode == intervals) {
      bool first = true;
      fmt::format_to(ctx.out(), "{{");
      for_each_interval(s, [&first, &ctx](size_t start_interval, size_t end_interval) {
        // Append a comma if the interval is not the first.
        if (first) {
          first = false;
        } else {
          fmt::format_to(ctx.out(), ", ");
        }

        // Print interval if it is more than one bit, otherwise a single value.
        if (end_interval - start_interval > 1) {
          fmt::format_to(ctx.out(), "[{}, {})", start_interval, end_interval);
        } else {
          fmt::format_to(ctx.out(), "{}", start_interval);
        }
      });
      fmt::format_to(ctx.out(), "}}");
      return ctx.out();
    }

    if (mode == bit_positions) {
      if (s.is_contiguous()) {
        unsigned lowest  = s.find_lowest();
        unsigned highest = s.find_highest();
        if (lowest == highest) {
          // Single value.
          fmt::format_to(ctx.out(), "{}", lowest);
        } else {
          // Format as a range.
          fmt::format_to(ctx.out(), "[{}, {})", lowest, highest + 1);
        }

      } else {
        // Format as a list of bit positions.
        srsran::static_vector<size_t, N> bit_pos = s.get_bit_positions();

        fmt::format_to(ctx.out(), "{}", srsran::span<size_t>(bit_pos));
      }
      return ctx.out();
    }

    return s.template to_string_of_bits<decltype(std::declval<FormatContext>().out())>(ctx.out(), order == reverse);
  }
};

} // namespace fmt
