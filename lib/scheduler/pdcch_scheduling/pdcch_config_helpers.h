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

#include "../support/config_helpers.h"
#include "srsgnb/ran/pdcch/cce_to_prb_mapping.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/config/bwp_configuration.h"

namespace srsgnb {

/// \brief Helper function to check whether a SearchSpace PDCCH is monitored by the UE for a given slot.
///
/// \param sl Slot in which the SearchSpace is being monitored.
/// \param ss_cfg SearchSpace configuration.
/// \return true if the SearchSpace is being monitored. False, otherwise.
inline bool is_pdcch_monitoring_active(slot_point sl, const search_space_configuration& ss_cfg)
{
  unsigned slot_offset  = sl.to_uint() % ss_cfg.monitoring_slot_period;
  unsigned window_start = ss_cfg.monitoring_slot_offset;
  unsigned window_end   = (window_start + ss_cfg.duration) % ss_cfg.monitoring_slot_period;

  // Checks whether slot_offset falls [window_start, window_end), taking into account the wrap-around of "window_end".
  if (window_start < window_end) {
    return slot_offset >= window_start and slot_offset < window_end;
  }
  return slot_offset >= window_start or slot_offset < window_end;
}

inline prb_index_list cce_to_prb_mapping(const bwp_configuration&     bwp_cfg,
                                         const coreset_configuration& cs_cfg,
                                         pci_t                        pci,
                                         aggregation_level            aggr_lvl,
                                         unsigned                     ncce)
{
  if (cs_cfg.id == 0) {
    return cce_to_prb_mapping_coreset0(
        get_coreset_start_crb(cs_cfg), get_coreset_nof_prbs(cs_cfg), cs_cfg.duration, pci, to_nof_cces(aggr_lvl), ncce);
  }
  if (cs_cfg.interleaved.has_value()) {
    return cce_to_prb_mapping_interleaved(bwp_cfg.crbs.start(),
                                          cs_cfg.freq_domain_resources(),
                                          cs_cfg.duration,
                                          cs_cfg.interleaved->reg_bundle_sz,
                                          cs_cfg.interleaved->interleaver_sz,
                                          cs_cfg.interleaved->shift_index,
                                          to_nof_cces(aggr_lvl),
                                          ncce);
  }
  return cce_to_prb_mapping_non_interleaved(
      bwp_cfg.crbs.start(), cs_cfg.freq_domain_resources(), cs_cfg.duration, to_nof_cces(aggr_lvl), ncce);
}

/// Calculates \f$n_ID\f$ as per TS38.211 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned get_scrambling_n_ID(const cell_configuration&         cell_cfg,
                                    const coreset_configuration&      cs_cfg,
                                    const search_space_configuration& ss_cfg)
{
  // For a UE-specifica search space [...] equals the higher-layer parameter pdcch-DMRS-ScramblingID if configured,
  if (ss_cfg.type == search_space_configuration::type::ue_dedicated and cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return *cs_cfg.pdcch_dmrs_scrambling_id;
  }
  // \f$n_id = N_\{ID}^{cell}\f$ otherwise.
  return cell_cfg.pci;
}

/// Calculates \f$n_{RNTI}\f$ as per TS38.211, 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned
get_scrambling_n_RNTI(rnti_t rnti, const coreset_configuration& cs_cfg, const search_space_configuration& ss_cfg)
{
  // \f$n_{RNTI}\f$ is given by the C-RNTI for a PDCCH in a UE-speicfic search space if the higher-layer parameter
  // pdcch-DMRS-ScramblingID is configured,
  if (ss_cfg.type == search_space_configuration::type::ue_dedicated and cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return rnti;
  }
  return 0;
}

} // namespace srsgnb
