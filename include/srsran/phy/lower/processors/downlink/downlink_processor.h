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

class downlink_processor_notifier;
class downlink_processor_baseband;
class pdxch_processor_request_handler;
class downlink_processor_notifier;
class pdxch_processor_notifier;

/// \brief Downlink processor main interface.
///
/// Provides a single access point to all downlink-related interfaces, that is \ref downlink_processor_notifier,
/// \ref downlink_processor_request_handler and \ref downlink_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the downlink processor is not
/// connected to a notifier before calling \ref get_downlink_request_handler() or \ref get_baseband().
class lower_phy_downlink_processor
{
public:
  /// Default destructor.
  virtual ~lower_phy_downlink_processor() = default;

  /// Connects the downlink processor with notifiers.
  virtual void connect(downlink_processor_notifier& notifier, pdxch_processor_notifier& pdxch_notifier) = 0;

  /// Gets the downlink processor request handler.
  virtual pdxch_processor_request_handler& get_downlink_request_handler() = 0;

  /// \brief Gets the downlink processor baseband interface.
  /// \return A reference to the internal downlink processor baseband interface.
  virtual downlink_processor_baseband& get_baseband() = 0;
};

} // namespace srsran