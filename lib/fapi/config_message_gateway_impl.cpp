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

#include "config_message_validator.h"
#include "configuration_procedure.h"
#include "srsran/fapi/config_message_notifier.h"
#include "srsran/fapi/error_message_notifier.h"
#include "srsran/fapi/messages/config_messages.h"
#include "srsran/fapi/messages/error_indication.h"

using namespace srsran;
using namespace fapi;

namespace {

/// FAPI configuration message notifier dummy implementation.
class config_message_notifier_dummy : public config_message_notifier
{
public:
  void on_param_response(const param_response& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on PARAM.response");
  }

  void on_config_response(const config_response& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on CONFIG.response");
  }

  void on_stop_indication(const stop_indication& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on STOP.indication");
  }
};

class error_message_notifier_dummy : public error_message_notifier
{
public:
  void on_error_indication(const error_indication_message& msg) override
  {
    report_error("Dummy FAPI ERROR.indication notifier cannot notify errors");
  }
};

class cell_operation_request_notifier_dummy : public cell_operation_request_notifier
{
public:
  bool on_start_request(const fapi_cell_config& config) override { return false; }
  void on_stop_request() override {}
};

} // namespace

static config_message_notifier_dummy         dummy_msg_notifier;
static error_message_notifier_dummy          dummy_error_msg_notifier;
static cell_operation_request_notifier_dummy dummy_cell_operation_notifier;

configuration_procedure::configuration_procedure(srslog::basic_logger& logger_) :
  logger(logger_),
  notifier(&dummy_msg_notifier),
  error_notifier(&dummy_error_msg_notifier),
  cell_operation_notifier(&dummy_cell_operation_notifier)
{
}

void configuration_procedure::param_request(const fapi::param_request& msg)
{
  // Do nothing for the PARAM.response.
  param_response reponse;
  reponse.num_tlv = 0;

  // If current status is running, report back error code, as per SCF-222 v4.0 section 3.3.1.3.
  reponse.error_code = (status == cell_status::RUNNING) ? error_code_id::msg_invalid_state : error_code_id::msg_ok;

  notifier->on_param_response(reponse);
}

void configuration_procedure::config_request(const fapi::config_request& msg)
{
  cell_status current_status = status;

  bool config_updated = false;

  // [Implementation defined] Do not allow configuring the cell in the running state.
  if (current_status != cell_status::RUNNING) {
    config_updated = update_cell_config(msg);
  }

  config_response reponse;
  reponse.error_code = config_updated ? error_code_id::msg_ok : error_code_id::msg_invalid_config;

  // Move cell to configured status if current status is idle and the configuration update was successful.
  if ((current_status == cell_status::IDLE) && config_updated) {
    status = cell_status::CONFIGURED;
  }

  notifier->on_config_response(reponse);
}

void configuration_procedure::start_request(const fapi::start_request& msg)
{
  cell_status current_status = status;

  // Unexpected status, notify error.
  if (current_status != cell_status::CONFIGURED) {
    error_indication_message indication;
    indication.message_id = message_type_id::start_request;
    indication.error_code = error_code_id::msg_invalid_state;
    error_notifier->on_error_indication(indication);

    return;
  }

  // Status is configured. Create DU low and set the status to start.
  if (!cell_operation_notifier->on_start_request(cell_cfg)) {
    logger.error("Failed to start cell id '{}'", cell_cfg.cell_cfg.phy_cell_id);
    error_indication_message indication;
    indication.message_id = message_type_id::start_request;
    indication.error_code = error_code_id::msg_invalid_config;
    error_notifier->on_error_indication(indication);

    return;
  }

  // Set the cell status to running.
  status = cell_status::RUNNING;
}

void configuration_procedure::stop_request(const fapi::stop_request& msg)
{
  cell_status current_status = status;

  // Unexpected status, notify error.
  if (current_status != cell_status::RUNNING) {
    error_indication_message indication;
    indication.message_id = message_type_id::stop_request;
    indication.error_code = error_code_id::msg_invalid_state;
    error_notifier->on_error_indication(indication);

    return;
  }

  // Status is configured. Create DU low and set the status to start.
  cell_operation_notifier->on_stop_request();

  // Set the cell status to configured.
  status = cell_status::CONFIGURED;

  stop_indication indication;
  notifier->on_stop_indication(indication);
}

bool configuration_procedure::update_cell_config(const fapi::config_request& msg)
{
  // Validate the config request message contents before updating.
  if (!validate_config_request_message(msg, logger)) {
    return false;
  }

  // Update the configuration.
  cell_cfg.carrier_cfg = msg.carrier_cfg;
  cell_cfg.cell_cfg    = msg.cell_cfg;
  cell_cfg.phy_cfg     = msg.phy_cfg;
  cell_cfg.prach_cfg   = msg.prach_cfg;
  cell_cfg.ssb_cfg     = msg.ssb_cfg;
  cell_cfg.tdd_cfg     = msg.tdd_cfg;

  return true;
}
