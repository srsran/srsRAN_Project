
#pragma once

#include "../cell/cell_configuration.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/scheduler/config/bwp_configuration.h"

namespace srsgnb {

/// UE-dedicated configuration for a given cell.
class ue_cell_configuration
{
public:
  ue_cell_configuration(const cell_configuration& cell_cfg_common_, const serving_cell_config& serv_cell_cfg_);
  ue_cell_configuration(const ue_cell_configuration&)            = delete;
  ue_cell_configuration(ue_cell_configuration&&)                 = delete;
  ue_cell_configuration& operator=(const ue_cell_configuration&) = delete;
  ue_cell_configuration& operator=(ue_cell_configuration&&)      = delete;

  void reconfigure(const serving_cell_config& cell_cfg_ded_);

  const cell_configuration& cell_cfg_common;

  const serving_cell_config& cfg_dedicated() const { return cell_cfg_ded; }

  /// Fetches DL BWP common configuration based on BWP-Id.
  const bwp_downlink_common* find_dl_bwp_common(bwp_id_t bwp_id) const { return bwp_table[bwp_id].dl_bwp_common; }
  const bwp_downlink_common& dl_bwp_common(bwp_id_t bwp_id) const
  {
    const bwp_downlink_common* ret = find_dl_bwp_common(bwp_id);
    srsgnb_assert(ret != nullptr, "Inexistent BWP-Id={}", bwp_id);
    return *ret;
  }

  /// Fetches DL BWP dedicated configuration based on BWP-Id.
  const bwp_downlink_dedicated* find_dl_bwp_ded(bwp_id_t bwp_id) const { return bwp_table[bwp_id].dl_bwp_ded; }

  /// Fetches DL CORESET configuration based on Coreset-Id.
  const coreset_configuration* find_dl_coreset(coreset_id cs_id) const { return dl_coresets[cs_id]; }
  const coreset_configuration& dl_coreset(coreset_id cs_id) const
  {
    const coreset_configuration* ret = find_dl_coreset(cs_id);
    srsgnb_assert(ret != nullptr, "Inexistent CORESET-Id={}", cs_id);
    return *ret;
  }

  /// Fetches DL SearchSpace configuration based on SearchSpace-Id.
  const search_space_configuration* find_dl_search_space(search_space_id ss_id) const
  {
    return dl_search_spaces[ss_id];
  }
  const search_space_configuration& dl_search_space(search_space_id ss_id) const
  {
    const search_space_configuration* ret = find_dl_search_space(ss_id);
    srsgnb_assert(ret != nullptr, "Inexistent SearchSpace-Id={}", ss_id);
    return *ret;
  }

  /// Get Search Space List for a given BWP-Id.
  const static_vector<const search_space_configuration*, MAX_NOF_SEARCH_SPACE_PER_BWP>&
  get_dl_search_spaces(bwp_id_t bwpid) const
  {
    return bwp_table[bwpid].dl_search_spaces;
  }

  /// Get UE list of pdsch-TimeDomainAllocationList as per TS38.214 clause 5.1.2.1.1.
  span<const pdsch_time_domain_resource_allocation> get_pdsch_time_domain_list(search_space_id ss_id) const;

  /// Fetches UL BWP common configuration based on BWP-Id.
  const bwp_uplink_common* find_ul_bwp_common(bwp_id_t bwp_id) const { return bwp_table[bwp_id].ul_bwp_common; }
  const bwp_uplink_common& ul_bwp_common(bwp_id_t bwp_id) const
  {
    const bwp_uplink_common* ret = find_ul_bwp_common(bwp_id);
    srsgnb_assert(ret != nullptr, "Inexistent BWP-Id={}", bwp_id);
    return *ret;
  }

private:
  /// Combination of common and dedicated BWP Parameters for a given UE that can be quickly fechted via BWP-Id.
  struct bwp_params {
    const bwp_downlink_common*                                                     dl_bwp_common = nullptr;
    const bwp_downlink_dedicated*                                                  dl_bwp_ded    = nullptr;
    static_vector<const search_space_configuration*, MAX_NOF_SEARCH_SPACE_PER_BWP> dl_search_spaces;
    const bwp_uplink_common*                                                       ul_bwp_common = nullptr;
    const bwp_uplink_dedicated*                                                    ul_bwp_ded    = nullptr;
  };

  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_downlink_common& bwp_dl_common);
  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded);

  /// Dedicated cell configuration.
  serving_cell_config cell_cfg_ded;

  /// Lookup table for BWP params indexed by BWP-Id.
  std::array<bwp_params, MAX_NOF_BWPS> bwp_table = {};

  /// This array maps Coreset-Ids (the array indexes) to CORESET configurations (the array values).
  /// Note: The ID space of CoresetIds is common across all the BWPs of a Serving Cell.
  std::array<const coreset_configuration*, MAX_NOF_CORESETS> dl_coresets = {};

  /// This array maps SearchSpace-Ids (the array indexes) to SearchSpace configurations (the array values).
  /// Note: The ID space of SearchSpaceIds is common across all the BWPs of a Serving Cell.
  std::array<const search_space_configuration*, MAX_NOF_SEARCH_SPACES> dl_search_spaces = {};

  /// This array maps Coreset-Ids (the array indexes) to BWP-Ids (the array values).
  std::array<bwp_id_t, MAX_NOF_BWPS> coreset_id_to_bwp_id;
};

} // namespace srsgnb
