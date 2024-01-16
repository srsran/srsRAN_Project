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

#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Downlink processor state management class.
///
/// Represents the internal state of the upper PHY downlink processor single executor implementation.
class downlink_processor_single_executor_state
{
public:
  downlink_processor_single_executor_state() : state(states::idle), pending_pdus(0)
  {
    // Do nothing.
  }

  /// Possible states of the downlink proecsosr.
  enum class states {
    /// The processor is ready for a new resource grid.
    idle,
    /// The DL PDUs are accepted and processed.
    processing,
    /// No new DL PDUs are accepted. The resource grid is sent when there are no ongoing processing tasks.
    finishing
  };

  /// Resource grid configured callback. It is meant to be called when a new resource grid has been accepted.
  void on_resource_grid_configured()
  {
    srsran_assert(
        (state == states::idle), "DL processor grid was configured in an invalid state, i.e., {}.", to_string(state));
    srsran_assert((pending_pdus == 0), "DL processor grid was configured with {} pending PDUs.", pending_pdus);
    state = states::processing;
  }

  /// \brief Finish processing PDUs callback.
  ///
  /// \return \c true if the resource grid can be immediately sent, \c false otherwise.
  bool on_finish_requested()
  {
    srsran_assert(
        (state != states::idle), "DL processor finish was requested in an invalid state, i.e., {}.", to_string(state));
    state = states::finishing;
    return (pending_pdus == 0);
  }

  /// Resource grid sent callback.
  void on_grid_sent()
  {
    srsran_assert((pending_pdus == 0), "Grid was sent with {} pending PDUs.", pending_pdus);
    srsran_assert(
        (state == states::finishing), "DL processor grid was sent in an invalid state, i.e., {}.", to_string(state));
    state = states::idle;
  }

  /// Task creation callback. Increases the pending task counter.
  void on_task_creation()
  {
    srsran_assert(
        (state == states::processing), "DL processor task created in an invalid state, i.e., {}.", to_string(state));
    increase_pending_pdus();
  }

  /// Task completion callback. Decreases the pending task counter and checks wether the resource grid can be sent.
  ///
  /// \return \c true if the resource grid can be immediately sent, \c false otherwise.
  bool on_task_completion()
  {
    decrease_pending_pdus();
    return (state == states::finishing) && (pending_pdus == 0);
  }

  /// Checks wether the current state allows for a new resource grid to be configured.
  bool is_idle() const { return state == states::idle; }

  /// Checks wether the current state allows for a new task to be enqueued.
  bool is_processing() const { return state == states::processing; }

private:
  /// Increases the pending PDU counter.
  void increase_pending_pdus() { ++pending_pdus; }

  /// Decreases the pending PDU counter.
  void decrease_pending_pdus()
  {
    if (pending_pdus > 0) {
      --pending_pdus;
    }
  }

  static std::string to_string(states state_)
  {
    switch (state_) {
      case states::idle:
        return "idle";
      case states::processing:
        return "processing";
      case states::finishing:
      default:
        return "finishing";
    }
  }

  /// Downlink processor current state.
  states state;
  /// Number of pending PDUs to process before sending the resource grid.
  unsigned pending_pdus;
};
} // namespace srsran
