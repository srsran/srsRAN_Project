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

#include "prach_processor_worker.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

void prach_processor_worker::run_state_wait(span<const cf_t>                                samples,
                                            const prach_processor_baseband::symbol_context& context)
{
  // Check if the context slot is in the past even if the sector/port does not match.
  if (context.slot > prach_context.slot ||
      (context.slot == prach_context.slot && context.symbol > prach_context.start_symbol)) {
    // Notify a late PRACH request.
    notifier->on_prach_request_late(prach_context);

    // Transition to idle.
    state = states::idle;

    // Early return
    return;
  }

  // Ignore symbol if the sector and port do not match with the prach_context.
  if (context.sector != prach_context.sector || context.port != prach_context.port) {
    return;
  }

  // The slot and symbol have not reached the beginning of the PRACH window.
  if (context.slot != prach_context.slot || context.symbol != prach_context.start_symbol) {
    return;
  }

  // Reset the number of collected samples.
  nof_samples = 0;

  // Otherwise, transition to process CP.
  state = states::collecting;

  // Accumulate the samples for the first segment.
  accumulate_samples(samples);
}

void prach_processor_worker::run_state_collecting(span<const cf_t>                                samples,
                                                  const prach_processor_baseband::symbol_context& context)
{
  // Ignore symbol if the sector and port do not match with the prach_context.
  if (context.sector != prach_context.sector || context.port != prach_context.port) {
    return;
  }

  // Accumulate samples of the n-th segment.
  accumulate_samples(samples);
}

void prach_processor_worker::accumulate_samples(span<const cf_t> samples)
{
  // Select number of samples to append.
  unsigned count = std::min(window_length - nof_samples, static_cast<unsigned>(samples.size()));

  // Append samples in temporary buffer.
  srsvec::copy(temp_baseband.subspan(nof_samples, count), samples.first(count));

  // Increment number of samples.
  nof_samples += count;

  // Keep collecting if the number of samples has not reached the window length.
  if (nof_samples < window_length) {
    return;
  }

  // Otherwise, transition to processing.
  state = states::processing;

  async_task_executor.execute([this]() {
    ofdm_prach_demodulator::configuration config;
    config.format          = prach_context.format;
    config.rb_offset       = prach_context.rb_offset;
    config.nof_prb_ul_grid = prach_context.nof_prb_ul_grid;
    config.pusch_scs       = prach_context.pusch_scs;

    demodulator->demodulate(*buffer, temp_baseband.first(nof_samples), config);

    notifier->on_rx_prach_window(*buffer, prach_context);

    // Transition to idle.
    state = states::idle;
  });
}

void prach_processor_worker::handle_request(prach_buffer& buffer_, const prach_buffer_context& context_)
{
  srsran_assert(state == states::idle, "Invalid state.");

  prach_context = context_;
  buffer        = &buffer_;

  srsran_assert(prach_context.format.is_long_preamble(), "Only long preamble formats are currently supported.");

  // Get preamble format information.
  prach_preamble_information preamble_info = get_prach_preamble_long_info(prach_context.format);

  // Calculate the PRACH window size.
  window_length = (preamble_info.cp_length + preamble_info.symbol_length).to_samples(sampling_rate_Hz);

  // Reset number of collected samples.
  nof_samples = 0;

  // Transition to wait for the beginning of the PRACH window.
  state = states::wait;
}

void prach_processor_worker::process_symbol(span<const cf_t>                                samples,
                                            const prach_processor_baseband::symbol_context& context)
{
  // Run FSM.
  switch (state) {
    case states::idle:
      // Do nothing.
      break;
    case states::wait:
      run_state_wait(samples, context);
      break;
    case states::collecting:
      run_state_collecting(samples, context);
      break;
    case states::processing:
      // Do nothing.
      break;
  }
}
