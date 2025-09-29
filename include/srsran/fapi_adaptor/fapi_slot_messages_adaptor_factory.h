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

#include "srsran/fapi_adaptor/fapi_slot_messages_adaptor.h"
#include <memory>

namespace srsran {

class ru_controller;

namespace fapi {

class slot_last_message_notifier;
class slot_message_gateway;
struct fapi_cell_config;

/// FAPI slot messages adaptor abstract factory.
class slot_messages_adaptor_factory
{
public:
  virtual ~slot_messages_adaptor_factory() = default;

  /// Creates the FAPI slot messages adaptor.
  virtual std::unique_ptr<slot_messages_adaptor>
  create_slot_messages_adaptor(const fapi_cell_config&     fapi_cfg,
                               slot_message_gateway&       gateway,
                               slot_last_message_notifier& last_msg_notifier,
                               ru_controller&              ru_ctrl) = 0;
};

} // namespace fapi
} // namespace srsran
