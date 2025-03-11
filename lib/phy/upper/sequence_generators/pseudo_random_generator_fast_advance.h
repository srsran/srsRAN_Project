/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pseudo_random_generator_initializers.h"
#include <cstdint>

namespace srsran {

/// \brief Implements a pseudo-random fast state advance class.
///
/// The advance() method is design for advancing efficiently the \f$x_1\f$ and \f$x_2\f$ states up to
/// \f$2\cdot MaxNofStep - 1\f$ with a complexity of \f$O(log(n))\f$.
///
/// \tparam MaxNofStep Maximum number of steps of the instance.
template <unsigned MaxNofStep>
class pseudo_random_generator_fast_advance
{
private:
  /// \brief \f$x_1(n)\f$ initializer.
  ///
  /// It is used as a look-up table for calculating the final state of \f$x_1(n)\f$ after NofStep steps.
  const pseudo_random_initializer_x1 x1_init;
  /// \brief \f$x_2(n)\f$ initializer.
  ///
  /// It is used as a look-up table for calculating the final state of \f$x_2(n)\f$ after NofStep steps.
  const pseudo_random_initializer_x2 x2_init;
  /// \brief Fast advance for half of the number of steps.
  ///
  /// It is used for calculating the remainder steps.
  const pseudo_random_generator_fast_advance<MaxNofStep / 2> advance_half;

public:
  /// Default constructor.
  pseudo_random_generator_fast_advance() : x1_init(MaxNofStep), x2_init(MaxNofStep), advance_half() {}

  /// \brief Advances the given \f$x_1(n)\f$ and \f$x_2(n)\f$ states.
  ///
  /// \param[in,out] x1_state Reference to \f$x_1(n)\f$ initial and final state.
  /// \param[in,out] x2_state Reference to \f$x_2(n)\f$ initial and final state.
  /// \param[in]     count    Number of steps to advance.
  void advance(uint32_t& x1_state, uint32_t& x2_state, unsigned count) const
  {
    if (count >= MaxNofStep) {
      x1_state = x1_init.get(x1_state);
      x2_state = x2_init.get(x2_state);
      count -= MaxNofStep;
    }

    if (count == 0) {
      return;
    }

    advance_half.advance(x1_state, x2_state, count);
  }
};

/// Specialized fast advance with maximum number of steps equal to zero.
template <>
class pseudo_random_generator_fast_advance<0>
{
public:
  void advance(uint32_t& /* x1_state */, uint32_t& /* x2_state */, unsigned count) const
  {
    srsran_assert(count == 0, "Count must be zero.");
  }
};

} // namespace srsran
