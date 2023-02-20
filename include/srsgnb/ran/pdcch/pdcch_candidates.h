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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/pdcch/aggregation_level.h"

namespace srsran {

/// Maximum number of candidates per aggregation level in a SS as per TS38.331 SearchSpace.
static constexpr unsigned PDCCH_MAX_NOF_CANDIDATES_SS = 8;

/// PDCCH candidate data type.
using pdcch_candidate_list = static_vector<uint8_t, PDCCH_MAX_NOF_CANDIDATES_SS>;

/// \brief Parameters for calculating the PDCCH lowest Control Channel Element (CCE).
///
/// The parameters in this struct allow computing the PDCCH lowest CCE, for all candidates and for a Common SS,
/// following the specifics in TS38.213 Section 10.1.
///
/// Consider an active DL BWP of a serving cell with carrier indicator field \f$n_{CI}\f$. In the search space set
/// \f$s\f$, associated with CORESET \f$p\f$ in slot \f$n_{s,f}^\mu\f$, the lowest CCE index \f$n_{CCE}\f$ corresponding
/// to the PDCCH candidate \f$m_{s,n_{CI}}\f$ is given by
/// \[
/// n_{CCE} = L\cdot \left \{ \left ( Y_{p,n_{s,f}^\mu} + \left \lfloor \frac{m_{s,n_{CI}}\cdot N_{CCE,p}}{L\cdot
/// M_{s,max}} \right \rfloor +n_{CI} \right ) mod \left \lfloor N_{CCE,p}/L \right \rfloor \right \}
/// \]
/// where:
/// - \f$Y_{p,n_{s,f}^\mu}=0\f$,
/// - \f$n_{CI}=0\f$,
/// - \f$m_{s,n_{CI}}\in \{0,...,M_{s,n_{CI}}^{(L)}\}\f$,
struct pdcch_candidates_common_ss_configuration {
  /// Aggregation level \f$L\epsilon\{1,2,4,8,16\}\f$.
  aggregation_level L;
  /// \brief Number of PDCCH candidates \f$M_{s,max}^{(L)}\f$ the UE is configured to monitor for aggregation level
  /// \f$L\f$ of a search space set \f$s\f$ for a serving cell corresponding to \f$n_{CI}\f$.
  unsigned nof_candidates;
  /// Number of CCEs contained in CORESET \f$p\f$, \f$N_{CCE,p}\f$.
  unsigned nof_cce_coreset;
};

/// \brief Parameters for calculating the PDCCH lowest Control Channel Element (CCE).
///
/// The parameters in this struct allow computing the PDCCH lowest CCE, for all candidates and for a UE-Specific SS,
/// following the specifics in TS38.213 Section 10.1.
///
/// Consider an active DL BWP of a serving cell with carrier indicator field \f$n_{CI}\f$. In the search space set
/// \f$s\f$, associated with CORESET \f$p\f$ in slot \f$n_{s,f}^\mu\f$, the lowest CCE index \f$n_{CCE}\f$ corresponding
/// to the PDCCH candidate \f$m_{s,n_{CI}}\f$ is given by
/// \[
/// n_{CCE} = L\cdot \left \{ \left ( Y_{p,n_{s,f}^\mu} + \left \lfloor \frac{m_{s,n_{CI}}\cdot N_{CCE,p}}{L\cdot
/// M_{s,max}} \right \rfloor +n_{CI} \right ) mod \left \lfloor N_{CCE,p}/L \right \rfloor \right \}
/// \]
/// where:
/// - \f$Y_{p,n_{s,f}^\mu}\f$ is calculated as a function of parameters \f$p\f$, \f$n_\textup{RNTI}\f$ and
///   \f$n_{s,f}^\mu\f$;
/// - \f$n_{CI}=0\f$,
/// - \f$m_{s,n_{CI}}\in \{0,...,M_{s,n_{CI}}^{(L)}\}\f$.
struct pdcch_candidates_ue_ss_configuration {
  /// Aggregation level \f$L\epsilon\{1,2,4,8,16\}\f$.
  aggregation_level L;
  /// \brief Number of PDCCH candidates \f$M_{s,max}^{(L)}\f$ the UE is configured to monitor for aggregation level
  /// \f$L\f$ of a search space set \f$s\f$ for a serving cell corresponding to \f$n_{CI}\f$.
  unsigned nof_candidates;
  /// Number of CCEs contained in CORESET \f$p\f$, \f$N_{CCE,p}\f$.
  unsigned nof_cce_coreset;
  /// CORESET identifier. Parameter \f$p\f$.
  unsigned coreset_id;
  /// Parameter \f$n_\textup{RNTI}\f$.
  uint16_t rnti;
  /// Slot index within the radio frame. Parameter \f$n_{s,f}^\mu\f$.
  unsigned slot_index;
};

/// \brief Generates a PDCCH candidate list for Common SS as per TS38.213 Section 10.1.
///
/// An assertion is triggered if the number of candidates times the aggregation level exceeds the number of CCE in the
/// CORESET.
///
/// The resultant list contains the lowest CCE index of the \c config.nof_candidates.
pdcch_candidate_list pdcch_candidates_common_ss_get_lowest_cce(const pdcch_candidates_common_ss_configuration& config);

/// \brief Generates a PDCCH candidate list for UE-Specific SS as per TS38.213 Section 10.1.
///
/// An assertion is triggered if the number of candidates times the aggregation level exceeds the number of CCE in the
/// CORESET.
///
/// The resultant list contains the lowest CCE index of the \c config.nof_candidates.
pdcch_candidate_list pdcch_candidates_ue_ss_get_lowest_cce(const pdcch_candidates_ue_ss_configuration& config);

} // namespace srsran
