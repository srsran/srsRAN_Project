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
/// \brief Pseudo-random generator implementation declaration.

#pragma once

#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include <cstdint>
#include <memory>

namespace srsran {

/// SSE-optimized implementation of the pseudo-random generator.
class pseudo_random_generator_impl : public pseudo_random_generator
{
private:
  /// Memory register for sequence \f$x_1(n)\f$.
  uint32_t x1 = 0;
  /// Memory register for sequence \f$x_2(n)\f$.
  uint32_t x2 = 0;

public:
  /// Default constructor.
  pseudo_random_generator_impl() = default;

  // See interface for the documentation.
  void init(unsigned c_init) override;

  // See interface for the documentation.
  void init(const state_s& c_init) override;

  // See interface for the documentation.
  state_s get_state() const override;

  // See interface for the documentation.
  void advance(unsigned count) override;

  // See interface for the documentation.
  void apply_xor(bit_buffer& out, const bit_buffer& in) override;

  // See interface for the documentation.
  void apply_xor(span<uint8_t> out, span<const uint8_t> in) override;

  // See interface for the documentation.
  void apply_xor(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override;

  // See interface for the documentation.
  void generate(bit_buffer& data) override;

  // See interface for the documentation.
  void generate(span<float> buffer, float value) override;
};
} // namespace srsran
