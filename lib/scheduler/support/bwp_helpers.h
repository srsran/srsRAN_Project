/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_BWP_HELPERS_H
#define SRSGNB_BWP_HELPERS_H

#include "srsgnb/scheduler/bwp_configuration.h"

namespace srsgnb {

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
  srsran_sanity_check(bwp_cfg.crbs.contains(crb) or crb == bwp_cfg.crbs.stop(),
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

} // namespace srsgnb

#endif // SRSGNB_BWP_HELPERS_H
