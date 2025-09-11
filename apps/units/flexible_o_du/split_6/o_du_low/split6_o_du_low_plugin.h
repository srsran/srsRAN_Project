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

#include "split6_flexible_o_du_low_session_factory.h"
#include "srsran/fapi_adaptor/fapi_config_messages_adaptor.h"
#include "srsran/fapi_adaptor/fapi_slot_messages_adaptor_factory.h"
#include <memory>

namespace CLI {
class App;
} // namespace CLI

namespace srsran {

struct worker_manager_config;

/// \brief Split 6 O-DU low plugin interface.
///
/// The plugin interface allows different implementations of the split 6 O-DU low.
class split6_o_du_low_plugin
{
public:
  virtual ~split6_o_du_low_plugin() = default;

  /// Registers the parsing configuration properties that will be used by this application unit.
  virtual void on_parsing_configuration_registration(CLI::App& app) = 0;

  /// Validates the configuration of this application unit. Returns true on success, otherwise false.
  virtual bool on_configuration_validation() const = 0;

  /// Registers the loggers of this application unit.
  virtual void on_loggers_registration() = 0;

  /// Creates and returns a FAPI config messages adaptor using the given arguments.
  virtual std::unique_ptr<fapi::config_messages_adaptor>
  create_config_messages_adaptor(fapi::config_message_gateway& gateway,
                                 task_executor&                executor,
                                 task_executor&                control_executor) = 0;

  /// Creates and returns a FAPI slot messages adaptor factory using the given arguments.
  virtual std::unique_ptr<fapi::slot_messages_adaptor_factory>
  create_slot_messages_adaptor_factory(task_executor& executor, task_executor& control_executor) = 0;

  /// Fills the given worker manager split 6 configuration.
  virtual void fill_worker_manager_config(worker_manager_config& config) = 0;
};

/// Creates the split 6 O-DU low plugin.
std::unique_ptr<split6_o_du_low_plugin> create_split6_o_du_low_plugin(std::string_view app_name);

} // namespace srsran
