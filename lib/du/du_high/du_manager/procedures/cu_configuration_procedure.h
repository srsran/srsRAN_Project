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

#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

class du_cell_manager;
class du_ue_manager;
struct du_manager_params;
class du_manager_metrics_aggregator_impl;

/// \brief Procedure to handle context update request provided by CU.
class cu_configuration_procedure
{
public:
  cu_configuration_procedure(const gnbcu_config_update_request&  request_,
                             du_cell_manager&                    cell_mng_,
                             du_ue_manager&                      ue_mng_,
                             const du_manager_params&            du_params_,
                             du_manager_metrics_aggregator_impl& metrics_);

  void operator()(coro_context<async_task<gnbcu_config_update_response>>& ctx);

private:
  async_task<bool> start_cell(const nr_cell_global_id_t& cgi);
  async_task<void> stop_cell(const nr_cell_global_id_t& cgi);

  const gnbcu_config_update_request   request;
  du_cell_manager&                    cell_mng;
  du_ue_manager&                      ue_mng;
  const du_manager_params&            du_params;
  du_manager_metrics_aggregator_impl& metrics;

  unsigned list_index = 0;

  gnbcu_config_update_response resp;
};

} // namespace srs_du
} // namespace srsran
