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

class puxch_processor_notifier;
class puxch_processor_request_handler;
class puxch_processor_baseband;
class slot_point;

/// \brief PUCCH and PUSCH processor main interface.
///
/// Provides a single access point to all PUCCH- and PUSCH-related interfaces, that is \ref puxch_processor_notifier,
/// \ref puxch_processor_request_handler and \ref puxch_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the PUxCH processor is not connected
/// to a notifier before calling \ref get_request_handler() or \ref get_baseband().
class puxch_processor
{
public:
  /// Default destructor.
  virtual ~puxch_processor() = default;

  /// Connects the PUxCH processor with a notifier.
  virtual void connect(puxch_processor_notifier& notifier) = 0;

  /// \brief Gets the PUxCH processor request handler.
  /// \return A reference to the internal PUxCH processor request handler.
  virtual puxch_processor_request_handler& get_request_handler() = 0;

  /// \brief Gets the PUxCH processor baseband interface.
  /// \return A reference to the internal PUxCH processor baseband interface.
  virtual puxch_processor_baseband& get_baseband() = 0;
};

} // namespace srsran