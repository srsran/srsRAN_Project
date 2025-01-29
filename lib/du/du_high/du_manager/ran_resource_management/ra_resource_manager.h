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

#include "du_ran_resource_manager.h"
#include "du_ue_resource_config.h"

namespace srsran::srs_du {

/// Handler of allocation of RA preambles for Contention-free Random Access (CFRA).
class ra_resource_manager
{
public:
  ra_resource_manager(span<const du_cell_config> cell_cfg_list);

  /// Allocate RA resources to a given UE.
  void allocate_cfra_resources(du_ue_resource_config& ue_res_cfg);

  /// Deallocate RA resources of a given UE.
  void deallocate_cfra_resources(du_ue_resource_config& ue_res_cfg);

private:
  struct cell_ra_context {
    const du_cell_config* cell_cfg;
    std::vector<unsigned> free_preamble_idx_list;
  };

  std::vector<cell_ra_context> cells;
};

} // namespace srsran::srs_du
