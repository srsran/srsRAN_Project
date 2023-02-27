/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"
#include <cstdint>
#include <inttypes.h>
#include <string>

namespace srsran {

/// \brief Returns an unsigned integer with the N most significant bits (MSB) set to zero, and the remaining bits set
/// to 1.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of MSB bits set to zero.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_msb_zeros(size_t N)
{
  static_assert(std::is_unsigned<Integer>::value, "T must be unsigned integer");
  return (N == 0) ? static_cast<Integer>(-1) : (N == sizeof(Integer) * 8U) ? 0 : (static_cast<Integer>(-1) >> (N));
}

/// \brief Returns an unsigned integer with the N least significant bits (LSB) set to zero, and the remaining bits set
/// to 1.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of LSB bits set to zero.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_lsb_ones(size_t N)
{
  return mask_msb_zeros<Integer>(sizeof(Integer) * 8U - N);
}

/// \brief Returns an unsigned integer with the N most significant bits (MSB) set to one, and the remaining bits set
/// to zero.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of MSB bits set to one.
/// \return Resulting integer bitmap.
template <typename Integer>
constexpr Integer mask_msb_ones(size_t N)
{
  return ~mask_msb_zeros<Integer>(N);
}

/// \brief Returns an unsigned integer with the N least significant bits (LSB) set to one, and the remaining bits set
/// to zero.
/// \tparam Integer Type of unsigned integer returned by the function.
/// \param[in] N Number of LSB bits set to one.
/// \return Resulting integer bitmap.
template <typename Integer>
Integer mask_lsb_zeros(size_t N)
{
  return ~mask_lsb_ones<Integer>(N);
}

namespace detail {

template <typename Integer>
struct bitset_builtin_helper {
  /// \brief Returns the number of leading 0-bits in value, starting at the most significant bit position.
  /// If value is 0, the result is number of bits of Integer.
  static Integer zero_msb_count(Integer value)
  {
    if (value == 0) {
      return std::numeric_limits<Integer>::digits;
    }
    Integer ret = 0;
    for (Integer shift = std::numeric_limits<Integer>::digits >> 1; shift != 0; shift >>= 1) {
      Integer tmp = value >> shift;
      if (tmp != 0) {
        value = tmp;
      } else {
        ret |= shift;
      }
    }
    return ret;
  }
  /// \brief Returns the number of trailing 0-bits in value, starting at the least significant bit position.
  /// If value is 0, the result is number of bits of Integer.
  static Integer zero_lsb_count(Integer value)
  {
    if (value == 0) {
      return std::numeric_limits<Integer>::digits;
    }
    Integer ret = 0;
    for (Integer shift = std::numeric_limits<Integer>::digits >> 1, mask = std::numeric_limits<Integer>::max() >> shift;
         shift != 0;
         shift >>= 1, mask >>= shift) {
      if ((value & mask) == 0) {
        value >>= shift;
        ret |= shift;
      }
    }
    return ret;
  }

