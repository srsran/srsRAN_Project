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

#include "../config_helpers.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdcch/cce_to_prb_mapping.h"
#include "srsran/scheduler/config/bwp_configuration.h"

namespace srsran {
namespace pdcch_helper {

inline prb_index_list cce_to_prb_mapping(const bwp_configuration&     bwp_cfg,
                                         const coreset_configuration& cs_cfg,
                                         pci_t                        pci,
                                         aggregation_level            aggr_lvl,
                                         unsigned                     ncce)
{
  if (cs_cfg.id == 0) {
    return cce_to_prb_mapping_coreset0(cs_cfg.get_coreset_start_crb(),
                                       get_coreset_nof_prbs(cs_cfg),
                                       cs_cfg.duration,
                                       pci,
                                       to_nof_cces(aggr_lvl),
                                       ncce);
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

} // namespace pdcch_helper
} // namespace srsran
