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

#include "du_ue_resource_config.h"

namespace srsran {
namespace srs_du {

class drx_offset_pool;

/// \brief Class responsible for deriving the DRX-Config and measGaps of a given UE.
class du_drx_resource_manager
{
public:
  du_drx_resource_manager(span<const du_cell_config> cell_cfg_list_);
  du_drx_resource_manager(du_drx_resource_manager&&) noexcept = default;
  ~du_drx_resource_manager();

  du_drx_resource_manager& operator=(du_drx_resource_manager&&) noexcept = default;

  /// \brief Set the default DRX config when UE is created.
  void handle_ue_creation(cell_group_config& cell_grp_cfg);

  /// \brief Reserve gaps for DRX/measGaps for a given UE. The resources are stored in the UE's cell group config.
  /// \return true if allocation was successful.
  void handle_ue_cap_update(cell_group_config& cell_grp_cfg, bool long_drx_cycle_supported);

  /// \brief Deallocate DRX/measGaps resources previously given to a UE. The resources are returned back to a pool.
  void handle_ue_removal(cell_group_config& cell_grp_cfg);

private:
  span<const du_cell_config> cell_cfg_list;

  // Pool of slot offsets.
  std::unique_ptr<drx_offset_pool> offset_pool;
};

} // namespace srs_du
} // namespace srsran
