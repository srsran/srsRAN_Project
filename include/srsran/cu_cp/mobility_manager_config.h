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

namespace srs_cu_cp {

/// \brief Mobility manager configuration.
struct mobility_manager_cfg {
  bool trigger_handover_from_measurements = false; ///< Set to true to trigger HO when neighbor becomes stronger.
};

} // namespace srs_cu_cp

} // namespace srsran
