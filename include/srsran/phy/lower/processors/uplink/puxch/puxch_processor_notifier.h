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

namespace srsran {

struct lower_phy_rx_symbol_context;
struct resource_grid_context;
class resource_grid_reader;

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
  /// See \ref lower_phy_error_notifier::on_pdxch_request_late for more information.
  /// \param[in] context PUxCH context.
  virtual void on_puxch_request_late(const resource_grid_context& context) = 0;

  /// \brief Notifies the completion of the PUxCH request processing.
  ///
  /// See \ref lower_phy_error_notifier::on_puxch_request_late for more information.
  /// \param[in] grid    Received resource grid.
  /// \param[in] context Received symbol context.
  virtual void on_rx_symbol(const resource_grid_reader& grid, const lower_phy_rx_symbol_context& context) = 0;
};

} // namespace srsran