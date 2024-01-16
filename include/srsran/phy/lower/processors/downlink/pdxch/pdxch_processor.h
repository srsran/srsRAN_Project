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

class pdxch_processor_notifier;
class pdxch_processor_request_handler;
class pdxch_processor_baseband;
class slot_point;

/// \brief PDxCH processor main interface.
///
/// Provides a single access point to all downlink channel processor interfaces, that is \ref pdxch_processor_notifier,
/// \ref pdxch_processor_request_handler and \ref pdxch_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the PDxCH processor is not connected
/// to a notifier before calling \ref get_request_handler() or \ref get_baseband().
class pdxch_processor
{
public:
  /// Default destructor.
  virtual ~pdxch_processor() = default;

  /// Connects the PDxCH processor with a notifier.
  virtual void connect(pdxch_processor_notifier& notifier) = 0;

  /// \brief Gets the PDxCH processor request handler.
  /// \return A reference to the internal PDxCH processor request handler.
  virtual pdxch_processor_request_handler& get_request_handler() = 0;

  /// \brief Gets the PDxCH processor baseband interface.
  /// \return A reference to the internal PDxCH processor baseband interface.
  virtual pdxch_processor_baseband& get_baseband() = 0;
};

} // namespace srsran