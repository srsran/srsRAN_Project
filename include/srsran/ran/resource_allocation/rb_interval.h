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

#include "srsran/adt/interval.h"

namespace srsran {

/// Struct to express a {min,...,max} range of CRBs within a carrier.
struct crb_interval : public interval<unsigned> {
  using interval::interval;
};

/// Struct to express a {min,...,max} range of PRBs within a BWP.
struct prb_interval : public interval<unsigned> {
  using interval::interval;
};

/// Struct to express a {min,...,max} range of VRBs.
struct vrb_interval : public interval<unsigned> {
  using interval::interval;
};

/// \brief Convert CRB into PRB given the CRB assignment limits.
/// The CRB and PRB are assumed to use the same numerology as reference.
/// \param bwp_crb_lims BWP CRB limits set based on the active BWP configuration.
/// \param crb CRB to be converted to PRB.
/// \return Calculated PRB.
inline unsigned crb_to_prb(crb_interval bwp_crb_lims, unsigned crb)
{
  srsran_sanity_check(bwp_crb_lims.contains(crb) or crb == bwp_crb_lims.stop(),
                      "CRB={} falls outside BWP limits={}",
                      crb,
                      bwp_crb_lims);
  return crb - bwp_crb_lims.start();
}

/// \brief Convert an interval of CRBs to an interval of PRBs, given the BWP CRB limits.
inline prb_interval crb_to_prb(crb_interval bwp_crb_lims, crb_interval crbs)
{
  return prb_interval{crb_to_prb(bwp_crb_lims, crbs.start()), crb_to_prb(bwp_crb_lims, crbs.stop())};
}

/// \brief Convert a PRB into  CRB given the BWP CRB limits.
/// The CRB and PRB are assumed to use the same numerology as reference.
/// \param bwp_crb_lims BWP CRB limits set based on the active BWP configuration.
/// \param crb CRB to be converted to PRB.
/// \return Calculated PRB.
inline unsigned prb_to_crb(crb_interval bwp_crb_lims, unsigned prb)
{
  srsran_sanity_check(prb <= bwp_crb_lims.length(), "PRB={} falls outside BWP limits={}", prb, bwp_crb_lims);
  return prb + bwp_crb_lims.start();
}

/// \brief Convert an interval of PRBs to an interval of CRBs, given the BWP CRB limits.
inline crb_interval prb_to_crb(crb_interval bwp_crb_lims, prb_interval prbs)
{
  return crb_interval{prb_to_crb(bwp_crb_lims, prbs.start()), prb_to_crb(bwp_crb_lims, prbs.stop())};
}

} // namespace srsran

namespace fmt {

/// FMT formatter for prb_intervals.
template <>
struct formatter<srsran::prb_interval> : public formatter<srsran::interval<uint32_t>> {
};

/// FMT formatter for crb_intervals.
template <>
struct formatter<srsran::crb_interval> : public formatter<srsran::interval<uint32_t>> {
};

/// FMT formatter for vrb_intervals.
template <>
struct formatter<srsran::vrb_interval> : public formatter<srsran::interval<uint32_t>> {
};

} // namespace fmt
