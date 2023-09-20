/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "srsran/e2/e2sm/e2sm_du.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {
namespace srs_du {

class du_ue_ric_configuration_procedure
{
public:
  du_ue_ric_configuration_procedure(const ric_control_config& request_,
                                    du_ue_manager_repository& ue_mng_,
                                    const du_manager_params&  du_params_);

  void operator()(coro_context<async_task<ric_control_config_response>>& ctx);

  const char* name() const { return "UE RIC configuration"; }

private:
  manual_event<ric_control_config_response>& dispatch_ue_config_task();

  // Task run from within the UE task loop.
  async_task<mac_ue_reconfiguration_response> handle_mac_config();

  const ric_control_config  request;
  du_ue_manager_repository& ue_mng;
  const du_manager_params&  du_params;

  du_ue* ue = nullptr;

  manual_event<ric_control_config_response> ue_config_completed;
};

} // namespace srs_du
} // namespace srsran