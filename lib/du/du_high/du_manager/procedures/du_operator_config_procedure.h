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

class du_operator_config_procedure
{
public:
  du_operator_config_procedure(const du_operator_config_request& request_,
                               const du_manager_params&          du_params_,
                               du_cell_manager&                  du_cells_);

  void operator()(coro_context<async_task<du_operator_config_response>>& ctx);

private:
  void generate_new_configs();

  async_task<gnbdu_config_update_response> handle_f1_gnbdu_config_update();

  async_task<bool> handle_mac_cell_update(unsigned cell_idx);

  const du_operator_config_request request;
  const du_manager_params&         du_params;
  du_cell_manager&                 du_cells;

  unsigned next_cell_idx = 0;

  du_operator_config_response resp;
};

} // namespace srs_du
} // namespace srsran
