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
#include "srsran/support/resource_usage/scoped_resource_usage.h"

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
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->init(c_init);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::init;
    metrics.nof_bits = 0;
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void init(const state_s& state) override { base_generator->init(state); }

  // See interface for documentation.
  state_s get_state() const override { return base_generator->get_state(); }

  // See interface for documentation.
  void advance(unsigned count) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->advance(count);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::advance;
    metrics.nof_bits = count;
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_xor(bit_buffer& out, const bit_buffer& in) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->apply_xor(out, in);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_packed;
    metrics.nof_bits = in.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_xor(span<uint8_t> out, span<const uint8_t> in) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->apply_xor(out, in);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_unpacked;
    metrics.nof_bits = in.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void apply_xor(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->apply_xor(out, in);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::apply_xor_soft_bit;
    metrics.nof_bits = in.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void generate(bit_buffer& data) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->generate(data);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::generate_bit_packed;
    metrics.nof_bits = data.size();
    notifier.on_new_metric(metrics);
  }

  // See interface for documentation.
  void generate(span<float> buffer, float value) override
  {
    pseudo_random_sequence_generator_metrics metrics;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements);
      base_generator->generate(buffer, value);
    }
    // Report metrics.
    metrics.method   = pseudo_random_sequence_generator_metrics::methods::generate_float;
    metrics.nof_bits = buffer.size();
    notifier.on_new_metric(metrics);
  }

private:
  std::unique_ptr<pseudo_random_generator>          base_generator;
  pseudo_random_sequence_generator_metric_notifier& notifier;
};

} // namespace srsran
