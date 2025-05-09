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

#include "srsran/fapi/cell_operation_request_notifier.h"
#include "srsran/fapi/config_message_gateway.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace fapi {

class slot_error_message_notifier;
class config_message_notifier;

/// FAPI configuration message gateway implementation.
class config_message_gateway_impl : public config_message_gateway
{
  enum class cell_status : uint8_t { IDLE, CONFIGURED, RUNNING };

public:
  explicit config_message_gateway_impl(srslog::basic_logger& logger_);

  // See interface for documentation.
  void param_request(const fapi::param_request& msg) override;

  // See interface for documentation.
  void config_request(const fapi::config_request& msg) override;

  // See interface for documentation.
  void start_request(const fapi::start_request& msg) override;

  // See interface for documentation.
  void stop_request(const fapi::stop_request& msg) override;

  /// Sets the config message notifier to the given one.
  void set_config_message_notifier(config_message_notifier& config_notifier) { notifier = &config_notifier; }

  /// Sets the slot error message notifier to the given one.
  void set_slot_error_message_notifier(slot_error_message_notifier& err_notifier) { error_notifier = &err_notifier; }

  /// Sets the cell operation request notifier to the given one.
  void set_cell_operation_request_notifier(cell_operation_request_notifier& cell_notifier)
  {
    cell_operation_notifier = &cell_notifier;
  }

private:
  /// Updates the cell config using the given FAPI CONFIG.request message and returns true if the config was
  /// successfully updated, otherwise false.
  bool update_cell_config(const fapi::config_request& msg);

private:
  srslog::basic_logger&            logger;
  fapi_cell_config                 cell_cfg;
  config_message_notifier*         notifier                = nullptr;
  slot_error_message_notifier*     error_notifier          = nullptr;
  cell_operation_request_notifier* cell_operation_notifier = nullptr;
  cell_status                      status                  = cell_status::IDLE;
};

} // namespace fapi
} // namespace srsran
