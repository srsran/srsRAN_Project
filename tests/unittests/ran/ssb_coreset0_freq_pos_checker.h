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

#include "srsran/ran/band_helper.h"

namespace srsran {

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

} // namespace srsran
