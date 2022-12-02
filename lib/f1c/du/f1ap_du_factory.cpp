/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1c/du/f1ap_du_factory.h"
#include "f1ap_du_impl.h"
#include "srsgnb/du_manager/du_manager.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_du;

std::unique_ptr<f1_interface> srsgnb::srs_du::create_f1ap(f1c_message_notifier&       message_notifier,
                                                          f1c_du_configurator&        du_mng,
                                                          task_executor&              ctrl_exec,
                                                          du_high_ue_executor_mapper& ue_exec_mapper)
{
  auto f1ap_du = std::make_unique<f1ap_du_impl>(message_notifier, du_mng, ctrl_exec, ue_exec_mapper);
  return f1ap_du;
}
