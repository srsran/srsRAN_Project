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

#include "config_message_gateway_impl.h"
#include "srsran/fapi/fapi_config_message_interface_collection.h"

namespace srsran {
namespace fapi {

/// FAPI configuration message interface collection implementation.
class fapi_config_message_interface_collection_impl : public fapi_config_message_interface_collection
{
  config_message_gateway_impl gateway;

public:
  explicit fapi_config_message_interface_collection_impl(srslog::basic_logger& logger) : gateway(logger) {}

  // See interface for documentation.
  config_message_gateway& get_config_message_gateway() override;

  // See interface for documentation.
  void set_config_message_notifier(config_message_notifier& config_notifier) override;

  // See interface for documentation.
  void set_slot_error_message_notifier(slot_error_message_notifier& err_notifier) override;

  // See interface for documentation.
  void set_cell_operation_request_notifier(cell_operation_request_notifier& cell_notifier) override;
};

} // namespace fapi
} // namespace srsran
