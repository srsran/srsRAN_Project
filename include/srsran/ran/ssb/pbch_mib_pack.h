/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/ssb/pbch_mib_message.h"
#include <cstdint>

namespace srsran {

/// \brief Packs the PBCH timing parameters in 8 bits.
///
/// The PBCH payload generation is described in TS38.212 Section 7.1.1.
///
/// \param[in] sfn               System Frame Number (0..1023).
/// \param[in] hrf               \c true if the SS/PBCH block transmission is in an odd half frame, \c false otherwise.
/// \param[in] ssb_block_index   SS/PBCH block opportunity index in a 5ms burst.
/// \param[in] subcarrier_offset Parameter \e ssb-SubcarrierOffset.
/// \param[in] Lmax              Maximum number of SS/PBCH block candidates in a 5ms burst, described in TS38.213
///                              Section 4.1.
/// \return The packed PBCH parameters.
uint8_t pbch_timing_pack(uint32_t              sfn,
                         bool                  hrf,
                         uint8_t               ssb_block_idx,
                         ssb_subcarrier_offset subcarrier_offset,
                         unsigned              L_max);

/// \brief Packs the Master Information Block (MIB).
///
/// The MIB contents are described by the Information Element \e MIB in TS38.331 Section 6.2.2.
///
/// \param[in] msg   PBCH Message containing an MIB for packing.
/// \return Packed MIB payload.
uint32_t pbch_mib_pack(const pbch_mib_message& msg);

/// \brief Packs the PBCH payload including the Master Information Block (MIB) and the timing bits.
///
/// The PBCH payload generation is described in TS38.212 Section 7.1.1. The MIB contents are described by the
/// Information Element \e MIB in TS38.331 Section 6.2.2.
///
/// \param[in] msg   PBCH Message containing an MIB for packing.
/// \param[in] Lmax  Maximum number of SS/PBCH block candidates in a 5ms burst, described in TS38.213
///                  Section 4.1.
/// \return Packed PBCH payload.
uint32_t pbch_mib_and_timing_pack(const pbch_mib_message& msg, unsigned L_max);

} // namespace srsran
