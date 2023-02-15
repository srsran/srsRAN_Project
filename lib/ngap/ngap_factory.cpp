/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ngap/ngap_factory.h"
#include "ngap_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<ngap_interface> srsgnb::srs_cu_cp::create_ngap(ngap_configuration&     ngap_cfg_,
                                                               ngap_ue_task_scheduler& task_sched_,
                                                               ngap_ue_manager&        ue_manager_,
                                                               ngap_message_notifier&  ngap_notifier_,
                                                               task_executor&          ctrl_exec_)
{
  auto ngap = std::make_unique<ngap_impl>(ngap_cfg_, task_sched_, ue_manager_, ngap_notifier_, ctrl_exec_);
  return ngap;
}
