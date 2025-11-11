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

#include "srsran/ran/ntn.h"

namespace srsran {
namespace srs_ntn {

/// NTN Cell configuration.
struct ntn_cell_config {
  /// NR-CGI.
  nr_cell_global_id_t nr_cgi;
  /// Sector Id (4-14 bits).
  std::optional<unsigned> sector_id;
  /// NTN configuration.
  ntn_config ntn_cfg;
  /// SIB19 scheduling information.
  unsigned si_msg_idx;
  unsigned si_period_rf;
  unsigned si_window_len_slots;
  unsigned si_window_position;
};

/// NTN Configuration manager config.
struct ntn_configuration_manager_config {
  /// NTN Cell configuration.
  std::vector<ntn_cell_config> cells;
};

} // namespace srs_ntn
} // namespace srsran