  /// \brief Returns the number of 1-bits in value.
  static int count_ones(Integer value)
  {
    // Note: use an "int" for count triggers popcount optimization if SSE instructions are enabled.
    int c = 0;
    for (Integer w = value; w > 0; c++) {
      w &= w - 1;
    }
    return c;
  }
};

#ifdef __GNUC__ // clang and gcc

/// Specializations of bitset operations for unsigned.
template <>
struct bitset_builtin_helper<unsigned> {
  static unsigned zero_msb_count(unsigned value)
  {
    return (value) ? __builtin_clz(value) : std::numeric_limits<unsigned>::digits;
  }
  static unsigned zero_lsb_count(unsigned value)
  {
    return (value) ? __builtin_ctz(value) : std::numeric_limits<unsigned>::digits;
  }
  static int count_ones(unsigned value) { return __builtin_popcount(value); }
};

/// Specializations of bitset operations for unsigned long.
template <>
struct bitset_builtin_helper<unsigned long> {
  static unsigned long zero_msb_count(unsigned long value)
  {
    return (value) ? __builtin_clzl(value) : std::numeric_limits<unsigned long>::digits;
  }
  static unsigned long zero_lsb_count(unsigned long value)
  {
    return (value) ? __builtin_ctzl(value) : std::numeric_limits<unsigned long>::digits;
  }
  static int count_ones(unsigned long value) { return __builtin_popcountl(value); }
};

/// Specializations of bitset operations for unsigned long long.
template <>
struct bitset_builtin_helper<unsigned long long> {
  static unsigned long long zero_msb_count(unsigned long long value)
  {
    return (value) ? __builtin_clzll(value) : std::numeric_limits<unsigned long long>::digits;
  }
  static unsigned long long zero_lsb_count(unsigned long long value)
  {
    return (value) ? __builtin_ctzll(value) : std::numeric_limits<unsigned long long>::digits;
  }
  static int count_ones(unsigned long long value) { return __builtin_popcountll(value); }
};
#endif

} // namespace detail

/// \brief Counts the number of contiguous bits set to zero, starting from the MSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return count of bits set to zero.
template <typename Integer>
Integer zero_msb_count(Integer value)
{
  return (~value) ? detail::bitset_builtin_helper<Integer>::zero_msb_count(value) : 0;
}

/// \brief Finds the position of the first bit set to one, starting from the MSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return MSB position with the bit set to one. The MSB has position sizeof(Integer) * 8 - 1.
template <typename Integer>
Integer find_first_msb_one(Integer value)
{
  return (value) ? (sizeof(Integer) * 8U - 1 - detail::bitset_builtin_helper<Integer>::zero_msb_count(value))
                 : std::numeric_limits<Integer>::digits;
}

/// \brief Finds the position of the first bit set to one, starting from the LSB.
/// \tparam Integer Integer type of received bitmap.
/// \param[in] value Integer bitmap
/// \return LSB position with the bit set to one. The LSB has position zero.
template <typename Integer>
Integer find_first_lsb_one(Integer value)
{
  return detail::bitset_builtin_helper<Integer>::zero_lsb_count(value);
}

/// \brief Counts the number of bits set to one in an integer.
template <typename Integer>
int count_ones(Integer value)
{
  return detail::bitset_builtin_helper<Integer>::count_ones(value);
}

/// \brief Represents a dynamically-sized bitset with an upper bound capacity of N bits.
///
/// The bounded_bitset is represented internally via an array of unsigned integers, with each integer storing a bitmap.
/// This class also offers many standard logic manipulation methods, like ::any(), operators &=, &, |=, |, etc. and
/// utility methods to convert the bitset into strings or integers.
///
/// Depending on the passed \c FirstBitIsLeftmost template argument, this class can represent the bits of the bitset in
/// different orders.
/// E.g. bounded_bitset<4, false> a;
/// a.set(1);
/// fmt::print("{:b}", a); // prints "0001".
/// bounded_bitset<4, true> b;
/// b.set(1);
/// fmt::print("{:b}", b); // prints "1000".
///
/// \tparam N Upper bound for bitset size in number of bits.
/// \tparam FirstBitIsLeftmost Bit index order in memory. If set to (false / true), the bit index 0 corresponds to the
/// (right-most / left-most) index of the bitset. Note that this argument has an effect on the underlying bitset memory
/// layout.
template <size_t N, bool FirstBitIsLeftmost = false>
class bounded_bitset
{
  using word_t                      = uint64_t;
  static const size_t bits_per_word = 8 * sizeof(word_t);

public:
  constexpr bounded_bitset() = default;

  constexpr explicit bounded_bitset(size_t cur_size_) : cur_size(cur_size_) {}

