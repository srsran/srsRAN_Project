/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_CANDIDATES_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_CANDIDATES_H

#include "srsgnb/adt/static_vector.h"

namespace srsgnb {

/// Maximum number of candidates per aggregation level in a SS as per TS38.331 SearchSpace.
static constexpr unsigned PDCCH_MAX_NOF_CANDIDATES_SS = 8;

/// PDCCH candidate data type.
using pdcch_candidate_list = static_vector<uint8_t, PDCCH_MAX_NOF_CANDIDATES_SS>;

/// \brief Groups the required parameters for calculating the PDCCH lowest CCE for all candidates for common SS as per
/// TS38.213 Section 10.1.
///
/// The lower CCE index \f$n_{CCE}\f$ corresponding to the PDCCH candidate \f$m_{s,n_{CI}}\f$, in the search space set
/// \f$s\f$ associated with the CORESET \f$p\f$ in slot \f$n_{s,f}^\mu\f$ for an active DL BWP of a serving cell
/// corresponding to the carrier indicator field \f$n_{CI}\f$:
///
/// \f$ n_{CCE} = \left \{ \left ( Y_{p,n_{s,f}^\mu} + \left \lfloor \frac{m_{s,n_{CI}}\cdot N_{CCE,p}}{L\cdot
/// M_{s,max}} \right \rfloor +n_{CI} \right ) mod \left \lfloor N_{CCE,p}/L \right \rfloor \right \}\f$
///
/// Where:
/// - \f$Y_{p,n_{s,f}^\mu}=0\f$,
/// - \f$n_{CI}=0\f$,
/// - \f$m_{s,n_{CI}}\epsilon {0,...,M_{s,n_{CI}}^{(L)}}\f$,
struct pdcch_candidates_common_ss_configuration {
  /// Aggregation level \f$L\epsilon\{1,2,4,8,16\}\f$.
  unsigned aggregation_level;
  /// Number of PDCCH candidates \f$M_{s,max}^{(L)}\f$ the UE is configured to monitor for aggregation level \f$L\f$ of
  /// a search space set \f$s\f$ for a serving cell corresponding to \f$n_{CI}\f$.
  unsigned nof_candidates;
  /// Number of CCEs contained in CORESET \f$p\f$, \f$N_{CCE,p}\f$.
  unsigned nof_cce_coreset;
};

/// \brief Generates a PDCCH candidate list for Common SS as per TS38.213 Section 10.1.
///
/// Assertion is triggered if:
/// - the aggregation level is zero,
/// - the number of CCE in the CORESET is zero, or
/// - the number of candidates times the aggregation level exceeds the number of CCE in the CORESET.
///
/// The number of the resultant candidates is equal to \c nof_candidates.
pdcch_candidate_list pdcch_candidates_common_ss_get(const pdcch_candidates_common_ss_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_CANDIDATES_H
