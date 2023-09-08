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

#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "srsran/e2/e2sm/e2sm.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {
namespace srs_du {

class du_ue_ric_configuration_procedure
{
public:
  du_ue_ric_configuration_procedure(const ric_control_config& request_,
                                    du_ue_manager_repository& ue_mng_,
                                    const du_manager_params&  du_params_);

  void operator()(coro_context<async_task<ric_control_config>>& ctx);

  const char* name() const { return "UE RIC configuration"; }

private:
  manual_event<ric_control_config>& dispatch_ue_config_task();

  // Task run from within the UE task loop.
  async_task<mac_ue_reconfiguration_response> handle_mac_config();

  const ric_control_config  request;
  du_ue_manager_repository& ue_mng;
  const du_manager_params&  du_params;

  du_ue* ue = nullptr;

  manual_event<ric_control_config> ue_config_completed;
};

} // namespace srs_du
} // namespace srsran