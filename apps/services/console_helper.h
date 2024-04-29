/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "metrics_plotter_json.h"
#include "metrics_plotter_stdout.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/io/io_broker.h"
#include <list>

namespace srsran {

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
class console_helper : public app_state_notifier
{
public:
  console_helper(io_broker&                        io_broker_,
                 srslog::log_channel&              log_chan_,
                 srs_cu_cp::cu_cp_command_handler& cu_cp_,
                 bool                              autostart_stdout_metrics_ = false);
  ~console_helper();

  scheduler_ue_metrics_notifier& get_stdout_metrics_notifier() { return metrics_plotter; };
  scheduler_ue_metrics_notifier& get_json_metrics_notifier() { return metrics_json; };

  void on_app_starting() override;
  void on_app_running() override;
  void on_app_stopping() override;

  void set_cells(const span<du_cell_config>& cells_);
  void set_ru_controller(ru_controller& controller);

  void handle_tx_gain_command(const std::list<std::string>& gain_args);
  void handle_rx_gain_command(const std::list<std::string>& gain_args);

  void handle_handover_command(const std::list<std::string>& ho_args);
  void handle_log_command(const std::list<std::string>& gain_args);

  void handle_sleep_command(const std::list<std::string>& gain_args);

private:
  void stdin_handler(int fd);
  void handle_command(const std::string& command);
  void print_help();

  srslog::basic_logger&             logger;
  io_broker&                        io_broker_handle;
  metrics_plotter_stdout            metrics_plotter;
  metrics_plotter_json              metrics_json;
  std::vector<du_cell_config>       cells;
  optional<ru_controller*>          radio_controller;
  srs_cu_cp::cu_cp_command_handler& cu_cp;
  bool                              autostart_stdout_metrics = false;

  io_broker::io_handle stdin_handle;
};

} // namespace srsran
