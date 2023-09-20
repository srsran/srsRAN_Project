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
#include "srsran/adt/interval.h"

namespace srsran {

/// \brief Generates the pseudo-random sequence defined in TS38.211 Section 5.2.1.
///
/// The parallel generation of the state sequences \f$x_1(n)\f$ and \f$x_2(n)\f$, as defined in
/// TS38.211 Section 5.2.1, exploits the fact that these sequences have a memory of 31 terms, but only the four
/// oldest terms contribute to the generation of the next one.
class pseudo_random_generator_sequence
{
  /// Internal state type.
  using state_type = uint32_t;
  /// Resultant sequence type.
  using sequence_type = uint32_t;

  /// \brief Maximum number of bits that can be generated in parallel.
  ///
  /// It is defined by the number of bits in the \f$x1\f$ and \f$x2\f$ sequences that can be updated in one go using the
  /// current state.
  static constexpr unsigned max_step_size = 28;
  /// State number of bits.
  static constexpr unsigned state_nof_bits = 31;

public:
  /// Gets the maximum number of bits that can be generated in a single step.
  static constexpr unsigned get_max_step_size() { return max_step_size; }

  pseudo_random_generator_sequence(state_type x1_, state_type x2_) : x1(x1_), x2(x2_)
  {
    srsran_assert(x1 <= mask_lsb_ones<state_type>(state_nof_bits), "Invalid X1 state (i.e., {}).", x1);
    srsran_assert(x2 <= mask_lsb_ones<state_type>(state_nof_bits), "Invalid X1 state (i.e., {}).", x2);
  }

  /// \brief Steps the sequence a number of bits simultaneously.
  /// \tparam StepSize Number of bits generated in this step.
  /// \return The next sequence \c StepSize bits.
  /// \remark The step size must not exceed \ref max_step_size.
  template <unsigned StepSize = 1>
  sequence_type step()
  {
    static_assert((StepSize > 0) && (StepSize <= max_step_size), "Invalid step size.");

    // Calculate result from the current states.
    sequence_type c = x1 ^ x2;

    // Perform XOR.
    uint32_t f1 = x1 ^ (x1 >> 3U);
    uint32_t f2 = x2 ^ (x2 >> 1U) ^ (x2 >> 2U) ^ (x2 >> 3U);

    uint32_t mask = (1U << StepSize) - 1U;

    // Prepare feedback.
    f1 = ((f1 & mask) << (31U - StepSize));
    f2 = ((f2 & mask) << (31U - StepSize));

    // Insert feedback.
    x1 = (x1 >> StepSize) ^ f1;
    x2 = (x2 >> StepSize) ^ f2;

    return c;
  }

  /// \brief Steps the sequence a number of bits simultaneously.
  /// \param[in] step_size Number of bits generated in this step.
  /// \return The next sequence \c StepSize bits.
  /// \remark The step size must not exceed \ref max_step_size.
  sequence_type step(unsigned step_size = 1)
  {
    srsran_assert(step_size <= max_step_size,
                  "The step size (i.e., {}) exceeds the maximum step size (i.e., {}).",
                  step_size,
                  static_cast<unsigned>(max_step_size));

    if (step_size == 0) {
      return 0;
    }

    // Calculate result from the current states.
    sequence_type c = x1 ^ x2;

    // Perform XOR.
    uint32_t f1 = x1 ^ (x1 >> 3U);
    uint32_t f2 = x2 ^ (x2 >> 1U) ^ (x2 >> 2U) ^ (x2 >> 3U);

    uint32_t mask = (1U << step_size) - 1U;

    // Prepare feedback.
    f1 = ((f1 & mask) << (31U - step_size));
    f2 = ((f2 & mask) << (31U - step_size));

    // Insert feedback.
    x1 = (x1 >> step_size) ^ f1;
    x2 = (x2 >> step_size) ^ f2;

    return c;
  }

  /// Gets the current \f$x_1(n)\f$ state.
  state_type get_x1() const { return x1; }

  /// Gets the current \f$x_2(n)\f$ state.
  state_type get_x2() const { return x2; }

private:
  /// Current \f$x_1(n)\f$ state.
  state_type x1;
  /// Current \f$x_2(n)\f$ state.
  state_type x2;
};

} // namespace srsran