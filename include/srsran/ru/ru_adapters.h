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

#pragma once

#include "ru_downlink_plane.h"
#include "ru_timing_notifier.h"
#include "ru_uplink_plane.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_handler.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "srsran/phy/upper/upper_phy_timing_handler.h"

namespace srsran {

/// Upper PHY - Radio Unit downlink adapter.
class upper_ru_dl_rg_adapter : public upper_phy_rg_gateway
{
public:
  // See interface for documentation.
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    srsran_assert(dl_handler, "Adapter is not connected.");
    dl_handler->handle_dl_data(context, grid);
  }

  /// Connects this adapter with the given RU downlink handler.
  void connect(ru_downlink_plane_handler& handler) { dl_handler = &handler; }

private:
  ru_downlink_plane_handler* dl_handler = nullptr;
};

/// Upper PHY - Radio Unit uplink request adapter.
class upper_ru_ul_request_adapter : public upper_phy_rx_symbol_request_notifier
{
public:
  // See interface for documentation.
  void on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    srsran_assert(ul_handler, "Adapter is not connected.");
    ul_handler->handle_prach_occasion(context, buffer);
  }

  // See interface for documentation.
  void on_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) override
  {
    srsran_assert(ul_handler, "Adapter is not connected.");
    ul_handler->handle_new_uplink_slot(context, grid);
  }

  /// Connects this adapter with the given RU uplink handler.
  void connect(ru_uplink_plane_handler& handler) { ul_handler = &handler; }

private:
  ru_uplink_plane_handler* ul_handler = nullptr;
};

/// Upper PHY - Radio Unit uplink adapter.
class upper_ru_ul_adapter : public ru_uplink_plane_rx_symbol_notifier
{
public:
  explicit upper_ru_ul_adapter(unsigned nof_sectors) : handlers(nof_sectors) {}

  // See interface for documentation.
  void on_new_uplink_symbol(const ru_uplink_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    srsran_assert(context.sector < handlers.size(), "Unsupported sector {}", context.sector);
    handlers[context.sector]->handle_rx_symbol({context.sector, context.slot, context.symbol_id}, grid);
  }

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    srsran_assert(context.sector < handlers.size(), "Unsupported sector {}", context.sector);
    handlers[context.sector]->handle_rx_prach_window(context, buffer);
  }

  // See interface for documentation.
  void on_rx_srs_symbol(const ru_uplink_rx_symbol_context& context) override
  {
    srsran_assert(context.sector < handlers.size(), "Unsupported sector {}", context.sector);
    handlers[context.sector]->handle_rx_srs_symbol({context.sector, context.slot, context.symbol_id});
  }

  /// Maps the given upper PHY received symbol handler and sector to this adapter.
  void map_handler(unsigned sector, upper_phy_rx_symbol_handler& hndlr)
  {
    srsran_assert(sector < handlers.size(), "Unsupported sector {}", sector);

    handlers[sector] = &hndlr;
  }

private:
  std::vector<upper_phy_rx_symbol_handler*> handlers;
};

/// Upper PHY - Radio Unit timing adapter.
class upper_ru_timing_adapter : public ru_timing_notifier
{
public:
  explicit upper_ru_timing_adapter(unsigned nof_sectors) : handlers(nof_sectors) {}

  // See interface for documentation.
  void on_tti_boundary(slot_point slot) override
  {
    srsran_assert(!handlers.empty(), "Adapter is not connected.");
    for (auto& handler : handlers) {
      handler->handle_tti_boundary({slot});
    }
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(slot_point slot) override
  {
    srsran_assert(!handlers.empty(), "Adapter is not connected.");
    for (auto& handler : handlers) {
      handler->handle_ul_half_slot_boundary({slot});
    }
  }

  // See interface for documentation.
  void on_ul_full_slot_boundary(slot_point slot) override
  {
    srsran_assert(!handlers.empty(), "Adapter is not connected.");
    for (auto& handler : handlers) {
      handler->handle_ul_full_slot_boundary({slot});
    }
  }

  /// Maps the given upper PHY timing handler and sector to this adapter.
  void map_handler(unsigned sector, upper_phy_timing_handler& hndlr)
  {
    srsran_assert(sector < handlers.size(), "Unsupported sector {}", sector);

    handlers[sector] = &hndlr;
  }

private:
  std::vector<upper_phy_timing_handler*> handlers;
};

} // namespace srsran