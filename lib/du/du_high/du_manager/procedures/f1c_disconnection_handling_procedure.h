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

#include "du_proc_context_view.h"
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
  f1c_disconnection_handling_procedure(du_proc_context_view ctxt_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const du_proc_context_view ctxt;
  du_procedure_logger        proc_logger;

  unsigned i = 0;
};

} // namespace srs_du
} // namespace srsran
