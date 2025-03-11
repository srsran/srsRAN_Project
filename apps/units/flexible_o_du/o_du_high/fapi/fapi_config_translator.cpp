/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_config_translator.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "fapi_config.h"

using namespace srsran;

void srsran::fill_fapi_worker_manager_config(worker_manager_config&  config,
                                             const fapi_unit_config& unit_cfg,
                                             unsigned                nof_cells)
{
  // No configuration for FAPI if there is no buffered module.
  if (unit_cfg.l2_nof_slots_ahead == 0) {
    return;
  }

  auto& fapi_cfg     = config.fapi_cfg.emplace();
  fapi_cfg.nof_cells = nof_cells;
}
