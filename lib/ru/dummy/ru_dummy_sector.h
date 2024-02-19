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

#include "ru_dummy_rx_prach_buffer.h"
#include "ru_dummy_rx_resource_grid.h"
#include "srsran/adt/circular_array.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ru/ru_downlink_plane.h"
#include "srsran/ru/ru_uplink_plane.h"
#include "srsran/srslog/logger.h"
#include <mutex>
#include <utility>

namespace srsran {

/// Implements a RU dummy sector.
class ru_dummy_sector : public ru_uplink_plane_handler, public ru_downlink_plane_handler
{
public:
  /// \brief Creates a dummy RU sector.
  /// \param sector_id          Sector identifier.
  /// \param rx_rg_nof_prb      Receive resource grid number of PRB.
  /// \param rx_rg_nof_ports    Receive resource grid number of ports.
  /// \param rx_prach_nof_ports Receive PRACH buffer number of ports.
  /// \param dl_data_margin_    Downlink data handling margin.
  /// \param logger_            Internal logger, used for notifying real-time failures.
  /// \param symbol_notifier_   Receive symbol notifier.
  ru_dummy_sector(unsigned                            sector_id,
                  unsigned                            rx_rg_nof_prb,
                  unsigned                            rx_rg_nof_ports,
                  unsigned                            rx_prach_nof_ports,
                  unsigned                            dl_data_margin_,
                  srslog::basic_logger&               logger_,
                  ru_uplink_plane_rx_symbol_notifier& symbol_notifier_) :
    logger(logger_),
    symbol_notifier(symbol_notifier_),
    rx_symbols_resource_grid(sector_id, rx_rg_nof_prb * NRE, MAX_NSYMB_PER_SLOT, rx_rg_nof_ports),
    rx_symbols_prach_buffer(sector_id, rx_prach_nof_ports),
    dl_data_margin(dl_data_margin_)
  {
  }

  ru_dummy_sector(ru_dummy_sector&& other) noexcept :
    logger(other.logger),
    symbol_notifier(other.symbol_notifier),
    rx_symbols_resource_grid(std::move(other.rx_symbols_resource_grid)),
    rx_symbols_prach_buffer(std::move(other.rx_symbols_prach_buffer)),
    dl_data_margin(other.dl_data_margin)
  {
  }

  // See ru_downlink_plane_handler interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    std::lock_guard<std::mutex> lock(dl_request_mutex);
    resource_grid_context       prev_context = std::exchange(dl_request[context.slot.system_slot()], context);

    // If the previous slot is valid, it is a late.
    if (prev_context.slot.valid()) {
      logger.warning("Real-time failure in RU: received late DL request from slot {} in sector {}.",
                     prev_context.slot,
                     prev_context.sector);
    }
  }

  // See ru_uplink_plane_handler interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    std::lock_guard<std::mutex> lock(prach_request_mutex);
    prach_buffer_context        prev_context = std::exchange(prach_request[context.slot.system_slot()], context);

    // Detect if there is an unhandled request from a different slot.
    if (prev_context.slot.valid()) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late PRACH request from slot {} in sector {}.",
                     prev_context.slot,
                     prev_context.sector);
    }
  }

  // See ru_uplink_plane_handler interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid) override
  {
    std::lock_guard<std::mutex> lock(ul_request_mutex);
    resource_grid_context       prev_context = std::exchange(ul_request[context.slot.system_slot()], context);

    // Detect if there is an unhandled request from a different slot.
    if (prev_context.slot.valid()) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "Real-time failure in RU: received late UL request from slot {} in sector {}.",
                     prev_context.slot,
                     prev_context.sector);
    }
  }

  /// Notifies a new slot boundary.
  void new_slot_boundary(slot_point slot)
  {
    // Process DL request for this slot.
    slot_point current_dl_slot = slot + dl_data_margin;

    resource_grid_context context = {slot_point(), 0};
    {
      std::lock_guard<std::mutex> lock(dl_request_mutex);
      context = std::exchange(dl_request[current_dl_slot.system_slot()], context);
    }

    // Notify with a warning message if the DL previous saved context do not match with the current slot.
    if (context.slot.valid() && (context.slot != current_dl_slot)) {
      logger.warning(current_dl_slot.sfn(),
                     current_dl_slot.slot_index(),
                     "Real-time failure in RU: detected late DL request from slot {} in sector {}.",
                     context.slot,
                     context.sector);
    }

    // Process UL request for this slot.
    resource_grid_context ul_context = {slot_point(), 0};
    {
      std::lock_guard<std::mutex> lock(ul_request_mutex);
      ul_context = std::exchange(ul_request[slot.system_slot()], ul_context);
    }

    // Check if the UL context from the request list is valid.
    if (ul_context.slot.valid()) {
      if (ul_context.slot == slot) {
        // Prepare receive symbol context.
        ru_uplink_rx_symbol_context rx_context;
        rx_context.slot      = ul_context.slot;
        rx_context.sector    = ul_context.sector;
        rx_context.symbol_id = MAX_NSYMB_PER_SLOT - 1;

        // Notify received resource grid.
        symbol_notifier.on_new_uplink_symbol(rx_context, rx_symbols_resource_grid);
      } else {
        // Notify with a warning message if the UL previous saved context do not match with the current slot.
        logger.warning(slot.sfn(),
                       slot.slot_index(),
                       "Real-time failure in RU: detected late UL request from slot {} in sector {}.",
                       ul_context.slot,
                       ul_context.sector);
      }
    }

    // Process PRACH request for this slot.
    prach_buffer_context prach_context{};
    {
      std::lock_guard<std::mutex> lock(prach_request_mutex);
      prach_context = std::exchange(prach_request[slot.system_slot()], prach_context);
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
      }
    }
  }

private:
  /// Maximum number of requests stored in the ring buffers.
  static constexpr unsigned max_nof_request = 40;

  /// Logger.
  srslog::basic_logger& logger;
  /// Receive symbol notifier.
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  /// Receive resource grid.
  ru_dummy_rx_resource_grid rx_symbols_resource_grid;
  /// Receive PRACH buffer.
  ru_dummy_rx_prach_buffer rx_symbols_prach_buffer;
  /// Downlink request margin.
  unsigned dl_data_margin;
  /// Buffer containing the UL requests slots.
  circular_array<resource_grid_context, max_nof_request> ul_request;
  /// Protects the circular buffer containing the UL requests.
  std::mutex ul_request_mutex;
  /// Buffer containing the PRACH requests slots.
  circular_array<prach_buffer_context, max_nof_request> prach_request;
  /// Protects the circular buffer containing the PRACH requests.
  std::mutex prach_request_mutex;
  /// Circular buffer containing the DL requests indexed by system slot.
  circular_array<resource_grid_context, max_nof_request> dl_request;
  /// Protects the circular buffer containing the DL requests.
  std::mutex dl_request_mutex;
};

} // namespace srsran
