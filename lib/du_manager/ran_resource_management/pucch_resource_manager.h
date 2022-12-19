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

class pucch_resource_manager
{
public:
  explicit pucch_resource_manager(span<const du_cell_config> cell_cfg_list_, const pucch_config& default_pucch_cfg_);
  pucch_resource_manager(const pucch_resource_manager&)            = delete;
  pucch_resource_manager(pucch_resource_manager&&)                 = delete;
  pucch_resource_manager& operator=(const pucch_resource_manager&) = delete;
  pucch_resource_manager& operator=(pucch_resource_manager&&)      = delete;

  bool alloc_resources(cell_group_config& cell_grp_cfg);
  void dealloc_resources(cell_group_config& cell_grp_cfg);

private:
  const pucch_config& default_pucch_cfg;

  unsigned sr_period;

  /// Lists of PHY/MAC resources available to be allocated.
  static_vector<std::vector<unsigned>, MAX_NOF_DU_CELLS> sr_offset_free_list;
};

} // namespace srs_du
} // namespace srsgnb