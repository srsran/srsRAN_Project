/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Log-likelihood ratio type declaration.
///
/// Type used for representing log-likelihood ratios. Values are stored in an \c int8_t variable, but the sum operator
/// is overloaded with a saturated sum, meaning that the result is clipped if it (absolutely) exceeds \c LLR_MAX.

#ifndef SRSGNB_PHY_UPPER_LOG_LIKELIHOOD_RATIO_H
#define SRSGNB_PHY_UPPER_LOG_LIKELIHOOD_RATIO_H

#include "srsgnb/adt/span.h"
#include "srsgnb/srslog/bundled/fmt/format.h"
#include "srsgnb/srsvec/detail/traits.h"
#include "srsgnb/support/srsran_assert.h"

#include <numeric>

namespace srsgnb {

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
    if (((val <= LLR_MAX) && (val >= -LLR_MAX)) || (val == LLR_INFTY) || (val == -LLR_INFTY)) {
      value = static_cast<value_type>(val);
      return;
    }
    srsran_terminate("Invalid LLR value.");
  };

  /// \name Conversion operators.
  ///@{

  /// Converts the LLR to a plain \c int8_t value.
  explicit constexpr operator int8_t() const { return value; };
  int8_t constexpr   to_int8_t() const { return value; };
  ///@}

  /// Default assignment operator.
  constexpr log_likelihood_ratio& operator=(const log_likelihood_ratio& other) = default;

  /// Negation (additive inverse).
  constexpr log_likelihood_ratio operator-() const { return -value; }

  /// \brief Saturated sum.
  ///
  /// The result is clipped if larger (in absolute value) than log_likelihood_ratio::LLR_MAX. Then, for instance,
  /// <tt>LLR_MAX + 2 = LLR_MAX</tt> and <tt>-LLR_MAX - 2 = -LLR_MAX</tt>.
  log_likelihood_ratio operator+(log_likelihood_ratio rhs) const;

  /// \brief Addition assignment with saturated sum.
  log_likelihood_ratio operator+=(log_likelihood_ratio rhs)
  {
    *this = *this + rhs;
    return *this;
  }

  /// \brief Saturated difference.
  ///
  /// Follows naturally from the saturated sum: <tt>a - b = a + (-b)</tt>.
  log_likelihood_ratio operator-(log_likelihood_ratio rhs) const { return *this + (-rhs); }

  /// \brief Multiplication by an arithmetic type.
  template <typename T>
  T operator*(T rhs) const
  {
    static_assert(std::is_arithmetic<T>::value, "LLRs can only be multiplied by arithmetic types.");
    return rhs * value;
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

  /// Returns the special value "positive infinity."
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

namespace srsvec {
namespace detail {

/// Checks if \c T is compatible with a span of log_likelihood_ratios.
template <typename T, typename = void>
struct is_llr_span_compatible : std::false_type {};

/// Checks if \c T is compatible with a span of log_likelihood_ratios.
template <typename T>
struct is_llr_span_compatible<T,
                              std::enable_if_t<std::is_convertible<T, span<log_likelihood_ratio> >::value ||
                                               std::is_convertible<T, span<const log_likelihood_ratio> >::value> >
  : std::true_type {};

} // namespace detail

/// \brief Dot product of a span of LLRs and a span of an arithmetic type.
///
/// Computes the dot product (a.k.a. inner product or scalar product) of the two sequences represented by the input
/// spans, adding an initial offset.
/// \tparam T         A span of a log-likelihood ratios (either constant or volatile).
/// \tparam U         A span of an arithmetic type.
/// \tparam V         Output type (must be compatible with the product of object of type \c T and \c U).
/// \param[in] x      Span of (possibly constant) log-likelihood ratios.
/// \param[in] y      Second span.
/// \param[in] init   Initialization value.
/// \return The dot product between the two spans plus \c init, i.e. \f$ x \cdot y + \mathrm{init} = \sum_i x_i y_i +
/// \mathrm{init}\f$.
/// \remark The two input spans must have the same length.
template <typename T, typename U, typename V>
inline V dot_prod_llr(const T& x, const U& y, V init)
{
  static_assert(detail::is_llr_span_compatible<T>::value, "Template type is not compatible with a span of LLRs");
  static_assert(detail::is_arithmetic_span_compatible<U>::value,
                "Template type is not compatible with a span of arithmetics");
  assert(x.size() == y.size());
  return std::inner_product(x.begin(), x.end(), y.begin(), init);
}

/// \brief Squared Euclidean norm of a span of LLRs.
///
/// Computes the squared Euclidean norm (sum of the squares of the elements) of the sequence represented by the input
/// span.
///
/// \tparam T     A span of a log-likelihood ratios (either constant or volatile).
/// \param[in] x  Span of (possibly constant) log-likelihood ratios.
/// \return The squared Euclidean norm of \c x as an integer.
template <typename T>
inline int norm_sqr_llr(const T& x)
{
  static_assert(detail::is_llr_span_compatible<T>::value, "Template type is not compatible with a span of LLRs");
  return std::accumulate(
      x.begin(), x.end(), 0, [](int a, log_likelihood_ratio b) { return a + b.to_int8_t() * b.to_int8_t(); });
}

} // namespace srsvec
} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for srsgnb::log_likelihood_ratio.
///
/// See https://fmt.dev/latest/api.html#formatting-user-defined-types.
template <>
struct formatter<srsgnb::log_likelihood_ratio> {
  /// Parser.
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  /// Formatter.
  template <typename FormatContext>
  auto format(srsgnb::log_likelihood_ratio llr, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "LLR({})", static_cast<srsgnb::log_likelihood_ratio::value_type>(llr));
  }
};

} // namespace fmt

#endif // SRSGNB_PHY_UPPER_LOG_LIKELIHOOD_RATIO_H
