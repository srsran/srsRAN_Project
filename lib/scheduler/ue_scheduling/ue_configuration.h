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

#include "../cell/cell_configuration.h"
#include "../support/pdcch/search_space_helper.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/config/bwp_configuration.h"

namespace srsran {

/// \brief Grouping of information associated with a given BWP.
struct bwp_info {
  bwp_id_t                      bwp_id;
  const bwp_downlink_common*    dl_common = nullptr;
  const bwp_downlink_dedicated* dl_ded    = nullptr;
  const bwp_uplink_common*      ul_common = nullptr;
  const bwp_uplink_dedicated*   ul_ded    = nullptr;

  static_vector<const search_space_configuration*, MAX_NOF_SEARCH_SPACE_PER_BWP> search_spaces;
};

/// \brief Grouping of information associated with a given search space.
struct search_space_info {
  const search_space_configuration*                 cfg     = nullptr;
  const coreset_configuration*                      coreset = nullptr;
  const bwp_info*                                   bwp     = nullptr;
  span<const pdsch_time_domain_resource_allocation> pdsch_time_domain_list;
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list;

  /// \brief Gets DL DCI format type to use based on SearchSpace configuration.
  /// \return DL DCI format.
  dci_dl_format get_dl_dci_format() const { return search_space_helper::get_dl_dci_format(*cfg); }

  /// \brief Gets UL DCI format type to use based on SearchSpace configuration.
  /// \return UL DCI format.
  dci_ul_format get_ul_dci_format() const { return search_space_helper::get_ul_dci_format(*cfg); }
};

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

  /// Get BWP information given a BWP-Id.
  const bwp_info* find_bwp(bwp_id_t bwp_id) const
  {
    return bwp_table[bwp_id].dl_common != nullptr ? &bwp_table[bwp_id] : nullptr;
  }
  const bwp_info& bwp(bwp_id_t bwp_id) const
  {
    const bwp_info* bwp = find_bwp(bwp_id);
    srsran_assert(bwp != nullptr, "Invalid BWP-Id={} access", bwp_id);
    return *bwp;
  }

  /// Fetches CORESET configuration based on Coreset-Id.
  const coreset_configuration* find_coreset(coreset_id cs_id) const { return coresets[cs_id]; }
  const coreset_configuration& coreset(coreset_id cs_id) const
  {
    const coreset_configuration* ret = find_coreset(cs_id);
    srsran_assert(ret != nullptr, "Inexistent CORESET-Id={}", cs_id);
    return *ret;
  }

  /// Fetches SearchSpace configuration based on SearchSpace-Id.
  const search_space_info* find_search_space(search_space_id ss_id) const
  {
    return ss_list.contains(ss_id) ? &ss_list[ss_id] : nullptr;
  }
  const search_space_info& search_space(search_space_id ss_id) const { return ss_list[ss_id]; }

  /// Get Search Space List for a given BWP-Id.
  const static_vector<const search_space_configuration*, MAX_NOF_SEARCH_SPACE_PER_BWP>&
  get_search_spaces(bwp_id_t bwpid) const
  {
    return bwp_table[bwpid].search_spaces;
  }

  /// \brief Gets UL DCI RNTI type to use based on SearchSpace configuration.
  /// \param[in] ss_id SearchSpace Id.
  /// \return UL DCI RNTI type.
  dci_ul_rnti_config_type get_ul_rnti_config_type(search_space_id ss_id) const;
  /// \brief Gets DL DCI RNTI type to use based on SearchSpace configuration.
  /// \param[in] ss_id SearchSpace Id.
  /// \return DL DCI RNTI type.
  dci_dl_rnti_config_type get_dl_rnti_config_type(search_space_id ss_id) const;

  span<const uint8_t> get_k1_candidates(dci_dl_rnti_config_type dci_type) const
  {
    // TS38.213, 9.2.3 - For DCI f1_0, the PDSCH-to-HARQ-timing-indicator field values map to {1, 2, 3, 4, 5, 6, 7, 8}.
    // However, the tested UEs only support k1 >= 4.
    static constexpr std::array<uint8_t, 5> dl_data_to_ul_ack_f1_0 = {4, 5, 6, 7, 8};
    // For a DCI format, other than DCI format 1_0, scheduling a PDSCH reception or a SPS PDSCH release, the PDSCH-to-
    // HARQ_feedback timing indicator field values, if present, map to values for a set of number of slots provided by
    // dl-DataToUL-ACK, or dl-DataToUL-ACKForDCIFormat1_2 for DCI format 1_2, as defined in Table 9.2.3-1
    const auto& pucch_cfg = *cfg_dedicated().ul_config->init_ul_bwp.pucch_cfg;
    if (dci_type != dci_dl_rnti_config_type::c_rnti_f1_1) {
      return span<const uint8_t>{dl_data_to_ul_ack_f1_0};
    }
    srsran_assert(not pucch_cfg.dl_data_to_ul_ack.empty(),
                  "List of timing for given PDSCH to the DL ACK cannot be empty");
    return span<const uint8_t>{pucch_cfg.dl_data_to_ul_ack};
  }

private:
  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_downlink_common& bwp_dl_common);
  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded);

  /// Dedicated cell configuration.
  serving_cell_config cell_cfg_ded;

  /// Lookup table for BWP params indexed by BWP-Id.
  std::array<bwp_info, MAX_NOF_BWPS> bwp_table = {};

  /// This array maps SearchSpace-Ids (the array indexes) to SearchSpace parameters (the array values).
  slotted_array<search_space_info, MAX_NOF_SEARCH_SPACES> ss_list;

  /// This array maps Coreset-Ids (the array indexes) to CORESET configurations (the array values).
  /// Note: The ID space of CoresetIds is common across all the BWPs of a Serving Cell.
  std::array<const coreset_configuration*, MAX_NOF_CORESETS> coresets = {};

  /// This array maps SearchSpace-Ids (the array indexes) to SearchSpace configurations (the array values).
  /// Note: The ID space of SearchSpaceIds is common across all the BWPs of a Serving Cell.
  std::array<const search_space_configuration*, MAX_NOF_SEARCH_SPACES> search_spaces = {};

  /// This array maps Coreset-Ids (the array indexes) to BWP-Ids (the array values).
  std::array<bwp_id_t, MAX_NOF_BWPS> coreset_id_to_bwp_id;
};

} // namespace srsran
