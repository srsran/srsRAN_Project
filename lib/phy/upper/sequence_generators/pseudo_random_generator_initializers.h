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

#pragma once

#include "pseudo_random_generator_sequence.h"
#include <array>
#include <cstdint>

namespace srsran {

/// Length of the generator state in bits.
static constexpr unsigned pseudo_random_generator_state_size = 31;

/// State sequence \f$x_1(n)\f$ initializer.
class pseudo_random_initializer_x1
{
private:
  /// Memory register for sequence \f$x_1(n)\f$.
  std::array<uint32_t, pseudo_random_generator_state_size> table;

public:
  /// \brief Initializes the first 31 elements of \f$x_1(n)\f$ and advances to position \f$N_{\textup{C}}\f$.
  ///
  /// \param[in] Nc Parameter \f$N_{\textup{C}}\f$.
  pseudo_random_initializer_x1(unsigned Nc)
  {
    static constexpr unsigned max_step_size = pseudo_random_generator_sequence::get_max_step_size();

    // For each bit of the initial state.
    for (unsigned i = 0; i != pseudo_random_generator_state_size; ++i) {
      pseudo_random_generator_sequence sequence(1 << (31 - i), 0);

      unsigned n = 0;
      for (unsigned n_end = (Nc / max_step_size) * max_step_size; n != n_end; n += max_step_size) {
        sequence.step<max_step_size>();
      }
      for (; n != Nc; ++n) {
        sequence.step();
      }

      table[i] = sequence.get_x1();
    }
  }

  /// \brief Gets the \f$x_1(n)\f$ state register after initialization.
  ///
  /// \param[in] c_init Initial \f$x_1(n)\f$ state as specified by TS38.211 Section 5.2.1. Set to 1 by default.
  /// \return The \f$x_1(n)\f$ state register after initialization.
  uint32_t get_reverse(unsigned c_init = 1) const
  {
    uint32_t ret = 0;
    for (unsigned i = 0; i != pseudo_random_generator_state_size; ++i) {
      if ((c_init >> i) & 1U) {
        ret ^= table[i];
      }
    }

    return ret;
  }

  /// \brief Gets the \f$x_1(n)\f$ state register after initialization.
  ///
  /// \param[in] init_state Initial \f$x_1(n)\f$ state.
  /// \return The \f$x_1(n)\f$ state register after initialization.
  uint32_t get(unsigned init_state) const
  {
    uint32_t ret = 0;
    for (unsigned i = 0; i != pseudo_random_generator_state_size; ++i) {
      if ((init_state << i) & (1U << 31U)) {
        ret ^= table[i];
      }
    }

    return ret;
  }
};

/// \brief State sequence \f$x_2(n)\f$ initializer.
///
/// Similarly to pseudo_random_initializer_x1, this class is used to initialize the sequence \f$x_2(n)\f$ and advance it
/// until position \f$n = N_{\textup{C}}\f$. Here, however, the process is carried out simultaneously for all possible
/// seeds of the form \f$c_{\textup{init}} = 2^k\f$ for \f$k=0,1,\dots,30\f$. The initial state corresponding to any
/// other seed can easily be computed from these basic ones after noticing that the map sending a seed to the
/// corresponding sequence defines a group isomorphism between the set of seeds and the set of sequences (both groups
/// under bitwise XOR). That is, if \f{align*} c_{\textup{init}}^{(1)} &\mapsto x_2^{(1)}(n) &&\text{and} &
/// c_{\textup{init}}^{(2)} &\mapsto x_2^{(2)}(n)
/// \f}
/// then
/// \f[
/// c_{\textup{init}}^{(1)} \oplus c_{\textup{init}}^{(2)} \mapsto x_2^{(1)}(n) \oplus x_2^{(2)}(n)
/// \f]
/// with the XOR operator acting bitwise between seeds and for all \f$n\f$ between sequences.
class pseudo_random_initializer_x2
{
private:
  /// Memory register for sequence \f$x_2(n)\f$, for all basic seeds.
  std::array<uint32_t, pseudo_random_generator_state_size> table;

public:
  /// Initializes the first 31 elements of \f$x_2(n)\f$ and advances to position \f$N_{\textup{C}}\f$.
  pseudo_random_initializer_x2(unsigned Nc)
  {
    static constexpr unsigned max_step_size = pseudo_random_generator_sequence::get_max_step_size();

    // For each bit of the seed.
    for (uint32_t i = 0; i != pseudo_random_generator_state_size; ++i) {
      pseudo_random_generator_sequence sequence(0, 1 << (31 - i));

      unsigned n = 0;
      for (unsigned n_end = (Nc / max_step_size) * max_step_size; n != n_end; n += max_step_size) {
        sequence.step<max_step_size>();
      }
      for (; n != Nc; ++n) {
        sequence.step();
      }

      table[i] = sequence.get_x2();
    }
  }

  /// \brief Gets the \f$x_2(n)\f$ state register after initialization.
  ///
  /// \param[in] c_init Initial \f$x_2(n)\f$ state as specified by TS38.211 Section 5.2.1.
  /// \return The \f$x_2(n)\f$ state register after initialization.
  uint32_t get_reverse(unsigned c_init) const
  {
    uint32_t ret = 0;
    for (unsigned i = 0; i != pseudo_random_generator_state_size; ++i) {
      if ((c_init >> i) & 1U) {
        ret ^= table[i];
      }
    }

    return ret;
  }

  /// \brief Gets the \f$x_2(n)\f$ state register after initialization.
  ///
  /// \param[in] init_state Initial \f$x_2(n)\f$ state.
  /// \return The \f$x_2(n)\f$ state register after initialization.
  uint32_t get(unsigned init_state) const
  {
    uint32_t ret = 0;
    for (unsigned i = 0; i != pseudo_random_generator_state_size; ++i) {
      if ((init_state << i) & (1U << 31U)) {
        ret ^= table[i];
      }
    }

    return ret;
  }
};

} // namespace srsran