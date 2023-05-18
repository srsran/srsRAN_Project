/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/upper_phy_rg_gateway.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_handler.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "srsran/phy/upper/upper_phy_timing_handler.h"
#include "srsran/ru/ru_downlink_plane.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// Upper PHY RU downlink adapter.
class upper_ru_dl_rg_adapter : public upper_phy_rg_gateway
{
public:
  // See interface for documentation.
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override;

  /// Connects this adapter with the given RU downlink handler.
  void connect(ru_downlink_plane_handler& handler) { dl_handler = &handler; }

private:
  ru_downlink_plane_handler* dl_handler = nullptr;
};

/// Upper PHY RU uplink request adapter.
class upper_ru_ul_request_adapter : public upper_phy_rx_symbol_request_notifier
{
public:
  // See interface for documentation.
  void on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void on_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) override;

  /// Connects this adapter with the given RU uplink handler.
  void connect(ru_uplink_plane_handler& handler) { ul_handler = &handler; }

private:
  ru_uplink_plane_handler* ul_handler = nullptr;
};

/// Upper PHY RU uplink adapter.
class upper_ru_ul_adapter : public ru_uplink_plane_rx_symbol_notifier
{
public:
  // See interface for documentation.
  void on_new_uplink_symbol(const ru_uplink_rx_symbol_context& context, const resource_grid_reader& grid) override;

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override;

  /// Connects this adapter with the given upper PHY received symbol handler.
  void connect(upper_phy_rx_symbol_handler& hndlr) { handler = &hndlr; }

private:
  upper_phy_rx_symbol_handler* handler = nullptr;
};

/// Upper PHY RU timing adapter.
class upper_ru_timing_adapter : public ru_timing_notifier
{
public:
  // See interface for documentation.
  void on_tti_boundary(slot_point slot) override;

  /// Connects this adapter with the given upper PHY timing handler.
  void connect(upper_phy_timing_handler& hndlr) { handler = &hndlr; }

private:
  upper_phy_timing_handler* handler = nullptr;
};

} // namespace srsran