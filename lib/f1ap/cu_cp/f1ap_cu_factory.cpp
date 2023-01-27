/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1ap/cu_cp/f1ap_cu_factory.h"
#include "f1ap_cu_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<f1ap_cu> srsgnb::srs_cu_cp::create_f1ap(f1c_message_notifier&       f1c_pdu_notifier_,
                                                        f1c_du_processor_notifier&  f1c_du_processor_notifier_,
                                                        f1c_du_management_notifier& f1c_du_management_notifier_,
                                                        task_executor&              ctrl_exec_)
{
  auto f1c_cu = std::make_unique<f1ap_cu_impl>(
      f1c_pdu_notifier_, f1c_du_processor_notifier_, f1c_du_management_notifier_, ctrl_exec_);
  return f1c_cu;
}
