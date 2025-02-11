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

#include "srsran/fapi/decorator.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {

class task_executor;

namespace fapi {

/// FAPI message bufferer decorator configuration.
struct message_bufferer_decorator_config {
  unsigned                    sector_id;
  unsigned                    l2_nof_slots_ahead;
  subcarrier_spacing          scs;
  task_executor&              executor;
  slot_message_gateway&       gateway;
  slot_last_message_notifier& last_msg_notifier;
};

/// FAPI logging decorator configuration.
struct logging_decorator_config {
  unsigned                    sector_id;
  srslog::basic_logger&       logger;
  slot_message_gateway&       gateway;
  slot_last_message_notifier& last_msg_notifier;
};

/// FAPI decorator configurations.
struct decorator_config {
  std::optional<message_bufferer_decorator_config> bufferer_cfg;
  std::optional<logging_decorator_config>          logging_cfg;
};

/// Creates and returns FAPI decorators using the given configuration or nullptr if no decorators are requested.
std::unique_ptr<fapi_decorator> create_decorators(const decorator_config& config);

} // namespace fapi
} // namespace srsran
