
#ifndef SRSGNB_UE_CONFIGURATION_H
#define SRSGNB_UE_CONFIGURATION_H

#include "../cell/cell_configuration.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/bwp_configuration.h"
#include "srsgnb/ran/du_types.h"

namespace srsgnb {

/// UE-dedicated configuration for a given cell.
class ue_cell_configuration
{
public:
  ue_cell_configuration(const cell_configuration&                    cell_cfg_common_,
                        const serving_cell_ue_configuration_request& cell_cfg_ded_);

  void reconfigure(const serving_cell_ue_configuration_request& cell_cfg_ded_);

  const cell_configuration& cell_cfg_common;

  /// List of DL BWPs.
  slot_vector<bwp_configuration> dl_bwps;

  /// List of UE CORESETs.
  /// Note: The ID space of CoresetIds is common across all the BWPs of a Serving Cell.
  slot_array<coreset_configuration, MAX_NOF_CORESETS> dl_coresets;

  /// List of UE SearchSpaces.
  /// Note: The ID space of SearchSpaceIds is common across all the BWPs of a Serving Cell.
  slot_array<search_space_configuration, MAX_NOF_SEARCH_SPACES> dl_search_spaces;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CONFIGURATION_H
