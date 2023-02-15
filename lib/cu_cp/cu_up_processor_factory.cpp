/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/cu_cp/cu_up_processor_factory.h"
#include "cu_up_processor_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<cu_up_processor_interface>
srsgnb::srs_cu_cp::create_cu_up_processor(const cu_up_processor_config_t             cu_up_processor_config_,
                                          cu_up_processor_cu_up_management_notifier& cu_cp_notifier_,
                                          e1ap_message_notifier&                     e1ap_notifier_,
                                          cu_up_processor_task_scheduler&            task_sched_,
                                          task_executor&                             ctrl_exec_)
{
  auto cu_up_processor = std::make_unique<cu_up_processor_impl>(
      cu_up_processor_config_, cu_cp_notifier_, e1ap_notifier_, task_sched_, ctrl_exec_);
  return cu_up_processor;
}
