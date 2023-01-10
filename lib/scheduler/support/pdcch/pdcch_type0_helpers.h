/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Type0-PDCCH CSS n0/nc slot, SFNc set derivation as per TS 38.213, clause 13.
#pragma once

#include "../../cell/cell_configuration.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

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

} // namespace srsgnb
