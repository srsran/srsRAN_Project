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

#include "srsgnb/srslog/bundled/fmt/format.h"
#include "srsgnb/support/srsran_assert.h"

#include <cstdint>

namespace srsgnb {

/// Represents log-likelihood ratio (LLR) values.
class log_likelihood_ratio
{
public:
  /// \brief Represents a log-likelihood ratio set to infinity.
  ///
  /// An LLR takes the value \c LLR_INFTY when the corresponding bit is certainly zero. Similarly, an LLR takes the
  /// value <tt>-LLR_INFTY</tt> when the corresponding bit is certainly one.
  static constexpr int8_t LLR_INFTY = 127;

  /// \brief Maximum (finite) value of a log-likelihood ratio.
  ///
  /// Finite LLRs take values between <tt>-LLR_MAX</tt> and \c LLR_MAX (both included).
  static constexpr int8_t LLR_MAX = 120;

  /// \brief Constructor from integral type.
  ///
  /// Creates a new LLR set to the provided value if the latter is in the range <tt>-LLR_MAX</tt> to \c LLR_MAX, or
  /// if it is <tt>&plusmn;LLR_INFTY</tt>. Otherwise, throws an "out of range" exception.
  /// \tparam    T    The input type. Must be an integral type.
  /// \param[in] val  The value the LLR is set to.
  /// \remark Implicit conversions are allowed on purpose.
  template <typename T>
  log_likelihood_ratio(T val)
  {
    static_assert(std::is_integral<T>::value, "LLRs must be initialized with integer values.");
    T abs_val = std::abs(val);
    if ((abs_val <= LLR_MAX) || (abs_val == LLR_INFTY)) {
      value = static_cast<int8_t>(val);
      return;
    }
    srsran_terminate("Invalid LLR value.");
  };

  /// Converts the LLR to a plain \c int8_t value.
  explicit operator int8_t() const { return value; };

  /// Default assignment operator.
  log_likelihood_ratio& operator=(const log_likelihood_ratio& other) = default;

  /// Negation (additive inverse).
  log_likelihood_ratio operator-() const { return -value; }

  /// \brief Saturated sum.
  ///
  /// The result is clipped if larger (in absolute value) than log_likelihood_ratio::LLR_MAX. Then, for instance,
  /// <tt>LLR_MAX + 2 = LLR_MAX</tt> and <tt>-LLR_MAX - 2 = -LLR_MAX</tt>.
  log_likelihood_ratio operator+(log_likelihood_ratio rhs) const;

  /// \brief Saturated difference.
  ///
  /// Follows naturally from the saturated sum: <tt>a - b = a + (-b)</tt>.
  log_likelihood_ratio operator-(log_likelihood_ratio rhs) const { return *this + (-rhs); }

  /// \name Comparison operators.
  ///@{

  /// Plain comparison operator.
  bool operator==(log_likelihood_ratio rhs) const { return value == rhs.value; }
  bool operator!=(log_likelihood_ratio rhs) const { return value != rhs.value; }
  bool operator>(log_likelihood_ratio rhs) const { return value > rhs.value; }
  bool operator<(log_likelihood_ratio rhs) const { return value < rhs.value; }
  bool operator>=(log_likelihood_ratio rhs) const { return value >= rhs.value; }
  bool operator<=(log_likelihood_ratio rhs) const { return value <= rhs.value; }
  ///@}

  /// \brief Promotion sum.
  ///
  /// As opposed to the saturated sum (i.e., log_likelihood_ratio::operator+), results larger (in absolut value) than
  /// \c LLR_MAX are set to <tt>&plusmn;LLR_INFTY</tt>.
  /// \param[in] a First summand.
  /// \param[in] b Second summand.
  /// \return The promotion sum of \c a and \c b.
  static log_likelihood_ratio promotion_sum(log_likelihood_ratio a, log_likelihood_ratio b);

private:
  /// Actual LLR value.
  int8_t value = 0;
};

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
    return format_to(ctx.out(), "LLR({})", static_cast<int8_t>(llr));
  }
};

} // namespace fmt

#endif // SRSGNB_PHY_UPPER_LOG_LIKELIHOOD_RATIO_H
