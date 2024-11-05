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

namespace srsran {

struct gnb_appconfig;
struct worker_manager_config;

/// Fills the gNB worker manager parameters of the given worker manager configuration.
void fill_gnb_worker_manager_config(worker_manager_config& config, const gnb_appconfig& unit_cfg);

} // namespace srsran
