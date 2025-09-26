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

#include "procedure_logger.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

struct du_manager_params;
class du_cell_manager;
class du_ue_manager;
class du_manager_metrics_aggregator_impl;

class f1c_disconnection_handling_procedure
{
public:
  f1c_disconnection_handling_procedure(const du_manager_params&            params,
                                       du_cell_manager&                    cell_mng,
                                       du_ue_manager&                      ue_mng,
                                       du_manager_metrics_aggregator_impl& metrics);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const du_manager_params&            params;
  du_cell_manager&                    cell_mng;
  du_ue_manager&                      ue_mng;
  du_manager_metrics_aggregator_impl& metrics;
  srslog::basic_logger&               logger;
  du_procedure_logger                 proc_logger;

  unsigned i = 0;
};

} // namespace srs_du
} // namespace srsran
