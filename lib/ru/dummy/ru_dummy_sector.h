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

#include "ru_dummy_rx_prach_buffer.h"
#include "ru_dummy_rx_resource_grid.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ru/dummy/ru_dummy_metrics.h"
#include "srsran/ru/ru_downlink_plane.h"
#include "srsran/ru/ru_error_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"
#include "srsran/srslog/logger.h"
#include <thread>
#include <utility>
#include <vector>

namespace srsran {

/// Implements a RU dummy sector.
class ru_dummy_sector : public ru_uplink_plane_handler, public ru_downlink_plane_handler
{
  /// Calculates the request buffer size from a downlink data margin.
  static constexpr unsigned get_request_buffer_size(unsigned margin)
  {
    // Start selecting a minimum of 8 slots.
    unsigned ret = std::max(margin, 8U);

    // Ensure 10240 can be divisible by the selected buffer size.
    while (10240 % ret != 0) {
      ++ret;
    }

    return ret;
  }

public:
  /// \brief Creates a dummy RU sector.
  /// \param sector_id          Sector identifier.
  /// \param rx_rg_nof_prb      Receive resource grid number of PRB.
  /// \param rx_rg_nof_ports    Receive resource grid number of ports.
  /// \param rx_prach_nof_ports Receive PRACH buffer number of ports.
  /// \param dl_data_margin_    Downlink data handling margin.
  /// \param logger_            Internal logger, used for notifying real-time failures.
  /// \param symbol_notifier_   Receive symbol notifier.
  /// \param error_notifier_    RU error notifier notifier.
  ru_dummy_sector(unsigned                            sector_id,
                  unsigned                            rx_rg_nof_prb,
                  unsigned                            rx_rg_nof_ports,
                  unsigned                            rx_prach_nof_ports,
                  unsigned                            dl_data_margin_,
                  srslog::basic_logger&               logger_,
                  ru_uplink_plane_rx_symbol_notifier& symbol_notifier_,
                  ru_error_notifier&                  error_notifier_) :
    logger(logger_),
    symbol_notifier(symbol_notifier_),
    error_notifier(error_notifier_),
    rx_symbols_resource_grid(sector_id, rx_rg_nof_prb * NRE, MAX_NSYMB_PER_SLOT, rx_rg_nof_ports),
    rx_symbols_prach_buffer(sector_id, rx_prach_nof_ports),
    dl_data_margin(dl_data_margin_),
    ul_request(get_request_buffer_size(dl_data_margin)),
    prach_request(get_request_buffer_size(dl_data_margin)),
    dl_request(get_request_buffer_size(dl_data_margin))
  {
  }

  ru_dummy_sector(ru_dummy_sector&& other) noexcept :
    logger(other.logger),
    symbol_notifier(other.symbol_notifier),
    error_notifier(other.error_notifier),
    rx_symbols_resource_grid(std::move(other.rx_symbols_resource_grid)),
    rx_symbols_prach_buffer(std::move(other.rx_symbols_prach_buffer)),
    dl_data_margin(other.dl_data_margin),
    ul_request(get_request_buffer_size(dl_data_margin)),
    prach_request(get_request_buffer_size(dl_data_margin)),
    dl_request(get_request_buffer_size(dl_data_margin))
  {
  }

  // See ru_downlink_plane_handler interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    // Set the current slot information.
    std::optional<resource_grid_context> late_context =
        dl_request[context.slot.system_slot() % dl_request.size()].new_request(context, grid.copy());
    total_dl_request_count.fetch_add(1, std::memory_order_relaxed);

