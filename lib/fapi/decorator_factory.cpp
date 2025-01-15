/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
        cfg.l2_nof_slots_ahead, cfg.scs, cfg.gateway, cfg.last_msg_notifier, cfg.executor);
  }

  // Create logging decorator.
  if (!config.bufferer_cfg && config.logging_cfg) {
    const logging_decorator_config& cfg = config.logging_cfg.value();
    return std::make_unique<logging_decorator_impl>(cfg.logger, cfg.gateway, cfg.last_msg_notifier);
  }

  // Create message bufferer and logging decorators.
  const message_bufferer_decorator_config& bufferer_cfg = config.bufferer_cfg.value();
  auto bufferer_decorator = std::make_unique<message_bufferer_decorator_impl>(bufferer_cfg.l2_nof_slots_ahead,
                                                                              bufferer_cfg.scs,
                                                                              bufferer_cfg.gateway,
                                                                              bufferer_cfg.last_msg_notifier,
                                                                              bufferer_cfg.executor);

  return std::make_unique<logging_decorator_impl>(config.logging_cfg.value().logger, std::move(bufferer_decorator));
}
