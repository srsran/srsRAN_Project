
#ifndef SRSGNB_UE_CONFIGURATION_H
#define SRSGNB_UE_CONFIGURATION_H

#include "../cell/cell_configuration.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/scheduler/bwp_configuration.h"

namespace srsgnb {

/// UE-dedicated configuration for a given cell.
class ue_cell_configuration
{
public:
  ue_cell_configuration(const cell_configuration&                    cell_cfg_common_,
                        const serving_cell_ue_configuration_request& cell_cfg_ded_);

  void reconfigure(const serving_cell_ue_configuration_request& cell_cfg_ded_);

  const cell_configuration& cell_cfg_common;

  /// This array maps BWP-Ids (the array indexes) to DL BWP configurations (the array values).
  std::array<const bwp_configuration*, MAX_NOF_BWPS> dl_bwps = {};

  /// This array maps Coreset-Ids (the array indexes) to CORESET configurations (the array values).
  /// Note: The ID space of CoresetIds is common across all the BWPs of a Serving Cell.
  std::array<const coreset_configuration*, MAX_NOF_CORESETS> dl_coresets = {};

  /// This array maps SearchSpace-Ids (the array indexes) to SearchSpace configurations (the array values).
  /// Note: The ID space of SearchSpaceIds is common across all the BWPs of a Serving Cell.
  std::array<const search_space_configuration*, MAX_NOF_SEARCH_SPACES> dl_search_spaces = {};

  /// This array maps BWP-Ids (the array indexes) to UL BWP configurations (the array values).
  std::array<const bwp_configuration*, MAX_NOF_BWPS> ul_bwps = {};

  /// Get Search Space List for a given BWP-Id.
  span<const search_space_configuration> get_dl_search_spaces(bwp_id_t bwpid) const
  {
    return dl_bwps_cfg[bwpid].bwp_dl_ded.pdcch_cfg->ss_to_addmod_list;
  }

private:
  void addmod_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded);
  void rel_bwp_ded_cfg(bwp_id_t bwpid);

  /// List of UE BWP configurations.
  slot_array<bwp_downlink, MAX_NOF_BWPS> dl_bwps_cfg;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CONFIGURATION_H