    // If the previous slot is valid, it is a late.
    if (late_context.has_value()) {
      error_notifier.on_late_downlink_message({.slot = late_context->slot, .sector = late_context->sector});
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late DL request from slot {} in sector {}.",
                     late_context->slot,
                     late_context->sector);
      late_dl_request_count.fetch_add(1, std::memory_order_relaxed);
    }
  }

  // See ru_uplink_plane_handler interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    std::optional<prach_buffer_context> late_context =
        prach_request[context.slot.system_slot() % prach_request.size()].new_request(context, {});
    total_prach_request_count.fetch_add(1, std::memory_order_relaxed);

    // Detect if there is an unhandled request from a different slot.
    if (late_context.has_value()) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late PRACH request from slot {} in sector {}.",
                     late_context->slot,
                     late_context->sector);
      late_prach_request_count.fetch_add(1, std::memory_order_relaxed);
    }
  }

  // See ru_uplink_plane_handler interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    std::optional<resource_grid_context> late_context =
        ul_request[context.slot.system_slot() % ul_request.size()].new_request(context, grid.copy());
    total_ul_request_count.fetch_add(1, std::memory_order_relaxed);

    // Detect if there is an unhandled request from a different slot.
    if (late_context.has_value()) {
      error_notifier.on_late_uplink_message({.slot = late_context->slot, .sector = late_context->sector});
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late UL request from slot {} in sector {}.",
                     late_context->slot,
                     late_context->sector);
      late_ul_request_count.fetch_add(1, std::memory_order_relaxed);
    }
  }

  /// Notifies a new slot boundary.
  void new_slot_boundary(slot_point slot)
  {
    // Process DL request for this slot.
    slot_point current_dl_slot = slot + dl_data_margin;

    // Obtain current slot information.
    resource_grid_context dl_context = {};
    shared_resource_grid  dl_grid    = {};

    std::tie(dl_context, dl_grid) = dl_request[current_dl_slot.system_slot() % dl_request.size()].pop();

    // Notify with a warning message if the DL previous saved context do not match with the current slot.
    if (dl_context.slot.valid() && (dl_context.slot != current_dl_slot)) {
      error_notifier.on_late_downlink_message({.slot = dl_context.slot, .sector = dl_context.sector});
      logger.warning(current_dl_slot.sfn(),
                     current_dl_slot.slot_index(),
                     "Real-time failure in RU: detected late DL request from slot {} in sector {}.",
                     dl_context.slot,
                     dl_context.sector);
      late_dl_request_count.fetch_add(1, std::memory_order_relaxed);
    }

    // Process UL request for this slot.
    resource_grid_context ul_context = {};
    shared_resource_grid  ul_grid    = {};
    std::tie(ul_context, ul_grid)    = ul_request[slot.system_slot() % ul_request.size()].pop();

    // Check if the UL context from the request list is valid.
    if (ul_context.slot.valid()) {
      if (ul_context.slot == slot) {
        // Prepare receive symbol context.
        ru_uplink_rx_symbol_context rx_context;
        rx_context.slot   = ul_context.slot;
        rx_context.sector = ul_context.sector;

        // Notify received resource grid for each symbol.
        for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
          rx_context.symbol_id = i_symbol;
          symbol_notifier.on_new_uplink_symbol(rx_context, ul_grid, true);
        }

      } else {
        // Notify with a warning message if the UL previous saved context do not match with the current slot.
        error_notifier.on_late_uplink_message({.slot = ul_context.slot, .sector = ul_context.sector});
        logger.warning(slot.sfn(),
                       slot.slot_index(),
                       "Real-time failure in RU: detected late UL request from slot {} in sector {}.",
                       ul_context.slot,
                       ul_context.sector);
        late_ul_request_count.fetch_add(1, std::memory_order_relaxed);
      }
    }

    // Process PRACH request for this slot.
    prach_buffer_context prach_context = prach_request[slot.system_slot() % prach_request.size()].pop().first;

    // Check if the UL context from the request list is valid.
    if (prach_context.slot.valid()) {
      if (prach_context.slot == slot) {
        // Notify received PRACH buffer.
        symbol_notifier.on_new_prach_window_data(prach_context, rx_symbols_prach_buffer.get_buffer(prach_context));
      } else {
        // Notify with a warning message if the UL previous saved context do not match with the current slot.
        logger.warning(slot.sfn(),
                       slot.slot_index(),
                       "Real-time failure in RU: detected late PRACH request from slot {} in sector {}.",
                       prach_context.slot,
                       prach_context.sector);
        late_prach_request_count.fetch_add(1, std::memory_order_relaxed);
      }
    }
  }

  /// Instruct the sector radio unit to stop and wait for the RU to clear all pending request.
  void stop()
  {
    for (auto& req : dl_request) {
      req.stop();
    }
    for (auto& req : ul_request) {
      req.stop();
    }
    for (auto& req : prach_request) {
      req.stop();
    }
  }

  /// Collects the RU dummy sector metrics. It does not reset the metrics.
  void collect_metrics(ru_dummy_sector_metrics& metrics) const
  {
    metrics.total_dl_request_count    = total_dl_request_count.load(std::memory_order_relaxed);
    metrics.late_dl_request_count     = late_dl_request_count.load(std::memory_order_relaxed);
    metrics.total_ul_request_count    = total_ul_request_count.load(std::memory_order_relaxed);
    metrics.late_ul_request_count     = late_ul_request_count.load(std::memory_order_relaxed);
    metrics.total_prach_request_count = total_prach_request_count.load(std::memory_order_relaxed);
    metrics.late_prach_request_count  = late_prach_request_count.load(std::memory_order_relaxed);
  }

