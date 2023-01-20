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

#include "srsgnb/cu_cp/cu_up_processor.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Service provided by CU-UP Processor to handle routines.
class cu_up_processor_routine_manager
{
public:
  explicit cu_up_processor_routine_manager(cu_up_processor_context&                   context_,
                                           e1_connection_manager&                     e1_conn_mng_,
                                           cu_up_processor_cu_up_management_notifier& cu_cp_notifier_,
                                           cu_up_processor_task_scheduler&            task_sched_);
  ~cu_up_processor_routine_manager() = default;

  void start_initial_cu_up_processor_setup_routine();

private:
  cu_up_processor_context&                   context;
  e1_connection_manager&                     e1_conn_mng;
  cu_up_processor_cu_up_management_notifier& cu_cp_notifier;
  cu_up_processor_task_scheduler&            task_sched;
};

} // namespace srs_cu_cp
} // namespace srsgnb
