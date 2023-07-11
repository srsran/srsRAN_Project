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
  unsigned pucch_res_idx_to_sr_du_res_idx(unsigned pucch_res_idx);
  unsigned pucch_res_idx_to_csi_du_res_idx(unsigned pucch_res_idx);

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
