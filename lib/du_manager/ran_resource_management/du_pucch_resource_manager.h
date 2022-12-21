/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cell_group_config.h"

namespace srsgnb {
namespace srs_du {

/// \brief This class manages the allocation of RAN PUCCH Resources to DU UEs. For instance, this class ensures that
/// UEs do not collide in the usage of the PUCCH for SRs and CSI. For HARQ-ACKs, we rely on the MAC scheduler to ensure
/// no collisions take place in the PUCCH.
class du_pucch_resource_manager
{
public:
  explicit du_pucch_resource_manager(span<const du_cell_config> cell_cfg_list_, const pucch_config& default_pucch_cfg_);
  du_pucch_resource_manager(const du_pucch_resource_manager&)            = delete;
  du_pucch_resource_manager(du_pucch_resource_manager&&)                 = delete;
  du_pucch_resource_manager& operator=(const du_pucch_resource_manager&) = delete;
  du_pucch_resource_manager& operator=(du_pucch_resource_manager&&)      = delete;

  bool alloc_resources(cell_group_config& cell_grp_cfg);
  void dealloc_resources(cell_group_config& cell_grp_cfg);

private:
  const pucch_config& default_pucch_cfg;

  struct cell_resource_context {
    /// Pool of PUCCH SR offsets currently available to be allocated to UEs.
    std::vector<unsigned> sr_offset_free_list;
  };

  /// Resources for the different cells of the DU.
  static_vector<cell_resource_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace srs_du
} // namespace srsgnb
