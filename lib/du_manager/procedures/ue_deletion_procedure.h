/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_ue/du_ue.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "srsran/du_manager/du_manager.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

class ue_deletion_procedure
{
public:
  ue_deletion_procedure(const f1ap_ue_delete_request& msg_,
                        du_ue_manager_repository&     ue_mng_,
                        const du_manager_params&      du_params);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Delete"; }

private:
  async_task<mac_ue_delete_response_message> launch_mac_ue_delete();

  async_task<void> disconnect_drbs();

  const f1ap_ue_delete_request msg;
  du_ue_manager_repository&    ue_mng;
  const du_manager_params&     du_params;
  ue_procedure_logger          proc_logger;

  du_ue* ue = nullptr;
};

} // namespace srs_du
} // namespace srsran
