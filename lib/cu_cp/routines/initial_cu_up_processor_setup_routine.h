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
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class initial_cu_up_processor_setup_routine
{
public:
  initial_cu_up_processor_setup_routine(cu_up_processor_context&                   context_,
                                        cu_up_processor_e1ap_control_notifier&     e1_conn_notifier_,
                                        cu_up_processor_cu_up_management_notifier& cu_cp_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<cu_cp_e1_setup_response> start_cu_cp_e1_setup_request();
  void                                handle_cu_cp_e1_setup_response(const cu_cp_e1_setup_response& resp);

  cu_up_processor_context& context;

  cu_up_processor_e1ap_control_notifier&     e1_conn_notifier;
  cu_up_processor_cu_up_management_notifier& cu_cp_notifier;

  cu_cp_e1_setup_response response = {};
};

} // namespace srs_cu_cp
} // namespace srsgnb
