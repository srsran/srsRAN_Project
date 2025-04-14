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

#include "../du_metrics_collector_impl.h"
#include "srsran/f1ap/du/f1ap_du_connection_manager.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_du {

class du_cell_manager;
struct du_manager_params;

class initial_du_setup_procedure
{
public:
  initial_du_setup_procedure(const du_manager_params&           params_,
                             du_cell_manager&                   cell_mng_,
                             du_manager_metrics_collector_impl& metrics_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // Handle passed DU unit config.
  void configure_du_cells();

  async_task<f1_setup_result> start_f1_setup_request();

  // Handle F1 setup response with list of cells to activate.
  async_task<void> handle_f1_setup_response(const f1_setup_result& resp);

  const du_manager_params&           params;
  du_cell_manager&                   cell_mng;
  du_manager_metrics_collector_impl& metrics;
  srslog::basic_logger&              logger;

  f1_setup_result response_msg = {};
};

} // namespace srs_du
} // namespace srsran
