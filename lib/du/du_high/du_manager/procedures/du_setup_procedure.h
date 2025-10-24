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

#include "../du_metrics_aggregator_impl.h"
#include "du_proc_context_view.h"
#include "procedure_logger.h"
#include "srsran/f1ap/du/f1ap_du_connection_manager.h"

namespace srsran {
namespace srs_du {

class du_cell_manager;
struct du_manager_params;

/// Request to transition the DU to operational mode.
struct du_start_request {
  /// Whether the cells need to be reconfigured.
  bool                      configure_cells                = true;
  unsigned                  max_f1c_tnl_connection_retries = 1;
  std::chrono::milliseconds f1c_tnl_connection_retry_wait{1000};
};

/// Procedure to transition the DU state to operation mode.
class du_setup_procedure
{
public:
  du_setup_procedure(const du_proc_context_view& ctxt, const du_start_request& request = {});

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // Handle passed DU unit config.
  void configure_du_cells();

  async_task<f1_setup_result> start_f1_setup_request();

  // Handle F1 setup response with list of cells to activate.
  async_task<void> handle_f1_setup_response(const f1_setup_result& resp);

  const du_proc_context_view& ctxt;
  const du_start_request      request;
  du_procedure_logger         proc_logger;

  f1_setup_result response_msg = {};

  unique_timer timer;

  unsigned    count = 0;
  std::string failure_cause;
};

} // namespace srs_du
} // namespace srsran