  /// \brief Constructs a bitset using iterators.
  ///
  /// The constructed bitset size is equal to <tt> end - begin </tt> size. The values in the list are mapped one to one
  /// starting from begin.
  ///
  /// \tparam Iterator Boolean iterator type.
  /// \param[in] begin Begin iterator.
  /// \param[in] end End iterator.
  template <
      typename Iterator,
      typename std::enable_if<std::is_convertible<typename std::iterator_traits<Iterator>::value_type, bool>::value,
                              bool>::type = 0>
  constexpr bounded_bitset(Iterator begin, Iterator end)
  {
    resize(end - begin);
    std::for_each(begin, end, [this, n = 0](bool value) mutable { set(n++, value); });
  }

  /// \brief Constructs a bitset from an initializer list.
  ///
  /// The constructed bitset size is equal to \c values size. The values in the list are mapped one to one.
  ///
  /// \param[in] values Boolean initializer list.
  constexpr bounded_bitset(std::initializer_list<const bool> values)
  {
    resize(values.size());
    std::for_each(values.begin(), values.end(), [this, n = 0](bool value) mutable { set(n++, value); });
  }

  static constexpr bool bit_order() noexcept { return FirstBitIsLeftmost; }

  static constexpr size_t max_size() noexcept { return N; }

  /// Current size of the bounded_bitset.
  size_t size() const noexcept { return cur_size; }

  /// Resize of the bounded_bitset. If <tt> new_size > max_size() </tt>, an assertion is triggered.
  void resize(size_t new_size)
  {
    srsran_assert(new_size <= max_size(), "ERROR: new size='{}' exceeds bitset capacity='{}'", new_size, max_size());
    if (new_size == cur_size) {
      return;
    }
    cur_size = new_size;
    sanitize_();
    for (size_t i = nof_words_(); i < max_nof_words_(); ++i) {
      buffer[i] = static_cast<word_t>(0);
    }
  }

