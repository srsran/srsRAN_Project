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

class prach_processor_notifier;
class prach_processor_request_handler;
class prach_processor_baseband;

/// \brief PRACH processor main interface.
///
/// Provides a single access point to all PRACH-related interfaces, that is \ref prach_processor_notifier,
/// \ref prach_processor_request_handler and \ref prach_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the PRACH processor is not connected
/// to a notifier before calling \ref get_request_handler() or \ref get_baseband().
class prach_processor
{
public:
  /// Default destructor.
  virtual ~prach_processor() = default;

  /// Connects the PRACH processor with a notifier.
  virtual void connect(prach_processor_notifier& notifier) = 0;

  /// \brief Gets the PRACH processor request handler.
  /// \return A reference to the internal PRACH processor request handler.
  virtual prach_processor_request_handler& get_request_handler() = 0;

  /// \brief Gets the PRACH processor baseband interface.
  /// \return A reference to the internal PRACH processor baseband interface.
  virtual prach_processor_baseband& get_baseband() = 0;
};

} // namespace srsran