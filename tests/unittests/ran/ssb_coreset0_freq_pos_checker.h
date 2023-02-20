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

#include "srsgnb/ran/band_helper.h"

namespace srsgnb {

/// \brief Verify that the SSB and CORESET 0 configuration parameters are valid and the allocation is within the band.
///
/// \param[in] dl_arfcn is <em>DL-ARFCN<\em> corresponding to \f$F_{REF}\f$, as per TS 38.104, Section 5.4.2.1.
/// \param[in] nr_band is <em>NR operating band<\em>, as per TS 38.104, Table 5.2-1. Only FR1 values are supported.
/// \param[in] n_rbs is <em>Transmission bandwidth<\em> or \f$N_{RB}\f$ in number of RBs, as per TS 38.104, Table 5.2-1.
/// \param[in] scs_common is <em>subCarrierSpacingCommon<\em>, as per TS 38.331.
/// \param[in] scs_ssb is ssb subcarrier spacing.
/// \param[in] params is set of parameters defining the SSB position within the band and the CORESET0 and
/// SearchSpaceZero indices.
void test_ssb_coreset0_allocation(unsigned                                       dl_arfcn,
                                  nr_band                                        nr_band,
                                  unsigned                                       n_rbs_,
                                  subcarrier_spacing                             scs_common,
                                  subcarrier_spacing                             scs_ssb,
                                  const band_helper::ssb_coreset0_freq_location& params);

} // namespace srsgnb
