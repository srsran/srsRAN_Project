/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
