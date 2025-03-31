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

#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/ofh_sector.h"
#include "srsran/ofh/ofh_sector_config.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ofh/receiver/ofh_sequence_id_checker.h"
#include "srsran/ofh/timing/ofh_timing_manager.h"
#include <memory>

namespace srsran {
class task_executor;

namespace ofh {

/// Creates an Open Fronthaul sequence identifier checker.
std::unique_ptr<sequence_id_checker> create_sequence_id_checker();

/// Open Fronthaul controller config.
struct controller_config {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta;
};

/// Creates an Open Fronthaul timing manager with the given parameters.
std::unique_ptr<timing_manager>
create_ofh_timing_manager(const controller_config& config, srslog::basic_logger& logger, task_executor& executor);

/// Creates an Open Fronthaul sector.
std::unique_ptr<sector> create_ofh_sector(const sector_configuration& sector_cfg, sector_dependencies&& sector_deps);

} // namespace ofh
} // namespace srsran
