/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
/// \brief Log-likelihood ratio type declaration.
///
/// Type used for representing log-likelihood ratios. Values are stored in an \c int8_t variable, but the sum operator
/// is overloaded with a saturated sum, meaning that the result is clipped if it (absolutely) exceeds \c LLR_MAX.

#pragma once

#include "srsran/adt/bit_buffer.h"
#include "srsran/adt/span.h"
#include "srsran/srsvec/detail/traits.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"

#include <numeric>

namespace srsran {

/// \brief Represents log-likelihood ratio (LLR) values.
///
/// Log-likelihood ratios are represented as \c int8_t values between <tt>-LLR_MAX</tt> and \c LLR_MAX. The values
/// <tt>&plusmn;LLR_INFTY = &plusmn;127</tt> are reserved for denoting fixed bits, that is bits that are surely set to
/// zero (log-likelihood ratio equal to <tt>+LLR_INFTY</tt>) or one (log-likelihood ratio equal to <tt>-LLR_INFTY</tt>).
class log_likelihood_ratio
{
public:
  /// Alias for the underlying data type used for representing the LLR value.
  using value_type = int8_t;

  /// Default constructor.
  constexpr log_likelihood_ratio() = default;

  /// \brief Constructor from integral type.
  ///
  /// Creates a new LLR set to the provided value if the latter is in the range <tt>-LLR_MAX</tt> to \c LLR_MAX, or
  /// if it is <tt>&plusmn;LLR_INFTY</tt>. Otherwise, terminates the program execution.
  /// \tparam    T    The input type. Must be an integral type.
  /// \param[in] val  The value the LLR is set to.
  /// \remark Implicit conversions are allowed on purpose.
  template <typename T>
  constexpr log_likelihood_ratio(T val)
  {
    static_assert(std::is_integral<T>::value, "LLRs must be initialized with integer values.");
    srsran_assert(((val <= LLR_MAX) && (val >= -LLR_MAX)) || (val == LLR_INFTY) || (val == -LLR_INFTY),
                  "Invalid LLR value.");
    value = static_cast<value_type>(val);
  }

  /// \name Conversion operators.
  ///@{

  /// Converts the LLR to a plain \c value_type value.
  explicit constexpr operator value_type() const { return value; };
  /// Converts the LLR to a plain \c value_type value.
  constexpr value_type to_value_type() const { return value; };

  /// Converts the LLR to a plain \c int value.
  explicit constexpr operator int() const { return value; };
  /// Converts the LLR to a plain \c int value.
  constexpr int to_int() const { return value; };

  /// \brief Converts the LLR to a hard bit.
  /// \note By convention, the null LLR indeterminate case returns a 1 bit.
  constexpr uint8_t to_hard_bit() const { return static_cast<uint8_t>(value <= 0); }
  ///@}

  /// Default assignment operator.
  constexpr log_likelihood_ratio& operator=(const log_likelihood_ratio& other)
  {
    value = other.value;
    return *this;
  }

  /// Negation (additive inverse).
  constexpr log_likelihood_ratio operator-() const { return -value; }

  /// \brief Saturated sum.
  ///
  /// The result is clipped if larger (in absolute value) than log_likelihood_ratio::LLR_MAX. Then, for instance,
  /// <tt>LLR_MAX + 2 = LLR_MAX</tt> and <tt>-LLR_MAX - 2 = -LLR_MAX</tt>.
  log_likelihood_ratio operator+(log_likelihood_ratio rhs) const
  {
    rhs += *this;
    return rhs;
  }

  /// \brief Addition assignment with saturated sum.
  log_likelihood_ratio operator+=(log_likelihood_ratio rhs);

  /// \brief Saturated difference.
  ///
  /// Follows naturally from the saturated sum: <tt>a - b = a + (-b)</tt>.
  log_likelihood_ratio operator-(log_likelihood_ratio rhs) const { return *this + (-rhs); }

  /// \brief Subtraction assignment with saturated sum.
  log_likelihood_ratio operator-=(log_likelihood_ratio rhs)
  {
    *this += (-rhs);
    return *this;
  }

  /// \name Comparison operators.
  ///@{

  /// Plain comparison operator.
  constexpr bool operator==(log_likelihood_ratio rhs) const { return value == rhs.value; }
  constexpr bool operator!=(log_likelihood_ratio rhs) const { return value != rhs.value; }
  constexpr bool operator>(log_likelihood_ratio rhs) const { return value > rhs.value; }
  constexpr bool operator<(log_likelihood_ratio rhs) const { return value < rhs.value; }
  constexpr bool operator>=(log_likelihood_ratio rhs) const { return value >= rhs.value; }
  constexpr bool operator<=(log_likelihood_ratio rhs) const { return value <= rhs.value; }
  ///@}

