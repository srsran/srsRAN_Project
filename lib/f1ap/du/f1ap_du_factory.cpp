/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "f1ap_du_impl.h"
#include "srsran/du/du_high/du_manager/du_manager.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_du;

std::unique_ptr<f1ap_du> srsran::srs_du::create_f1ap(f1c_connection_client&      f1c_client_handler,
                                                     f1ap_du_configurator&       du_mng,
                                                     task_executor&              ctrl_exec,
                                                     du_high_ue_executor_mapper& ue_exec_mapper,
                                                     f1ap_du_paging_notifier&    paging_notifier)
{
  auto f1ap_du = std::make_unique<f1ap_du_impl>(f1c_client_handler, du_mng, ctrl_exec, ue_exec_mapper, paging_notifier);
  return f1ap_du;
}
