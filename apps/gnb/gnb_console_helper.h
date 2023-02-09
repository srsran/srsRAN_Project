/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/scheduler/scheduler_metrics.h"
#include "srsgnb/support/io_broker/io_broker.h"
#include "srsgnb/support/metrics_plotter_stdout.h"
#include <fcntl.h>

namespace srsgnb {

/// \brief Notifier from application to signal current operation state.
class app_state_notfier
{
public:
  virtual ~app_state_notfier() = default;

  virtual void on_start_begin()    = 0;
  virtual void on_start_complete() = 0;

  virtual void on_stop_begin() = 0;
};

/// \brief Helper class to manager interaction with console, i.e. reading from stdin as well as writing
/// to stdout to print traces. Class uses the IO broker to register callback handlers. When they are
/// called they run in the context of the IO broker thread.
class gnb_console_helper : public app_state_notfier
{
public:
  gnb_console_helper(io_broker& io_broker_);
  ~gnb_console_helper();

  scheduler_ue_metrics_notifier& get_metrics_notifier() { return metrics_plotter; };

  void on_start_begin() override;
  void on_start_complete() override;
  void on_stop_begin() override;

  void set_cells(const std::vector<du_cell_config>& cells_);

private:
  void stdin_handler(int fd);
  void handle_command(const std::string& command);
  void print_help();

  srslog::basic_logger&  logger;
  io_broker&             io_broker_handle;
  metrics_plotter_stdout metrics_plotter;

  std::vector<du_cell_config> cells;
};

} // namespace srsgnb