  /// \brief Clips and quantizes a real-valued (\c float), continuous log-likelihood ratio to the discrete
  /// representation of type \c log_likelihood_ratio.
  ///
  /// \param[in] value        The real-valued log-likelihood ratio to quantize.
  /// \param[in] range_limit  The input value mapped to \c LLR_MAX.
  /// \return A quantized representation of \c value as a \c log_likelihood_ratio quantity.
  /// \note The quantization in the range <tt>(-range_limit, range_limit)</tt> is [mid-tread
  /// uniform](https://en.wikipedia.org/wiki/Quantization_(signal_processing)#Mid-riser_and_mid-tread_uniform_quantizers),
  /// with quantization step <tt> range_limit / LLR_MAX </tt>.
  /// \note All values larger (in magnitude) than \c range_limit, will be clipped and mapped to
  /// <tt>&plusmn;LLR_MAX</tt>, depending on their sign.
  static log_likelihood_ratio quantize(float value, float range_limit);

  /// Returns the special value "positive infinity".
  static constexpr log_likelihood_ratio infinity() { return LLR_INFTY; }

  /// Returns the maximum finite LLR value.
  static constexpr log_likelihood_ratio max() { return LLR_MAX; }

  /// Returns the minimum finite LLR value.
  static constexpr log_likelihood_ratio min() { return -LLR_MAX; }

  /// Checks whether a log-likelihood ratio has a finite value.
  static constexpr bool isfinite(log_likelihood_ratio llr)
  {
    return ((llr.value >= -LLR_MAX) && (llr.value <= LLR_MAX));
  }

  /// Checks whether a log-likelihood ratio has an infinite value.
  static constexpr bool isinf(log_likelihood_ratio llr) { return !isfinite(llr); }

  /// \brief Promotion sum.
  ///
  /// As opposed to the saturated sum (i.e., log_likelihood_ratio::operator+), results larger (in absolute value) than
  /// \c LLR_MAX are set to <tt>&plusmn;LLR_INFTY</tt>.
  /// \param[in] a First summand.
  /// \param[in] b Second summand.
  /// \return The promotion sum of \c a and \c b.
  static log_likelihood_ratio promotion_sum(log_likelihood_ratio a, log_likelihood_ratio b);

  /// \brief Copy sign from a number.
  ///
  /// \tparam T An arithmetic type.
  /// \param[in] a A log-likelihood ratio.
  /// \param[in] b A real number.
  /// \return A log-likelihood ratio with the magnitude of \c a and the sign of \c b.
  /// \remark A zero in the second argument is considered as a positive number, following the convention of \c
  /// std::copysign.
  template <typename T>
  static constexpr log_likelihood_ratio copysign(log_likelihood_ratio a, T b)
  {
    static_assert(std::is_arithmetic<T>::value, "Template type is not an arithmetic type.");
    if (b < 0) {
      return -std::abs(a.value);
    }
    return std::abs(a.value);
  }

  /// Absolute value.
  static value_type abs(log_likelihood_ratio a) { return static_cast<value_type>(std::abs(a.to_int())); }

  /// \brief Soft XOR operation.
  ///
  /// The soft XOR between two LLRs \c x and \c y is the log-likelihood ratio corresponding to the logical XOR operation
  /// \f$a \oplus b\f$, where \f$a\f$ and \f$b\f$ are the bits whose soft representation is \c x and \c y, respectively.
  ///
  /// \remark For complexity reasons, the true soft xor function
  /// \f[
  /// \text{soft_xor}(x, y) = \sign(x y) \cdot \Phi\Bigl(\Phi(\abs{x}) +
  /// \Phi(\abs{y})\Bigr) \qquad\text{with }
  /// \Phi: \mathbb{R}_+ \to \mathbb{R}_+; u \mapsto \Phi(u) = -\ln\Bigl(\tanh\Bigl(\frac{u}{2}\Bigr)\Bigr)
  /// \f]
  /// is replaced by the common approximation
  /// \f[
  /// \text{soft_xor}(x, y) \approx \sign(x y) \cdot \min\bigl\{\abs{x}, \abs{y}\bigr\}.
  /// \f]
  static log_likelihood_ratio soft_xor(log_likelihood_ratio x, log_likelihood_ratio y)
  {
    int abs_x = std::abs(x.value);
    int abs_y = std::abs(y.value);
    if (x.value * y.value < 0) {
      return -std::min(abs_x, abs_y);
    }
    return std::min(abs_x, abs_y);
  }

  // Documentation at definition (see below).
  template <typename T>
  static int norm_squared(const T& x);

  // Documentation at definition (see below).
  template <typename T, typename U, typename V>
  static V dot_prod(const T& x, const U& y, V init);

private:
  /// Actual LLR value.
  value_type value = 0;

  /// \brief Represents a log-likelihood ratio set to infinity.
  ///
  /// An LLR takes the value \c LLR_INFTY when the corresponding bit is certainly zero. Similarly, an LLR takes the
  /// value <tt>-LLR_INFTY</tt> when the corresponding bit is certainly one.
  static constexpr value_type LLR_INFTY = 127;

