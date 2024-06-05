/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "e1ap_cu_up_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<e1ap_interface> srsran::srs_cu_up::create_e1ap(e1_connection_client& e1_client_handler_,
                                                               e1ap_cu_up_notifier&  cu_up_notifier_,
                                                               timer_manager&        timers_,
                                                               task_executor&        cu_up_exec_)
{
  auto e1ap_cu_up = std::make_unique<e1ap_cu_up_impl>(e1_client_handler_, cu_up_notifier_, timers_, cu_up_exec_);
  return e1ap_cu_up;
}
