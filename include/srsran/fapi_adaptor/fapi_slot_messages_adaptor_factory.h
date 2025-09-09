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

#include "srsran/fapi_adaptor/fapi_slot_messages_adaptor.h"
#include <memory>

namespace srsran {
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
  create_slot_messages_adaptor(const fapi_cell_config&     config,
                               slot_message_gateway&       gateway,
                               slot_last_message_notifier& last_msg_notifier) = 0;
};

} // namespace fapi
} // namespace srsran
