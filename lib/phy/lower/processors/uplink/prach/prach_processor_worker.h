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

#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_notifier.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/srsvec/aligned_vec.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// \brief Lower PHY PRACH processor worker subcomponent.
///
/// The worker contains a finite-state machine with four possible states: idle, wait, collecting and processing. See
/// \ref states for more information.
///
/// The implementation assumes that:
/// - handle_request() and is_available() are called from a first thread, and
/// - process_symbol() is called from a second, different thread.
class prach_processor_worker
{
private:
  /// Worker internal states.
  enum class states {
    /// The worker has not been configured yet with any request or the previously configured request has already been
    /// processed. It transitions to \c wait when a request is set.
    idle,
    /// A PRACH request is configured and the worker is waiting for the start of the PRACH occasion window. The worker
    /// transitions to \c collecting if it detects the beginning of the PRACH occasion window for the configured PRACH
    /// context. When in this state, the worker also detects late requests, in which case it notifies the event and
    /// transitions back to \c idle.
    wait,
    /// The PRACH occasion window started and baseband samples are collected. Once the internal buffer has been filled
    /// with the PRACH occasion window samples, the worker transitions to \c processing for the demodulation phase.
    collecting,
    /// The PRACH occasion has been captured and it is processed by the asynchronous task executor. The worker
    /// transitions to \c idle as soon as the PRACH occasion is processed and notified.
    processing
  };

  /// OFDM PRACH demodulator.
  std::unique_ptr<ofdm_prach_demodulator> demodulator;
  /// Asynchronous task executor.
  task_executor& async_task_executor;
  /// Sampling rate in Hz.
  unsigned sampling_rate_Hz = 0;
  /// Temporary baseband buffer of size \ref TEMP_BASEBAND_BUFFER_SIZE.
  baseband_gateway_buffer_dynamic temp_baseband;
  /// PRACH processor notifier.
  prach_processor_notifier* notifier = nullptr;
  /// Current worker state.
  std::atomic<states> state = {states::idle};
  /// Current context.
  prach_buffer_context prach_context;
  /// Current PRACH buffer.
  prach_buffer* buffer = nullptr;
  /// Current PRACH occasion window length.
  unsigned window_length = 0;
  /// Current number of collected samples.
  unsigned nof_samples = 0;

  /// Runs state \c wait.
  void run_state_wait(const baseband_gateway_buffer_reader&           samples,
                      const prach_processor_baseband::symbol_context& context);

  /// Runs state \c collecting.
  void run_state_collecting(const baseband_gateway_buffer_reader&           samples,
                            const prach_processor_baseband::symbol_context& context);

  /// Accumulates \c samples in the internal buffer.
  void accumulate_samples(const baseband_gateway_buffer_reader& samples);

public:
  /// Creates a PRACH processor worker.
  prach_processor_worker(std::unique_ptr<ofdm_prach_demodulator> demodulator_,
                         task_executor&                          async_task_executor_,
                         sampling_rate                           srate,
                         unsigned                                max_nof_ports) :
    demodulator(std::move(demodulator_)),
    async_task_executor(async_task_executor_),
    sampling_rate_Hz(srate.to_Hz()),
    temp_baseband(max_nof_ports, prach_constants::MAX_WINDOW_LENGTH.to_samples(sampling_rate_Hz))
  {
    srsran_assert(sampling_rate_Hz && prach_constants::MAX_WINDOW_LENGTH.is_sample_accurate(sampling_rate_Hz),
                  "Invalid sampling rate of {} Hz.",
                  sampling_rate_Hz);
  }

  /// Connects the worker with the given PRACH processor notifier.
  void connect(prach_processor_notifier& notifier_) { notifier = &notifier_; }

  /// \brief Handles a PRACH occasion request.
  /// \param[in] buffer  PRACH buffer.
  /// \param[in] context PRACH occasion context.
  void handle_request(prach_buffer& buffer, const prach_buffer_context& context);

  /// \brief Processes an OFDM symbol.
  /// \param[in] samples Baseband samples.
  /// \param[in] context OFDM symbol context.
  void process_symbol(const baseband_gateway_buffer_reader&           samples,
                      const prach_processor_baseband::symbol_context& context);

  /// \brief Determines whether the PRACH processor worker is available.
  ///
  /// A PRACH processor is available when it is \c idle. See \ref states for more information regarding the PRACH worker
  /// internal states.
  bool is_available() const { return state == states::idle; }
};

} // namespace srsran
