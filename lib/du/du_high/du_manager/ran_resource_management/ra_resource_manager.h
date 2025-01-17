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

#include "du_ran_resource_manager.h"
#include "du_ue_resource_config.h"

namespace srsran::srs_du {

/// Handler of allocation of RA preambles for Contention-free Random Access (CFRA).
class ra_resource_manager
{
public:
  ra_resource_manager(span<const du_cell_config> cell_cfg_list);

  /// Allocate RA resources to a given UE.
  void allocate_cfra_resources(du_ue_resource_config& ue_res_cfg, const f1ap_ue_context_update_request& upd_req);

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
