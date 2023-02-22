/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// \file
/// \brief Type0-PDCCH CSS n0/nc slot, SFNc set derivation as per TS 38.213, clause 13.
#pragma once

#include "../../cell/cell_configuration.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// \brief Helper function that returns whether slot n0 of Type0-PDCCH CSS is in an even/odd frame.
/// \param[in] table_13_11_and_13_12_O Parameter \c O provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] table_13_11_and_13_12_M Parameter \c M provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] numerology_mu Numerlogy based on SCS for PDCCH receptions in the CORESET.
/// \param[in] ssb_index SS/PBCH block with index.
/// \return 0 if slot n0 is located in an even frame or 1 if slot n0 is located in an odd frame.
unsigned type0_pdcch_css_n0_is_even_frame(double   table_13_11_and_13_12_O,
                                          double   table_13_11_and_13_12_M,
                                          uint8_t  numerology_mu,
                                          unsigned ssb_index);

/// \brief Helper function that returns slot n0 (where UE should monitor Type0-PDCCH CSS) for a given SSB (beam) index.
/// \param[in] table_13_11_and_13_12_O Parameter \c O provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] table_13_11_and_13_12_M Parameter \c M provided in Table 13-11 and 13-12 of TS 38 213.
/// \param[in] scs_common SCS for PDCCH receptions in the CORESET.
/// \param[in] ssb_index SS/PBCH block with index.
/// \return Returns slot n0 where UE should monitor Type0-PDCCH CSS.
slot_point get_type0_pdcch_css_n0(double             table_13_11_and_13_12_O,
                                  double             table_13_11_and_13_12_M,
                                  subcarrier_spacing scs_common,
                                  unsigned           ssb_index);

/// \brief Computes the Type0-PDCCH CSS n0 slot for each beam [TS 38.213, Section 13].
///
/// \param[in] searchspace0 Search Space Zero index.
/// \param[in] coreset0 CORESET 0 index.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon, which must coincide with SCS if initial DL BWP.
/// \param[in] ssb_index SSB Beam index.
/// \return Returns slot n0 where UE should monitor Type0-PDCCH CSS.
slot_point precompute_type0_pdcch_css_n0(uint8_t                   searchspace0,
                                         uint8_t                   coreset0,
                                         const cell_configuration& cell_cfg,
                                         subcarrier_spacing        scs_common,
                                         unsigned                  ssb_index);

/// \brief Computes the Type0-PDCCH CSS n0 + 1 slot for each beam [TS 38.213, Section 13].
///
/// \param[in] searchspace0 Search Space Zero index.
/// \param[in] coreset0 CORESET 0 index.
/// \param[in] cell_cfg Cell configuration.
/// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon, which must coincide with SCS if initial DL BWP.
/// \param[in] ssb_index SSB Beam index.
/// \return Returns slot n0 + 1 where UE should monitor Type0-PDCCH CSS.
slot_point precompute_type0_pdcch_css_n0_plus_1(uint8_t                   searchspace0,
                                                uint8_t                   coreset0,
                                                const cell_configuration& cell_cfg,
                                                subcarrier_spacing        scs_common,
                                                unsigned                  ssb_index);

} // namespace srsran
