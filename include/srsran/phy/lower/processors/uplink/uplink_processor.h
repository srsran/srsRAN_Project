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

class uplink_processor_notifier;
class uplink_processor_baseband;
class prach_processor_request_handler;
class puxch_processor_request_handler;
class prach_processor_notifier;
class puxch_processor_notifier;

/// \brief Uplink processor main interface.
///
/// Provides a single access point to all uplink-related interfaces, that is uplink_processor_notifier,
/// uplink_processor_request_handler and uplink_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the uplink processor is not connected
/// to a notifier before calling get_prach_request_handler(), get_puxch_request_handler() or get_baseband().
class lower_phy_uplink_processor
{
public:
  /// Default destructor.
  virtual ~lower_phy_uplink_processor() = default;

  /// Connects the uplink processor with notifiers.
  virtual void connect(uplink_processor_notifier& notifier,
                       prach_processor_notifier&  prach_notifier,
                       puxch_processor_notifier&  puxch_notifier) = 0;

  /// Gets the PRACH request handler.
  virtual prach_processor_request_handler& get_prach_request_handler() = 0;

  /// Gets the PUxCH request handler.
  virtual puxch_processor_request_handler& get_puxch_request_handler() = 0;

  /// \brief Gets the uplink processor baseband interface.
  /// \return A reference to the internal uplink processor baseband interface.
  virtual uplink_processor_baseband& get_baseband() = 0;
};

} // namespace srsran