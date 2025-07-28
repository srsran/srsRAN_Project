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

namespace srsran {
namespace fapi {

struct fapi_cell_config;

/// \brief FAPI cell operation request notifier.
///
/// Notifies that FAPI START/STOP request messages have been processed and request a change of operation of the cell.
class cell_operation_request_notifier
{
public:
  virtual ~cell_operation_request_notifier() = default;

  /// \brief Notifies a start request from FAPI.
  ///
  /// This method is synchronous, as such, when this callback returns the start procedure will be completed. Returns
  /// true on start success, otherwise false.
  virtual bool on_start_request(const fapi_cell_config& config) = 0;

  /// \brief Notifies a stop request from FAPI.
  ///
  /// This method is synchronous, so when this function returns the stop process is already finished.
  virtual void on_stop_request() = 0;
};

} // namespace fapi
} // namespace srsran
