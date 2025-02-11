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

#include "srsran/fapi/decorator_factory.h"
#include "loggers/logging_decorator_impl.h"
#include "message_buffering/message_bufferer_decorator_impl.h"

using namespace srsran;
using namespace fapi;

std::unique_ptr<fapi_decorator> fapi::create_decorators(const decorator_config& config)
{
  // Nothing to create.
  if (!config.bufferer_cfg && !config.logging_cfg) {
    return nullptr;
  }

  // Create message bufferer decorator.
  if (config.bufferer_cfg && !config.logging_cfg) {
    const message_bufferer_decorator_config& cfg = config.bufferer_cfg.value();
    return std::make_unique<message_bufferer_decorator_impl>(
        cfg.sector_id, cfg.l2_nof_slots_ahead, cfg.scs, cfg.gateway, cfg.last_msg_notifier, cfg.executor);
  }

  // Create logging decorator.
  if (!config.bufferer_cfg && config.logging_cfg) {
    const logging_decorator_config& cfg = config.logging_cfg.value();
    return std::make_unique<logging_decorator_impl>(cfg.sector_id, cfg.logger, cfg.gateway, cfg.last_msg_notifier);
  }

  // Create message bufferer and logging decorators.
  const message_bufferer_decorator_config& bufferer_cfg = config.bufferer_cfg.value();
  auto bufferer_decorator = std::make_unique<message_bufferer_decorator_impl>(bufferer_cfg.sector_id,
                                                                              bufferer_cfg.l2_nof_slots_ahead,
                                                                              bufferer_cfg.scs,
                                                                              bufferer_cfg.gateway,
                                                                              bufferer_cfg.last_msg_notifier,
                                                                              bufferer_cfg.executor);

  return std::make_unique<logging_decorator_impl>(
      config.logging_cfg.value().sector_id, config.logging_cfg.value().logger, std::move(bufferer_decorator));
}