  /// \brief Set bit with provided index to either true or false. Assertion is triggered if pos >= max_size().
  /// \param[in] pos Position in bitset.
  /// \param[in] val Value to set the bit.
  void set(size_t pos, bool val)
  {
    if (val) {
      assert_within_bounds_(pos, true);
      set_(pos);
    } else {
      assert_within_bounds_(pos, true);
      reset_(pos);
    }
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
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] = static_cast<word_t>(0);
    }
  }

  /// \brief Appends a bit with value \c val to the set.
  ///
  /// Assertion is triggered if the resultant size exceeds the maximum size of the bitset.
  void push_back(bool val)
  {
    unsigned bitpos = size();
    resize(size() + 1);
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
    static_assert(std::is_unsigned<Integer>::value, "push_back only works for unsigned integers");
    unsigned bitpos = size();
    resize(size() + nof_bits);
    for (unsigned bit_index = 0; bit_index != nof_bits; ++bit_index) {
      set(bitpos + bit_index, (val >> (nof_bits - 1 - bit_index)) & 1U);
    }
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
  template <unsigned Factor>
  bounded_bitset<Factor * N> kronecker_product(const bounded_bitset<Factor>& other) const
  {
    static_assert(Factor <= bits_per_word,
                  "The current algorithm does not support a filter containing more than one word.");

    // Prepare an empty result.
    bounded_bitset<Factor * N> result(size() * other.size());

    for_each(0, size(), [&](unsigned bit_index) {
      unsigned bitpos = bit_index * Factor;
      word_t   word   = other.buffer[0];

      unsigned bit_offset = bitpos % bits_per_word;
      unsigned word_index = bitpos / bits_per_word;

      result.buffer[word_index] |= (word << bit_offset);
      if (bit_offset && (bit_offset + other.size() > bits_per_word)) {
        result.buffer[word_index + 1] |= (word >> (bits_per_word - bit_offset));
      }
    });

    srsran_assert(count() * other.count() == result.count(),
                  "The resultant number of ones is not consistent with inputs. It expected {} but got {}.",
                  count() * other.count(),
                  result.count());

    return result;
  }

  /// \brief Check if bit with provided index is set to true.
  /// \param[in] pos Position in bitset.
  /// \return Returns true if bit at position pos is set.
  bool test(size_t pos) const
  {
    assert_within_bounds_(pos, true);
    return test_(pos);
  }

  /// \brief Toggle values of bits in bitset.
  /// \return Returns this object.
  bounded_bitset<N, FirstBitIsLeftmost>& flip() noexcept
  {
    for (size_t i = 0; i < nof_words_(); ++i) {
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
  bounded_bitset<N, FirstBitIsLeftmost>& fill(size_t startpos, size_t endpos, bool value = true)
  {
    find_first_word_(startpos, endpos, [this, value](size_t word_idx, const word_t& mask) {
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
  /// it is specified, the returned slice has the same template parameters "N" and "FirstBitIsLeftmost" of "this".
  ///
  /// \param[in] startpos The bit index where the subview starts.
  /// \param[in] endpos The bit index where the subview stops.
  template <size_t N2 = N>
  bounded_bitset<N2, FirstBitIsLeftmost> slice(size_t startpos, size_t endpos) const
  {
    bounded_bitset<N2, FirstBitIsLeftmost> sliced(endpos - startpos);
    if (FirstBitIsLeftmost) {
      std::swap(startpos, endpos);
      startpos = get_bitidx_(startpos) + 1;
      endpos   = get_bitidx_(endpos) + 1;
    }
    unsigned start_word = startpos / bits_per_word;
    unsigned start_mod  = startpos % bits_per_word;

    if (start_mod != 0) {
      word_t left_mask  = mask_lsb_ones<word_t>(bits_per_word - start_mod);
      word_t right_mask = mask_lsb_ones<word_t>(start_mod);
      for (unsigned i = 0; i != sliced.nof_words_(); ++i) {
        sliced.buffer[i] = (buffer[i + start_word] >> start_mod) & left_mask;
        if (i + start_word + 1 < nof_words_()) {
          sliced.buffer[i] |= (buffer[i + start_word + 1] & right_mask) << (bits_per_word - start_mod);
        }
      }
    } else {
      for (unsigned i = 0; i != sliced.nof_words_(); ++i) {
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
    assert_range_bounds_(startpos, endpos);
    if (startpos == endpos) {
      return -1;
    }

    if (not FirstBitIsLeftmost) {
      return find_first_(startpos, endpos, value);
    }
    return find_first_reversed_(startpos, endpos, value);
  }

  /// \brief Executes a function for all \c true (or all \c false) bits in the given bitset interval.
  ///
  /// \param[in] startpos Smallest bit index considered for the function execution (included).
  /// \param[in] endpos   Largest bit index considered for the function execution (excluded).
  /// \param[in] function Function to execute - the signature should be compatible with <tt>void ()(unsigned)</tt>.
  /// \param[in] value    Bit value that triggers the function execution.
  template <class T>
  inline void for_each(size_t startpos, size_t endpos, T&& function, bool value = true) const noexcept
  {
    static_assert(std::is_convertible<T, std::function<void(size_t)>>::value,
                  "The function must have void(size_t) signature.");
    static_assert(!FirstBitIsLeftmost, "The for_each method is not yet available for reversed bitsets.");

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
    return buffer[nw - 1] == (allset >> (nw * bits_per_word - size()));
  }

  /// \brief Checks if all bits within a bit index range are set to 1.
  /// \return Returns true if all the bits within the range are 1.
  bool all(size_t start, size_t stop) const
  {
    bool not_all_found = find_first_word_(start, stop, [this](size_t word_idx, const word_t& mask) {
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

    if (FirstBitIsLeftmost) {
      int ret = find_first_(startpos, endpos, value);
      if (ret == -1) {
        return ret;
      }
      return size() - 1 - ret;
    }
    return find_last_(startpos, endpos, value);
  }

  /// \brief Checks if at least one bit in the bitset is set to 1.
  /// \return Returns true if at least one bit is 1.
  bool any() const noexcept
  {
    for (size_t i = 0; i < nof_words_(); ++i) {
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

    // Condition 3. The number of elements must match with the star to end number of elements.
    return (value_count == static_cast<size_t>((endpos + 1) - startpos));
  }

  /// \brief Counts the number of bits set to 1.
  /// \return Returns the number of bits set to 1.
  size_t count() const noexcept
  {
    int result = 0;
    for (size_t i = 0; i < nof_words_(); i++) {
      result += count_ones(buffer[i]);
    }
    return result;
  }

  /// \brief Compares two bitsets.
  /// \return Returns true if both bitsets are equal in size and values of bits.
  bool operator==(const bounded_bitset<N, FirstBitIsLeftmost>& other) const noexcept
  {
    if (size() != other.size()) {
      return false;
    }
    for (uint32_t i = 0; i < nof_words_(); ++i) {
      if (buffer[i] != other.buffer[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const bounded_bitset<N, FirstBitIsLeftmost>& other) const noexcept { return not(*this == other); }

  /// \brief Applies bitwise OR operation lhs |= rhs.
  /// \param[in] other Bitset which corresponds to the rhs of the operation.
  /// \return This object updated after the bitwise OR operation.
  bounded_bitset<N, FirstBitIsLeftmost>& operator|=(const bounded_bitset<N, FirstBitIsLeftmost>& other)
  {
    srsran_assert(other.size() == size(),
                  "ERROR: operator|= called for bitsets of different sizes ('{}'!='{}')",
                  size(),
                  other.size());
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] |= other.buffer[i];
    }
    return *this;
  }

  /// \brief Applies bitwise AND operation lhs &= rhs.
  /// \param[in] other Bitset which corresponds to the rhs of the operation.
  /// \return This object updated after the bitwise AND operation.
  bounded_bitset<N, FirstBitIsLeftmost>& operator&=(const bounded_bitset<N, FirstBitIsLeftmost>& other)
  {
    srsran_assert(other.size() == size(),
                  "ERROR: operator&= called for bitsets of different sizes ('{}'!='{}')",
                  size(),
                  other.size());
    for (size_t i = 0; i < nof_words_(); ++i) {
      buffer[i] &= other.buffer[i];
    }
    return *this;
  }

  /// \brief Flips values of bits in the bitset.
  /// \return Returns reference to this object, updated after the flip operation.
  bounded_bitset<N, FirstBitIsLeftmost> operator~() const noexcept
  {
    bounded_bitset<N, FirstBitIsLeftmost> ret(*this);
    ret.flip();
    return ret;
  }

  /// \brief Formatting helper for bitset.
  /// \tparam OutputIt Output fmt memory buffer type.
  /// \param[in] mem_buffer Fmt memory buffer.
  /// \return The memory buffer passed as argument.
  template <typename OutputIt>
  OutputIt to_string(OutputIt&& mem_buffer, bool reverse) const
  {
    if (size() == 0) {
      return mem_buffer;
    }

    reverse = reverse ^ FirstBitIsLeftmost;

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

  /// \brief Conversion of bounded_bitset to unsigned integer of 64 bits. If bitset size is larger than 64 bits, an
  /// assertion is triggered.
  /// \return Unsigned integer representation of the bounded_bitset.
  uint64_t to_uint64() const
  {
    srsran_assert(nof_words_() == 1, "ERROR: cannot convert bitset of size='{}' to uint64_t", size());
    return get_word_(0);
  }

  /// \brief Conversion of unsigned integer of 64 bits to bounded_bitset. If passed bitmap doesn't fit in the bitset,
  /// an assertion is triggered.
  /// \param[in] v Integer bitmap that is going to be stored in the bitset.
  void from_uint64(uint64_t v)
  {
    srsran_assert(nof_words_() == 1, "ERROR: cannot convert bitset of size='{}' to uint64_t", size());
    srsran_assert(v < (1U << size()), "ERROR: Provided mask='{}' does not fit in bitset of size='{}'", v, size());
    buffer[0] = v;
  }

  /// \brief Formatting of bounded_bitset to hex format.
  /// \tparam OutputIt Fmt memory buffer type.
  /// \param[in] mem_buffer Fmt memory buffer.
  /// \return The input fmt memory buffer.
  template <typename OutputIt>
  OutputIt to_hex(OutputIt&& mem_buffer) const noexcept
  {
    if (size() == 0) {
      return mem_buffer;
    }
    // first word may not print 16 hex digits
    int    i           = nof_words_() - 1;
    size_t rem_symbols = divide_ceil((size() - (size() / bits_per_word) * bits_per_word), 4U);
    fmt::format_to(mem_buffer, "{:0>{}x}", buffer[i], rem_symbols);
    // remaining words will occupy 16 hex digits
    for (--i; i >= 0; --i) {
      fmt::format_to(mem_buffer, "{:0>16x}", buffer[i]);
    }
    return mem_buffer;
  }

private:
  template <size_t N2, bool reversed2>
  friend class bounded_bitset;

  std::array<word_t, (N - 1) / bits_per_word + 1> buffer   = {0};
  size_t                                          cur_size = 0;

  void sanitize_()
  {
    size_t n      = size() % bits_per_word;
    size_t nwords = nof_words_();
    if (n != 0 and nwords > 0) {
      buffer[nwords - 1] &= ~((~static_cast<word_t>(0)) << n);
    }
  }

  size_t get_bitidx_(size_t bitpos) const noexcept { return FirstBitIsLeftmost ? size() - 1 - bitpos : bitpos; }

  bool test_(size_t bitpos) const noexcept
  {
    bitpos = get_bitidx_(bitpos);
    return ((get_word_(bitpos) & maskbit(bitpos)) != static_cast<word_t>(0));
  }

  void set_(size_t bitpos) noexcept
  {
    bitpos = get_bitidx_(bitpos);
    get_word_(bitpos) |= maskbit(bitpos);
  }

  void reset_(size_t bitpos) noexcept
  {
    bitpos = get_bitidx_(bitpos);
    get_word_(bitpos) &= ~(maskbit(bitpos));
  }

  size_t nof_words_() const noexcept { return size() > 0 ? (size() - 1) / bits_per_word + 1 : 0; }

  word_t& get_word_(size_t bitidx) noexcept { return buffer[bitidx / bits_per_word]; }

  const word_t& get_word_(size_t bitidx) const { return buffer[bitidx / bits_per_word]; }

  size_t word_idx_(size_t bitidx) const { return bitidx / bits_per_word; }

  void assert_within_bounds_(size_t pos, bool strict) const noexcept
  {
    srsran_assert(pos < size() or (not strict and pos == size()),
                  "ERROR: index='{}' is out-of-bounds for bitset of size='{}'",
                  pos,
                  size());
  }

  void assert_range_bounds_(size_t startpos, size_t endpos) const noexcept
  {
    srsran_assert(startpos <= endpos and endpos <= size(),
                  "ERROR: range ['{}', '{}') out-of-bounds for bitsize of size='{}'",
                  startpos,
                  endpos,
                  size());
  }

  static word_t maskbit(size_t pos) noexcept { return (static_cast<word_t>(1)) << (pos % bits_per_word); }

  static size_t max_nof_words_() noexcept { return (N - 1) / bits_per_word + 1; }

  int find_last_(size_t startpos, size_t endpos, bool value) const noexcept
  {
    size_t startword = startpos / bits_per_word;
    size_t lastword  = (endpos - 1) / bits_per_word;

    for (size_t i = lastword; i != startword - 1; --i) {
      word_t w = buffer[i];
      if (not value) {
        w = ~w;
      }

      if (i == startword) {
        size_t offset = startpos % bits_per_word;
        w &= (FirstBitIsLeftmost) ? mask_msb_zeros<word_t>(offset) : mask_lsb_zeros<word_t>(offset);
      }
      if (i == lastword) {
        size_t offset = (endpos - 1) % bits_per_word;
        w &= (FirstBitIsLeftmost) ? mask_msb_ones<word_t>(offset + 1) : mask_lsb_ones<word_t>(offset + 1);
      }
      if (w != 0) {
        return static_cast<int>(i * bits_per_word + find_first_msb_one(w));
      }
    }
    return -1;
  }

  int find_first_(size_t startpos, size_t endpos, bool value) const noexcept
  {
    size_t startword = startpos / bits_per_word;
    size_t lastword  = (endpos - 1) / bits_per_word;

    for (size_t i = startword; i <= lastword; ++i) {
      word_t w = buffer[i];
      if (not value) {
        w = ~w;
      }

      if (i == startword) {
        size_t offset = startpos % bits_per_word;
        w &= mask_lsb_zeros<word_t>(offset);
      }
      if (i == lastword) {
        size_t offset = (endpos - 1) % bits_per_word;
        w &= mask_lsb_ones<word_t>(offset + 1);
      }
      if (w != 0) {
        return static_cast<int>(i * bits_per_word + find_first_lsb_one(w));
      }
    }
    return -1;
  }

  int find_first_reversed_(size_t startpos, size_t endpos, bool value) const noexcept
  {
    size_t startbitpos = get_bitidx_(startpos), lastbitpos = get_bitidx_(endpos - 1);
    size_t startword = startbitpos / bits_per_word;
    size_t lastword  = lastbitpos / bits_per_word;

    for (size_t i = startword; i != lastword - 1; --i) {
      word_t w = buffer[i];
      if (not value) {
        w = ~w;
      }

      if (i == startword) {
        size_t offset = startbitpos % bits_per_word;
        w &= mask_lsb_ones<word_t>(offset + 1);
      }
      if (i == lastword) {
        size_t offset = lastbitpos % bits_per_word;
        w &= mask_lsb_zeros<word_t>(offset);
      }
      if (w != 0) {
        word_t pos = find_first_msb_one(w);
        return static_cast<int>(size() - 1 - (pos + i * bits_per_word));
      }
    }
    return -1;
  }

  /// \brief Finds first word, aka integer bitmap, within the provided bit index bounds for which the provided callback
  /// returns true.
  /// This helper function iterates through the bounded_bitset on a word-by-word basis. Note that operations over words
  /// are generally faster than operations over individual bits.
  /// For each iterated word, a mask of the selected bits is computed (which depends on the provided "start" and
  /// "stop" bit indexes), and the provided callback is invoked. The callback receives as arguments the word index
  /// and the selected-bit mask. If the callback returns true, the iteration stops and the function returns true.
  ///
  /// \param start first bit index of the bounded_bitset.
  /// \param stop end bit index of the bounded_bitset.
  /// \param c Callback with signature "bool(size_t word_index, word_t active_mask)" called for each word of the bitset.
  ///          When this callback returns true, the iteration is stopped.
  /// \return true if the provided callback returns true for a given word. False otherwise.
  template <typename C>
  bool find_first_word_(size_t start, size_t stop, const C& c) const
  {
    assert_range_bounds_(start, stop);
    return find_first_word_(start, stop, c, std::integral_constant<bool, FirstBitIsLeftmost>{});
  }

  template <typename C>
  bool find_first_word_(size_t start, size_t stop, const C& c, std::true_type t) const
  {
    std::swap(start, stop);
    start = get_bitidx_(start) + 1;
    stop  = get_bitidx_(stop) + 1;
    return find_first_word_(start, stop, c, std::false_type{});
  }

  template <typename C>
  bool find_first_word_(size_t start, size_t stop, const C& c, std::false_type t) const
  {
    size_t start_word = word_idx_(start);
    size_t end_word   = word_idx_(stop) + (stop % bits_per_word > 0 ? 1U : 0U);
    for (size_t i = start_word; i != end_word; ++i) {
      word_t mask = ~static_cast<word_t>(0);
      if (i == start_word) {
        mask &= mask_lsb_zeros<word_t>(start % bits_per_word);
      }
      if (i == end_word - 1) {
        mask &= mask_msb_zeros<word_t>(end_word * bits_per_word - stop);
      }
      if (c(i, mask)) {
        return true;
      }
    }
    return false;
  }
};

/// \brief Bitwise AND operation result = lhs & rhs.
/// \return new bounded_bitset that results from the Bitwise AND operation.
template <size_t N, bool FirstBitIsRightmost>
inline bounded_bitset<N, FirstBitIsRightmost> operator&(const bounded_bitset<N, FirstBitIsRightmost>& lhs,
                                                        const bounded_bitset<N, FirstBitIsRightmost>& rhs) noexcept
{
  bounded_bitset<N, FirstBitIsRightmost> res(lhs);
  res &= rhs;
  return res;
}

/// \brief Bitwise AND operation result = lhs | rhs.
/// \return new bounded_bitset that results from the Bitwise OR operation.
template <size_t N, bool FirstBitIsRightmost>
inline bounded_bitset<N, FirstBitIsRightmost> operator|(const bounded_bitset<N, FirstBitIsRightmost>& lhs,
                                                        const bounded_bitset<N, FirstBitIsRightmost>& rhs) noexcept
{
  bounded_bitset<N, FirstBitIsRightmost> res(lhs);
  res |= rhs;
  return res;
}

/// \brief Flip bits from left to right.
/// \return new bounded_bitset that results from the fliplr operation.
template <size_t N, bool FirstBitIsRightmost>
inline bounded_bitset<N, FirstBitIsRightmost> fliplr(const bounded_bitset<N, FirstBitIsRightmost>& other) noexcept
{
  bounded_bitset<N, FirstBitIsRightmost> ret(other.size());
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
template <size_t N2, size_t N, bool FirstBitIsRightmost>
inline bounded_bitset<N2, FirstBitIsRightmost> fold_and_accumulate(const bounded_bitset<N, FirstBitIsRightmost>& other,
                                                                   size_t fold_length,
                                                                   size_t slice_offset,
                                                                   size_t slice_length) noexcept
{
  srsran_assert(
      other.size() % fold_length == 0, "Invalid fold length={} for bitset of size={}", fold_length, other.size());
  bounded_bitset<N2, FirstBitIsRightmost> ret(slice_length);
  for (size_t i = 0; i != other.size(); i += fold_length) {
    ret |= other.template slice<N2>(i + slice_offset, i + slice_offset + slice_length);
  }
  return ret;
}

/// \brief Performs the fold and accumulate operation, but without slicing at the end.
///
/// \tparam N2 maximum bitset size for returned bitset.
/// \tparam FirstBitIsRightmost internal bit order representation of returned bitset.
/// \param[in] other original bitset from where folds are generated.
/// \param[in] fold_length length of each fold bitset.
/// \return bitset of size fold_length with the accumulated folds.
template <size_t N2, size_t N, bool FirstBitIsRightmost>
inline bounded_bitset<N2, FirstBitIsRightmost> fold_and_accumulate(const bounded_bitset<N, FirstBitIsRightmost>& other,
                                                                   size_t fold_length) noexcept
{
  return fold_and_accumulate<N2, N, FirstBitIsRightmost>(other, fold_length, 0, fold_length);
}

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for bounded_bitset<N, FirstBitIsLeftmost>
template <size_t N, bool FirstBitIsRightmost>
struct formatter<srsran::bounded_bitset<N, FirstBitIsRightmost>> {
  enum { hexadecimal, binary } mode = binary;
  enum { forward, reverse } order   = forward;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'x') {
        mode = hexadecimal;
      }
      if (*it == 'r') {
        order = reverse;
      }
      ++it;
    }

    return it;
  }

  template <typename FormatContext>
  auto format(const srsran::bounded_bitset<N, FirstBitIsRightmost>& s, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (mode == hexadecimal) {
      return s.template to_hex(ctx.out());
    }
    return s.template to_string(ctx.out(), order == reverse);
  }
};
} // namespace fmt
