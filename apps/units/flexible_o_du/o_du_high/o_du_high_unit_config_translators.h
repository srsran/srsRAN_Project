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

#include "du_high/du_high_config_translators.h"
#include "e2/o_du_high_e2_config_translators.h"
#include "fapi/fapi_config_translator.h"
#include "o_du_high_unit_config.h"

namespace srsran {

/// Fills the O-RAN DU high worker manager parameters of the given worker manager configuration.
inline void fill_o_du_high_worker_manager_config(worker_manager_config&       config,
                                                 const o_du_high_unit_config& unit_cfg,
                                                 bool                         is_blocking_mode_enabled = false)
{
  fill_du_high_worker_manager_config(config, unit_cfg.du_high_cfg.config, is_blocking_mode_enabled);
  fill_fapi_worker_manager_config(config, unit_cfg.fapi_cfg, unit_cfg.du_high_cfg.config.cells_cfg.size());
  fill_o_du_high_e2_worker_manager_config(config, unit_cfg.e2_cfg);
}

} // namespace srsran
