/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_high/du_high_config_translators.h"
#include "e2/o_du_high_e2_config_translators.h"
#include "fapi/fapi_config_translator.h"
#include "o_du_high_unit_config.h"

namespace srsran {

/// Fills the O-RAN DU high worker manager parameters of the given worker manager configuration.
void fill_o_du_high_worker_manager_config(worker_manager_config&       config,
                                          const o_du_high_unit_config& unit_cfg,
                                          bool                         is_blocking_mode_enabled = false)
{
  fill_du_high_worker_manager_config(config, unit_cfg.du_high_cfg.config, is_blocking_mode_enabled);
  fill_fapi_worker_manager_config(config, unit_cfg.fapi_cfg, unit_cfg.du_high_cfg.config.cells_cfg.size());
  fill_o_du_high_e2_worker_manager_config(config, unit_cfg.e2_cfg);
}

} // namespace srsran
