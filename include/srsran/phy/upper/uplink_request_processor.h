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

namespace srsran {

struct prach_buffer_context;
class resource_grid;
struct resource_grid_context;

/// \brief Interface of the uplink request processor.
///
/// The uplink request processor is in charge of setting up the PHY block responsible for the execution of an uplink
/// request (e.g., PRACH detection or PUSCH decoding).
class uplink_request_processor
{
public:
  /// Default destructor.
  virtual ~uplink_request_processor() = default;

  /// \brief Processes the PRACH request using the given configuration and context.
  ///
  /// The PRACH detection results will be notified by the \ref upper_phy_rx_results_notifier::on_new_prach_results
  /// "on_new_prach_results" event notification.
  ///
  /// \param[in] context Context used by the underlying PRACH detector.
  virtual void process_prach_request(const prach_buffer_context& context) = 0;

  /// \brief Processes the uplink request using the given context and grid.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid    Grid in which to store the captured data.
  virtual void process_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) = 0;
};

} // namespace srsran