private:
  /// Encapsulates transmit and receive requests with their respective context and resource grids.
  template <typename Context>
  class request_information
  {
  public:
    /// \brief Emplace a new request with a new context and grid.
    /// \return \c std::nullptr if there is no previous request, otherwise the context of the previous request.
    std::optional<Context> new_request(const Context& new_context, const shared_resource_grid& new_resource)
    {
      // If the state is stopped return an empty request.
      if (internal_state.load() == internal_states::stopped) {
        return {};
      }

      // Exchange the current internal state.
      internal_states previous_state = internal_state.exchange(internal_states::locked);

      // If the previous state is locked, then this request is late and returns this context.
      if (previous_state == internal_states::locked) {
        return new_context;
      }

      // Save previous context.
      Context prev_context = context;

      // Overwrite contents.
      context  = new_context;
      resource = new_resource.copy();

      // Transition to in-use.
      internal_state.store(internal_states::in_use);

      // If the previous state did not contain a context, then return nullopt.
      if (previous_state == internal_states::available) {
        return std::nullopt;
      }

      // Otherwise return the previous context.
      return prev_context;
    }

    /// Pops the current context and grid.
    std::pair<Context, shared_resource_grid> pop()
    {
      // If the state is stopped return an empty request.
      if (internal_state.load() == internal_states::stopped) {
        return {};
      }

      // Exchange the current internal state.
      internal_states previous_state = internal_state.exchange(internal_states::locked);

      // If the previous state is locked, then consider as there is no request.
      if (previous_state == internal_states::locked) {
        return {};
      }

      // Default context and resource.
      Context              current_context  = {};
      shared_resource_grid current_resource = {};

      // If the previous state is in-use, obtain the context and the resource.
      if (previous_state == internal_states::in_use) {
        current_context  = context;
        current_resource = std::move(resource);
      }

      // Transition to available.
      internal_state.store(internal_states::available);

      // Return the context and resource.
      return {current_context, std::move(current_resource)};
    }

    /// Stops the request information from handling new requests.
    void stop()
    {
      // Waits for the state to be available before transitioning to stopped.
      for (internal_states expected_state = internal_states::available;
           !internal_state.compare_exchange_weak(expected_state, internal_states::stopped);
           expected_state = internal_states::available) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }

  private:
    enum class internal_states : uint32_t { available = 0, locked, in_use, stopped };

    std::atomic<internal_states> internal_state = internal_states::available;
    Context                      context        = {};
    shared_resource_grid         resource;
  };

  /// Logger.
  srslog::basic_logger& logger;
  /// Receive symbol notifier.
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  /// RU error notifier.
  ru_error_notifier& error_notifier;
  /// Receive resource grid.
  ru_dummy_rx_resource_grid rx_symbols_resource_grid;
  /// Receive PRACH buffer.
  ru_dummy_rx_prach_buffer rx_symbols_prach_buffer;
  /// Downlink request margin.
  unsigned dl_data_margin;
  /// Buffer containing the UL requests slots.
  std::vector<request_information<resource_grid_context>> ul_request;
  /// Buffer containing the PRACH requests slots.
  std::vector<request_information<prach_buffer_context>> prach_request;
  /// Circular buffer containing the DL requests indexed by system slot.
  std::vector<request_information<resource_grid_context>> dl_request;
  /// \name  Group of event counters.
  ///
  /// These counters are informative and printed when print_metrics() is called.
  /// @{
  /// Total number of UL receive requests.
  std::atomic<uint64_t> total_ul_request_count = {};
  /// Total number of DL transmit requests.
  std::atomic<uint64_t> total_dl_request_count = {};
  /// Total number of PRACH receive requests.
  std::atomic<uint64_t> total_prach_request_count = {};
  /// Number of late UL receive request.
  std::atomic<uint64_t> late_ul_request_count = {};
  /// Number of late DL transmit request.
  std::atomic<uint64_t> late_dl_request_count = {};
  /// Number of late PRACH receive request.
  std::atomic<uint64_t> late_prach_request_count = {};
  /// @}
};

} // namespace srsran
