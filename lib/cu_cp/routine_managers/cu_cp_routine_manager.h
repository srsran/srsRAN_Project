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

#include "../adapters/cu_cp_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../cell_meas_manager/cell_meas_manager_impl.h"
#include "../cu_cp_impl_interface.h"
#include "../ue_manager/ue_manager_impl.h"
#include "../ue_manager/ue_task_scheduler.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to handle routines.
class cu_cp_routine_manager
{
public:
  explicit cu_cp_routine_manager(ue_task_scheduler_manager& ue_task_sched_);
  ~cu_cp_routine_manager() = default;

  bool schedule_async_task(async_task<void> task);

  void start_initial_cu_cp_setup_routine(const ngap_configuration& ngap_cfg);

  void start_ue_removal_routine(ue_index_t                      ue_index,
                                cu_cp_rrc_ue_removal_notifier&  rrc_du_notifier,
                                cu_cp_e1ap_ue_removal_notifier* e1ap_notifier,
                                cu_cp_f1ap_ue_removal_notifier& f1ap_notifier,
                                cu_cp_ngap_control_notifier&    ngap_notifier,
                                cell_meas_manager&              cell_meas_mng,
                                ue_manager&                     ue_mng,
                                srslog::basic_logger&           logger);

private:
  ue_task_scheduler_manager& ue_task_sched;

  // cu-cp task event loop
  fifo_async_task_scheduler main_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsran
