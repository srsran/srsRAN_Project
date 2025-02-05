/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "srsran/du/du_high/du_manager/du_configurator.h"

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
  async_task<mac_cell_reconfig_response> handle_mac_cell_update(unsigned cell_idx);

  const du_param_config_request request;
  const du_manager_params&      du_params;
  du_cell_manager&              du_cells;
  srslog::basic_logger&         logger;

  unsigned next_cell_idx = 0;

  static_vector<du_cell_index_t, MAX_NOF_DU_CELLS> changed_cells;

  du_param_config_response resp;
};

} // namespace srs_du
} // namespace srsran
