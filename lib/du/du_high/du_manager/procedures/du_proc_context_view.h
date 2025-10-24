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

#include "srsran/srslog/logger.h"

namespace srsran {
namespace srs_du {

struct du_manager_params;
struct du_manager_context;
class du_cell_manager;
class du_ue_manager;
class du_manager_metrics_aggregator_impl;

struct du_proc_context_view {
  /// Static parameters of the DU.
  du_manager_params&                  params;
  du_manager_context&                 ctxt;
  du_cell_manager&                    cell_mng;
  du_ue_manager&                      ue_mng;
  du_manager_metrics_aggregator_impl& metrics;
  srslog::basic_logger&               logger;
};

} // namespace srs_du
} // namespace srsran
