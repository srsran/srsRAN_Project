/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "srsran/du/du_high/du_manager/du_configurator.h"
#include "srsran/mac/mac_cell_manager.h"

namespace srsran {
namespace srs_du {

/// DU procedure to dynamically change the parameters of the DU during its operation.
class du_param_config_procedure
{
public:
  du_param_config_procedure(const du_param_config_request& request_,
                            const du_manager_params&       init_du_params_,
                            du_cell_manager&               du_cells_);

  void operator()(coro_context<async_task<du_param_config_response>>& ctx);

private:
  /// Helper to configure the DU cell parameters.
  bool handle_cell_config_updates();

  /// Helper to run F1AP gNB-DU config update procedure.
  async_task<gnbdu_config_update_response> handle_f1_gnbdu_config_update();

  /// Helper to update MAC of the new cell parameters.
  async_task<mac_cell_reconfig_response> handle_mac_cell_update(const du_cell_reconfig_result& changed_cell) const;

  const du_param_config_request request;
  const du_manager_params&      du_params;
  du_cell_manager&              du_cells;
  error_type<std::string>       req_validation_outcome;
  srslog::basic_logger&         logger;

  unsigned next_cell_idx = 0;

  static_vector<du_cell_reconfig_result, MAX_NOF_DU_CELLS> changed_cells;

  du_param_config_response resp;
};

} // namespace srs_du
} // namespace srsran
