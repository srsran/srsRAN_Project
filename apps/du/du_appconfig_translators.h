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

namespace srsran {

struct du_appconfig;
struct worker_manager_config;

/// Fills the DU worker manager parameters of the given worker manager configuration.
void fill_du_worker_manager_config(worker_manager_config& config, const du_appconfig& unit_cfg);

} // namespace srsran
