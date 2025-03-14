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
#include "srsran/ru/ru_downlink_plane.h"
#include "srsran/ru/ru_error_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"
#include "srsran/srslog/logger.h"
#include <mutex>
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
    std::lock_guard<std::mutex> lock(dl_request_mutex);
    request_information         info = {context, grid.copy()};
    std::swap(info, dl_request[context.slot.system_slot() % dl_request.size()]);
    ++total_dl_request_count;

    // If the previous slot is valid, it is a late.
    if (info.context.slot.valid()) {
      error_notifier.on_late_downlink_message({.slot = info.context.slot, .sector = info.context.sector});
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late DL request from slot {} in sector {}.",
                     info.context.slot,
                     info.context.sector);
      ++late_dl_request_count;
    }
  }

  // See ru_uplink_plane_handler interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    std::lock_guard<std::mutex> lock(prach_request_mutex);
    prach_buffer_context        info =
        std::exchange(prach_request[context.slot.system_slot() % prach_request.size()], context);
    ++total_prach_request_count;

    // Detect if there is an unhandled request from a different slot.
    if (info.slot.valid()) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late PRACH request from slot {} in sector {}.",
                     info.slot,
                     info.sector);
      ++late_prach_request_count;
    }
  }

  // See ru_uplink_plane_handler interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    std::lock_guard<std::mutex> lock(ul_request_mutex);
    request_information         info = {context, grid.copy()};
    std::swap(info, ul_request[context.slot.system_slot() % ul_request.size()]);
    ++total_ul_request_count;

    // Detect if there is an unhandled request from a different slot.
    if (info.context.slot.valid()) {
      error_notifier.on_late_uplink_message({.slot = info.context.slot, .sector = info.context.sector});
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late UL request from slot {} in sector {}.",
                     info.context.slot,
                     info.context.sector);
      ++late_ul_request_count;
    }
  }

  /// Notifies a new slot boundary.
  void new_slot_boundary(slot_point slot)
  {
    // Process DL request for this slot.
    slot_point current_dl_slot = slot + dl_data_margin;

    request_information dl_info = {};
    {
      std::lock_guard<std::mutex> lock(dl_request_mutex);
      std::swap(dl_info, dl_request[current_dl_slot.system_slot() % dl_request.size()]);
    }

    // Notify with a warning message if the DL previous saved context do not match with the current slot.
    if (dl_info.context.slot.valid() && (dl_info.context.slot != current_dl_slot)) {
      error_notifier.on_late_downlink_message({.slot = dl_info.context.slot, .sector = dl_info.context.sector});
      logger.warning(current_dl_slot.sfn(),
                     current_dl_slot.slot_index(),
                     "Real-time failure in RU: detected late DL request from slot {} in sector {}.",
                     dl_info.context.slot,
                     dl_info.context.sector);
      ++late_dl_request_count;
    }

    // Process UL request for this slot.
    request_information ul_info = {};
    {
      std::lock_guard<std::mutex> lock(ul_request_mutex);
      std::swap(ul_info, ul_request[slot.system_slot() % ul_request.size()]);
    }

    // Check if the UL context from the request list is valid.
    if (ul_info.context.slot.valid()) {
      if (ul_info.context.slot == slot) {
        // Prepare receive symbol context.
        ru_uplink_rx_symbol_context rx_context;
        rx_context.slot   = ul_info.context.slot;
        rx_context.sector = ul_info.context.sector;

        // Notify received resource grid for each symbol.
        for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
          rx_context.symbol_id = i_symbol;
          symbol_notifier.on_new_uplink_symbol(rx_context, ul_info.grid);
        }

      } else {
        // Notify with a warning message if the UL previous saved context do not match with the current slot.
        error_notifier.on_late_uplink_message({.slot = ul_info.context.slot, .sector = ul_info.context.sector});
        logger.warning(slot.sfn(),
                       slot.slot_index(),
                       "Real-time failure in RU: detected late UL request from slot {} in sector {}.",
                       ul_info.context.slot,
                       ul_info.context.sector);
        ++late_ul_request_count;
      }
    }

    // Process PRACH request for this slot.
    prach_buffer_context prach_context{};
    {
      std::lock_guard<std::mutex> lock(prach_request_mutex);
      prach_context = std::exchange(prach_request[slot.system_slot() % prach_request.size()], prach_context);
    }

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
        ++late_prach_request_count;
      }
    }
  }

  /// Prints in \c stdout the current gathered metrics. It does not reset the metrics.
  void print_metrics() const
  {
    fmt::println("| {:^11} | {:^11} | {:^11} | {:^11} | {:^11} | {:^11} |",
                 total_dl_request_count.load(),
                 late_dl_request_count.load(),
                 total_ul_request_count.load(),
                 late_ul_request_count.load(),
                 total_prach_request_count.load(),
                 late_prach_request_count.load());
  }

private:
  struct request_information {
    resource_grid_context context;
    shared_resource_grid  grid;
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
  std::vector<request_information> ul_request;
  /// Protects the circular buffer containing the UL requests.
  std::mutex ul_request_mutex;
  /// Buffer containing the PRACH requests slots.
  std::vector<prach_buffer_context> prach_request;
  /// Protects the circular buffer containing the PRACH requests.
  std::mutex prach_request_mutex;
  /// Circular buffer containing the DL requests indexed by system slot.
  std::vector<request_information> dl_request;
  /// Protects the circular buffer containing the DL requests.
  std::mutex dl_request_mutex;
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
