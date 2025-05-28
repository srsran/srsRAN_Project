/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_flexible_o_du_low_manager.h"

using namespace srsran;

split6_flexible_o_du_low_manager::split6_flexible_o_du_low_manager(
    split6_flexible_o_du_low_manager_dependencies&& dependencies) :
  factory_odu_low(std::move(dependencies.factory_odu_low)),
  cell_controller(*factory_odu_low),
  cell_plugin(std::move(dependencies.cell_plugin)),
  config_interface_collection(std::move(dependencies.config_interface_collection))
{
  srsran_assert(cell_plugin, "Invalid split 6 cell plugin");
  srsran_assert(config_interface_collection, "Invalid FAPI configuration message interface collection");

  config_interface_collection->set_cell_operation_request_notifier(cell_controller);
  config_interface_collection->set_slot_error_message_notifier(cell_plugin->get_slot_error_message_notifier());
  config_interface_collection->set_config_message_notifier(cell_plugin->get_config_message_notifier());
}

void split6_flexible_o_du_low_manager::start()
{
  cell_plugin->get_operation_controller().start();
}

void split6_flexible_o_du_low_manager::stop()
{
  cell_plugin->get_operation_controller().stop();
}
