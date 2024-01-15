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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include <cstdint>

namespace srsran {

namespace srs_cu_cp {

// Cell-related configuration used by the RRC.
struct rrc_cell_context {
  nr_cell_global_id_t  cgi;
  uint32_t             tac;
  pci_t                pci;
  unsigned             ssb_arfcn; ///< Absolute SSB position.
  std::vector<nr_band> bands;     ///< Required for capability band filter.
};

} // namespace srs_cu_cp

} // namespace srsran
