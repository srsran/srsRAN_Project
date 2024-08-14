/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

struct lower_phy_rx_symbol_context;
struct resource_grid_context;
class shared_resource_grid;

/// \brief Lower physical layer PUxCH processor - Notifier interface.
///
/// Notifies a variety of PUxCH-related events such as requests outside the occasion window or overflows of PUxCH
/// requests.
class puxch_processor_notifier
{
public:
  /// Default destructor.
  virtual ~puxch_processor_notifier() = default;

  /// \brief Notifies a PUxCH request outside the slot window.
  ///
  /// See \ref lower_phy_error_notifier::on_puxch_request_late for more information.
  /// \param[in] context PUxCH context.
  virtual void on_puxch_request_late(const resource_grid_context& context) = 0;

  /// \brief Notifies the completion of the PUxCH request processing.
  ///
  /// See \ref lower_phy_error_notifier::on_puxch_request_late for more information.
  /// \param[in] grid    Received resource grid.
  /// \param[in] context Received symbol context.
  virtual void on_rx_symbol(const shared_resource_grid& grid, const lower_phy_rx_symbol_context& context) = 0;
};

} // namespace srsran
