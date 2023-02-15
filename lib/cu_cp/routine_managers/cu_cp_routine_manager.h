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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/cu_cp/cu_up_processor.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/async_task_loop.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to handle routines.
class cu_cp_routine_manager
{
public:
  explicit cu_cp_routine_manager(
      cu_cp_ngap_control_notifier&                                                   ngap_ctrl_notifier_,
      ngap_cu_cp_connection_notifier&                                                cu_cp_ngap_ev_notifier_,
      std::unordered_map<cu_up_index_t, std::unique_ptr<cu_up_processor_interface>>& cu_up_db_);
  ~cu_cp_routine_manager() = default;

  void start_initial_cu_cp_setup_routine(const ngap_configuration& ngap_cfg);

private:
  cu_cp_ngap_control_notifier&    ngap_ctrl_notifier;
  ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier;

  std::unordered_map<cu_up_index_t, std::unique_ptr<cu_up_processor_interface>>& cu_up_db;

  // cu-cp task event loop
  async_task_sequencer main_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsgnb
