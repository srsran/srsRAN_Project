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

#include "config_helpers.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// \brief Convert PRB within a BWP into a Common RB, which use pointA as reference point.
/// The CRB and PRB are assumed to have the same numerology of the provided BWP configuration.
/// The existence of a CORESET#0 may also affect the rules for CRB<->PRB conversion.
/// \param bwp_cfg BWP configuration of the respective PRB.
/// \param prb PRB to be converted to CRB.
/// \return Calculated CRB.
inline unsigned prb_to_crb(const bwp_configuration& bwp_cfg, unsigned prb)
{
  srsran_sanity_check(prb <= bwp_cfg.crbs.length(), "PRB={} falls outside BWP limits={}", prb, bwp_cfg.crbs);
  return prb + bwp_cfg.crbs.start();
}

/// Convert PRBs within a BWP into Common RBs, which use pointA as reference point. CRBs and PRBs are assumed to have
/// the same numerology of the provided BWP configuration.
/// \param bwp_cfg BWP configuration of the respective PRB interval.
/// \param prbs PRBs to be converted to CRBs.
/// \return Calculated CRB interval.
inline crb_interval prb_to_crb(const bwp_configuration& bwp_cfg, prb_interval prbs)
{
  return {prb_to_crb(bwp_cfg, prbs.start()), prb_to_crb(bwp_cfg, prbs.stop())};
}

/// \brief Convert CRB within a BWP into a PRB.
/// The CRB and PRB are assumed to use the same numerology as reference.
/// \param bwp_cfg BWP configuration of the respective CRB.
/// \param crb CRB to be converted to PRB.
/// \return Calculated PRB.
inline unsigned crb_to_prb(const bwp_configuration& bwp_cfg, unsigned crb)
{
  return crb_to_prb(bwp_cfg.crbs, crb);
}

/// Convert CRBs to PRBs within a BWP. CRBs and PRBs are assumed to have the same numerology of the provided
/// BWP configuration.
inline prb_interval crb_to_prb(const bwp_configuration& bwp_cfg, crb_interval crbs)
{
  return crb_to_prb(bwp_cfg.crbs, crbs);
}

/// \brief Determine CRB limits for PDSCH grant, based on BWP config, SearchSpace type and DCI format as per
/// TS38.214, 5.1.2.2.2.
///
/// \param dci_type DL DCI format.
/// \param init_dl_bwp Initial DL BWP configuration.
/// \param active_dl_bwp Active DL BWP configuration.
/// \param ss_type SearchSpace configuration.
/// \param cs_cfg CORESET configuration corresponding to SerachSpace.
/// \return Calculated CRB limits.
inline crb_interval get_resource_alloc_dl_crb_limits(dci_dl_rnti_config_type           dci_type,
                                                     const bwp_downlink_common&        init_dl_bwp,
                                                     const bwp_downlink_common&        active_dl_bwp,
                                                     const search_space_configuration& ss_cfg,
                                                     const coreset_configuration&      cs_cfg)
{
  crb_interval crbs = active_dl_bwp.generic_params.crbs;

  if (dci_type == dci_dl_rnti_config_type::si_f1_0 || dci_type == dci_dl_rnti_config_type::ra_f1_0 ||
      dci_type == dci_dl_rnti_config_type::c_rnti_f1_0 || dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 ||
      dci_type == dci_dl_rnti_config_type::p_rnti_f1_0) {
    if (ss_cfg.type == search_space_type::common) {
      // See TS 38.211, 7.3.1.6 Mapping from virtual to physical resource blocks and TS38.214, 5.1.2.2. Resource
      // Allocation in frequency domain.
      crbs = {cs_cfg.get_coreset_start_crb(), crbs.stop()};

      // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
      if (init_dl_bwp.pdcch_common.coreset0.has_value()) {
        crbs.resize(std::min(crbs.length(), init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()));
      } else {
        crbs.resize(std::min(crbs.length(), init_dl_bwp.generic_params.crbs.length()));
      }
    }
  }
  return crbs;
}

/// \brief Determine CRB limits for PUSCH grant, based on BWP config, SearchSpace type and DCI format as per
/// TS38.214, 6.1.2.2.2.
///
/// \param dci_type DL DCI format.
/// \param init_ul_bwp Initial UL BWP configuration.
/// \param active_ul_bwp Active UL BWP configuration.
/// \param ss_type SearchSpace type.
/// \return Calculated CRB limits.
inline crb_interval get_resource_alloc_ul_crb_limits(dci_ul_rnti_config_type  dci_type,
                                                     const bwp_configuration& init_ul_bwp,
                                                     const bwp_configuration& active_ul_bwp,
                                                     search_space_type        ss_type)
{
  // See TS 38.214, 6.1.2.2.2, Uplink resource allocation type 1.
  if (dci_type == dci_ul_rnti_config_type::c_rnti_f0_0 and ss_type == search_space_type::common) {
    return init_ul_bwp.crbs;
  }
  return active_ul_bwp.crbs;
}

} // namespace srsran
