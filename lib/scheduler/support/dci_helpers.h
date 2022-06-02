/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DCI_HELPERS_H
#define SRSGNB_DCI_HELPERS_H

#include "srsgnb/ran/bwp_configuration.h"

namespace srsgnb {

inline unsigned coreset_nof_prbs(const coreset_configuration& cs_cfg)
{
  // Each bit corresponds a group of 6 RBs, with grouping starting from the first RB group (see TS 38.213 [13],
  // clause 10.1) in the BWP.
  return cs_cfg.freq_domain_resources.count() * 6U;
}

/// \brief Converts PRBs into DCI format 1_0 frequency domain resource assignment.
/// \remark See TS 38.214, 7.3.1.2.1 - Format 1_0.
/// \return frequency domain resource assignment.
inline uint8_t get_dci_f1_0_freq_domain_assignment(const bwp_downlink_common&        init_bwp_cfg,
                                                   const search_space_configuration& ss_cfg,
                                                   rnti_type                         rntitype,
                                                   const prb_interval&               prbs)
{
  unsigned nof_prb;
  if (init_bwp_cfg.pdcch_common.coreset0.has_value()) {
    // CORESET 0 is configured for the cell.
    nof_prb = coreset_nof_prbs(*init_bwp_cfg.pdcch_common.coreset0);
  } else {
    // CORESET 0 is not configured for the cell.
    nof_prb = init_bwp_cfg.generic_params.crbs.length();
  }

  unsigned rb_start = prbs.start();
  srsran_sanity_check(prbs.length() <= nof_prb, "Invalid PRB grant");
  return sliv_from_s_and_l(nof_prb, rb_start, prbs.length());
}

inline uint8_t get_dci_f0_0_freq_domain_assignment(const bwp_uplink_common& init_bwp_cfg, const prb_interval& prbs)
{
  // TODO
  return sliv_from_s_and_l(init_bwp_cfg.generic_params.crbs.length(), prbs.start(), prbs.stop());
}

} // namespace srsgnb

#endif // SRSGNB_DCI_HELPERS_H