  /// \brief Maximum (finite) value of a log-likelihood ratio.
  ///
  /// Finite LLRs take values between <tt>-LLR_MAX</tt> and \c LLR_MAX (both included).
  static constexpr value_type LLR_MAX = 120;
};

/// \name Common numerical constants.
///@{

/// Maximum finite log-likelihood ratio value.
constexpr log_likelihood_ratio LLR_MAX = log_likelihood_ratio::max();

/// Minimum finite log-likelihood ratio value.
constexpr log_likelihood_ratio LLR_MIN = log_likelihood_ratio::min();

/// "Positive infinity" log-likelihood ratio.
constexpr log_likelihood_ratio LLR_INFINITY = log_likelihood_ratio::infinity();
///@}

namespace detail {

/// Checks if \c T is compatible with a span of log_likelihood_ratios.
template <typename T, typename = void>
struct is_llr_span_compatible : std::false_type {
  // Intentionally empty.
};

/// Checks if \c T is compatible with a span of log_likelihood_ratios.
template <typename T>
struct is_llr_span_compatible<T,
                              std::enable_if_t<std::is_convertible<T, span<log_likelihood_ratio>>::value ||
                                               std::is_convertible<T, span<const log_likelihood_ratio>>::value>>
  : std::true_type {
  // Intentionally empty.
};

} // namespace detail

/// \brief Dot product of a sequence of LLRs and a sequence of values of an arithmetic type.
///
/// Computes the dot product (a.k.a. inner product or scalar product) of the two input sequences, adding an initial
/// offset.
/// \tparam T         A span (or any container convertible to a span) of log-likelihood ratios (either constant or
///                   volatile).
/// \tparam U         A span (or any container convertible to a span) of an arithmetic type.
/// \tparam V         Output type (must be compatible with the product of objects of type \c T and \c U).
/// \param[in] x      A sequence of (possibly constant) log-likelihood ratios.
/// \param[in] y      A second sequence of values.
/// \param[in] init   Initialization value.
/// \return The dot product between the two spans plus \c init, i.e. \f$ x \cdot y + \textup{init} = \sum_i x_i y_i +
/// \textup{init}\f$.
/// \remark The two input spans must have the same length.
/// \remark This function treats the LLRs as real values, thus neither saturation nor promotion apply.
template <typename T, typename U, typename V>
V log_likelihood_ratio::dot_prod(const T& x, const U& y, V init)
{
  static_assert(detail::is_llr_span_compatible<T>::value, "Template type is not compatible with a span of LLRs");
  static_assert(srsvec::detail::is_arithmetic_span_compatible<U>::value,
                "Template type is not compatible with a span of arithmetics");
  srsran_assert(x.size() == y.size(), "Input spans must have identical sizes: '{}' vs '{}'", x.size(), y.size());
  return std::inner_product(
      x.begin(), x.end(), y.begin(), init, std::plus<V>(), [](log_likelihood_ratio a, log_likelihood_ratio b) {
        return a.to_int() * b.to_int();
      });
}

/// \brief Squared Euclidean norm of a sequence of LLRs.
///
/// Computes the squared Euclidean norm (sum of the squares of the elements) of the input sequence of LLRs.
///
/// \tparam T     A span (or any container convertible to a span) of a log-likelihood ratios (either constant or
///               volatile).
/// \param[in] x  A sequence of (possibly constant) log-likelihood ratios.
/// \return The squared Euclidean norm of \c x as an integer.
/// \remark This function treats the LLRs as real values, thus neither saturation nor promotion apply.
template <typename T>
inline int log_likelihood_ratio::norm_squared(const T& x)
{
  static_assert(detail::is_llr_span_compatible<T>::value, "Template type is not compatible with a span of LLRs");
  return std::accumulate(
      x.begin(), x.end(), 0, [](int a, log_likelihood_ratio b) { return a + b.to_int() * b.to_int(); });
}

/// \brief Obtains hard bits from a vector of soft bits.
///
/// Performs hard decision on a vector of soft bits. The resulting bits are packed and stored in an \ref
/// srsran::bit_buffer.
///
/// \param[out] hard_bits Buffer of hard bits.
/// \param[in] soft_bits  Buffer of soft bits.
/// \return \c true if none of the soft bits are zero. Otherwise, \c false.
bool hard_decision(bit_buffer& hard_bits, span<const log_likelihood_ratio> soft_bits);

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for srsran::log_likelihood_ratio.
///
/// See https://fmt.dev/latest/api.html#formatting-user-defined-types.
template <>
struct formatter<srsran::log_likelihood_ratio> {
  /// Parser.
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  /// Formatter.
  template <typename FormatContext>
  auto format(srsran::log_likelihood_ratio llr, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", static_cast<srsran::log_likelihood_ratio::value_type>(llr));
  }
};

} // namespace fmt
