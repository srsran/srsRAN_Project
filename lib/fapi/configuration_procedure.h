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

#include "srsran/fapi/cell_config.h"
#include "srsran/fapi/cell_operation_request_notifier.h"
#include "srsran/fapi/config_message_gateway.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace fapi {

class error_message_notifier;
class config_message_notifier;

/// FAPI configuration procedure.
class configuration_procedure : public config_message_gateway
{
  enum class cell_status : uint8_t { IDLE, CONFIGURED, RUNNING };

public:
  explicit configuration_procedure(srslog::basic_logger& logger_);

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

  /// Sets the error message notifier to the given one.
  void set_error_message_notifier(error_message_notifier& err_notifier) { error_notifier = &err_notifier; }

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
  error_message_notifier*          error_notifier          = nullptr;
  cell_operation_request_notifier* cell_operation_notifier = nullptr;
  cell_status                      status                  = cell_status::IDLE;
};

} // namespace fapi
} // namespace srsran
