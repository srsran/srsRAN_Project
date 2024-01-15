/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/interval.h"

namespace srsran {

/// Struct to express a {min,...,max} range of CRB indexes within a carrier.
struct crb_interval : public interval<unsigned> {
  using interval::interval;
};

/// Struct to express a {min,...,max} range of PRB indexes within a BWP.
struct prb_interval : public interval<unsigned> {
  using interval::interval;
};

/// Struct to express a {min,...,max} range of VRB indexes.
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
  srsran_sanity_check(prb <= bwp_crb_lims.length(), "PRB={} exceeds BWP width={}", prb, bwp_crb_lims.length());
  return prb + bwp_crb_lims.start();
}

/// \brief Convert an interval of PRBs to an interval of CRBs, given the BWP CRB limits.
inline crb_interval prb_to_crb(crb_interval bwp_crb_lims, prb_interval prbs)
{
  return crb_interval{prb_to_crb(bwp_crb_lims, prbs.start()), prb_to_crb(bwp_crb_lims, prbs.stop())};
}

/// \brief Conversion of CRBs to VRBs for a common search space and DCI format 1_0 as per TS38.211, clause 7.3.1.6.
/// \param crbs CRB interval to be converted to VRB interval.
/// \param coreset_crb_start Start of the coreset CRB interval.
/// \return VRB interval.
inline vrb_interval crb_to_vrb_f1_0_common_ss_non_interleaved(crb_interval crbs, unsigned coreset_crb_start)
{
  srsran_sanity_check(crbs.start() >= coreset_crb_start, "Invalid CRB start");
  return vrb_interval{crbs.start() - coreset_crb_start, crbs.stop() - coreset_crb_start};
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
