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

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsran {

/// Pseudo-random sequence generator metric decorator.
class phy_metrics_pseudo_random_generator_decorator : public pseudo_random_generator
{
public:
  phy_metrics_pseudo_random_generator_decorator(std::unique_ptr<pseudo_random_generator>          base_generator_,
                                                pseudo_random_sequence_generator_metric_notifier& notifier_) :
    base_generator(std::move(base_generator_)), notifier(notifier_)
  {
    srsran_assert(base_generator, "Invaluid base generator.");
  }

  // See interface for documentation.
  void init(unsigned c_init) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_generator->init(c_init);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.method   = pseudo_random_sequence_generator_metrics::methods::init,
                         .nof_bits = 0,
                         .elapsed  = tp_after - tp_before});
  }

  // See interface for documentation.
  void init(const state_s& state) override { base_generator->init(state); }

  // See interface for documentation.
  state_s get_state() const override { return base_generator->get_state(); }

  // See interface for documentation.
  void advance(unsigned count) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_generator->advance(count);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.method   = pseudo_random_sequence_generator_metrics::methods::advance,
                         .nof_bits = count,
                         .elapsed  = tp_after - tp_before});
  }

  // See interface for documentation.
  void apply_xor(bit_buffer& out, const bit_buffer& in) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_generator->apply_xor(out, in);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_packed,
                         .nof_bits = in.size(),
                         .elapsed  = tp_after - tp_before});
  }

  // See interface for documentation.
  void apply_xor(span<uint8_t> out, span<const uint8_t> in) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_generator->apply_xor(out, in);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_unpacked,
                         .nof_bits = in.size(),
                         .elapsed  = tp_after - tp_before});
  }

  // See interface for documentation.
  void apply_xor(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_generator->apply_xor(out, in);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_soft_bit,
                         .nof_bits = in.size(),
                         .elapsed  = tp_after - tp_before});
  }

  // See interface for documentation.
  void generate(bit_buffer& data) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_generator->generate(data);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.method   = pseudo_random_sequence_generator_metrics::methods::generate_bit_packed,
                         .nof_bits = data.size(),
                         .elapsed  = tp_after - tp_before});
  }

  // See interface for documentation.
  void generate(span<float> buffer, float value) override
  {
    auto tp_before = std::chrono::high_resolution_clock::now();
    base_generator->generate(buffer, value);
    auto tp_after = std::chrono::high_resolution_clock::now();

    notifier.new_metric({.method   = pseudo_random_sequence_generator_metrics::methods::generate_float,
                         .nof_bits = buffer.size(),
                         .elapsed  = tp_after - tp_before});
  }

private:
  std::unique_ptr<pseudo_random_generator>          base_generator;
  pseudo_random_sequence_generator_metric_notifier& notifier;
};

} // namespace srsran