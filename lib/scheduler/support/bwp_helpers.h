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

#include "config_helpers.h"
#include "srsgnb/scheduler/config/bwp_configuration.h"
#include "srsgnb/scheduler/scheduler_dci.h"
#include "srsgnb/support/error_handling.h"

namespace srsgnb {

/// \brief Convert PRB within a BWP into a Common RB, which use pointA as reference point.
/// The CRB and PRB are assumed to have the same numerology of the provided BWP configuration.
/// The existence of a CORESET#0 may also affect the rules for CRB<->PRB conversion.
/// \param bwp_cfg BWP configuration of the respective PRB.
/// \param prb PRB to be converted to CRB.
/// \return Calculated CRB.
inline unsigned prb_to_crb(const bwp_configuration& bwp_cfg, unsigned prb)
{
  srsgnb_sanity_check(prb <= bwp_cfg.crbs.length(), "PRB={} falls outside BWP limits={}", prb, bwp_cfg.crbs);
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
  srsgnb_sanity_check(bwp_cfg.crbs.contains(crb) or crb == bwp_cfg.crbs.stop(),
                      "CRB={} falls outside BWP limits={}",
                      crb,
                      bwp_cfg.crbs);
  return crb - bwp_cfg.crbs.start();
}

/// Convert CRBs to PRBs within a BWP. CRBs and PRBs are assumed to have the same numerology of the provided
/// BWP configuration.
inline prb_interval crb_to_prb(const bwp_configuration& bwp_cfg, crb_interval crbs)
{
  prb_interval prbs{crb_to_prb(bwp_cfg, crbs.start()), crb_to_prb(bwp_cfg, crbs.stop())};
  return prbs;
}

/// \brief Fetch DL BWP configuration for resource allocation type 1 based on SearchSpace type and DCI format.
/// \param dci_type DL DCI format.
/// \param init_dl_bwp Initial DL BWP configuration.
/// \param active_dl_bwp Active DL BWP configuration.
/// \param ss_type SearchSpace type.
/// \return Calculated BWP configuration.
inline bwp_configuration get_resource_alloc_type_1_dl_bwp_size(dci_dl_rnti_config_type            dci_type,
                                                               const bwp_downlink_common&         init_dl_bwp,
                                                               const bwp_downlink_common&         active_dl_bwp,
                                                               search_space_configuration::type_t ss_type)
{
  // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
  if (dci_type == dci_dl_rnti_config_type::si_f1_0 || dci_type == dci_dl_rnti_config_type::ra_f1_0 ||
      dci_type == dci_dl_rnti_config_type::c_rnti_f1_0 || dci_type == dci_dl_rnti_config_type::tc_rnti_f1_0 ||
      dci_type == dci_dl_rnti_config_type::p_rnti_f1_0) {
    bwp_configuration bwp_cfg = init_dl_bwp.generic_params;
    if (ss_type == search_space_configuration::type_t::common) {
      if (init_dl_bwp.pdcch_common.coreset0.has_value()) {
        bwp_cfg.crbs = get_coreset0_crbs(init_dl_bwp.pdcch_common);
      }
      return bwp_cfg;
    }
    // UE Search Space.
    bwp_cfg = active_dl_bwp.generic_params;
    return bwp_cfg;
  }
  report_fatal_error("Unsupported DL DCI format={}", dci_type);
}

/// \brief Fetch UL BWP configuration for resource allocation type 1 based on SearchSpace type and DCI format.
/// \param dci_type DL DCI format.
/// \param init_ul_bwp Initial UL BWP configuration.
/// \param active_ul_bwp Active UL BWP configuration.
/// \param ss_type SearchSpace type.
/// \return Calculated BWP configuration.
inline bwp_configuration get_resource_alloc_type_1_ul_bwp_size(dci_ul_rnti_config_type            dci_type,
                                                               const bwp_uplink_common&           init_ul_bwp,
                                                               const bwp_uplink_common&           active_ul_bwp,
                                                               search_space_configuration::type_t ss_type)
{
  // See TS 38.214, 6.1.2.2.2, Uplink resource allocation type 1.
  if (dci_type == dci_ul_rnti_config_type::tc_rnti_f0_0 || dci_type == dci_ul_rnti_config_type::c_rnti_f0_0) {
    if (ss_type == search_space_configuration::type_t::common) {
      return init_ul_bwp.generic_params;
    }
    // UE Search Space.
    return active_ul_bwp.generic_params;
  }
  report_fatal_error("Unsupported UL DCI format={}", dci_type);
}

} // namespace srsgnb
