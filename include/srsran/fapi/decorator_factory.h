/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  unsigned                    l2_nof_slots_ahead;
  subcarrier_spacing          scs;
  task_executor&              executor;
  slot_message_gateway&       gateway;
  slot_last_message_notifier& last_msg_notifier;
};

/// FAPI logging decorator configuration.
struct logging_decorator_config {
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
