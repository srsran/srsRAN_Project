/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "cell_group_config.h"
#include "pucch_resource_generator.h"

namespace srsran {
namespace srs_du {

/// \brief This class manages the allocation of RAN PUCCH Resources to DU UEs. For instance, this class ensures that
/// UEs do not collide in the usage of the PUCCH for SRs and CSI. For HARQ-ACKs, we rely on the MAC scheduler to ensure
/// no collisions take place in the PUCCH.
class du_pucch_resource_manager
{
public:
  explicit du_pucch_resource_manager(span<const du_cell_config> cell_cfg_list_);
  du_pucch_resource_manager(const du_pucch_resource_manager&)            = delete;
  du_pucch_resource_manager(du_pucch_resource_manager&&)                 = delete;
  du_pucch_resource_manager& operator=(const du_pucch_resource_manager&) = delete;
  du_pucch_resource_manager& operator=(du_pucch_resource_manager&&)      = delete;

  /// \brief Allocate PUCCH resources for a given UE. The resources are stored in the UE's cell group config.
  /// \return true if allocation was successful.
  bool alloc_resources(cell_group_config& cell_grp_cfg);

  /// \brief Deallocate PUCCH resources previously given to a UE. The resources are returned back to a pool.
  void dealloc_resources(cell_group_config& cell_grp_cfg);

private:
  /// \brief Computes the DU index for PUCCH SR resource from the UE's PUCCH-Config \ref res_id index.
  ///
  /// Each cell has nof_cell_pucch_f1_res_sr PUCCH Format 1 resources that can be used for SR. Within the DU, these
  /// resources are indexed with the values: {0, ..., nof_cell_pucch_f1_res_sr-1}. However, in the UE's PUCCH-Config,
  /// the PUCCH F1 resources use different indices (see \ref res_id in \ref pucch_resource). The mapping between the DU
  /// index and the UE's PUCCH-Config for SR PUCCH resources is defined in \ref srs_du::ue_pucch_config_builder.
  unsigned pucch_res_idx_to_sr_du_res_idx(unsigned pucch_res_idx) const;

  /// \brief Computes the DU index for PUCCH CSI resource from the UE's PUCCH-Config \ref res_id index.
  ///
  /// Each cell has nof_cell_pucch_f2_res_csi PUCCH Format 2 resources that can be used for CSI. Within the DU, these
  /// resources are indexed with the values: {0, ..., nof_cell_pucch_f2_res_csi-1}. However, in the UE's PUCCH-Config,
  /// the PUCCH F2 resources use different indices (see \ref res_id in \ref pucch_resource). The mapping between the DU
  /// index and the UE's PUCCH-Config for CSI PUCCH resources is defined in \ref srs_du::ue_pucch_config_builder.
  unsigned pucch_res_idx_to_csi_du_res_idx(unsigned pucch_res_idx) const;

  // Parameters for PUCCH configuration passed by the user.
  const pucch_builder_params        user_defined_pucch_cfg;
  const std::vector<pucch_resource> default_pucch_res_list;
  const pucch_config                default_pucch_cfg;
  const optional<csi_report_config> default_csi_report_cfg;

  struct cell_resource_context {
    /// \brief Pool of PUCCH SR offsets currently available to be allocated to UEs. Each element is represented by a
    /// pair (pucch_resource_id, slot_offset).
    std::vector<std::pair<unsigned, unsigned>> sr_res_offset_free_list;
    /// Pool of PUCCH CSI offsets currently available to be allocated to UEs.
    std::vector<std::pair<unsigned, unsigned>> csi_res_offset_free_list;
    /// UE index for randomization of resources.
    unsigned ue_idx = 0;
  };

  /// Resources for the different cells of the DU.
  static_vector<cell_resource_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace srs_du
} // namespace srsran
