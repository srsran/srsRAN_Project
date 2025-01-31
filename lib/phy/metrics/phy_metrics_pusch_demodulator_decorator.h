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

#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"

namespace srsran {

/// PUSCH demodulator metric decorator.
class phy_metrics_pusch_demodulator_decorator : public pusch_demodulator, private pusch_codeword_buffer
{
public:
  /// Creates a PUSCH demodulator decorator from a base instance and a notifier.
  phy_metrics_pusch_demodulator_decorator(std::unique_ptr<pusch_demodulator> base_,
                                          pusch_demodulator_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid PUSCH demodulator.");
  }

  void demodulate(pusch_codeword_buffer&      codeword_buffer,
                  pusch_demodulator_notifier& demodulator_notifier,
                  const resource_grid_reader& grid,
                  const channel_estimate&     estimates,
                  const configuration&        config) override
  {
    base_buffer             = &codeword_buffer;
    elapsed_on_new_block    = {};
    elapsed_on_end_codeword = {};

    // Prepare base and save the base buffer.
    auto tp_before = std::chrono::high_resolution_clock::now();
    base->demodulate(*this, demodulator_notifier, grid, estimates, config);
    auto tp_after = std::chrono::high_resolution_clock::now();

    // Notify metrics.
    notifier.new_metric(
        {.elapsed = tp_after - tp_before, .elapsed_buffer = elapsed_on_new_block + elapsed_on_end_codeword});
  }

private:
  // See pusch_codeword_buffer interface for documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    srsran_assert(base_buffer != nullptr, "Invalid base buffer.");

    // Assume this method takes a small amount of time.
    return base_buffer->get_next_block_view(block_size);
  }

  // See pusch_codeword_buffer interface for documentation.
  void on_new_block(span<const log_likelihood_ratio> data, const bit_buffer& scrambling_seq) override
  {
    srsran_assert(base_buffer != nullptr, "Invalid base buffer.");

    auto tp_before = std::chrono::high_resolution_clock::now();
    base_buffer->on_new_block(data, scrambling_seq);
    auto tp_after = std::chrono::high_resolution_clock::now();

    // Accumulate elapsed time.
    elapsed_on_new_block += tp_after - tp_before;
  }

  // See pusch_codeword_buffer interface for documentation.
  void on_end_codeword() override
  {
    srsran_assert(base_buffer != nullptr, "Invalid base buffer.");

    auto tp_before = std::chrono::high_resolution_clock::now();
    base_buffer->on_end_codeword();
    auto tp_after = std::chrono::high_resolution_clock::now();

    elapsed_on_end_codeword = tp_after - tp_before;
  }

  std::unique_ptr<pusch_demodulator> base;
  pusch_demodulator_metric_notifier& notifier;
  pusch_codeword_buffer*             base_buffer             = nullptr;
  std::chrono::nanoseconds           elapsed_on_new_block    = {};
  std::chrono::nanoseconds           elapsed_on_end_codeword = {};
};

} // namespace srsran
