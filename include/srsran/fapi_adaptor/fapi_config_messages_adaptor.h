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

#include "srsran/srslog/logger.h"

namespace srsran {
namespace fapi {

class config_message_gateway;
class config_message_notifier;
class operation_controller;
class error_message_notifier;

/// \brief FAPI configuration messages adaptor interface.
///
/// The FAPI configuration messages adaptor will adapt configuration messages to FAPI configuration messages. It adapts
/// configuration requests to FAPI configuration request messages and FAPI configuration responses to the adaptor
/// configuration response messages.
class config_messages_adaptor
{
public:
  virtual ~config_messages_adaptor() = default;

  /// Returns the FAPI configuration message notifier of this adaptor.
  virtual config_message_notifier& get_config_message_notifier() = 0;

  /// Returns the FAPI error message notifier of this adaptor.
  virtual error_message_notifier& get_error_message_notifier() = 0;

  /// Returns the operation controller of this adaptor.
  virtual operation_controller& get_operation_controller() = 0;
};

} // namespace fapi
} // namespace srsran
