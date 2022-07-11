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

#include "adapters/f1ap_adapters.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

class cu_cp
{
public:
  explicit cu_cp(const cu_cp_configuration& cfg_);
  ~cu_cp();

  void start();
  void stop();

  f1c_message_handler* get_f1c_message_handler(du_index_t du_index);

  cu_cp_manager_interface& get_cu_cp_manager();

  // CU-CP statistics
  size_t get_nof_dus();
  size_t get_nof_ues();

private:
  timer_manager       timers;
  cu_cp_configuration cfg;

  std::unique_ptr<cu_cp_manager_interface> manager;

  cu_cp_manager_f1ap_event_indicator f1ap_ev_notifier;
};

} // namespace srs_cu_cp
} // namespace srsgnb
