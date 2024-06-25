/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/nr_cgi.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Basic cell system information provided by DU
struct du_sys_info {
  byte_buffer packed_mib;
  byte_buffer packed_sib1;
};

struct du_cell_configuration {
  /// CU-CP specific DU cell identifier.
  du_cell_index_t cell_index = du_cell_index_t::invalid;
  /// Global cell ID.
  nr_cell_global_id_t cgi;
  /// Tracking Area Code
  uint32_t tac;
  /// Physical cell ID
  pci_t pci;
  /// NR bands provided/supported by the cell.
  std::vector<nr_band> bands;
  /// System Information provided by the DU for this cell.
  du_sys_info sys_info;
};

} // namespace srs_cu_cp
} // namespace srsran
