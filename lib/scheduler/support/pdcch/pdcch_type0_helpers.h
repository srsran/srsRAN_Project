/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Type0-PDCCH CSS n0/nc slot, SFNc set derivation as per TS 38.213, clause 13.
#pragma once

#include "../../config/cell_configuration.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

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
