/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/band_helper.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/rrc/meas_types.h"
#include <map>
#include <vector>

namespace srsran {

namespace srs_cu_cp {

/// \brief Essential parameters required to configure cell measurements in the UE.
/// Note that some optional values need to be provided by the DU upon F1Setup.
struct cell_meas_cfg {
  nr_cell_id_t              nci;    ///< The NR cell identifier.
  std::vector<nr_cell_id_t> ncells; ///< List of NCIs of neighbor cells.

  /// If not set in config must be provided by config update after DU attach.
  optional<nr_band>            band;      ///< NR band.
  optional<rrc_ssb_mtc>        ssb_mtc;   ///< SSB measurement and timing config.
  optional<unsigned>           ssb_arfcn; ///< SSB ARFCN.
  optional<subcarrier_spacing> ssb_scs;   ///< SSB subcarrier spacing.
};

/// \brief Verifies required parameters are set. Returns true if config is valid, false otherwise.
bool is_complete(const cell_meas_cfg& cfg);

/// \brief Cell manager configuration.
struct cell_meas_manager_cfg {
  std::map<nr_cell_id_t, cell_meas_cfg> cells;           // Measurement related configs for all known cells.
  optional<rrc_cond_event_a3>           a3_event_config; // A3 event config is currently the only supported event.
};

/// \brief Validates configuration but doesn't verify if all provided cells have complete configuration (yet). Returns
/// true if config is valid, false otherwise.
bool is_valid_configuration(const cell_meas_manager_cfg& cfg);

/// \brief Same as config validation but additionally verfies that the measurement related parameters are present for
/// all cells.
bool is_complete(const cell_meas_manager_cfg& cfg);

} // namespace srs_cu_cp

} // namespace srsran
