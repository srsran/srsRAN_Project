
#pragma once

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

  /// This array maps BWP-Ids (the array indexes) to DL BWPs (the array values).
  std::array<const bwp_downlink*, MAX_NOF_BWPS> dl_bwps = {};

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
    if (bwpid == to_bwp_id(0)) {
      return dl_bwps_cfg[bwpid].bwp_dl_common->pdcch_common.search_spaces;
    }
    return dl_bwps_cfg[bwpid].bwp_dl_ded->pdcch_cfg->ss_to_addmod_list;
  }

  /// Get UE list of pdsch-TimeDomainAllocationList as per TS38.214 clause 5.1.2.1.1.
  span<const pdsch_time_domain_resource_allocation> get_pdsch_time_domain_list(search_space_id ss_id) const;

  /// UL configuration.
  optional<uplink_config> ul_config;

private:
  void addmod_bwp_cfg(bwp_id_t bwpid, const bwp_downlink& bwp_dl);
  void addmod_bwp_common_cfg(bwp_id_t bwpid, const bwp_downlink_common& bwp_dl);
  void addmod_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded);
  void release_bwp_ded_cfg(bwp_id_t bwpid);
  void update_config_maps();
  void update_ul_config(const serving_cell_ue_configuration_request& cell_cfg_ded_req);

  /// List of UE BWP configurations.
  slot_array<bwp_downlink, MAX_NOF_BWPS> dl_bwps_cfg;

  /// Mapping of coreset-IDs to BWP-IDs.
  std::array<bwp_id_t, MAX_NOF_CORESETS> coreset_to_bwp_id = {};
};

} // namespace srsgnb
