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

#include "split6_o_du_low_plugin_dummy.h"
#include "srsran/fapi/config_message_notifier.h"
#include "srsran/fapi/error_message_notifier.h"
#include "srsran/fapi/slot_data_message_notifier.h"
#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/fapi_adaptor/fapi_operation_controller.h"

using namespace srsran;

namespace {

/// FAPI operation controller dummy implementation.
class operation_controller_dummy : public fapi::operation_controller
{
public:
  bool start() override { return false; }
  bool stop() override { return false; }
};

/// FAPI error message notifier dummy implementation.
class error_message_notifier_dummy : public fapi::error_message_notifier
{
public:
  void on_error_indication(const fapi::error_indication_message& msg) override {}
};

/// FAPI slot data message notifier dummy implementation.
class slot_data_message_notifier_dummy : public fapi::slot_data_message_notifier
{
public:
  void on_rx_data_indication(const fapi::rx_data_indication_message& msg) override {}
  void on_crc_indication(const fapi::crc_indication_message& msg) override {}
  void on_uci_indication(const fapi::uci_indication_message& msg) override {}
  void on_srs_indication(const fapi::srs_indication_message& msg) override {}
  void on_rach_indication(const fapi::rach_indication_message& msg) override {}
};

/// FAPI slot time message notifier dummy implementation.
class slot_time_message_notifier_dummy : public fapi::slot_time_message_notifier
{
public:
  void on_slot_indication(const fapi::slot_indication_message& msg) override {}
};

/// Split 6 slot messages adaptor dummy implementation.
class split6_slot_messages_adaptor_dummy : public fapi::slot_messages_adaptor
{
  slot_data_message_notifier_dummy dummy_data_notifier;
  slot_time_message_notifier_dummy dummy_time_notifier;
  error_message_notifier_dummy     dummy_error_notifier;
  operation_controller_dummy       dummy_controller;

public:
  // See interface for documentation.
  fapi::operation_controller& get_operation_controller() override { return dummy_controller; }

  // See interface for documentation.
  fapi::slot_data_message_notifier& get_slot_data_message_notifier() override { return dummy_data_notifier; }

  // See interface for documentation.
  fapi::slot_time_message_notifier& get_slot_time_message_notifier() override { return dummy_time_notifier; }

  // See interface for documentation.
  fapi::error_message_notifier& get_error_message_notifier() override { return dummy_error_notifier; }
};

/// FAPI slot messages adaptor factory dummy implementation.
class slot_messages_adaptor_factory_dummy : public fapi::slot_messages_adaptor_factory
{
  // See interface for documentation.
  std::unique_ptr<fapi::slot_messages_adaptor>
  create_slot_messages_adaptor(const fapi::fapi_cell_config&     fapi_cfg,
                               fapi::slot_message_gateway&       gateway,
                               fapi::slot_last_message_notifier& last_msg_notifier,
                               ru_controller&                    ru_ctrl) override
  {
    return std::make_unique<split6_slot_messages_adaptor_dummy>();
  }
};

/// FAPI config message notifier dummy implementation.
class config_message_notifier_dummy : public fapi::config_message_notifier
{
public:
  void on_param_response(const fapi::param_response& msg) override {}
  void on_config_response(const fapi::config_response& msg) override {}
  void on_stop_indication(const fapi::stop_indication& msg) override {}
};

/// Configuration messages adaptor dummy implementation.
class config_messages_adaptor_dummy : public fapi::config_messages_adaptor
{
  operation_controller_dummy    dummy_controller;
  config_message_notifier_dummy dummy_config_notifier;
  error_message_notifier_dummy  dummy_error_notifier;

public:
  // See interface for documentation.
  fapi::config_message_notifier& get_config_message_notifier() override { return dummy_config_notifier; }

  // See interface for documentation.
  fapi::error_message_notifier& get_error_message_notifier() override { return dummy_error_notifier; }

  // See interface for documentation.
  fapi::operation_controller& get_operation_controller() override { return dummy_controller; }
};

} // namespace

std::unique_ptr<fapi::config_messages_adaptor>
split6_o_du_low_plugin_dummy::create_config_messages_adaptor(fapi::config_message_gateway& gateway,
                                                             task_executor&                executor,
                                                             task_executor&                control_executor)
{
  return std::make_unique<config_messages_adaptor_dummy>();
}

std::unique_ptr<fapi::slot_messages_adaptor_factory>
split6_o_du_low_plugin_dummy::create_slot_messages_adaptor_factory(task_executor& executor,
                                                                   task_executor& control_executor)
{
  return std::make_unique<slot_messages_adaptor_factory_dummy>();
}

#ifndef SRSRAN_HAS_SPLIT6_ENTERPRISE
std::unique_ptr<split6_o_du_low_plugin> srsran::create_split6_o_du_low_plugin(std::string_view app_name)
{
  return std::make_unique<split6_o_du_low_plugin_dummy>();
}
#endif
