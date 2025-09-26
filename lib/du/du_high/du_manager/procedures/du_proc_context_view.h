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
