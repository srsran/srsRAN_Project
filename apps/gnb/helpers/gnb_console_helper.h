/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "metrics_plotter_json.h"
#include "metrics_plotter_stdout.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

class io_broker;

/// \brief Notifier from application to signal current operation state.
class app_state_notifier
{
public:
  virtual ~app_state_notifier() = default;

  virtual void on_app_starting() = 0;
  virtual void on_app_running()  = 0;
  virtual void on_app_stopping() = 0;
};

/// \brief Helper class to manager interaction with console, i.e. reading from stdin as well as writing
/// to stdout to print traces. Class uses the IO broker to register callback handlers. When they are
/// called they run in the context of the IO broker thread.
class gnb_console_helper : public app_state_notifier
{
public:
  gnb_console_helper(io_broker& io_broker_, srslog::log_channel& log_chan_);
  ~gnb_console_helper();

  scheduler_ue_metrics_notifier& get_stdout_metrics_notifier() { return metrics_plotter; };
  scheduler_ue_metrics_notifier& get_json_metrics_notifier() { return metrics_json; };

  void on_app_starting() override;
  void on_app_running() override;
  void on_app_stopping() override;

  void set_cells(const span<du_cell_config>& cells_);

private:
  void stdin_handler(int fd);
  void handle_command(const std::string& command);
  void print_help();

  srslog::basic_logger&       logger;
  io_broker&                  io_broker_handle;
  metrics_plotter_stdout      metrics_plotter;
  metrics_plotter_json        metrics_json;
  std::vector<du_cell_config> cells;
};

} // namespace srsran
