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
  unsigned                si_msg_idx;
  unsigned                si_period_rf;
  unsigned                si_window_len_slots;
  std::optional<unsigned> si_window_position;
};

/// NTN Configuration manager config.
struct ntn_configuration_manager_config {
  /// NTN Cell configuration.
  std::vector<ntn_cell_config> cells;
};

} // namespace srs_ntn
} // namespace srsran
